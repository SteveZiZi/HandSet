#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "rfidTest.h"
#include "assertMsg.h"

CRfidTest::CRfidTest()
{

}
CRfidTest::~CRfidTest()
{

}

BOOL CRfidTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"RFIDTest_\0\0";
    strSection[9]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"RFID",
        m_strTestName,31,strIniFile);



    m_uiCmd=index+CMD_TEST_RFID;
    m_pManager= pManager;
    return TRUE;
}

TEST_STATUS CRfidTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    m_pManager->MessageBox(L"请将工作证放于RFID模块的上面",L"注意",MB_OK|MB_ICONINFORMATION);
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