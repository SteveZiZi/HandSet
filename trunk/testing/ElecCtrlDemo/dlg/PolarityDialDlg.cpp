// PolarityDialDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "PolarityDialDlg.h"


// CPolarityDialDlg �Ի���

IMPLEMENT_DYNAMIC(CPolarityDialDlg, CDialog)

CPolarityDialDlg::CPolarityDialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolarityDialDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CPolarityDialDlg::~CPolarityDialDlg()
{
	m_HandBmp.DeleteObject();
}

void CPolarityDialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CPolarityDialDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &CPolarityDialDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &CPolarityDialDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPolarityDialDlg ��Ϣ�������

BOOL CPolarityDialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_HandBmp.LoadBitmap(IDB_HAND);

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0 , 0 , 267, 102*3), this, IDC_POLARITY_DIAL);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif


	SetTimer(1,200,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPolarityDialDlg::OnBnClickedReset()
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

void CPolarityDialDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
	OnOK();
}

void CPolarityDialDlg::OnTimer(UINT_PTR nIDEvent)
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

void CPolarityDialDlg::Init()
{
	// ��ʼ�����Ա���
	CBitmap bmp;
	bmp.LoadBitmap(IDB_POLARITY_DIAL);

	m_ctrl.SetBmpTransColor(RGB(255,255,255));
	m_ctrl.SetDialCount(1,3);
//	m_ctrl.SetBkgndBmp(&bmp);
	m_ctrl.SetBkgndColor(RGB(255,255,255));
	m_ctrl.SetDialDegree(57.0f,123.0f);
	m_ctrl.SetDialInitState(50.0f);
	m_ctrl.SetStdDialCircleCentre(CPoint(134,220));
	m_ctrl.SetStdDialInRadii(137.0f);
	m_ctrl.SetStdDialHandRadii(176.0f);
	m_ctrl.SetStdDialRect(CRect(0,0,267,120));
	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
	bmp.DeleteObject();
}

void CPolarityDialDlg::Draw()
{
	DWORD startTm = GetTickCount();
	float percentVal = (float)(100.0*rand() / (RAND_MAX + 1.0));
	m_ctrl.EraseHand();
	m_ctrl.DrawHand(0,0);
	m_ctrl.DrawHand(1,100);
 	m_ctrl.DrawHand(2,50);
// 	m_ctrl.DrawHand(3,percentVal);
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CPolarityDialDlg::Reset()
{
	m_ctrl.SetDialCount(1,2);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_POLARITY_DIAL);
//	m_ctrl.MoveWindow(CRect(0,0,100,100));
	//		m_ctrl.SetBkgndColor(RGB(225,111,0));
	m_ctrl.SetBkgndColor(RGB(255,0,0));
	m_ctrl.SetBkgndBmp(&bmp,TRUE);
	m_ctrl.SetHandBmp(&m_HandBmp);
	m_ctrl.Init();
	bmp.DeleteObject();
}
