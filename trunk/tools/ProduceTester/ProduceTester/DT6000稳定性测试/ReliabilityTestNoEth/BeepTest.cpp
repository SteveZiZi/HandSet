#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "BeepTest.h"
#include "iniFile.h"

CBeepTest::CBeepTest():
m_hInstBeepDll(NULL),
epcBuzzerGetStatus(NULL),
epcBuzzerOn(NULL),
epcBuzzerOff(NULL)
{
    wcscpy(m_reportNape[0].strNape,L"测试次数");
    wcscpy(m_reportNape[1].strNape,L"错误次数");
    wcscpy(m_reportNape[2].strNape,L"正确率");
    wcscpy(m_reportNape[3].strNape,L"持续时间"); 
}

CBeepTest::~CBeepTest()
{
    if (m_hInstBeepDll !=NULL)
    {
        CloseHandle(m_hInstBeepDll);
        m_hInstBeepDll=NULL;
    }
    epcBuzzerGetStatus=NULL;
    epcBuzzerOn=NULL;
    epcBuzzerOff=NULL;
}

BOOL CBeepTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"BEEPTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"蜂鸣器",
        m_strTestName,31,strIniFile); 

    //载入蜂鸣器操作dll
    m_hInstBeepDll=LoadLibrary(_T("epcBuzzerLib.dll"));
    if (m_hInstBeepDll==NULL)
    {
        MessageBox(NULL,_T("加载epcBuzzerLib.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }

    epcBuzzerOn=(BOOL (*)(DWORD) )GetProcAddress(m_hInstBeepDll,TEXT("epcBuzzerOn"));
    epcBuzzerOff=( BOOL (*)(void) ) GetProcAddress(m_hInstBeepDll,TEXT("epcBuzzerOff"));
    epcBuzzerGetStatus=( DWORD  (*) (void) ) GetProcAddress
        (m_hInstBeepDll,TEXT("epcBuzzerGetStatus"));

    if (epcBuzzerOn ==NULL ||
        epcBuzzerOff==NULL ||
        epcBuzzerGetStatus ==NULL)
    {
        return FALSE;
    }
    m_pManager = pManager;

    return TRUE;
}

TEST_SITUATION CBeepTest::ExcuteOneTest()
{
    TEST_SITUATION      siRet=SITUATION_NORMAL;
    
    DWORD   dwRet1,dwRet2;  

    epcBuzzerOn(0);
    Sleep(30);
    dwRet1=epcBuzzerGetStatus();
    epcBuzzerOff();
    Sleep(30);
    dwRet2 =epcBuzzerGetStatus();

    if ( (dwRet1) || !dwRet2 )
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,L"蜂鸣器状态校验失败");
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }

    return siRet;
}

const SReportNape *CBeepTest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d次",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d次",m_uiErrCount);    
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

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}