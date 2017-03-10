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
    wcscpy(m_reportNape[0].strNape,L"ÿ���ֽ���");
    wcscpy(m_reportNape[1].strNape,L"ͨ������");
    wcscpy(m_reportNape[2].strNape,L"���԰���");
    wcscpy(m_reportNape[3].strNape,L"�������");
    wcscpy(m_reportNape[4].strNape,L"��ȷ��");
    wcscpy(m_reportNape[5].strNape,L"����ʱ��");
}

CEthernetTest::~CEthernetTest()
{
    //�ر������׽���
    m_clientSocket.CloseSocket();
}

BOOL CEthernetTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"EthTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"��̫��1",
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
    //��ȡ�ͻ��˵ĳ�ʼ����Ϣ���ɴ˻�ȡ��IP��ַ
    assertMsg(pConect,L"pConect����ΪNULL");
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
            //�������жϿͻ��˳�ʼ�������
            if (ei.bErr)
            {
                //�ͻ��˳�ʼ������
                m_pManager->WriteLog(this,ei.strError);
                m_bInitError=TRUE;
                return 0;
            }
            else
            {
                //���Կͻ��˳�ʼ���ɹ��������ϲ��Կͻ��˵�IP
                struct in_addr ipList[10];
                int     cnt =GetLocalIps(ipList,10);
                BOOL    bConnect=FALSE;

                //ͨ�����ֿ��ܵ��������ӵ�ָ���Ĳ��Կͻ���IP
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
            m_pManager->WriteLog(this,L"���ӵ�Ŀ��IPʧ��");
            m_bInitError=TRUE;
            return 0;
        }        
    }

    //ִ�е������ʾͨ�ų���
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
                swprintf(m_strLastError,L"���ӵ�������ʧ��");
                return SITUATION_ERROR;
            }
        }
        else
        {
            m_uiErrCount ++;
            return SITUATION_ERROR;
        }
       
        
    }

    //������ʽ��ʼ����
    //���Ͳ���������
    DWORD  dw1=GetTickCount();
    if (m_clientSocket.sendNBytes(m_pBuf,ETH_TEST_PACKET_LEN)>0 )
    {
        //��������֮���������
        nRet=m_clientSocket.RecvNBytesTimeOut(m_pBuf,ETH_TEST_PACKET_LEN,30000);
        if (nRet>0 )
        {
            //�������ݳɹ����ز��Գɹ�             
            m_uiTestTime+=GetTickCount()-dw1;
            return SITUATION_NORMAL;
        }
        else if (nRet ==0)        
        {
            //���ճ�ʱ
            m_pManager->WriteLog(this,L"��30s�ڻ�δ���յ����ݣ����������ѶϿ���������������");           
        }
        else
        {
            m_pManager->WriteLog(this,L"�������ݳ������������ѶϿ���������������");
        }
    }
    else
    {        
        //���ͳ���Ĵ���Ӧ�ùر��׽��ֲ������Զ��������ӵ�������
        m_pManager->WriteLog(this,L"�������ݳ��ִ��������ѶϿ�����ʼ������������");
    }

    //�����׽������ӶϿ�
    m_uiErrCount++;
    m_clientSocket.CloseSocket();
    //�����������ӵ��������̫������
    m_bInitError=TRUE;
    m_uiTestTime+=GetTickCount()-dw1;
    return SITUATION_ERROR;    
}

const SReportNape *CEthernetTest::GetReportNapes(int &count)
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=6;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d�ֽ�",ETH_TEST_PACKET_LEN);
    m_reportNape[0].bHighLight=FALSE;
   
    if (m_uiTestTime !=0)
    {
        swprintf(m_reportNape[1].strNapeDescriptor,L"%d�ֽ�/��",
            (UINT)((UINT64)m_uiTestCount*1000/m_uiTestTime*ETH_TEST_PACKET_LEN) );
        m_reportNape[1].bHighLight=FALSE;
    }
    else
    {
        swprintf(m_reportNape[1].strNapeDescriptor,L"0�ֽ�/��" );
        m_reportNape[1].bHighLight=FALSE;
    }
   

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[2].bHighLight=FALSE;

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d��",m_uiErrCount);    
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

    swprintf(m_reportNape[5].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
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
                //���ҵ�ָ�����������Ի�ȡ��IP��ַ
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
            swprintf(m_strLastError,L"ָ��������%s�ƺ�û�в�������",m_wszEthName);
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
        swprintf(m_strLastError,L"û�ҵ�����%s",m_wszEthName);
    }


    Sleep(500);
    return FALSE;
}