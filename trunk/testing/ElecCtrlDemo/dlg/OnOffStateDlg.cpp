// OnOffStateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"

#ifndef WINCE
#include "../Win32/ChartCtrlDemo.h"
#else
#include "../WinCE/ChartCtrl.h"
#endif

#include "OnOffStateDlg.h"


// COnOffStateDlg �Ի���

IMPLEMENT_DYNAMIC(COnOffStateDlg, CDialog)

COnOffStateDlg::COnOffStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnOffStateDlg::IDD, pParent)
	, m_InitTime(0)
	, m_DrawTime(0)
	, m_bReset(FALSE)
{

}

COnOffStateDlg::~COnOffStateDlg()
{
	m_cOnOffBmp1.DeleteObject();
	m_cOnOffBmp2.DeleteObject();
}

void COnOffStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INIT_TIME, m_InitTime);
	DDX_Text(pDX, IDC_DRAW_TIME, m_DrawTime);
}


BEGIN_MESSAGE_MAP(COnOffStateDlg, CDialog)
	ON_BN_CLICKED(IDC_RESET, &COnOffStateDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EXIT, &COnOffStateDlg::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// COnOffStateDlg ��Ϣ�������

BOOL COnOffStateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_cOnOffBmp1.LoadBitmap(IDB_ON_STATE);
	m_cOnOffBmp2.LoadBitmap(IDB_OFF_STATE);

	m_ctrl.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0, 0, 400,
		80), this, IDC_ON_OFF_STATE);
	m_ctrl.ModifyStyle(0,SS_OWNERDRAW);

#ifdef WINCE
	Init();
#endif

	m_ctrl.ShowWindow(TRUE);


	SetTimer(1,300,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void COnOffStateDlg::OnBnClickedReset()
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

void COnOffStateDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
	OnOK();
}

void COnOffStateDlg::OnTimer(UINT_PTR nIDEvent)
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

void COnOffStateDlg::Init()
{
	// ��ʼ������DI״̬ͼ
	m_ctrl.SetOnOffStyle(0,&m_cOnOffBmp1);
	m_ctrl.SetOnOffStyle(1,&m_cOnOffBmp2);
	m_ctrl.SetOnOffDICount(6,1);
	m_ctrl.SetOnOffStyleShowSize(25,25);

	m_ctrl.SetOnOffState(0,0,_T(""));
	m_ctrl.SetOnOffState(1,1,_T(""));
	m_ctrl.SetOnOffState(2,0,_T("DI3"));
	m_ctrl.SetOnOffState(3,1,_T("DI4"));
	m_ctrl.SetOnOffState(4,0,_T("DI5"));
	m_ctrl.SetOnOffState(5,1,_T("DI6"));
	m_ctrl.Init();
}

void COnOffStateDlg::Draw()
{
//	m_ctrl.EraseOnOffState();
	// 	CBitmap cBmp;
	// 	cBmp.LoadBitmap(IDB_ON_STATE);

	DWORD startTm = GetTickCount();

	static bool b = true;
	if ( b )
	{
		m_ctrl.SetOnOffState(0,0,_T(""));
		m_ctrl.SetOnOffState(1,1,_T(""));
	}
	else
	{
		m_ctrl.SetOnOffState(0,1,_T("DI1"));
		m_ctrl.SetOnOffState(1,0,_T("DI2"));
	}
	b = !b;

	m_ctrl.DrawOnOffState();
	m_ctrl.Invalidate();
	//	cBmp.DeleteObject();
	DWORD endTm = GetTickCount();
	m_DrawTime = endTm - startTm;
	UpdateData(FALSE);
}

void COnOffStateDlg::Reset()
{
	// ��ʼ������DI״̬ͼ
//	m_ctrl.SetOnOffDICount(3,2);
	m_ctrl.SetOnOffStyleShowSize(30,30);
	m_ctrl.SetOnOffState(0,0,_T("DI1"));
	m_ctrl.SetOnOffState(1,1,_T("DI2"));
	m_ctrl.Init();
}
