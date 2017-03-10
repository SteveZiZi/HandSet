// ReportControlDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ReportControlDemoDlg.h"
#include "ExlvDlg.h"
#include "ElTestDlg.h"
#include "ELTest2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define TREEBOX_CTRL_ID  200

// CReportControlDemoDlg 对话框

CReportControlDemoDlg::CReportControlDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportControlDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // 加载雅黑字体
    m_bodyFont.CreateFont(LBT_3L_TEXTLFHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_COMPAT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");

    m_bmp1.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\1.bmp"));
    m_bmp2.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\2.bmp"));
    m_bmp3.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\3.bmp"));
    m_bmp4.Attach(::SHLoadDIBitmap(L"\\FlashDisk2\\bmp\\4.bmp"));

}

void CReportControlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_REPORT_TEST, m_ctlReportTest);
}

BEGIN_MESSAGE_MAP(CReportControlDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_TEST1, &CReportControlDemoDlg::OnBnClickedBtnTest1)
    ON_BN_CLICKED(IDC_BTN_TEST2, &CReportControlDemoDlg::OnBnClickedBtnTest2)
    ON_MESSAGE(PWID_UPDATE_INPUTMODE, &CReportControlDemoDlg::OnUpdateInputMode)
    ON_BN_CLICKED(IDC_BUTTON1, &CReportControlDemoDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CReportControlDemoDlg::OnBnClickedButton2)
    ON_NOTIFY(NM_RETURN, TREEBOX_CTRL_ID, OnTreeBoxClick)
    ON_BN_CLICKED(IDC_BUTTON3, &CReportControlDemoDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CReportControlDemoDlg 消息处理程序

BOOL CReportControlDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    //m_ctlBox.Create(WS_CHILD|WS_VISIBLE|WS_BORDER, CRect(0, 0, 100, 100), this, 0);
    m_ctlBox.CreateEx(WS_EX_TOOLWINDOW, WS_POPUP|WS_VISIBLE, CRect(0, 0, 150, 300), this, 0);
    m_ctlBox.SetFont(&m_bodyFont);
    m_ctlBox.PostInitTemplateStyle();


    //m_ctlBox.SetMargin(5, 10, 15, 20);
    m_ctlBox.AddString(L"基本设置");
    m_ctlBox.AddString(L"通道设置");
    m_ctlBox.AddString(L"存储管理");
    m_ctlBox.AddString(L"软件升级");
    m_ctlBox.AddString(L"关于");



    //m_ctlTree.CreateEx(WS_EX_TOOLWINDOW, WS_POPUP|WS_VISIBLE|WS_BORDER, CRect(100, 0, 300, 200), this, 0);
    m_ctlTree.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, CRect(0, 0, 200, 208), this, TREEBOX_CTRL_ID);
    m_ctlTree.SetFont(&m_bodyFont);
    m_ctlTree.SetHeaderFont(&m_bodyFont);
    //m_ctlTree.SetTreeExpandImage(&m_bmp1, &m_bmp2);
    //m_ctlTree.SetTreeCheckImage(&m_bmp3, &m_bmp4);
    m_ctlTree.PostInitTemplateStyle();


    m_ctlTree.InsertTreeColumn(0, L"列1", EL_LEFT, 100);
    m_ctlTree.InsertTreeColumn(1, L"列2", EL_LEFT, 100);


    LBT_HTREEITEM  h1 = m_ctlTree.InsertTreeItem(L"111111");
    LBT_HTREEITEM  h11 = m_ctlTree.InsertTreeItem(L"aaaa", h1);
    LBT_HTREEITEM  h12 = m_ctlTree.InsertTreeItem(L"bbbb", h1);
    LBT_HTREEITEM  h13 = m_ctlTree.InsertTreeItem(L"cccc", h1);
    LBT_HTREEITEM  h121 = m_ctlTree.InsertTreeItem(L"[[[]]]", h12);
    m_ctlTree.InsertTreeItem(L"****", h12);
    m_ctlTree.InsertTreeItem(L"(())))", h12);
    m_ctlTree.SetTreeHasCheckbox(h121);
    m_ctlTree.SetTreeHasCheckbox(h12);
    m_ctlTree.SetTreeHasCheckbox(h13);
    m_ctlTree.SetTreeCheck(h121);

    m_ctlTree.SetTreeItemText(h12, 1, L"大大方方");
    m_ctlTree.SetTreeItemText(h13, 1, L"许许多多");
    m_ctlTree.SetTreeItemText(h121, 1, L"随时随地的");
    m_ctlTree.SetTreeItemColor(h121, 1, RGB(255, 0, 0), RGB(255, 255, 255));

    LBT_HTREEITEM  h2 = m_ctlTree.InsertTreeItem(L"222222");
    LBT_HTREEITEM  h3 = m_ctlTree.InsertTreeItem(L"333333");
    LBT_HTREEITEM  h4 = m_ctlTree.InsertTreeItem(L"444444");
    LBT_HTREEITEM  h41 = m_ctlTree.InsertTreeItem(L"xxxxx", h4);
    LBT_HTREEITEM  h42 = m_ctlTree.InsertTreeItem(L"zzzzz", h4);
    m_ctlTree.InsertTreeItem(L"mm", h2);

    m_ctlTree.SetTreeItemText(h42, 1, L"乒乒乓乓");
    



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CReportControlDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_REPORTCONTROLDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_REPORTCONTROLDEMO_DIALOG));
	}
}
#endif


void CReportControlDemoDlg::OnBnClickedBtnTest1()
{
    // TODO: 在此添加控件通知处理程序代码

    CExlvDlg  dlg;
    dlg.DoModal();
}

void CReportControlDemoDlg::OnBnClickedBtnTest2()
{
    // TODO: 在此添加控件通知处理程序代码

    CElTestDlg  dlg;
    dlg.DoModal();
}



LRESULT  CReportControlDemoDlg::OnUpdateInputMode (WPARAM wParam, LPARAM)
{
    CString  str;
    str.Format(L"输入法切换 %d", wParam);
    EL_DEBUGLOG(L"%s\n", str);
    return 0;
}

void CReportControlDemoDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    //m_ctlBox.InsertString(-1, L"zzzz");
    m_ctlBox.InsertString(3, L"氢气球");
}

void CReportControlDemoDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    //m_ctlBox.DeleteString(m_ctlBox.GetCurSel());
    //m_ctlBox.DeleteString(m_ctlBox.GetCount() - 1);
    m_ctlBox.DeleteString(0);

    LBT_HTREEITEM  hItem = m_ctlTree.GetTreeSelectedItem();
    if (hItem)
    {
        m_ctlTree.DeleteTreeItem(hItem);
    }
}

void CReportControlDemoDlg::OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult)
{
    // 相应回车消息, 操作tree的checkbox
    LBT_HTREEITEM  hItem = m_ctlTree.GetTreeSelectedItem();
    if (hItem)
    {
        m_ctlTree.SetTreeCheck(hItem, !m_ctlTree.GetTreeCheck(hItem));
    }
    *pResult = TRUE;
}

void CReportControlDemoDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码


    CELTest2Dlg  dlg;
    dlg.DoModal();
}
