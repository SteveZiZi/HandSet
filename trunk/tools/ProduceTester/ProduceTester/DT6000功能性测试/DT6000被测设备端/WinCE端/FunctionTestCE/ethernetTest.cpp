#include "stdafx.h"
#include <WinSock2.h>
#include "connect.h"
#include "testBase.h"
#include "TSocket.h"
#include "testManager.h"
#include "ethernetTest.h"
#include "iniFile.h"

#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib")



//以太网测试的类
CEthernetTest::CEthernetTest():
m_pSocketComm(NULL),
m_bFindEth(FALSE),
m_bEthLinked(FALSE),
m_hServerThread(NULL),
m_hTestThread(NULL),
m_bKillServerThread(FALSE)
{

}

CEthernetTest::~CEthernetTest()
{
    //首先尝试退出服务器线程
    m_bKillServerThread=TRUE;
    m_socketServer.CloseSocket();
    if (m_hServerThread !=NULL)
    {
        WaitForSingleObject(m_hServerThread,INFINITE);
        CloseHandle(m_hServerThread);
        m_hServerThread=NULL;
    }

    //尝试退出测试线程
    if (m_pSocketComm !=NULL)
    {
        m_pSocketComm->CloseSocket();
    }

    if (m_hTestThread !=NULL)
    {
        WaitForSingleObject(m_hTestThread,INFINITE);
        CloseHandle(m_hTestThread);
        m_hTestThread=NULL;
    }

    if (m_pSocketComm !=NULL)
    {
        delete m_pSocketComm;
        m_pSocketComm=NULL;
    }
}


BOOL CEthernetTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"EthTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"以太网1",
        m_strTestName,31,strIniFile); 

    //读取指定网卡的名称
    nRet=GetPrivateProfileString(strSection,L"EthName",L"ETHERNET1",
        m_strEthName,31,strIniFile);

    //创建服务器线程
    m_bKillServerThread=FALSE;
    m_hServerThread=CreateThread(NULL,0,ServerThreadProc,this,0,NULL);

    m_uiCmd=index+CMD_TEST_ETHERNET1;
    *uipCmdIndex=m_uiCmd;

    return m_hServerThread!=NULL;
}

DWORD  CEthernetTest::ServerThreadProc(void *pVoid)
{
    CEthernetTest *pTest=(CEthernetTest *)pVoid;
    
    //循环查找指定的网卡是否存在 
    CString     str;
    CStringA    strIP;

    PIP_ADAPTER_INFO  p=NULL,p1;
    ULONG       size=0;
    BOOL        bFind=FALSE;            //是否查找到本索引项对应的网卡
    BOOL        bBuidServer;            //是否成功建立服务器

TRY_BUILD_SERVER:
     if (pTest ->m_bKillServerThread)
     {
         return 0;                      //如果要终止此线程
     }

    size=0;
    bFind=FALSE;
    bBuidServer=FALSE;
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
            if (str ==pTest->m_strEthName )
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
        pTest->m_bFindEth=TRUE;
        //成功找到指定的网卡，验证其IP地址是否有效
        if (inet_addr(strIP )==INADDR_NONE ||
            inet_addr(strIP )==INADDR_ANY)
        {
            pTest->m_bEthLinked=FALSE;
            //IP地址无效,表明这块网卡虽然存在但网线没有插好，等待300ms后重试
            Sleep(300);
            goto TRY_BUILD_SERVER;
        }
    }
    else
    {
        pTest->m_bFindEth=FALSE;
        pTest->m_bEthLinked=FALSE;
        //没有找到指定的网卡，可能是因为指定的网卡被禁用，等待300ms后重试
        Sleep(300);
        goto TRY_BUILD_SERVER;
    }

    //找到了指定的网卡，并且IP地址有效
    pTest->m_addLocalEth.s_addr=inet_addr(strIP);
    pTest->m_bEthLinked=TRUE;

    TSocket &s=pTest->m_socketServer;
    s.socketTcp();
    s.BuildTcpSever(strIP,ETH_TEST_SERVER_PORT,1);


    TSocket *pS=new TSocket();
    //开始接受新的连接
    while(s.Accept(pS))
    {
        //如果连接被接受，则启动测试线程
        //先要尝试退出测试线程
        if (pTest->m_hTestThread !=NULL)
        {
            if (pTest->m_pSocketComm !=NULL)
            {
                pTest->m_pSocketComm->CloseSocket();                
            }
            WaitForSingleObject(pTest->m_hTestThread ,INFINITE);
            CloseHandle(pTest->m_hTestThread);
            pTest->m_hTestThread=NULL;

            if (pTest->m_pSocketComm !=NULL)
            {
                delete pTest->m_pSocketComm;
                pTest->m_pSocketComm=NULL;
            }
        }
        //创建新的测试线程的处理
        pTest->m_pSocketComm=pS;
        HANDLE h=CreateThread(NULL,0,TestThreadProc,pVoid,0,NULL);
        if (h==NULL)
        {
            pTest->m_pSocketComm=NULL;
            pS->CloseSocket();
        }
        else
        {
            pTest->m_hTestThread=h;
            pS=new TSocket();
        }
    }

    delete pS;
    return 0;
}


TEST_STATUS CEthernetTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
                                            const BYTE *pData,CConnect *pConnect)
{
    TEST_INFO ti;

    ti.bError=1;
    if (m_bFindEth)
    {
        if (m_bEthLinked)
        {
            ti.bError=0;
            memcpy(ti.bAppend,&m_addLocalEth,sizeof(m_addLocalEth));
        }
        else
        {
            ti.uiErrorLen=swprintf(ti.strErr,L"指定的网卡没有连接网线");
        }

    }
    else
    {
        ti.uiErrorLen=swprintf(ti.strErr,L"未找到指定名称的网卡");
    }

    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&ti);
    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(TEST_INFO_TRANSFER),(BYTE *)&tit);

    return nRet>0?TEST_NORMAL:TEST_ERROR;
}

DWORD CEthernetTest::TestThreadProc(void *pVoid)
{

    CEthernetTest *pTest=(CEthernetTest *)pVoid;
    TSocket *s=pTest->m_pSocketComm;

    BYTE buf[ETH_TEST_PACKET_LEN];    

    while(1)
    {
        if (! (s->RecvNBytes(buf,ETH_TEST_PACKET_LEN) <0) )
        {
            if (! (s->sendNBytes(buf,ETH_TEST_PACKET_LEN)<0) )
            {

            }
            else
            {
                break;
            }
        }
        else
        {  
            break;
        }
    }
  
    return 0;
}
