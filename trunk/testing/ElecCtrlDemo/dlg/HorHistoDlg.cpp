// HorHistoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "HorHistoDlg.h"


// CHorHistoDlg �Ի���

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


// CHorHistoDlg ��Ϣ�������
BOOL CHorHistoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_pillarBmp.LoadBitmap(IDB_PILLAR_1);

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 350,
		250), this, IDC_HORHISTO);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif

	SetTimer(1,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CHorHistoDlg::OnBnClickedReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
	OnOK();
}

void CHorHistoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// ��ʼ����ɢ��״ͼ

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
	// ��ʼ����ɢ��״ͼ
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
