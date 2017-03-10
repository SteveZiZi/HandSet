// SliderBoxDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SliderBoxDemo.h"
#include "SliderBoxDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSliderBoxDemoDlg 对话框

CSliderBoxDemoDlg::CSliderBoxDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSliderBoxDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSliderBoxDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSliderBoxDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSliderBoxDemoDlg 消息处理程序

BOOL CSliderBoxDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    SetWindowPos(NULL, 0, 0, 640, 380, 0);
    CRect rect;
    GetClientRect(&rect);
	m_SliderBox.Create(WS_VISIBLE|WS_CHILD|WS_TABSTOP, rect,this, 
        1000);
    //CString strImgDir = L"\\Storage Card\\images\\";
    CString strImgDir = L"\\FlashDisk2\\images\\";
    BOOL ret = m_SliderBox.LoadImages(strImgDir);
    if(!ret)
    {
        NKDbgPrintfW(L"载入图片失败!\n");
    }
    //ASSERT(ret);
    m_SliderBox.SetAnimateFreq(12);
    m_SliderBox.SetScrollTimeout(350);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSliderBoxDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SLIDERBOXDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SLIDERBOXDEMO_DIALOG));
	}
}
#endif

