#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "rtcTest.h"
#include "assertMsg.h"

CRtcTest::CRtcTest()
{
    m_strTestName[0]=0;    
}

CRtcTest::~CRtcTest()
{

}

BOOL CRtcTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"RTCTest_\0\0";
    strSection[8]=L'1'+index;

    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"RTC",
        m_strTestName,31,strIniFile);

    m_pManager=pManager;
    m_uiCmd=index+CMD_TEST_RTC;
    return TRUE;
}

TEST_STATUS CRtcTest::OnBeginTest(CConnect *pConect)
{
    //RTC���ó�ʼ��
    return TEST_NORMAL;
}

TEST_STATUS CRtcTest::OnEndTest(CConnect *pConect)
{
    return TEST_NORMAL;
}

TEST_STATUS CRtcTest::ExcuteOneTest(CConnect *pConect)
{
    PACKET      pa;
    int         nRet;
    SYSTEMTIME  stPC,*pstCE;   

    //���Ͳ������� 
    GetLocalTime(&stPC);    
    nRet=pConect->SendAPacket(m_uiCmd,0,sizeof(stPC),(BYTE *)&stPC );

    if (nRet >0)
    {
        nRet=pConect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {
            GetLocalTime(&stPC);
            m_uiTestCount++;
            pstCE=(SYSTEMTIME *)pa.pData;
            //����PC��ʱ���CE��ʱ��Ĳ�ֵ
            CTime tmPc,tmCE;
            tmPc=CTime::GetCurrentTime();
            tmCE=*pstCE;

            CTimeSpan tSpan = tmPc - tmCE;
            LONGLONG nSecDif = tSpan.GetTotalSeconds();
            if (nSecDif <-1 ||nSecDif >=1)
            {
                m_pManager->WriteLog(this,L"����ʱ����֤����");
                return TEST_ERROR;
            }

            return TEST_NORMAL;            
        }
    }

    if (nRet ==0)
    {
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }
    else
    {
        m_pManager->WriteLog(this,L"����ͨ�ų��ִ���");
    }

    return TEST_CONNECTDOWN;
}
