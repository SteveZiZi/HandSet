#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "LedTest.h"
#include "iniFile.h"

CLedTest::CLedTest():
m_hInstLedDll(NULL),
epcLedOn(NULL),
epcLedOff(NULL),
epcLedGetStatus(NULL)
{
    wcscpy(m_reportNape[0].strNape,L"���Դ���");
    wcscpy(m_reportNape[1].strNape,L"�������");
    wcscpy(m_reportNape[2].strNape,L"��ȷ��");
    wcscpy(m_reportNape[3].strNape,L"����ʱ��"); 
}

CLedTest::~CLedTest()
{
    if (m_hInstLedDll)
    {
        CloseHandle(m_hInstLedDll);
        m_hInstLedDll=NULL;
    }

    epcLedOn=NULL;
    epcLedOff=NULL;
    epcLedGetStatus=NULL;
}

int CLedTest::Init(const WCHAR *strIniFile, int index ,CTestManager *pManager)
{
    WCHAR strSection[]=L"LEDTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"LED",
        m_strTestName,31,strIniFile); 

    //�������������dll
    m_hInstLedDll=LoadLibrary(_T("epcLedLib.dll"));
    if (m_hInstLedDll==NULL)
    {
        MessageBox(NULL,_T("����epcLedLib.dllʧ�ܣ�������������ά����Ա��ϵ"),
            _T("���ش���"),MB_OK|MB_ICONERROR);
        return FALSE;
    }

    epcLedOn=( BOOL (*)(DWORD , DWORD ) )GetProcAddress(m_hInstLedDll,TEXT("epcLedOn"));
    epcLedOff=( BOOL (*) (DWORD) ) GetProcAddress(m_hInstLedDll,TEXT("epcLedOff"));
    epcLedGetStatus=( DWORD (*) (DWORD) ) GetProcAddress
        (m_hInstLedDll,TEXT("epcLedGetStatus"));

    if (epcLedOn ==NULL ||
        epcLedOff==NULL ||
        epcLedGetStatus ==NULL)
    {
        return FALSE;
    }

    m_pManager = pManager;
    return TRUE;
}

TEST_SITUATION CLedTest::ExcuteOneTest()
{
    TEST_SITUATION  siRet=SITUATION_NORMAL;

    DWORD   dwRet1,dwRet2;   

    epcLedOn(LED1,0);
    epcLedOn(LED2,0);
    Sleep(50);
    dwRet1=epcLedGetStatus(LED1);
    dwRet2=epcLedGetStatus(LED2);

    if ( dwRet1 || dwRet2 )
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"led״̬У��ʧ��");
    }

    epcLedOff(LED1);
    epcLedOff(LED2);
    Sleep(50);  

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }
    
    return siRet;
}

const SReportNape *CLedTest::GetReportNapes(int &count)
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