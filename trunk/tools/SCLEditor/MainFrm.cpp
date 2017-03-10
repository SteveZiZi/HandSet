// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SCLEditor.h"

#include "MainFrm.h"

// CStrManager
#include "ResourceStr.h"
#include "base/StrManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_UPDATE_COMMAND_UI(ID_SEPARATOR, OnUpdateStatusBarText)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT uHideCmds[] =
{
	ID_FILE_PRINT,
	ID_FILE_PRINT_PREVIEW,
};

static UINT s_toolbarCmds[] =
{
	ID_SCL_IMPORT,
	ID_SCX_EXPORT,
	ID_SCX_SAVE_AS,
	ID_SMV_VIEW,
	ID_GOOSE_VIEW,
	ID_FIND_IED
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	

	// Initialize the command bars
	if (!InitCommandBars())
		return -1;

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		TRACE0("Failed to create command bars object.\n");
		return -1;      // fail to create
	}

	// Create ToolBar
	CXTPToolBar* pToolBar = (CXTPToolBar*)
		pCommandBars->Add(_T("Standard"), xtpBarTop);
	if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
// 
// 	// ���ù�������ťͼ��
// 	pCommandBars->GetImageManager()->RemoveAll();
// 	pCommandBars->GetImageManager()->SetMaskColor(RGB(255,255,255));
// 	pCommandBars->GetImageManager()->SetIcons(IDB_TOOL_BAR,s_toolbarCmds,
// 		sizeof(s_toolbarCmds)/sizeof(UINT), CSize(32, 32));
// 
// 	// ���ù�������ť��ʾ��Ϣ
// 	CXTPControls * pToolBarCtrls = pToolBar->GetControls();
// 	for ( int i = 0; i < sizeof(s_toolbarCmds)/sizeof(UINT); i++ )
// 	{
// 		pToolBarCtrls->GetAt(i)->SetTooltip(CStrManager::Get(RCSTR_TOOLTIP_SCL_OPEN+i));
// 	}
// 	// ���ܹرհ�ť
// 	pToolBar->SetCloseable(FALSE);
// 
// 	// ������չ��ť
// 	pToolBar->ShowExpandButton(FALSE);

	// Create ToolBar
	// ���ܹرհ�ť
	pToolBar->SetCloseable(FALSE);

	// ������չ��ť
	pToolBar->ShowExpandButton(FALSE);

	// �򹤾�����Ӱ�ť
	CreateToolBarButtons(pToolBar);

/****************************************************************************************
	// Set Office 2003 Theme
	CXTPPaintManager::SetTheme(xtpThemeOffice2003);
****************************************************************************************/
	//�޸�ToolBar�����ʽΪ   xtpThemeVisualStudio2008
	CXTPPaintManager::SetTheme( xtpThemeVisualStudio2008);
/*************************************************************************************/

	// Hide array of commands
	pCommandBars->HideCommands(uHideCmds, _countof(uHideCmds));

	// Set "Always Show Full Menus" option to the FALSE
	pCommandBars->GetCommandBarsOptions()->bAlwaysShowFullMenus = FALSE;

	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);

	// Load the previous state for toolbars and menus.
//	LoadCommandBars(_T("CommandBars"));

	// Initialize the docking pane manager and set the
	// initial them for the docking panes.  Do this only after all
	// control bars objects have been created and docked.
	m_paneManager.InstallDockingPanes(this);
	// Set Office 2003 Theme
	m_paneManager.SetTheme(xtpPaneThemeOffice2003);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	cs.hMenu = NULL;
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnClose()
{

	// Save the current state for toolbars and menus.
	SaveCommandBars(_T("CommandBars"));

	// Save the current state for docking panes.
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.Save(_T("NormalLayout"));

	CFrameWnd::OnClose();
}


