// CommonCtrlDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CommonCtrlDemo.h"
#include "CommonCtrlDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCommonCtrlDemoDlg �Ի���

CCommonCtrlDemoDlg::CCommonCtrlDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonCtrlDemoDlg::IDD, pParent)
	, m_bIsModal(false)
	, m_CtrlSelected(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommonCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO1, m_CtrlSelected);
}

BEGIN_MESSAGE_MAP(CCommonCtrlDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TIME_EDIT_APPLY,&CCommonCtrlDemoDlg::OnTimeEditApply)//�����Զ�����Ϣ
	ON_MESSAGE(WM_TIME_EDIT_CANCLE,&CCommonCtrlDemoDlg::OnTimeEditCancle)//�����Զ�����Ϣ
	ON_MESSAGE(WM_MAC_EDIT_APPLY,&CCommonCtrlDemoDlg::OnMacEditApply)//�����Զ�����Ϣ
	ON_MESSAGE(WM_MAC_EDIT_CANCLE,&CCommonCtrlDemoDlg::OnMacEditCancle)//�����Զ�����Ϣ
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON1, &CCommonCtrlDemoDlg::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCommonCtrlDemoDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CCommonCtrlDemoDlg ��Ϣ�������

BOOL CCommonCtrlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_TIME_STATIC)->SetWindowText(_T("1970 - 01 - 01   00 : 00 : 00"));
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CCommonCtrlDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_COMMONCTRLDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_COMMONCTRLDEMO_DIALOG));
	}
}
#endif

LRESULT CCommonCtrlDemoDlg::OnTimeEditApply(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_TimeEdit.GetWindowText(str);
//	m_TimeEdit.HideWindow();
	CWnd   *pWnd = this->GetDlgItem(IDC_TIME_STATIC); // ȡ�ÿؼ���ָ��
	pWnd->SetWindowText(str);
	return 0;
}

LRESULT CCommonCtrlDemoDlg::OnTimeEditCancle(WPARAM wParam, LPARAM lParam)
{
//	m_TimeEdit.HideWindow();
	return 0;
}

LRESULT CCommonCtrlDemoDlg::OnMacEditApply(WPARAM wParam, LPARAM lParam)
{
	CString str;
	m_MacEdit.GetWindowText(str);
	//	m_TimeEdit.HideWindow();
	CWnd   *pWnd = this->GetDlgItem(IDC_TIME_STATIC); // ȡ�ÿؼ���ָ��
	pWnd->SetWindowText(str);
	return 0;
}

LRESULT CCommonCtrlDemoDlg::OnMacEditCancle(WPARAM wParam, LPARAM lParam)
{
	//	m_MacEdit.HideWindow();
	return 0;
}

void CCommonCtrlDemoDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	UpdateData(TRUE);
	if ( m_CtrlSelected == 0 )
		TimeEditTest(point);
	else if ( m_CtrlSelected == 1 )
		MacEditTest(point);

	CDialog::OnLButtonUp(nFlags, point);
}

void CCommonCtrlDemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bIsModal = !m_bIsModal;
	CWnd   *pWnd = this->GetDlgItem(IDC_BUTTON1); // ȡ�ÿؼ���ָ��
	if ( !m_bIsModal )
		pWnd->SetWindowText(_T("ģ̬���ڷ�ʽ"));
	else
		pWnd->SetWindowText(_T("ֱ�Ӵ�����ʽ"));
}

void CCommonCtrlDemoDlg::TimeEditTest(CPoint point)
{
	CRect rt(0,0,200,30);
	rt.OffsetRect(point);
	CWnd   *pWnd = this->GetDlgItem(IDC_TIME_STATIC); // ȡ�ÿؼ���ָ��
	CString str;
	pWnd->GetWindowText(str);
	SYSTEMTIME time;
	m_TimeEdit.Str2SysTime(str,time);

	//	TRACE(str);
	if ( m_bIsModal )
	{
		m_TimeEdit.SetBkColor(RGB(0,0,0));
		m_TimeEdit.SetTextColor(RGB(255,255,255));
		if ( m_TimeEdit.GoModal(this,rt,time) == IDOK )
		{
			m_TimeEdit.SysTime2Str(time,str);
			pWnd->SetWindowText(str);
		}
	}
	else
	{
		m_TimeEdit.SetBkColor(RGB(0,128,0));
		m_TimeEdit.SetTextColor(RGB(255,255,255));
		m_TimeEdit.Create(this,rt,time);
	}
}

void CCommonCtrlDemoDlg::MacEditTest(CPoint point)
{
	CRect rt(0,0,200,30);
	rt.OffsetRect(point);
	CWnd   *pWnd = this->GetDlgItem(IDC_TIME_STATIC); // ȡ�ÿؼ���ָ��
	CString str;
	pWnd->GetWindowText(str);
	unsigned char macArr[6];
	m_MacEdit.Str2MacAddr(str,macArr);

	//	TRACE(str);
	if ( m_bIsModal )
	{
		m_MacEdit.SetBkColor(RGB(0,0,0));
		m_MacEdit.SetTextColor(RGB(255,255,255));
		if ( m_MacEdit.GoModal(this,rt,macArr) == IDOK )
		{
			m_MacEdit.MacAddr2Str(macArr,str);
			pWnd->SetWindowText(str);
		}
	}
	else
	{
		m_MacEdit.SetBkColor(RGB(0,128,0));
		m_MacEdit.SetTextColor(RGB(255,255,255));
		m_MacEdit.Create(this,rt,macArr);
	}
}

void CCommonCtrlDemoDlg::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if ( m_CtrlSelected == 0 )
	{
		GetDlgItem(IDC_TIME_STATIC)->SetWindowText(_T("1970 - 01 - 01   00 : 00 : 00"));
	}
	else
	{
		GetDlgItem(IDC_TIME_STATIC)->SetWindowText(_T("00 - 00 - 00 - 00 - 00 - 00"));
	}
}
