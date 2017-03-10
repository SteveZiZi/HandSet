// ProgressBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProgressBarDlg.h"

// CProgressBarDlg 对话框

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialog)

#define SC_PROGRESSBAR_TIME_INTERVAL 200                       ///< 定时器间隔

#define SC_PROGRESS_BAR_RECT         CRect(0, 0, 180, 80)      ///< 页面区域

#ifdef PROGRESS_BAR_ENABLE
#define SC_PROGRESS_BAR_WIDTH        144                       ///< 进度条宽度
#define SC_PROGRESS_BAR_HEIGHT       20                        ///< 进度条高度
#define SC_PROGRESS_BAR_BORDER_COLOR RGB(0,0,0)                ///< 进度条外框颜色
#define SC_PROGRESS_BAR_COLOR        RGB(0,0,0)                ///< 进度条颜色
#endif

#define SC_PROGRESS_BAR_CRBK         RGB(0,0,0)                ///< 进度条窗口背景颜色

#define SC_PROGRESS_BAR_CRTEXT       RGB(105,105,105)          ///< 进度条窗口字体颜色

const UINT k_LoadingBmpID[] = {
	IDB_LOADING1,IDB_LOADING2,IDB_LOADING3,IDB_LOADING4,IDB_LOADING5,IDB_LOADING6,
	IDB_LOADING7,IDB_LOADING8,IDB_LOADING9,IDB_LOADING10,IDB_LOADING11,IDB_LOADING12
};

CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressBarDlg::IDD, pParent)
    , m_nTimerID(0)
    , m_csTips(L"loading")
    , m_colorBk(SC_PROGRESS_BAR_CRBK)
    , m_colorText(SC_PROGRESS_BAR_CRTEXT)
    , m_pFont(NULL)
	, m_pTask(NULL)
    , m_pArgs(NULL)
    , m_hThread(NULL)
{

}

CProgressBarDlg::~CProgressBarDlg()
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CProgressBarDlg 消息处理程序

BOOL CProgressBarDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

	// 调整窗口位置
	CRect rcRect;
	AfxGetMainWnd()->GetClientRect(rcRect);
//	CRect rect(SC_PROGRESS_BAR_RECT);
// 	CPoint pt(rcRect.CenterPoint());
// 	rect.MoveToXY(pt.x-rect.Width()/2,pt.y-rect.Height()/2);
//     AfxGetMainWnd()->ClientToScreen(rect);
	AfxGetMainWnd()->ClientToScreen(rcRect);
    MoveWindow(rcRect);

	// 半透明化窗口
	SetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(this->GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));
	if (hInst)
	{
			typedef BOOL (WINAPI * MYFUNC)(HWND,COLORREF,BYTE,DWORD);
			MYFUNC fun = NULL;
			fun = (MYFUNC)GetProcAddress(hInst,"SetLayeredWindowAttributes");
			if (fun)
			fun(this->GetSafeHwnd(),0,128,2);
			FreeLibrary(hInst);
	}

#ifdef PROGRESS_BAR_ENABLE
	// 设置进度条外框的端点
	CRect rt;
	GetClientRect(rt);
	m_ptProgressBarBorder[0].SetPoint(rt.CenterPoint().x-SC_PROGRESS_BAR_WIDTH/2,
		                              rt.CenterPoint().y-SC_PROGRESS_BAR_HEIGHT/2);
	m_ptProgressBarBorder[1].SetPoint(rt.CenterPoint().x-SC_PROGRESS_BAR_WIDTH/2,
		                              rt.CenterPoint().y+SC_PROGRESS_BAR_HEIGHT/2);
	m_ptProgressBarBorder[2].SetPoint(rt.CenterPoint().x+SC_PROGRESS_BAR_WIDTH/2,
		                              rt.CenterPoint().y+SC_PROGRESS_BAR_HEIGHT/2);
	m_ptProgressBarBorder[3].SetPoint(rt.CenterPoint().x+SC_PROGRESS_BAR_WIDTH/2,
		                              rt.CenterPoint().y-SC_PROGRESS_BAR_HEIGHT/2);
	m_ptProgressBarBorder[4].SetPoint(rt.CenterPoint().x-SC_PROGRESS_BAR_WIDTH/2,
		                              rt.CenterPoint().y-SC_PROGRESS_BAR_HEIGHT/2);
	// 设置内部进度条的矩形域
	m_rtProgressBar.SetRect(m_ptProgressBarBorder[0],m_ptProgressBarBorder[2]);
	m_rtProgressBar.DeflateRect(2,2);
	m_rtProgressBar.SetRect(m_rtProgressBar.TopLeft(),
		CPoint(m_rtProgressBar.left+m_rtProgressBar.Width()/5,m_rtProgressBar.bottom));
