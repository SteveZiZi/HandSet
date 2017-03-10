// FunctionTestCE.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FunctionTestCE.h"
#include <WinSock.h>
#include "FunctionTestCEDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFunctionTestCEApp

BEGIN_MESSAGE_MAP(CFunctionTestCEApp, CWinApp)
END_MESSAGE_MAP()


// CFunctionTestCEApp construction
CFunctionTestCEApp::CFunctionTestCEApp()
	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFunctionTestCEApp object
CFunctionTestCEApp theApp;

// CFunctionTestCEApp initialization

BOOL CFunctionTestCEApp::InitInstance()
{

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA data;

    int err;
    err = WSAStartup(wVersionRequested, &data);
    if ( err != 0 )
    {
        AfxMessageBox(_T("初始化网络使用环境失败!"));
        return 0;
    }

    //只允许同时只能有一个测试软件运行
    HANDLE hNotifyEvent=CreateEvent(NULL,FALSE,FALSE,L"Test_Event");
    if (hNotifyEvent !=NULL &&GetLastError()==ERROR_ALREADY_EXISTS )
    {
        return FALSE;
    }


	CFunctionTestCEDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CFunctionTestCEApp::ExitInstance()
{
    WSACleanup();
    return CWinApp::ExitInstance();
}
