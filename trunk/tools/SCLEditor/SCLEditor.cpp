// SCLEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
// 
// // Visual Leak Detector
// #include <vld.h>

#include <AFXPRIV.H>
#include "SCLEditor.h"
#include "MainFrm.h"

#include "SCLEditorDoc.h"
#include "view/SclSmvView.h"
#include "view/SclGooseView.h"
#include "dlg/FindDlg.h"
#include "dlg/ProgressBarDlg.h"
#include "dlg/QuickSaveDlg.h"

#include "scl/SclManager.h"
#include "ResourceStr.h"
#include "base/StrManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSCLEditorApp

BEGIN_MESSAGE_MAP(CSCLEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSCLEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_SCL_IMPORT, &CSCLEditorApp::OnSCLImport)
	ON_COMMAND(ID_SCX_EXPORT, &CSCLEditorApp::OnSCXQuickSave)
	ON_COMMAND(ID_SCX_SAVE_AS, &CSCLEditorApp::OnSCXSave)
	ON_COMMAND(ID_SMV_VIEW, &CSCLEditorApp::OnSwitchToSmvView)
	ON_COMMAND(ID_GOOSE_VIEW, &CSCLEditorApp::OnSwitchToGooseView)
	ON_COMMAND(ID_FIND_IED, &CSCLEditorApp::OnFindIED)
END_MESSAGE_MAP()


// CSCLEditorApp construction

CSCLEditorApp::CSCLEditorApp()
: m_csLastSCLOpenDir(_T(""))
, m_csLastSCXOpenDir(_T(""))
, m_csLastSaveDir(_T(""))
, m_csDefSaveFileName(_T(""))
, m_eActiveViewType(VIEW_TYPE_SMV)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSCLEditorApp object

CSCLEditorApp theApp;


// CSCLEditorApp initialization

BOOL CSCLEditorApp::InitInstance()
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

	// ��������ID�����ַ�����Դ
	CStrManager::Load(2052);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSCLEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSclSmvView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	CView* pActiveView = ((CFrameWnd*) m_pMainWnd)->GetActiveView();
	m_pSmvView   = pActiveView;
	m_pGooseView = (CView*) new CSclGooseView;

	CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

	// Initialize a CCreateContext to point to the active document.
	// With this context, the new view is added to the document
	// when the view is created in CView::OnCreate().
	CCreateContext newContext;
	newContext.m_pNewViewClass = NULL;
	newContext.m_pNewDocTemplate = NULL;
	newContext.m_pLastView = NULL;
	newContext.m_pCurrentFrame = NULL;
	newContext.m_pCurrentDoc = pCurrentDoc;

	// The ID of the initial active view is AFX_IDW_PANE_FIRST.
	// Incrementing this value by one for additional views works
	// in the standard document/view case but the technique cannot
	// be extended for the CSplitterWnd case.
	UINT viewID = AFX_IDW_PANE_FIRST + 1;
	CRect rect(0, 0, 0, 0); // Gets resized later.

	// Create the new view. In this example, the view persists for
	// the life of the application. The application automatically
	// deletes the view when the application is closed.
	m_pGooseView->Create(NULL, NULL, WS_BORDER|WS_CHILD, rect, m_pMainWnd, viewID, &newContext);

	// When a document template creates a view, the WM_INITIALUPDATE
	// message is sent automatically. However, this code must
	// explicitly send the message, as follows.
	m_pGooseView->SendMessage(WM_INITIALUPDATE, 0, 0);

	// ���������ڱ���
	CString title(_T("�� - SCLEditor"));
	title.Append(CStrManager::Get(RCSTR_SOFTWARE_VERSION));
	AfxGetApp()->GetMainWnd()->SetWindowText(title);

	return TRUE;
}

