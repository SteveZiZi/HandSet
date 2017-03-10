#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ZigBeeTest.h"
#include "assertMsg.h"

CZigBeeTest::CZigBeeTest()
{

}
CZigBeeTest::~CZigBeeTest()
{

}

BOOL CZigBeeTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"ZigBeeTest_\0\0";
    strSection[11]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ZigBee",
        m_strTestName,31,strIniFile);



    m_uiCmd=index+CMD_TEST_ZIGBEE;
    return TRUE;
}

TEST_STATUS CZigBeeTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,20000);
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