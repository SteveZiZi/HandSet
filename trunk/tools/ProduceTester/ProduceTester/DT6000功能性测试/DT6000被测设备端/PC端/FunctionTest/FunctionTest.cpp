// FunctionTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include "FunctionTest.h"
#include "FunctionTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFunctionTestApp

BEGIN_MESSAGE_MAP(CFunctionTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFunctionTestApp construction

CFunctionTestApp::CFunctionTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFunctionTestApp object

CFunctionTestApp theApp;


// CFunctionTestApp initialization

BOOL CFunctionTestApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();	
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    //只允许同时只能有一个测试软件运行
    HANDLE hNotifyEvent=CreateEvent(NULL,FALSE,FALSE,L"Test_Event");
    if (hNotifyEvent !=NULL &&GetLastError()==ERROR_ALREADY_EXISTS )
    {
        return FALSE;
    }


    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA data;

    int err;
    err = WSAStartup(wVersionRequested, &data);
    if ( err != 0 )
    {
        AfxMessageBox(_T("初始化网络使用环境失败!"));
        return 0;
    }

	CFunctionTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

    CloseHandle(hNotifyEvent);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