///
/// @brief
///     ����SCL�ص���������
///
struct _SCLLoadTaskArgs
{
	CWnd    *m_pWnd;           ///< �������Ի���
	CView   *m_pSmvView;       ///< SMV������ͼ
	CView   *m_pGooseView;     ///< GOOSE������ͼ
	CString  csFile;           ///< SCL�ļ�·��
	CString  csFileName;       ///< SCL�ļ���
};

///
/// @brief
///     ����SCL�ص�����
///
DWORD WINAPI _LoadSCLTask(void* arg)
{
	_SCLLoadTaskArgs* pArgs = (_SCLLoadTaskArgs*)arg;
	HWND hWnd       = pArgs->m_pWnd->GetSafeHwnd();
	HWND hSmvView   = pArgs->m_pSmvView->GetSafeHwnd();
	HWND hGooseView = pArgs->m_pGooseView->GetSafeHwnd();

	ASSERT(pArgs);

	// ����scl�ļ�
	int nRet = CSCLManager::getInstance()->Load((LPCTSTR)pArgs->csFile);

	if ( nRet == 0 )
	{
		// ���±���.
		::SendMessage(hGooseView,WM_UPDATE_GOOSE_REPORT,0,0);
		::SendMessage(hSmvView,WM_UPDATE_SMV_REPORT,0,0);

		// ���������ڱ���
		CString title(pArgs->csFileName);
		title.Append(_T(" - SCLEditor"));
		title.Append(CStrManager::Get(RCSTR_SOFTWARE_VERSION));
		AfxGetApp()->GetMainWnd()->SetWindowText(title);
	}

	// �رս������Ի���
	if(pArgs->m_pWnd && pArgs->m_pWnd->GetSafeHwnd())
	{
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	return 1;
}

///
/// @brief
///     ����SCL�ļ�
///
void CSCLEditorApp::OnSCLImport()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		_T("SCL Files (*.scd;*.icd;*.cid;*.ssd;)|*.scd;*.icd;*.cid;*.ssd|SCX Files (*.scx;)|*.scx||"), this->GetMainWnd());
	// �������ѡ���Ŀ¼
	if ( m_csLastSCLOpenDir != _T("") )
	{
		dlg.m_ofn.lpstrInitialDir = m_csLastSCLOpenDir;
	}

	HWND hWnd = ::GetFocus();
	if( IDOK == dlg.DoModal() )
	{
		CString path = dlg.GetPathName();
		// ��¼��Ŀ¼
		m_csLastSCLOpenDir = path.Left(path.GetLength()-dlg.GetFileName().GetLength()-1);

		CProgressBarDlg dlgPro(m_pMainWnd);

		// ����SCL
		_SCLLoadTaskArgs arg;
		arg.csFile       = path;
		arg.csFileName   = dlg.GetFileName();
		arg.m_pWnd       = &dlgPro;
		arg.m_pSmvView   = m_pSmvView;
		arg.m_pGooseView = m_pGooseView;
		dlgPro._SetTask(_LoadSCLTask, &arg);

		// ��ʾLoad�������Ի���
		if(IDOK == dlgPro.DoModal())
		{

		}

		// ����scx�����ļ���Ĭ����
		int nPos = arg.csFileName.Find(_T("."));
		if ( nPos == -1 )
			nPos = arg.csFileName.GetLength();
		m_csDefSaveFileName = arg.csFileName.Left(nPos);
	}
	::SetFocus(hWnd);
}

///
/// @brief
///     ����SCX�ļ�
///
void CSCLEditorApp::OnSCXOpen()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		_T("SCX Files (*.scx;)|*.scx||"), this->GetMainWnd());
	// �������ѡ���Ŀ¼
	if ( m_csLastSCXOpenDir != _T("") )
	{
		dlg.m_ofn.lpstrInitialDir = m_csLastSCXOpenDir;
	}

	HWND hWnd = ::GetFocus();
	if( IDOK == dlg.DoModal() )
	{
		CString path = dlg.GetPathName();
		// ��¼��Ŀ¼
		m_csLastSCXOpenDir = path.Left(path.GetLength()-dlg.GetFileName().GetLength()-1);

		CProgressBarDlg dlgPro(m_pMainWnd);

		// ����SCL
		_SCLLoadTaskArgs arg;
		arg.csFile       = path;
		arg.csFileName   = dlg.GetFileName();
		arg.m_pWnd       = &dlgPro;
		arg.m_pSmvView   = m_pSmvView;
		arg.m_pGooseView = m_pGooseView;
		dlgPro._SetTask(_LoadSCLTask, &arg);

		// ��ʾLoad�������Ի���
		if(IDOK == dlgPro.DoModal())
		{

		}
	}
	::SetFocus(hWnd);
}

