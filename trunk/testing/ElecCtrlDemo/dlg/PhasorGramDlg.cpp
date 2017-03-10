// PhasorGramDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "PhasorGramDlg.h"


// CPhasorGramDlg �Ի���

IMPLEMENT_DYNAMIC(CPhasorGramDlg, CDialog)

CPhasorGramDlg::CPhasorGramDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPhasorGramDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

CPhasorGramDlg::~CPhasorGramDlg()
{
}

void CPhasorGramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(CPhasorGramDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &CPhasorGramDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &CPhasorGramDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/*#define WINCE*/
// CPhasorGramDlg ��Ϣ�������

BOOL CPhasorGramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 350,
		350), this, IDC_PHASEGRAM);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif


	SetTimer(1,100,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPhasorGramDlg::OnBnClickedReset()
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

void CPhasorGramDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
	OnOK();
}

void CPhasorGramDlg::OnTimer(UINT_PTR nIDEvent)
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

void CPhasorGramDlg::Init()
{
	// ��ʼ������ͼ

	m_ctrl.Init();
	m_ctrl.ShowWindow(TRUE);
}

void CPhasorGramDlg::Draw()
{
	float phasorLen[6];
	float phasorDegree[6];
	for ( int i = 0; i<sizeof(phasorDegree)/sizeof(float); i++ )
	{
		phasorDegree[i] = (float)(90.0*rand() / (RAND_MAX + 1.0) - 180.0*rand() / (RAND_MAX + 1.0));
		phasorLen[i] = (float)(140.0f/**rand() / (RAND_MAX + 1.0)*/);
// 		phasorLen[i] = 10;
// 		phasorDegree[i] = 270+i*10;
	}
	phasorDegree[0] = 0.0f;
//	CString str[6] = {_T("Ua"),_T("Ub"),_T("Uc"),_T("Ia"),_T("Ib"),_T("Ic")};
	CString str[6] = {_T("1"),_T("2"),_T("3"),_T("4"),_T("5"),_T("6")};

	int interVal = 20 - (10*rand() / (RAND_MAX + 1.0));
	DWORD startTm = GetTickCount();
	m_ctrl.SetLenScaleInterval(interVal);
	m_ctrl.ErasePhasor();
	m_ctrl.DrawPhasor(phasorLen,phasorDegree,str,4/*sizeof(phasorDegree)/sizeof(float)*/);
	m_ctrl.DrawExtDesc(CPoint(0,0),RGB(111,111,111),str,2);
	m_ctrl.DrawExtDesc(CPoint(99,0),RGB(200,111,111),str,2);
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);

	m_ctrl.Invalidate();
}

void CPhasorGramDlg::Reset()
{
	CFont font;
	font.CreateFont(25,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,_T("Arial"));
	m_ctrl.SetScaleValFont(&font);
	m_ctrl.SetPhasorDescFont(&font);
	m_ctrl.Init();
	font.DeleteObject();
}
