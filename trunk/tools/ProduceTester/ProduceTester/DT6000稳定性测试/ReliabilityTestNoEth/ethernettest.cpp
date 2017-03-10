#include "stdafx.h"
#include "TSocket.h"
#include <atlstr.h>
#include <IPHlpApi.h>
#include "assertMsg.h"
#include "testCommon.h"
#include "testManager.h"
#include "ethernetTest.h"
#include "iniFile.h"

#pragma comment(lib,"Iphlpapi.lib")

CEthernetTest::CEthernetTest():
m_bInitError(TRUE)

{
    wcscpy(m_reportNape[0].strNape,L"每包字节数");
    wcscpy(m_reportNape[1].strNape,L"通信速率");
    wcscpy(m_reportNape[2].strNape,L"测试包数");
    wcscpy(m_reportNape[3].strNape,L"出错包数");
    wcscpy(m_reportNape[4].strNape,L"正确率");
    wcscpy(m_reportNape[5].strNape,L"持续时间");
}

CEthernetTest::~CEthernetTest()
{
    //关闭连接套接字
    m_clientSocket.CloseSocket();
}

BOOL CEthernetTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"EthTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"以太网1",
        m_strTestName,31,strIniFile);

    nRet=GetPrivateProfileString(strSection,L"EthName",L"ETHERNET1",
        m_wszEthName,31,strIniFile);
    m_pManager=pManager;
    m_bInitError=TRUE;
    return TRUE;
}

/*int CEthernetTest::InitDevTest(CConnect *pConect)
{
    int nRet;
    //获取客户端的初始化信息，由此获取其IP地址
    assertMsg(pConect,L"pConect不能为NULL");
    if (!m_bInitError)
    {
        return 1;
    }

    nRet=pConect->SendAPacket(m_uiCmd,PARAM_GET_INIT_INFO,NULL,0);

    if (nRet >0)
    {
        ETN_INIT_INFO ei;
        UINT16 index,param, len;        
        nRet=pConect->RecvAPacket(index,param,len,(BYTE *)&ei,GLOBAL_RECV_TIME_OUT);
        if (nRet >0)
        {
            //在这里判断客户端初始化的情况
            if (ei.bErr)
            {
                //客户端初始化出错
                m_pManager->WriteLog(this,ei.strError);
                m_bInitError=TRUE;
                return 0;
            }
            else
            {
                //测试客户端初始化成功，连接上测试客户端的IP
                struct in_addr ipList[10];
                int     cnt =GetLocalIps(ipList,10);
                BOOL    bConnect=FALSE;

                //通过各种可能的网卡连接到指定的测试客户端IP
                for(int i=0;i<cnt;i++)
                {
                    m_clientSocket.CloseSocket();
                    m_clientSocket.socketTcp();
                    if ( m_clientSocket.Connect(ei.uiIp,ETHERNET_PORT,
                        ipList[i].s_addr) )
                    {
                        m_bInitError=FALSE;
                        return 1;
                    }                    
                }
            }
            m_pManager->WriteLog(this,L"连接到目标IP失败");
            m_bInitError=TRUE;
            return 0;
        }        
    }

    //执行到这里，表示通信出错
    m_bInitError=TRUE;
    return -1;
   
}

int CEthernetTest::DeInitDevTest(CConnect *pConnect)
{
    m_clientSocket.CloseSocket();
    m_bInitError=TRUE;
    return 0;
}*/