///
/// @brief
///     ����SCL�ص���������
///
struct _SCLSaveTaskArgs
{
	CWnd    *m_pWnd;      ///< �������Ի���
	CString  csFile;      ///< SCL�ļ�·��
};

///
/// @brief
///     ����SCL�ص�����
///
DWORD WINAPI _SaveSCLTask(void* arg)
{
	_SCLSaveTaskArgs* pArgs = (_SCLSaveTaskArgs*)arg;
	HWND hWnd       = pArgs->m_pWnd->GetSafeHwnd();

	ASSERT(pArgs);

	// ����scl�ļ�
	int nRet = CSCLManager::getInstance()->Save((LPCTSTR)pArgs->csFile);

	// �ر�Load�������Ի���
	if(pArgs->m_pWnd && pArgs->m_pWnd->GetSafeHwnd())
	{
		::SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	return 1;
}

///
/// @brief
///     ����ΪSCX�ļ�
///
void CSCLEditorApp::OnSCXSave()
{
	CFileDialog dlg(FALSE, _T(".scd"), m_csDefSaveFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		_T("SCX Files (*.scd);|*.scd;||"), this->GetMainWnd());
	// �������ѡ���Ŀ¼
	if ( m_csLastSaveDir != _T("") )
	{
		dlg.m_ofn.lpstrInitialDir = m_csLastSaveDir;
	}

	HWND hWnd = ::GetFocus();
	if( IDOK == dlg.DoModal() )
	{
		CString path = dlg.GetPathName();
		// ��¼����Ŀ¼
		m_csLastSaveDir = path.Left(path.GetLength()-dlg.GetFileName().GetLength()-1);

		CProgressBarDlg dlgPro(m_pMainWnd);

		// ����SCL
		_SCLSaveTaskArgs arg;
		arg.csFile       = path;
		arg.m_pWnd       = &dlgPro;
		dlgPro._SetTask(_SaveSCLTask, &arg);

		// ��ʾ�������Ի���
		dlgPro._SetTips(_T("Saving"));
		if(IDOK == dlgPro.DoModal())
		{

		}
	}
	::SetFocus(hWnd);
}

///
/// @brief
///     SCX�ļ���ݱ���
///
void CSCLEditorApp::OnSCXQuickSave()
{
	/************************************************************************************/
	//�޸� �رմ���
	AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);

	/************************************************************************************
	CQuickSaveDlg dlg;
	dlg.SetDefaultFileName(m_csDefSaveFileName);
	for ( size_t i = 0; i < m_dqFileNameHistory.size(); i++ )
	{
		dlg.AddFileNameHistory(m_dqFileNameHistory[i]);
	}
	HWND hWnd = ::GetFocus();
	if( IDOK == dlg.DoModal() )
	{
		// ��¼�ļ�����ʷ
		BOOL bExist = FALSE;
		for ( size_t i = 0; i < m_dqFileNameHistory.size(); i++ )
		{
			if ( m_dqFileNameHistory[i].Compare(dlg.GetFileName()) == 0 )
				bExist = TRUE;
		}
		if ( bExist == FALSE)
			m_dqFileNameHistory.push_back(dlg.GetFileName());
		if ( m_dqFileNameHistory.size() > 4 )
			m_dqFileNameHistory.pop_front();

		CString path = dlg.GetFilePath();

		CProgressBarDlg dlgPro(m_pMainWnd);

		// ����SCL
		_SCLSaveTaskArgs arg;
		arg.csFile       = path;
		arg.m_pWnd       = &dlgPro;
		dlgPro._SetTask(_SaveSCLTask, &arg);

		// ��ʾ�������Ի���
		dlgPro._SetTips(_T("Saving"));
		if(IDOK == dlgPro.DoModal())
		{

		}
	}
	::SetFocus(hWnd);
	**************************************************************************************/
}

