#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "RelayTest.h"
#include "assertMsg.h"

CRelayTest::CRelayTest()
    :m_iOpenInputNum(0)
    ,m_iRelayNum(0)
{
}

CRelayTest::~CRelayTest()
{

}

BOOL CRelayTest::Init(const WCHAR *strIniFile, int index, CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"RelayTest_\0\0";
    strSection[10]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"继电器",m_strTestName,31,strIniFile);

    m_iRelayNum = GetPrivateProfileInt(strSection,L"RelayNum",0,strIniFile);
    m_iOpenInputNum = GetPrivateProfileInt(strSection,L"OpenInputNum",0,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_RELAY;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CRelayTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    nRet=pConnect->SendAPacket(m_uiCmd,m_uiTestCount,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,15000);
        if (nRet >0)
        {
            m_uiTestCount++;
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT ->bError)
            {
                TEST_INFO ti;
                TestInfoTransferToLocal(&ti,pInfoT);
                m_pManager->WriteLog(this,ti.strErr);
                m_uiTestErrCount++;
                return TEST_ERROR;
            }
         
            return TEST_NORMAL;
        }
    }

    return TEST_CONNECTDOWN;

}

TEST_STATUS CRelayTest::OnBeginTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}