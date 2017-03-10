// BmpDisplayDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BmpDisplayDemo.h"
#include "BmpDisplayDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBmpDisplayDemoDlg 对话框

CBmpDisplayDemoDlg::CBmpDisplayDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpDisplayDemoDlg::IDD, pParent)
	, m_csBmpPath(_T(""))
	, m_csBmpDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBmpDisplayDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBmpDisplayDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CBmpDisplayDemoDlg 消息处理程序

BOOL CBmpDisplayDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 设定对话框全屏
	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&wndTopMost,  0, 0, iFullWidth, iFullHeight, 
		SWP_NOOWNERZORDER|SWP_SHOWWINDOW);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CBmpDisplayDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_BMPDISPLAYDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_BMPDISPLAYDEMO_DIALOG));
	}
}
#endif

void CBmpDisplayDemoDlg::DrawBitmap(CDC *pDC,CString path)
{

	CBitmap   bitmap; 

	//载入BMP图片
#ifdef WINCE
	HBITMAP   hBitmap = (HBITMAP)::SHLoadDIBitmap(path);
#else
	HBITMAP   hBitmap = (HBITMAP)LoadImage(NULL,bmpPath,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE); 
#endif
	bitmap.Attach(hBitmap);

	CDC    dcComp; 
	dcComp.CreateCompatibleDC(pDC);
	CBitmap* pOldBitMap = dcComp.SelectObject(&bitmap);

	//得到BMP图片具体信息
	BITMAP   bmInfo; 
	bitmap.GetObject(sizeof(bmInfo),&bmInfo); 
	//边界
	CRect rect;
	GetClientRect(&rect);
	//显示图片
	pDC->BitBlt(0,0,bmInfo.bmWidth,bmInfo.bmHeight,&dcComp,0,0,SRCCOPY);
	//	pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&dcComp,0,0,bmInfo.bmWidth,bmInfo.bmHeight,SRCCOPY);
	//清除
	dcComp.SelectObject(pOldBitMap);
	dcComp.DeleteDC();
	bitmap.DeleteObject();
}

void CBmpDisplayDemoDlg::OpenBmpFile()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		NULL, this);
	// 进入最近选择的目录
	if ( m_csBmpDir != _T("") )
	{
		dlg.m_ofn.lpstrInitialDir = m_csBmpDir;
	}

	if(dlg.DoModal() == IDOK)
	{
		m_csBmpPath = dlg.GetPathName();
		m_csBmpDir = m_csBmpPath.Left(m_csBmpPath.GetLength()-dlg.GetFileName().GetLength()-1);
		CString extName = dlg.GetFileExt();
		if ( extName.Compare(_T("bmp"))  == 0 || extName.Compare(_T("BMP"))  == 0 )
		{
			Invalidate();
		}
		else
		{
			m_csBmpPath = _T("");
		}
	}
}

void CBmpDisplayDemoDlg::OnPaint()
{
	CPaintDC dc(this);
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	//如果文件存放路径存在，则进行显示
	if (!(m_csBmpPath.IsEmpty()))
	{
		DrawBitmap(&dc,m_csBmpPath);
		m_csBmpPath = _T("");
	}
}

BOOL CBmpDisplayDemoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if ( pMsg->wParam == VK_RETURN)
	{
		if ( pMsg->message == WM_KEYDOWN )
			OpenBmpFile();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
