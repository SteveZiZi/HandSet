// ReliabilityTestNoEth.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include "ReliabilityTestNoEth.h"
#include "ReliabilityTestNoEthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReliabilityTestNoEthApp

BEGIN_MESSAGE_MAP(CReliabilityTestNoEthApp, CWinApp)
END_MESSAGE_MAP()


// CReliabilityTestNoEthApp construction
CReliabilityTestNoEthApp::CReliabilityTestNoEthApp()
	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CReliabilityTestNoEthApp object
CReliabilityTestNoEthApp theApp;

// CReliabilityTestNoEthApp initialization

BOOL CReliabilityTestNoEthApp::InitInstance()
{
    HANDLE hNotifyEvent=CreateEvent(NULL,FALSE,FALSE,_T("Test_Event") );
    if (hNotifyEvent !=NULL && GetLastError() ==ERROR_ALREADY_EXISTS)
    {
        //不允许同时运行测试软件的多个实例
        CloseHandle(hNotifyEvent);
        return FALSE;
    }

    if (hNotifyEvent ==NULL)
    {
        return FALSE;
    }

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
	CReliabilityTestNoEthDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	CloseHandle(hNotifyEvent);
    hNotifyEvent=NULL;
	return FALSE;
}
