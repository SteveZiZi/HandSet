#include "stdafx.h"
#include <WinSock2.h>
#include "connect.h"
#include "testCommon.h"
#include "testBase.h"
#include "testManager.h"
#include "TSocket.h"
#include "ethernetTest.h"
#include "assertMsg.h"

//��̫�����Ե�ʵ��
CEthernetTest::CEthernetTest()
{

}

CEthernetTest::~CEthernetTest()
{

}

BOOL CEthernetTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"EthTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"��̫��1",
        m_strTestName,31,strIniFile); 

    m_pManager=pManager;
    m_uiCmd=index+CMD_TEST_ETHERNET1;
    return TRUE;
}

TEST_STATUS CEthernetTest::OnBeginTest(CConnect *pConnect)
{
    PACKET                      pa;
    TEST_INFO_TRANSFER       *pInfo;

    m_pManager->showTestStatus(L"���ڻ�ȡָ����̫����IP��ַ");
    int nRet=pConnect->SendAPacket(m_uiCmd,PARAM_GET_INIT_INFO,0,NULL);
    if (nRet>0)
    {
        nRet =pConnect->RecvAPacket(&pa,10000);
    }
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    pInfo=(TEST_INFO_TRANSFER *)pa.pData;

    if (pInfo->bError)
    {
        TEST_INFO ti;
        TestInfoTransferToLocal(&ti,pInfo);
        m_pManager->MessageBox(ti.strErr,L"��̫��",MB_OK|MB_ICONERROR);
        return TEST_ERROR;
    }

    //��ȡ������̫��������IP��ַ
    struct  in_addr addr=*(struct in_addr*)pInfo->bAppend;

    //�������ӵ�ָ������̫������
    in_addr localIps[10];
    int cnt=GetLocalIps(localIps,10);
    int i;
    CString str;
    str.Format(L"���ڳ������ӵ�IP��ַ%d.%d.%d.%d...",
        addr.s_net,addr.s_host,addr.s_lh,addr.s_impno);
    m_pManager->showTestStatus(str);
    for (i=0;i<cnt;i++)
    {
        m_comSocket.CloseSocket();
        m_comSocket.socketTcp();

        if (m_comSocket.Connect(addr.s_addr,ETH_TEST_SERVER_PORT,localIps[i].s_addr )  )
        {
            break;
        }
    }

    if (i==cnt)
    {
        //���ӵ�ָ��������ʧ��
        m_pManager->MessageBox(L"���ӵ�ָ��������ʧ��",L"��̫������",MB_OK|MB_ICONERROR);
        m_comSocket.CloseSocket();
        return TEST_ERROR;
    }
    
    m_pManager->showTestStatus(L"��̫�������Ѿ��������ڲ���...");
    return TEST_NORMAL;
}

TEST_STATUS CEthernetTest::OnEndTest(CConnect *pConnect)
{
    m_comSocket.CloseSocket();


    return TEST_NORMAL;
}

TEST_STATUS CEthernetTest::ExcuteOneTest(CConnect *pConnect)
{
    BYTE buf[ETH_TEST_PACKET_LEN];

    int nRet;
    m_uiTestCount++;
    nRet =m_comSocket.sendNBytes(buf,4096);

    if (nRet >0)
    {
        nRet=m_comSocket.RecvNBytesTimeOut(buf,4096,1000);
        if (nRet >0)
        {
            return TEST_NORMAL;
        }
        else if (nRet ==0)
        {
            m_pManager->WriteLog(this,L"�������ݳ�ʱ");
        }
        else
        {
            m_pManager->WriteLog(this,L"��������ʧ��");
        }
    }
    else
    {
        m_pManager->WriteLog(this,L"��������ʧ��");
    }

    m_uiTestErrCount++;
    return TEST_ERROR;
}