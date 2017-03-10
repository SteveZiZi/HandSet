#include "stdafx.h"
#include "testCommon.h"
#include "connect.h"
#include "testBase.h"
#include "rtcTest.h"
#include "iniFile.h"
#include "assertMsg.h"


CRTCTest::CRTCTest()
{
}

CRTCTest::~CRTCTest()
{

}

BOOL CRTCTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager )
{

    WCHAR strSection[]=L"RTCTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"RTC",
        m_strTestName,31,strIniFile);
    m_uiCmd=index+CMD_TEST_RTC;
    *uipCmdIndex= m_uiCmd;
    return TRUE;
}

TEST_STATUS CRTCTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                       const BYTE *pData,CConnect *pConnect)
{
    int nRet;
    SYSTEMTIME st,*pPCTime;

    pPCTime=(SYSTEMTIME *)pData;
    SetLocalTime(pPCTime);
    Sleep(3000);
    GetLocalTime(&st);
    nRet =pConnect->SendAPacket(m_uiCmd,0,sizeof(st),(BYTE *)&st);
    if (nRet >0)
    {
        return TEST_NORMAL;
    }

    return TEST_CONNECTDOWN;
}