void CMainFrame::OnCustomize()
{
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars != NULL)
	{
		// Instanciate the customize dialog object.
		CXTPCustomizeSheet dlg(pCommandBars);

		// Add the keyboard page to the customize dialog.
		CXTPCustomizeKeyboardPage pageKeyboard(&dlg);
		dlg.AddPage(&pageKeyboard);
		pageKeyboard.AddCategories(IDR_MAINFRAME);

		// Add the options page to the customize dialog.
		CXTPCustomizeOptionsPage pageOptions(&dlg);
		dlg.AddPage(&pageOptions);

		// Add the commands page to the customize dialog.
		CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
		pCommands->AddCategories(IDR_MAINFRAME);

		// Use the command bar manager to initialize the
		// customize dialog.
		pCommands->InsertAllCommandsCategory();
		pCommands->InsertBuiltInMenus(IDR_MAINFRAME);
		pCommands->InsertNewMenuCategory();

		// Dispaly the dialog.
		dlg.DoModal();
	}
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case IDR_PANE_PROPERTIES:
				{
					if (m_wndProperties.GetSafeHwnd() == 0)
					{
						m_wndProperties.Create(WS_CHILD|
							ES_AUTOVSCROLL|ES_MULTILINE,
							CRect(0, 0, 0, 0), this, 0);
					}
					pPane->Attach(&m_wndProperties);
					break;
				}
			case IDR_PANE_OPTIONS:
				{
					if (m_wndOptions.GetSafeHwnd() == 0)
					{
						m_wndOptions.Create(_T("\n\nOptions"),
							WS_CHILD|WS_CLIPCHILDREN|
							WS_CLIPSIBLINGS|SS_CENTER,
							CRect(0, 0, 0, 0), this, 0);
					}
					pPane->Attach(&m_wndOptions);
					break;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CMainFrame::OnUpdateStatusBarText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(); 
}

void CMainFrame::SetStatusBarText(UINT uStrID)
{
	CString str = CStrManager::Get(uStrID);
	m_wndStatusBar.SetPaneText(0,str);
}

///
/// @brief
///     ��������ӵ�����ť
///
BOOL CMainFrame::AddButton(CRuntimeClass *pRuntimeClass, CXTPControls *pToolBarCtrls, int nID, CString szPath, CString szTips, BOOL bBeginGroup /* = FALSE */)
{
	// ��ð�ťͼ��
	HICON hIco = (HICON)LoadImage(NULL, szPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if (hIco == NULL)
	{
		hIco = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	}

	// ������ť
	CXTPControl * pControl = (CXTPControl *)pRuntimeClass->CreateObject();
	if (pControl && hIco)
	{
		// ���ð�ťID��
		pControl->SetID(nID);

		// ���ð�ťͼ��
		pControl->SetCustomIcon(hIco);

		// ���ð�ťtips
		pControl->SetTooltip(szTips);
		pControl->SetPrompt(szTips);

		// �����Ƿ�ʹ�÷ָ�����ʾ��ť��Ŀ�ʼ
		pControl->SetBeginGroup(bBeginGroup);

		// Ĭ��SMV���ƿ���Ϣ��ʾͼ��Ϊѡ��״̬
		if ( nID == s_toolbarCmds[3] )
		{
			pControl->SetChecked(TRUE);
		}

		// ��Ӱ�ť
		pToolBarCtrls->Add(pControl);
	}

	return TRUE;
}

///
/// @brief
///     �򹤾�����Ӱ�ť
///
BOOL CMainFrame::CreateToolBarButtons(CXTPToolBar *pToolBar)
{
	// ��������ʼ����Ҫ��һ����ť
	int nCount = pToolBar->GetControlCount();
	if (nCount < 1)
	{
		return FALSE;
	}

	// ����ͼ���С
	pToolBar->SetIconSize(CSize(32, 32));
	pToolBar->SetButtonSize(CSize(40,40));

	CXTPControls * pToolBarCtrls = pToolBar->GetControls();
	CXTPControl * pFirstCtrl = pToolBar->GetControl(0);
	CRuntimeClass * pRuntimeClass = pFirstCtrl->GetRuntimeClass();

	// �򹤾�����Ӱ�ť��ע��IDI_ICON_BASE~IDI_ICON_BASE+4��Ҫ�������ؼ���ID�ظ�����Ȼ�����ڴ�й¶
	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[0], _T("res\\scl.ico"),
		      CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT));
/*************************************************************************************************************/
	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[2], _T("res\\save.ico"),
		      CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+2));

//	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[1], _T("res\\quick_save.ico"),
//		      CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+1));


	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[3], _T("res\\smv.ico"),
		      CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+3), TRUE);

	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[4], _T("res\\goose.ico"),
		CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+4));

	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[5], _T("res\\find.ico"),
		CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+5), TRUE);

	AddButton(pRuntimeClass, pToolBarCtrls, s_toolbarCmds[1], _T("res\\quick_save.ico"),
		      CStrManager::Get(RCSTR_TOOLTIP_SCL_IMPORT+1));

/***********************************************************************************************************/


	// �Ƴ���һ����ť
	if (pToolBarCtrls->GetCount() > 1)
	{
		pToolBarCtrls->Remove(0);
		pToolBarCtrls->Remove(0);
		pToolBarCtrls->Remove(0);
		pToolBarCtrls->Remove(0);
		pToolBarCtrls->Remove(0);
		pToolBarCtrls->Remove(0);
	}

	pToolBar->ShowTextBelowIcons(TRUE);

	return TRUE;
}
