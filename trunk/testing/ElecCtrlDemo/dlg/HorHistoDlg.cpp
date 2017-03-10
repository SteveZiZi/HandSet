// HorHistoDlg.cpp : 实现文件
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "HorHistoDlg.h"


// CHorHistoDlg 对话框

IMPLEMENT_DYNAMIC(CHorHistoDlg, CDialog)

CHorHistoDlg::CHorHistoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHorHistoDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CHorHistoDlg::~CHorHistoDlg()
{
	m_pillarBmp.DeleteObject();
}

void CHorHistoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CHorHistoDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &CHorHistoDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &CHorHistoDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CHorHistoDlg 消息处理程序
BOOL CHorHistoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_pillarBmp.LoadBitmap(IDB_PILLAR_1);

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 350,
		250), this, IDC_HORHISTO);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif

	SetTimer(1,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CHorHistoDlg::OnBnClickedReset()
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

void CHorHistoDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	OnOK();
}

void CHorHistoDlg::OnTimer(UINT_PTR nIDEvent)
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

void CHorHistoDlg::Init()
{
	// 初始化离散柱状图

	m_ctrl.SetBmpTransColor(RGB(255,255,255));
	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
}

void CHorHistoDlg::Draw()
{
	float data[10];
	for ( int i = 0; i<sizeof(data)/sizeof(float); i++ )
	{
		data[i] = (float)(10.0*rand() / (RAND_MAX + 1.0));
	}

	DWORD startTm = GetTickCount();
	m_ctrl.ErasePillar();
	m_ctrl.DrawHorHistoGram(data,sizeof(data)/sizeof(float),_T("%"));
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CHorHistoDlg::Reset()
{
	// 初始化离散柱状图
	m_ctrl.SetPillarStyle(0,&m_pillarBmp);
	m_ctrl.SetSpaceFromLeftBorderToPillar(20);
	m_ctrl.SetPillarAreaBorderColor(RGB(255,0,0));
//	m_ctrl.SetYScaleValLen(10);
	m_ctrl.SetBorderVisible(0,TRUE);
	for ( size_t i = 0; i < 10; i++ )
	{
		CString str;
		str.Format(_T("%dus"),i*10*3);
		m_ctrl.SetYScaleVal(i,str);
	}
	m_ctrl.SetPillarBetween(6);
	m_ctrl.SetPillarHeight(30);
//	m_ctrl.SetYScaleValLen(20);
	m_ctrl.Init();
}
