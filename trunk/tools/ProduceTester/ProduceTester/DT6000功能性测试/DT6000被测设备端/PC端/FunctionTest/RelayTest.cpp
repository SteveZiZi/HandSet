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
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"RelayTest_\0\0";
    strSection[10]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"�̵���",m_strTestName,31,strIniFile);

    m_iRelayNum = GetPrivateProfileInt(strSection,L"RelayNum",0,strIniFile);
    m_iOpenInputNum = GetPrivateProfileInt(strSection,L"OpenInputNum",0,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_RELAY;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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