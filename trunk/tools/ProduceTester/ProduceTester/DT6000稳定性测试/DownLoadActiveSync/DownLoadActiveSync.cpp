// DownLoadActiveSync.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include "DownLoadActiveSync.h"
#include "DownLoadActiveSyncDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDownLoadActiveSyncApp

BEGIN_MESSAGE_MAP(CDownLoadActiveSyncApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDownLoadActiveSyncApp construction

CDownLoadActiveSyncApp::CDownLoadActiveSyncApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDownLoadActiveSyncApp object

CDownLoadActiveSyncApp theApp;


// CDownLoadActiveSyncApp initialization

BOOL CDownLoadActiveSyncApp::InitInstance()
{	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();

	
	CDownLoadActiveSyncDlg dlg;
	m_pMainWnd = &dlg;

    //加载套接字库
    WORD wVersionRequested = MAKEWORD(1,1);
    WSADATA data;

    int err;
    err = WSAStartup(wVersionRequested, &data);
    if ( err != 0 )
    {
        AfxMessageBox(_T("初始化网络使用环境失败!"));
        return 0;
    }

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

	WSACleanup();
	return FALSE;
}
