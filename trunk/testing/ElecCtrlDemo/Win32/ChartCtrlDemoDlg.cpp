// ChartCtrlDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChartCtrlDemo.h"
#include "ChartCtrlDemoDlg.h"

#include "../dlg/HistogramDlg.h"
#include "../dlg/ClockDlg.h"
#include "../dlg/HorHistoDlg.h"
#include "../dlg/OnOffStateDlg.h"
#include "../dlg/PhasorGramDlg.h"
#include "../dlg/PolarityDialDlg.h"
#include "../dlg/WaveFormDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChartCtrlDemoDlg 对话框




CChartCtrlDemoDlg::CChartCtrlDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChartCtrlDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CChartCtrlDemoDlg::~CChartCtrlDemoDlg()
{
	
}

void CChartCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChartCtrlDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HISTOGRAM_CREATE, &CChartCtrlDemoDlg::OnBnClickedHistogramCreate)
	ON_BN_CLICKED(IDC_WAVE_CREATE, &CChartCtrlDemoDlg::OnBnClickedWaveCreate)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PHASOR_CREATE, &CChartCtrlDemoDlg::OnBnClickedPhasorCreate)
//	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_POLARITY_CREATE, &CChartCtrlDemoDlg::OnBnClickedPolarityCreate)
	ON_BN_CLICKED(IDC_HORHISTO_CREATE, &CChartCtrlDemoDlg::OnBnClickedHorhistoCreate)
	ON_BN_CLICKED(IDC_CLOCK_CREATE, &CChartCtrlDemoDlg::OnBnClickedClockCreate)
	ON_BN_CLICKED(IDC_ONOFF_CREATE, &CChartCtrlDemoDlg::OnBnClickedOnoffCreate)
END_MESSAGE_MAP()


// CChartCtrlDemoDlg 消息处理程序

BOOL CChartCtrlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChartCtrlDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CChartCtrlDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChartCtrlDemoDlg::OnBnClickedHistogramCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CHistogramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedWaveCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaveFormDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedPhasorCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CPhasorGramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedPolarityCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CPolarityDialDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedHorhistoCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CHorHistoDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedClockCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	CClockDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedOnoffCreate()
{
	// TODO: 在此添加控件通知处理程序代码
	COnOffStateDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
}
