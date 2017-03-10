// MainSendDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainSendDlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resource.h"

// CMainSendDlg dialog

#define	ID_MAIN_SEND_RET	1121

IMPLEMENT_DYNAMIC(CMainSendDlg, CBaseDlg)

CMainSendDlg::CMainSendDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CMainSendDlg::IDD, DSM_STR_MANUAL_TEST_CAPTION ,pParent)
	, m_flag(FALSE)
{
	m_nMenuID = IDR_MENU_MAIN_SEND;
}

CMainSendDlg::~CMainSendDlg()
{
}

void CMainSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainSendDlg, CBaseDlg)
	ON_COMMAND(ID_MENU_MANUAL_SEND_F1, &CMainSendDlg::OnMenuF1)
	ON_COMMAND(ID_MENU_MANUAL_SEND_F2, &CMainSendDlg::OnMenuF2)
	ON_COMMAND(ID_MENU_MANUAL_SEND_F3, &CMainSendDlg::OnMenuF3)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMainSendDlg message handlers

BOOL CMainSendDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	_showWelcome();



	return FALSE;
}


void CMainSendDlg::OnMenuF1()
{
	CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_GOOSE_SEND_DLG);
}


void CMainSendDlg::OnMenuF2()
{
	CEventProc::GetInstance()->ShowPage(IDD_STATE_SEQUENCE_DLG);
}

void CMainSendDlg::OnMenuF3()
{
	CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DLG);
}

void CMainSendDlg::_createWelcome()
{
	CRect rcReport;
	GetClientRect(rcReport);
#if SHENG_ZE
	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlWelcome.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, ID_MAIN_SEND_RET);

	// 设置字体
	m_ctlWelcome.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlWelcome.PostInitTemplateStyle();

	m_ctlWelcome.InsertColumn(0, L"", EL_CENTER, 410);   // 标题


	m_ctlWelcome.InsertRecordRow(0, L"Welcome to Assisted Testing.");

	// 设置选择焦点样式
	m_ctlWelcome.SetRowFocusEnable(FALSE);
	m_ctlWelcome.SetRowSelectionVisible(FALSE);
#else
	// 加载手动试验欢迎图片
	/*CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rcMenu(CRect(0, 0, 640, 444));
	dcMem.SelectObject(dsm_bmpmgr::Get(DSM_BMP_MAIN_WELCOME));
	dc.BitBlt(rcMenu.left, rcMenu.top, rcMenu.Width(), rcMenu.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.DeleteDC();*/
#endif

	//设置标题
	CString		csCaption = L"报文发送";
	CStatusMgr::SetTitle(csCaption);
}

void CMainSendDlg::_showWelcome()
{
#if SHENG_ZE
	if (!m_ctlWelcome.GetSafeHwnd())
	{
		_createWelcome();
	}

	m_ctlWelcome.ShowWindow(SW_SHOW);
	m_ctlWelcome.SetFocus();
#else
	if (!m_flag)
	{
		_createWelcome();
		SetFocus();
		m_flag = TRUE;
	}
	else
	{
		ShowWindow(SW_SHOW);
		SetFocus();
	}
#endif
}


#if !SHENG_ZE
// 解决锁屏的时候卡屏的问题
void CMainSendDlg::OnPaint()
{
	CRect rcReport;
	GetClientRect(rcReport);
	CPaintDC dc(this);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CRect rcMenu(CRect(0, 0, 640, 444));
	dcMem.SelectObject(dsm_bmpmgr::Get(DSM_BMP_MAIN_WELCOME));
	dc.BitBlt(rcMenu.left, rcMenu.top, rcMenu.Width(), rcMenu.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.DeleteDC();
	//设置标题
	CString		csCaption = L"报文发送";
	CStatusMgr::SetTitle(csCaption);
}
#endif