#endif

	if(m_pTask)
	{
		m_hThread = CreateThread(NULL, 0, m_pTask, m_pArgs, 
			STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	}

    m_nTimerID = SetTimer(1, SC_PROGRESSBAR_TIME_INTERVAL, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CProgressBarDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect rcClient;
    GetClientRect(rcClient);
    InvalidateRect(rcClient);

    CDialog::OnTimer(nIDEvent);
}

BOOL CProgressBarDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    UNREFERENCED_PARAMETER(pDC);

    return TRUE;
}

void CProgressBarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()
    CRect rcClient;
    GetClientRect(rcClient);

	static int nIndex = 0;

#ifdef PROGRESS_BAR_ENABLE
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);

	memDC.SetBkColor(m_colorBk);
	memDC.FillSolidRect(0,0,rcClient.Width(),rcClient.Height(), m_colorBk);

	// 绘制进度条外框
	CPen  pen;
	pen.CreatePen(PS_SOLID,2,SC_PROGRESS_BAR_BORDER_COLOR);
	CPen *pOldPen = memDC.SelectObject(&pen);
	memDC.Polyline(m_ptProgressBarBorder,5);
	memDC.SelectObject(pOldPen);
	pen.DeleteObject();

	// 绘制进度条
	CRect rt(m_rtProgressBar);
	rt.MoveToX(m_rtProgressBar.left+m_rtProgressBar.Width()*nIndex/5);
	memDC.FillSolidRect(rt,SC_PROGRESS_BAR_COLOR);

	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),
		&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();

	nIndex = ++nIndex % 20;

#else
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);

	// 背景填充
	memDC.SetBkColor(m_colorBk);
	memDC.FillSolidRect(0,0,rcClient.Width(),rcClient.Height(), m_colorBk);

	// 贴图：loading位图
	CBitmap loadBmp;
	loadBmp.LoadBitmap(k_LoadingBmpID[nIndex]);
	CDC loadBmpDC;
	loadBmpDC.CreateCompatibleDC(&dc);
	CBitmap *pOldBmp_L = loadBmpDC.SelectObject(&loadBmp);
	BITMAP bmpInfo;
	loadBmp.GetBitmap(&bmpInfo);
	memDC.StretchBlt(rcClient.CenterPoint().x-bmpInfo.bmWidth/2,
		             rcClient.CenterPoint().y-bmpInfo.bmHeight/2,
					 bmpInfo.bmWidth,
					 bmpInfo.bmHeight,
		             &loadBmpDC,
					 0,0,bmpInfo.bmWidth,bmpInfo.bmHeight,SRCCOPY);
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&memDC,0,0,SRCCOPY);

	// 释放资源
	loadBmpDC.SelectObject(pOldBmp_L);
	loadBmpDC.DeleteDC();
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	loadBmp.DeleteObject();
	memBmp.DeleteObject();

	nIndex = (++nIndex) % (sizeof(k_LoadingBmpID)/sizeof(UINT));
#endif

// 	dc.FillSolidRect(rcClient, m_colorBk);
// 
//     int nOriDC = dc.SaveDC();
//     dc.SetBkMode(TRANSPARENT);
// 
//     if(m_pFont)
//     {
//         dc.SelectObject(m_pFont);
//     }
// 
//     dc.SetTextColor(SC_PROGRESS_BAR_CRTEXT);
// 
//     // 只有文字
//     CString csExt;
//     switch(nIndex % 3)
//     {
//     case 0:
//         csExt = L".";
//         break;
//     case 1:
//         csExt = L"..";
//         break;
//     case 2:
//         csExt = L"...";
//         break;
//     }
// 
// 	CSize sz = dc.GetTextExtent(m_csTips + csExt);
// 	CRect rcText;
// 	GetClientRect(rcText);
// 	rcText.DeflateRect((rcText.Width()-sz.cx)/2,(rcText.Height()-sz.cy)/2);
// 	dc.DrawText(m_csTips + csExt, rcText, DT_CENTER | DT_VCENTER);
//
//	dc.RestoreDC(nOriDC);
//
//    nIndex = ++nIndex % 20;
}


void CProgressBarDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    KillTimer(m_nTimerID);
    ::CloseHandle(m_hThread);
}


void CProgressBarDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
	CDialog::OnOK();
}

void CProgressBarDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
	CDialog::OnCancel();
}
