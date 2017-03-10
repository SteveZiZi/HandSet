// WebControlDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WebControlDemo.h"
#include "WebControlDemoDlg.h"
#include "WebCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWebControlDemoDlg 对话框

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


// CWebControlDemoDlg 消息处理程序

BOOL CWebControlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
      
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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
    // TODO: 在此添加控件通知处理程序代码
}
