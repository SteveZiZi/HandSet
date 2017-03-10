#include "stdafx.h"
#include <WinSock2.h>
#include "connect.h"
#include "testCommon.h"
#include "testBase.h"
#include "testManager.h"
#include "TSocket.h"
#include "ethernetTest.h"
#include "assertMsg.h"

//以太网测试的实现
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

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"以太网1",
        m_strTestName,31,strIniFile); 

    m_pManager=pManager;
    m_uiCmd=index+CMD_TEST_ETHERNET1;
    return TRUE;
}

TEST_STATUS CEthernetTest::OnBeginTest(CConnect *pConnect)
{
    PACKET                      pa;
    TEST_INFO_TRANSFER       *pInfo;

    m_pManager->showTestStatus(L"正在获取指定以太网的IP地址");
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
        m_pManager->MessageBox(ti.strErr,L"以太网",MB_OK|MB_ICONERROR);
        return TEST_ERROR;
    }

    //获取测试以太网网卡的IP地址
    struct  in_addr addr=*(struct in_addr*)pInfo->bAppend;

    //尝试连接到指定的以太网网卡
    in_addr localIps[10];
    int cnt=GetLocalIps(localIps,10);
    int i;
    CString str;
    str.Format(L"正在尝试连接到IP地址%d.%d.%d.%d...",
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
        //连接到指定的网卡失败
        m_pManager->MessageBox(L"连接到指定的网卡失败",L"以太网错误",MB_OK|MB_ICONERROR);
        m_comSocket.CloseSocket();
        return TEST_ERROR;
    }
    
    m_pManager->showTestStatus(L"以太网测试已就绪，正在测试...");
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
            m_pManager->WriteLog(this,L"接收数据超时");
        }
        else
        {
            m_pManager->WriteLog(this,L"接收数据失败");
        }
    }
    else
    {
        m_pManager->WriteLog(this,L"发送数据失败");
    }

    m_uiTestErrCount++;
    return TEST_ERROR;
}