TEST_SITUATION CEthernetTest::ExcuteOneTest()
{
    int nRet;
    m_uiTestCount++;
    if (m_bInitError)
    {
        m_clientSocket.CloseSocket();
        m_clientSocket.socketTcp();
        CStringA    str;
        str = m_pManager->GetServerIp();
        WCHAR       wszLocalIp[32];
        if (FindNetCard(wszLocalIp))
        {
            CStringA        strLocal;
            strLocal = wszLocalIp;
            if (m_clientSocket.Connect(str,ETHERNET_PORT,strLocal))
            {
                m_bInitError = FALSE;
            }
            else
            {
                m_uiErrCount ++;
                swprintf(m_strLastError,L"连接到服务器失败");
                return SITUATION_ERROR;
            }
        }
        else
        {
            m_uiErrCount ++;
            return SITUATION_ERROR;
        }
       
        
    }

    //现在正式开始测试
    //发送并接收数据
    DWORD  dw1=GetTickCount();
    if (m_clientSocket.sendNBytes(m_pBuf,ETH_TEST_PACKET_LEN)>0 )
    {
        //发送数据之后接收数据
        nRet=m_clientSocket.RecvNBytesTimeOut(m_pBuf,ETH_TEST_PACKET_LEN,30000);
        if (nRet>0 )
        {
            //接收数据成功返回测试成功             
            m_uiTestTime+=GetTickCount()-dw1;
            return SITUATION_NORMAL;
        }
        else if (nRet ==0)        
        {
            //接收超时
            m_pManager->WriteLog(this,L"在30s内还未接收到数据，怀疑连接已断开，尝试重新连接");           
        }
        else
        {
            m_pManager->WriteLog(this,L"接收数据出错，怀疑连接已断开，尝试重新连接");
        }
    }
    else
    {        
        //发送出错的处理，应该关闭套接字并尝试自动重新连接到服务器
        m_pManager->WriteLog(this,L"发送数据出现错误，连接已断开，开始尝试重新连接");
    }

    //测试套接字连接断开
    m_uiErrCount++;
    m_clientSocket.CloseSocket();
    //尝试重新连接到出错的以太网网卡
    m_bInitError=TRUE;
    m_uiTestTime+=GetTickCount()-dw1;
    return SITUATION_ERROR;    
}

const SReportNape *CEthernetTest::GetReportNapes(int &count)
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=6;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d字节",ETH_TEST_PACKET_LEN);
    m_reportNape[0].bHighLight=FALSE;
   
    if (m_uiTestTime !=0)
    {
        swprintf(m_reportNape[1].strNapeDescriptor,L"%d字节/秒",
            (UINT)((UINT64)m_uiTestCount*1000/m_uiTestTime*ETH_TEST_PACKET_LEN) );
        m_reportNape[1].bHighLight=FALSE;
    }
    else
    {
        swprintf(m_reportNape[1].strNapeDescriptor,L"0字节/秒" );
        m_reportNape[1].bHighLight=FALSE;
    }
   

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d包",m_uiTestCount);
    m_reportNape[2].bHighLight=FALSE;

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d包",m_uiErrCount);    
    if (m_uiTestCount ==0)
    {    swprintf(m_reportNape[4].strNapeDescriptor,L"0.00%% " );
    }
    else
    {
        swprintf(m_reportNape[4].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    
    if (m_uiErrCount >0) 
    {
        m_reportNape[3].bHighLight=TRUE;
        m_reportNape[4].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[3].bHighLight=FALSE;
        m_reportNape[4].bHighLight=FALSE;
    }  
   
    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[5].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[5].bHighLight=FALSE;

    return m_reportNape;
}

BOOL CEthernetTest::FindNetCard(WCHAR *wszIP)
{
    PIP_ADAPTER_INFO    p = NULL,p1;
    ULONG               size = 0;
    CString             str,str1;
    BOOL                bFind = FALSE;
    CStringA             strIP;

    GetAdaptersInfo(p,&size);
    if (size >0)
    {
        p=(PIP_ADAPTER_INFO)malloc(size);
        GetAdaptersInfo(p,&size); 
        p1=p;
        do 
        {
            str=p1->AdapterName;
            str.MakeUpper();
            str1 = m_wszEthName;
            str1.MakeUpper();
            if (str ==str1)
            {
                //查找到指定网卡，尝试获取其IP地址
                bFind=TRUE;
                strIP=p1->IpAddressList.IpAddress.String;
                break;
            }

            p1=p1->Next;
        } while (p1!=NULL);

        free(p);
        p=NULL;
    }

    if (bFind)
    {
        if (inet_addr(strIP )==INADDR_NONE || inet_addr(strIP )==0)
        {
            swprintf(m_strLastError,L"指定的网卡%s似乎没有插上网线",m_wszEthName);
        }
        else
        {
            CString     t;
            t=strIP;
            wcscpy(wszIP,t);
            return TRUE;
        }
    }
    else
    {
        swprintf(m_strLastError,L"没找到网卡%s",m_wszEthName);
    }


    Sleep(500);
    return FALSE;
}