#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "wifiTest.h"
#include "iniFile.h"

BOOL Ping(LPCWSTR lpDestIP, DWORD dwTimeOut, DWORD dwTimes);
// {
//     TCHAR				szCmd[50];
//     PROCESS_INFORMATION	pi;
//     STARTUPINFO			si = {sizeof(STARTUPINFO)};
//     BOOL				bRet;
//     DWORD				dwExitCode = (DWORD)-1;
// 
// 
//     swprintf(szCmd, L"ping %s -n %d -w %d", lpDestIP, dwTimes, dwTimeOut);
//     bRet = CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
//     if (!bRet) return FALSE;
//     WaitForSingleObject(pi.hProcess, INFINITE);
//     GetExitCodeProcess(pi.hProcess, &dwExitCode);
//     CloseHandle(pi.hProcess);
//     CloseHandle(pi.hThread);
// 
//     return (dwExitCode == 0);
// }

CWifiTest::CWifiTest()
{
    wcscpy(m_reportNape[0].strNape,L"测试次数");
    wcscpy(m_reportNape[1].strNape,L"错误次数");
    wcscpy(m_reportNape[2].strNape,L"正确率");
    wcscpy(m_reportNape[3].strNape,L"持续时间"); 
}

BOOL CWifiTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"WIFITest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"WIFI",
        m_strTestName,31,strIniFile); 

    nRet=GetPrivateProfileString(strSection,L"DestIp",L"192.168.0.254",
        m_strDestIp,31,strIniFile); 
    m_pManager = pManager;
    return TRUE;
}

const SReportNape *CWifiTest::GetReportNapes(int &count)
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

TEST_SITUATION CWifiTest::ExcuteOneTest()
{
    m_uiTestCount++;
    if (!Ping(m_strDestIp,500,1))
    {
        swprintf(m_strLastError,L"ping远程服务器失败");
        m_uiErrCount++;
        return SITUATION_ERROR;
    }

    Sleep(1);
    return SITUATION_NORMAL;
}




