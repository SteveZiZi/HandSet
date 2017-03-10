#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "gprsTest.h"
#include "iniFile.h"

CGPRSTest::CGPRSTest()
{
    m_strTestName[0]=0;
    wcscpy(m_reportNape[0].strNape,L"���Դ���");
    wcscpy(m_reportNape[1].strNape,L"�������");
    wcscpy(m_reportNape[2].strNape,L"��ȷ��");
    wcscpy(m_reportNape[3].strNape,L"����ʱ��");
}

BOOL CGPRSTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"GPRSTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",
        m_strTestName,31,strIniFile); 

    m_nPort=GetPrivateProfileInt(strSection,L"SerialPort",0,strIniFile);
    m_nPort++;
    m_uiBraudRate=GetPrivateProfileInt(strSection,L"BraudRate",9600,strIniFile);

    if (!m_GPRS.GPRS_Init(m_nPort,m_uiBraudRate,NULL))
    {
        MessageBox(NULL,L"��ʼ��GPRSģ��ʧ��",L"GPRS",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    m_pManager = pManager;
    return TRUE;
}

const SReportNape *CGPRSTest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d��",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}

TEST_SITUATION CGPRSTest::ExcuteOneTest()
{
    m_uiTestCount++;

    int signal=m_GPRS.GPRS_GetSignal();

    BOOL  bErr=FALSE;
    if (signal <0)
    {
        bErr=TRUE;
        m_uiErrCount++;
        swprintf(m_strLastError,L"��ȡ��ǰ�ź�ǿ��ʧ��");
    }
//     else if (signal >31)
//     {
//         bErr=TRUE;
//         swprintf(m_strLastError,L"�ź�̫��");
//     }
// 
//     if (bErr)
//     {
//         m_uiErrCount++;
//         return SITUATION_ERROR;
//     }

    return SITUATION_NORMAL;

}