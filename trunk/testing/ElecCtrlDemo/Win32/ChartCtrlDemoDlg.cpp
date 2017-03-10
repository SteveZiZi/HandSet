// ChartCtrlDemoDlg.cpp : ʵ���ļ�
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


// CChartCtrlDemoDlg �Ի���




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


// CChartCtrlDemoDlg ��Ϣ�������

BOOL CChartCtrlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChartCtrlDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CChartCtrlDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChartCtrlDemoDlg::OnBnClickedHistogramCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CHistogramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedWaveCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWaveFormDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedPhasorCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPhasorGramDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedPolarityCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPolarityDialDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedHorhistoCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CHorHistoDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedClockCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CClockDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnBnClickedOnoffCreate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	COnOffStateDlg dlg;
	dlg.DoModal();
}

void CChartCtrlDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnTimer(nIDEvent);
}
