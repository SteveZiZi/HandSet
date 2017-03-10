// ShowChartDlg.cpp : 实现文件
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "HistogramDlg.h"

// CHistogramDlg 对话框

IMPLEMENT_DYNAMIC(CHistogramDlg, CDialog)

CHistogramDlg::CHistogramDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistogramDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CHistogramDlg::~CHistogramDlg()
{
	
}

void CHistogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CHistogramDlg, CDialog)
	ON_BN_CLICKED(IDC_EXIT, &CHistogramDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_RESET, &CHistogramDlg::OnBnClickedReset)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CHistogramDlg 消息处理程序

BOOL CHistogramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 350,
		250), this, IDC_HISTOGRAM);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);
#ifdef WINCE
	Init();
#endif

	SetTimer(1,200,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CHistogramDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	OnOK();
}

void CHistogramDlg::OnBnClickedReset()
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

void CHistogramDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
#ifndef WINCE
	if ( m_bReset )
	{
		Reset();
		m_bReset = ~m_bReset;
	}
	else
	{
		Init();
	}
#endif
	Draw();
	CDialog::OnTimer(nIDEvent);
}

BOOL CHistogramDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( pMsg->message == WM_KEYUP )
	{
		// 			CString str;
		// 			str.Format(_T("%d\r\n"),pMsg->time);
		// 			OutputDebugString(str);
		if ( pMsg->wParam == VK_LEFT )
			m_ctrl.CursorMoveLeft();
		if ( pMsg->wParam == VK_RIGHT )
			m_ctrl.CursorMoveRight();
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CHistogramDlg::Init()
{
	// 初始化柱状图
	
	m_ctrl.RemoveASpecialPillar(0);
	m_ctrl.AddASpecialPillar(_T("THD"));
	m_ctrl.SetHorGridVisible(FALSE);
	m_ctrl.SetVerGridVisible(TRUE);
//	m_ctrl.SetFeaturePillar(3,17);
	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
}

void CHistogramDlg::Draw()
{
	float data[50];
	float phase[50];
	float volOrCur[50];
	float thd[50];
	float hz = 50;
	for ( int i = 0; i<sizeof(data)/sizeof(float); i++ )
	{
// 		data[i] = (float)(20-i*2);
// 		thd[i] = 12.0f;
		data[i] = (float)(110.0*rand() / (RAND_MAX + 1.0));
		thd[i] = 200.0f;
//		thd[i] = (120.0*rand() / (RAND_MAX + 1.0));
// 		phase[i] = (10.0*rand() / (RAND_MAX + 1.0));
// 		volOrCur[i] = (10.0*rand() / (RAND_MAX + 1.0));
	}

	int curPos  = m_ctrl.GetCursorPos();
	int spcCout = m_ctrl.GetSpecialPillarCount();
	BOOL isVol  = FALSE;
	CString str1,str2;
	if ( curPos < spcCout )
		str1.Format(L"THD:%0.2f",thd[curPos]);
	else
		str1.Format(L"[%d]HPer:%0.2f%% Harm:%0.2f%s HAng:%0.2f°Freq:%0.2fHz",
		curPos,data[curPos-spcCout],volOrCur[curPos-spcCout],
		(FALSE == isVol) ? _T("V") : _T("A"),phase[curPos-spcCout],hz);

	if ( curPos < spcCout )
		str2.Format(L"THD:%0.2f",thd[curPos]);
	else
		str2.Format(L"[%d]HPer:%0.2f%% Harm:%0.2f%s",
		curPos,data[curPos-spcCout],volOrCur[curPos-spcCout],
		(FALSE == isVol) ? _T("V") : _T("A"));

	DWORD startTm = GetTickCount();
	m_ctrl.ErasePillar();
	m_ctrl.DrawHistogram(0,data,thd,str1);
	m_ctrl.DrawHistogram(1,data,thd,str2);
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CHistogramDlg::Reset()
{
	m_ctrl.RemoveASpecialPillar(0);
	m_ctrl.AddASpecialPillar(_T("THD"));
	m_ctrl.AddASpecialPillar(_T("DC"));
// 	m_ctrl.AddASpecialPillar(_T("New"));
//	m_ctrl.SetFeaturePillar(3,(50*rand()/(RAND_MAX+1.0)));
//		m_ctrl.SetCursorVisible(FALSE);
	m_ctrl.SetCommonPillarCount(30);
	m_ctrl.SetGramCount(1,2);
	m_ctrl.SetTitleVisible(TRUE);
	m_ctrl.SetHorGridVisible(TRUE);
	m_ctrl.SetVerGridVisible(FALSE);
	m_ctrl.SetTitle(1,_T("柱状图"));
	m_ctrl.SetScaleInterval(1,20,20,_T("%"));
	m_ctrl.SetPillarColor(1,RGB(255,165,0));
	m_ctrl.SetCursorColor(1,RGB(220,20,60));
	m_ctrl.Init();
}