///
/// @brief
///     �л���SMV������ͼ
///
void CSCLEditorApp::OnSwitchToSmvView()
{
	m_pCurView = SwitchView(VIEW_TYPE_SMV);
	m_eActiveViewType = VIEW_TYPE_SMV;

	// ȡ��GOOSE���ƿ���Ϣ��ʾͼ���ѡ��״̬������SMV���ƿ���Ϣ��ʾͼ���ѡ��״̬
	CXTPToolBar *pToolBar = ((CMainFrame *)GetMainWnd())->GetCommandBars()->GetAt(0);
	/*************************************************************************************/
	CXTPControl *pControl = pToolBar->GetControl(2);
	pControl->SetChecked(TRUE);
	pControl = pToolBar->GetControl(3);
	pControl->SetChecked(FALSE);
}

///
/// @brief
///     �л���GOOSE������ͼ
///
void CSCLEditorApp::OnSwitchToGooseView()
{
	m_pCurView = SwitchView(VIEW_TYPE_GOOSE);
	m_eActiveViewType = VIEW_TYPE_GOOSE;

	// ȡ��SMV���ƿ���Ϣ��ʾͼ���ѡ��״̬������GOOSE���ƿ���Ϣ��ʾͼ���ѡ��״̬
	CXTPToolBar *pToolBar = ((CMainFrame *)GetMainWnd())->GetCommandBars()->GetAt(0);
	/****************************************************************************************/
	CXTPControl *pControl = pToolBar->GetControl(2);			
	pControl->SetChecked(FALSE);
	pControl = pToolBar->GetControl(3);
	pControl->SetChecked(TRUE);
}

///
/// @brief
///     ��ͼ�л�
///
CView* CSCLEditorApp::SwitchView(VIEW_TYPE type)
{
	CView* pActiveView =
		((CFrameWnd*) m_pMainWnd)->GetActiveView();

	CView* pNewView = NULL;
	switch (type)
	{
	case VIEW_TYPE_SMV:
		pNewView = m_pSmvView;
		break;
	case VIEW_TYPE_GOOSE:
		pNewView = m_pGooseView;
		break;
	}

	// Exchange view window IDs so RecalcLayout() works.
#ifndef _WIN32
	UINT temp = ::GetWindowWord(pActiveView->m_hWnd, GWW_ID);
	::SetWindowWord(pActiveView->m_hWnd, GWW_ID, ::GetWindowWord(pNewView->m_hWnd, GWW_ID));
	::SetWindowWord(pNewView->m_hWnd, GWW_ID, temp);
#else
	UINT temp = ::GetWindowLong(pActiveView->m_hWnd, GWL_ID);
	::SetWindowLong(pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID));
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, temp);
#endif

	pActiveView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);
	((CFrameWnd*) m_pMainWnd)->SetActiveView(pNewView);
	((CFrameWnd*) m_pMainWnd)->RecalcLayout();
	pNewView->Invalidate();
	
	return pActiveView;
}

///
/// @brief
///     ����IED
///
void CSCLEditorApp::OnFindIED()
{
	CString iedName(_T(""));
	CFindDlg dlg;

	CView* pActiveView =
		((CFrameWnd*) m_pMainWnd)->GetActiveView();
	dlg.SetActiveView(pActiveView,m_eActiveViewType);
	if ( dlg.DoModal() == IDOK )
	{

	}
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CSCLEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSCLEditorApp message handlers

