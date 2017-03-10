// WaveFormDlg.cpp : 实现文件
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "WaveFormDlg.h"
#include <math.h>


// CWaveFormDlg 对话框

IMPLEMENT_DYNAMIC(CWaveFormDlg, CDialog)

CWaveFormDlg::CWaveFormDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaveFormDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CWaveFormDlg::~CWaveFormDlg()
{
	
}

void CWaveFormDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CWaveFormDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &CWaveFormDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &CWaveFormDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaveFormDlg 消息处理程序

BOOL CWaveFormDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 350,
		250), this, IDC_OSCILLOGRAM);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif

	SetTimer(1,200,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CWaveFormDlg::OnBnClickedReset()
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

void CWaveFormDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	OnOK();
}

void CWaveFormDlg::OnTimer(UINT_PTR nIDEvent)
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
	m_bReset = !m_bReset;
#endif
	Draw();
	CDialog::OnTimer(nIDEvent);
}

void CWaveFormDlg::Init()
{
	// 初始化波形图

	m_ctrl.SetTitle(0,_T("50Hz Wave"));
	m_ctrl.SetTitleVisible();
	m_ctrl.SetXScaleValVisible();
	// 	m_ctrl.SetMargin(10,10,10,10);
	// 	m_ctrl.SetScaleInterval(20,10);
	m_ctrl.SetWaveAreaGrid(3,2);
	m_ctrl.SetHorGridVisible(TRUE);
	m_ctrl.SetVerGridVisible(FALSE);
// 	m_ctrl.SetWaveDescColor(0,0,RGB(34,139,34));
// 	m_ctrl.SetWaveDescColor(1,0,RGB(145,124,224));
// 	m_ctrl.SetWaveDescColor(2,0,RGB(220,20,60));
	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
}

void CWaveFormDlg::Draw()
{
	float rms = 8.01f;
	int count = 0;
	static int sinPos = 0;
	float f = 0.0f;
	while ( count < sizeof(data1)/sizeof(float) )
	{
		data1[count] = 1.0f/f/*(float)(200000000*sin(2*3.1415*sinPos/40))*/;
		data2[count] = (float)(10*sin(2*3.1415*sinPos/40));
		data3[count] = (float)(5*sin((float)2*3.1415*sinPos/40));
		count++;
		sinPos++;
	}
	if ( sinPos % 40 == 0 )
	{
		sinPos = 0;
	}

	DWORD startTm = GetTickCount();
	m_ctrl.EraseWave();
	m_ctrl.DrawWave(0,data1,count,_T("RMS:90.1V"));
	m_ctrl.DrawWave(1,data2,count,_T("RMS:90.1V"));
	m_ctrl.DrawWave(2,data3,count,_T("RMS:90.1V"));

	CString desc1,desc2;
	size_t pos1 = m_ctrl.GetCursor1Pos(0);
	size_t pos2 = m_ctrl.GetCursor2Pos(0);
	desc1.Format(_T("T1-接收时间：[1970-01-01] 采样时间：[1970-01-01] 当前点：%d 点差： %d"),
		pos1+1,pos2-pos1);
	desc2.Format(_T("T2-接收时间：[1970-01-01] 采样时间：[1970-01-01] 当前点：%d 时间差： %d 平均时间差： %d"),
		pos2+1,(pos2-pos1)*250,250);
	m_ctrl.DrawCursorDesc(desc1,desc2);

	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CWaveFormDlg::Reset()
{
	m_ctrl.SetTitle(2,_T("60Hz Wave"));
	//		m_ctrl.SetTitleVisible(FALSE);
	m_ctrl.SetWaveDrawCount(50);
	m_ctrl.SetCursor1Visible();
	m_ctrl.SetCursor2Visible();
	m_ctrl.SetXScaleValVisible(FALSE);
	m_ctrl.SetXScaleValVisible(TRUE);
	m_ctrl.SetMargin(4,10,4,4);
	m_ctrl.SetGramCount(1,3);
	m_ctrl.SetScaleInterval(0,80,400,_T("us"));
	//m_ctrl.SetScaleInterval(2,25.5,30);
	m_ctrl.SetWaveAreaGrid(3,2);
	m_ctrl.SetHorGridVisible(FALSE);
	m_ctrl.SetVerGridVisible(TRUE);
// 	m_ctrl.SetWaveDescColor(0,0,RGB(220,20,60));
// 	m_ctrl.SetWaveDescColor(1,0,RGB(255,165,0));
// 	m_ctrl.SetWaveDescColor(2,0,RGB(124,252,0));
	m_ctrl.Init();
}

BOOL CWaveFormDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( pMsg->message == WM_KEYUP )
	{
// 		CString str;
// 		str.Format(_T("%d\r\n"),pMsg->time);
// 		OutputDebugString(str);
		if ( pMsg->wParam == VK_LEFT )
			m_ctrl.Cursor2MoveLeft();
		if ( pMsg->wParam == VK_RIGHT )
			m_ctrl.Cursor2MoveRight();
	}
	return CDialog::PreTranslateMessage(pMsg);
}
