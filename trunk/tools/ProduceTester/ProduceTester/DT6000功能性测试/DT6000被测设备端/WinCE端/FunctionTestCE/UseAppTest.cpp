#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "UseAppTest.h"
#include "assertMsg.h"
#include "UsuallyCommon.h"
#include "iniFile.h"


BOOL CUseAppTest::Init(const WCHAR *strIniFile, int index,
                       UINT16 *uipCmdIndex,CTestManager *pManager/* =NULL */)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"UseAppTest_\0\0";
    strSection[11]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ISA",m_strTestName,31,strIniFile);
    m_pTestManager=pManager;

    //��ȡӦ�ó����·��
    WCHAR       wszApp[MAX_PATH*2];
    GetPrivateProfileString(strSection,L"AppName",L"test.exe",wszApp,MAX_PATH*2,strIniFile);
    CString     str;
    WCHAR *p=str.GetBuffer(MAX_PATH*2);
    GetExeDirectory(p,MAX_PATH*2);
    str.ReleaseBuffer();
    if (wszApp[0]==L'*')
    {
        m_wstrApp = &wszApp[1];
    }
    else
    {
        m_wstrApp = (LPCWSTR)(str+wszApp);
    }
    //��ȡ��ֹdemo����ķ�ʽ
    m_iTerminateDemoWay=GetPrivateProfileInt(strSection,L"TerminateDemoWay",0,strIniFile);
    if (m_iTerminateDemoWay ==1)
    {
        GetPrivateProfileString(strSection,L"DemoWindowTitle",L"AD",wszApp,MAX_PATH*2,strIniFile);
        m_wstrDemoWindowtitle = wszApp;
        m_uiMessage=GetPrivateProfileInt(strSection,L"DemoMessage",0,strIniFile);
        m_dwParam1=GetPrivateProfileInt(strSection,L"DemoMessageParam1",0,strIniFile);
        m_dwParam2=GetPrivateProfileInt(strSection,L"DemoMessageParam2",0,strIniFile);

    }
    else if (m_iTerminateDemoWay ==2)
    {
        GetPrivateProfileString(strSection,L"TerminateDemoCmd",L"test.exe",wszApp,MAX_PATH*2,strIniFile);
        if (wszApp[0]==L'*')
        {
            m_wstrTerminateCmd = &wszApp[1];
        }
        else
        {
            m_wstrTerminateCmd = (LPCWSTR)(str+wszApp);
        }
    } 
    else if (m_iTerminateDemoWay == 3)
    {
    }
    else
    {
        m_iTerminateDemoWay = 0;
    }

    m_uiCmd=CMD_TEST_USE_APP+index;
    *uipCmdIndex=m_uiCmd;

    return TRUE;  

}

TEST_STATUS CUseAppTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
                                          const BYTE *pData,CConnect *pConnect)
{
    PROCESS_INFORMATION         pi;
    int             nRet;

    if (param == 0)
    {
        ShowWindow(m_pTestManager->GetMainWnd(),SW_HIDE);
        BOOL  b=CreateProcess(m_wstrApp.c_str(),m_wstrApp.c_str(),NULL,NULL,FALSE,CREATE_NEW_CONSOLE
            ,NULL,NULL,NULL,&pi);
        if (b)
        {
            m_hDemoProcesss = pi.hProcess;
            CloseHandle(pi.hThread);
            m_dwDemoProcessID = pi.dwProcessId;

            if (m_iTerminateDemoWay ==1)
            {
                //�����Ҫ��Demo���������ڷ���һ����Ϣ������ҵ�Demo�����������
                EnumWindows(__EnumWindowProc,(LPARAM)this);
                if (m_hDemoMainWnd ==NULL)
                {
                    //���ܲ��ҵ�Demo�����������
                    m_iTerminateDemoWay = 0;
                }
                
            }

            nRet=pConnect->SendAPacket(m_uiCmd,1,0,NULL);

        }
        else
        {
            nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
        }

    }
    else
    {
        DWORD dwExitCode =0;
        if (m_iTerminateDemoWay == 0)
        {

            TerminateProcess(m_hDemoProcesss,0);
            CloseHandle(m_hDemoProcesss);
        }
        else if(m_iTerminateDemoWay ==1) 
        {
            SendMessage(m_hDemoMainWnd,m_uiMessage,m_dwParam1,m_dwParam2);
            WaitForSingleObject(m_hDemoProcesss,INFINITE);
            GetExitCodeProcess(m_hDemoProcesss,&dwExitCode);
            CloseHandle(m_hDemoProcesss);

        }
        else if (m_iTerminateDemoWay ==2)
        {
            BOOL  b=CreateProcess(m_wstrTerminateCmd.c_str(),m_wstrTerminateCmd.c_str(),
                NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,NULL,&pi);
            if (b)
            {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }

            WaitForSingleObject(m_hDemoProcesss,INFINITE);
            GetExitCodeProcess(m_hDemoProcesss,&dwExitCode);
            CloseHandle(m_hDemoProcesss);
        }
        else if (m_iTerminateDemoWay==3)
        {
            WaitForSingleObject(m_hDemoProcesss,INFINITE);
            GetExitCodeProcess(m_hDemoProcesss,&dwExitCode);
            CloseHandle(m_hDemoProcesss);
        }
        ShowWindow(m_pTestManager->GetMainWnd(),SW_SHOW);

        nRet=pConnect->SendAPacket(m_uiCmd,0,4,(BYTE *)&dwExitCode);

    }
   

    return nRet>0?TEST_NORMAL:TEST_CONNECTDOWN;
}

BOOL CUseAppTest::__EnumWindowProc(HWND hwnd, LPARAM lParam)
{
    CUseAppTest         *pTest = (CUseAppTest *)lParam;
    WCHAR   str[512];
    GetWindowText(hwnd,str,511);
    if (pTest->m_wstrDemoWindowtitle == str)
    {
        DWORD       dwProcessID=0;
        GetWindowThreadProcessId(hwnd,&dwProcessID);
        if (dwProcessID == pTest->m_dwDemoProcessID)
        {
            pTest->m_hDemoMainWnd = hwnd;
            return FALSE;
        }
    }

    return TRUE;
}