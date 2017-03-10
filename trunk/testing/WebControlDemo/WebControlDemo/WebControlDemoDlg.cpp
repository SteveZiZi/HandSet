// WebControlDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebControlDemo.h"
#include "WebControlDemoDlg.h"
#include "WebCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWebControlDemoDlg �Ի���

CWebControlDemoDlg::CWebControlDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebControlDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebControlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebControlDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON1, &CWebControlDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CWebControlDemoDlg ��Ϣ�������

BOOL CWebControlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
      
    DWORD dwStyle =
        WS_VISIBLE | WS_OVERLAPPED | WS_THICKFRAME | WS_SYSMENU;
    /*|WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX*/
        

    m_webCtrl._lpszUrl = L"file:///Storage Card/test/test.html";
    m_webCtrl._fEm
        pty = TRUE;
    BOOL ret = m_webCtrl.Create(dwStyle, CRect(5, 5, 500, 200), 
        this, IDS_IE);
    if(ret)
    {
        //m_webCtrl.HandleCommand()
    }
    else
    {
        WC_DEBUGLOG(L"Create web control failed.");
    }
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CWebControlDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_WEBCONTROLDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_WEBCONTROLDEMO_DIALOG));
	}
}
#endif


void CWebControlDemoDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}
