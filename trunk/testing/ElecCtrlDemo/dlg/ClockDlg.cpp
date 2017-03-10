// ClockDlg.cpp : 实现文件
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "ClockDlg.h"


// CClockDlg 对话框

IMPLEMENT_DYNAMIC(CClockDlg, CDialog)

CClockDlg::CClockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClockDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CClockDlg::~CClockDlg()
{
	if ( m_ClockDialBmp.GetSafeHandle() != NULL)
	{
		m_ClockDialBmp.DeleteObject();
	}
	if ( m_HourHandBmp.GetSafeHandle() != NULL)
	{
		m_HourHandBmp.DeleteObject();
	}
	if ( m_MinHandBmp.GetSafeHandle() != NULL)
	{
		m_MinHandBmp.DeleteObject();
	}
	if ( m_SecHandBmp.GetSafeHandle() != NULL)
	{
		m_SecHandBmp.DeleteObject();
	}
}

void CClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CClockDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &CClockDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &CClockDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CClockDlg 消息处理程序

void CClockDlg::OnBnClickedReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bReset = ~m_bReset;
#ifdef WINCE
	if ( m_bReset )
	{
		Reset();
	}
	else
	{
		Init();
	}
#endif
}

void CClockDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	OnOK();
}

void CClockDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
#ifndef WINCE
	if ( m_bReset )
	{
		Reset();
	}
	else
	{
		Init();
	}
#endif
	Draw();
	CDialog::OnTimer(nIDEvent);
}

void CClockDlg::Init()
{
	// 初始化对时时钟

	m_ctrl.SetBmpTransColor(RGB(255,255,255));
	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
}

void CClockDlg::Draw()
{
	DWORD startTm = GetTickCount();
	m_ctrl.EraseTime();
	CTime tm = CTime::GetCurrentTime();
	m_ctrl.DrawTime(tm.GetHour(),tm.GetMinute(),tm.GetSecond());
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CClockDlg::Reset()
{
	m_ctrl.SetDialBmp(&m_ClockDialBmp);
	m_ctrl.SetHandBmp(&m_HourHandBmp,&m_MinHandBmp,&m_SecHandBmp);
	m_ctrl.Init();
}

BOOL CClockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ClockDialBmp.LoadBitmap(IDB_CLOCK);
	m_HourHandBmp.LoadBitmap(IDB_HOUR_HAND);
	m_MinHandBmp.LoadBitmap(IDB_MIN_HAND);
	m_SecHandBmp.LoadBitmap(IDB_SEC_HAND);

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE|SS_CENTER, 
		CRect(0,0,350,350), this,IDC_TIME_TICK_COLOK);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif

	SetTimer(1,900,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
