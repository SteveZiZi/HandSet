// FpgaUpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FpgaUpdate.h"
#include "FpgaUpdateDlg.h"
#include "FpgaBootOperate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///< FPGA固件存放路径
#define FPGA_BOOT_FILE	L"\\Storage Card\\fpgaBoot"

CRITICAL_SECTION g_cs;
// CFpgaUpdateDlg 对话框

CFpgaUpdateDlg::CFpgaUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFpgaUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_curRange = 0.0;
    m_curDetailRange = 0.0;
    InitializeCriticalSection(&g_cs);
}

void CFpgaUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFpgaUpdateDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CFpgaUpdateDlg 消息处理程序

BOOL CFpgaUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    //加载位图
	m_mainBG.LoadBitmap(IDB_BITMAP_DM8000);
    m_scroll.LoadBitmap(IDB_BITMAP_SCROLL);
    m_scrollBG.LoadBitmap(IDB_BITMAP_SCROLL_BG);
    m_SuccBG.LoadBitmap(IDB_BITMAP_SUCCESS);
    m_FailBG.LoadBitmap(IDB_BITMAP_FAIL);
    m_runScroll.LoadBitmap(IDB_BITMAP_RUN_SCROLL);

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    
    BITMAP bmp;
    m_mainBG.GetBitmap(&bmp);

    SetWindowPos(NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, WS_EX_TOPMOST);
    m_scrollBG.GetBitmap(&bmp);

    m_scrollLeftP.x = (nScreenWidth - bmp.bmWidth) / 2;
    m_scrollLeftP.y = (nScreenHeight - bmp.bmHeight) * 2 / 5;

    m_scrollDetailLeftP.x = m_scrollLeftP.x;
    m_scrollDetailLeftP.y = (nScreenHeight - bmp.bmHeight) * 3 / 5;

    m_textLeftP.x = 0; 
    m_textLeftP.y = m_scrollLeftP.y + 35;

    m_textDetailLeftP.x = 0;
    m_textDetailLeftP.y = m_scrollDetailLeftP.y + 35;

    m_SuccBG.GetBitmap(&bmp);
    m_resultLeftP.x = (nScreenWidth - bmp.bmWidth) / 2;
    m_resultLeftP.y = (nScreenHeight - bmp.bmHeight) * 2 / 5;

    //创建字体
    m_fntContent.CreateFont(
        20,				        //以逻辑单位方式指定字体的高度
        0,						//以逻辑单位方式指定字体中字符的平均宽度
        0,						//指定偏离垂线和X轴在显示面上的夹角（单位：0.1度）
        0,						//指定字符串基线和X轴之间的夹角（单位：0.1度）
        FW_NORMAL,			    //指定字体磅数
        FALSE,					//是不是斜体
        FALSE,					//加不加下划线
        0,						//指定是否是字体字符突出
        ANSI_CHARSET,			//指定字体的字符集
        OUT_DEFAULT_PRECIS,		//指定所需的输出精度
        CLIP_DEFAULT_PRECIS,	//指定所需的剪贴精度
        DEFAULT_QUALITY,		//指示字体的输出质量
        FF_ROMAN,               //指定字体的间距和家族
        _T("宋体")			    //指定字体的字样名称
        );

    ShowWindow(SW_SHOWMAXIMIZED);

    //启动工作线程
    AfxBeginThread(&StartUpdateThread, this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CFpgaUpdateDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_FPGAUPDATE_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_FPGAUPDATE_DIALOG));
	}
}
#endif


void CFpgaUpdateDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    CRect rectClient;
    GetClientRect(&rectClient);

    CDC dcMem;
    CBitmap bmpMem;
    CBitmap * pOldBitMap;
    dcMem.CreateCompatibleDC(&dc); 
    bmpMem.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());
    
    pOldBitMap = dcMem.SelectObject(&bmpMem);

    //将绘制内容输出到dcMem上
    DrawBG(&dcMem);

    dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(pOldBitMap);
    dcMem.DeleteDC(); 
    bmpMem.DeleteObject();
}

///
/// @brief
///     在内存DC中绘制内容
/// @param[CDC *]
///     pDC - 传进去的内存DC
/// @return
///
///
void CFpgaUpdateDlg::DrawBG( CDC * pDC )
{
    if(pDC == NULL || !pDC->GetSafeHdc())
    {
        return ;
    }

    //设置背景为透明
    pDC->SetBkMode(TRANSPARENT);

    CRect rc;
    CFont * pFont;
    BITMAP bm; 
    GetClientRect(&rc);
    m_mainBG.GetBitmap(&bm);

    int nStartX = (GetSystemMetrics(SM_CXSCREEN) - bm.bmWidth) / 2;

    CDC dc;
    dc.CreateCompatibleDC(pDC);
    CBitmap * pOldBmp = dc.SelectObject(&m_mainBG);

    //绘制背景位图
    pDC->BitBlt(nStartX, 0, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
    dc.SelectObject(pOldBmp);
    
    if(m_curRange <= 1.0)
    {
    
        pOldBmp = dc.SelectObject(&m_scrollBG);
        m_scrollBG.GetBitmap(&bm);    
        int nScrollLen = m_scrollLeftP.x + bm.bmWidth;

        //绘制进度滚动条框架
        pDC->BitBlt(m_scrollLeftP.x, m_scrollLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);

        //绘制具体滚动条框架
        pDC->BitBlt(m_scrollDetailLeftP.x, m_scrollDetailLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);

        int nCnt = (int)(m_curRange / 0.01);
        dc.SelectObject(m_scroll);
        m_scroll.GetBitmap(&bm);

        int nX = m_scrollLeftP.x;
        int nY = m_scrollLeftP.y + 2;

        //绘制进度滚动条填充的比例
        for(int i = 0; i < nCnt; i++)
        {
            pDC->BitBlt(nX, nY, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
            nX += bm.bmWidth;
        }

        dc.SelectObject(&m_runScroll);
        m_runScroll.GetBitmap(&bm);

        nCnt = (int)(m_curDetailRange / 0.01);
        nX = m_scrollDetailLeftP.x + nCnt * bm.bmWidth;
        nY = m_scrollDetailLeftP.y + 4;
        int nTimes = nCnt > 0 ? 4 : 2;
        
        if(nTimes == 2)
        {
            nX += 2;
        }

        //绘制具体滚动条的位置
        for(int i = 0; i < nTimes && m_curRange < 1.0; i++)
        {
            //判断是否出了边界
            if(nX + bm.bmWidth <= nScrollLen)
            {
                pDC->BitBlt(nX, nY, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
            }
            else
            {
                m_curDetailRange = 1.0;
            }
            nX += bm.bmWidth + 4;
        }

        dc.SelectObject(pOldBmp);

        //如果现实文字不为空
        if(m_strText.Compare(L"") != 0 && m_strDetailText.Compare(L"") != 0)
        {
           
            pFont = pDC->SelectObject(&m_fntContent);
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->SetBkColor(RGB(0, 0, 0));

            CSize size = pDC->GetTextExtent(m_strText);
            RECT rect;
            rect.left = m_textLeftP.x;
            rect.top = m_textLeftP.y;
            rect.bottom = rect.top + size.cy;
            rect.right = rc.Width();

            //绘制文字
            pDC->DrawText(m_strText, m_strText.GetLength(), &rect, 
                DT_SINGLELINE | DT_VCENTER | DT_CENTER);

            size = pDC->GetTextExtent(m_strDetailText);
            rect.left = m_textDetailLeftP.x;
            rect.top = m_textDetailLeftP.y;
            rect.bottom = rect.top + size.cy;

            pDC->DrawText(m_strDetailText, m_strDetailText.GetLength(), &rect, 
                DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        }
    }
    //如果升级已经结束
    else if(m_curRange > 1.0)
    {
        if(m_bIsSuccess)
        {
            pOldBmp = dc.SelectObject(&m_SuccBG);
            m_SuccBG.GetBitmap(&bm);
        }
        else
        {
            pOldBmp = dc.SelectObject(&m_FailBG);
            m_FailBG.GetBitmap(&bm);
        }
        
        //绘制结果的显示位图
        pDC->BitBlt(m_resultLeftP.x, m_resultLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
        dc.SelectObject(pOldBmp);

        if(m_strText.Compare(L"") != 0)
        {
            pFont = pDC->SelectObject(&m_fntContent);
            pDC->SetTextColor(RGB(255, 255, 255));

            CSize size = pDC->GetTextExtent(m_strText);
            RECT rect;
            rect.left = m_resultLeftP.x + 121;
            rect.top = m_resultLeftP.y + 63;
            rect.bottom = rect.top + size.cy;
            rect.right = rect.left + size.cx;

            pDC->DrawText(m_strText, m_strText.GetLength(), &rect, 
               DT_SINGLELINE|DT_VCENTER|DT_LEFT );
        }
    }

    dc.SelectObject(pFont);
    dc.DeleteDC();
}

///
/// @brief
///     设置进度条的显示情况
/// @param[double]
///     curRange - 当前工作的完成进度
/// @param[CString ]
///     strText - 进度滚动条要显示的文字
/// @param[CString ]
///     strDetailText - 具体滚动条要显示的文字
/// @param[bool ]
///     bIsSuccess - 是否升级成功
/// @return
///
///
void CFpgaUpdateDlg::SetProcessRange( double curRange, CString strText, 
                                     CString strDetailText, bool bIsSuccess )
{
    m_curRange = curRange;
    m_strText = strText;
    m_bIsSuccess = bIsSuccess;
    if(strDetailText.Compare(L"") != 0)
    {
        m_strDetailText = strDetailText;
    }
    Invalidate();
    UpdateWindow();
}

BOOL CFpgaUpdateDlg::OnEraseBkgnd(CDC* pDC)
{
    //删除全部临时创建的CDC
    CDC::DeleteTempMap();

    return TRUE;
}

void CFpgaUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if(nIDEvent == 1)
    {
        EnterCriticalSection(&g_cs);
        m_curDetailRange += 0.05;
        if(m_curDetailRange > 1.0)
        {
            m_curDetailRange = 0;
        }
       
        //如果升级程序已经结束
        if(m_curRange >= 1.0)
        {
            //结束定时器
            KillTimer(nIDEvent);
        } 
        LeaveCriticalSection(&g_cs);
        Invalidate();
        UpdateWindow();
        
    }
    CDialog::OnTimer(nIDEvent);
}

///
/// @brief
///     升级FPGA的工作线程
/// @param[LPVOID ]
///     pParam - 线程参数
/// @return
///     线程的退出值
///
UINT CFpgaUpdateDlg::StartUpdateThread( LPVOID pParam )
{
    CFpgaUpdateDlg * pFpgaUpdateDlg = (CFpgaUpdateDlg *)pParam;

    //创建FpgaBootOperate对象，并传入当前对话框的指针
    FpgaBootOperate * pFpgaBootOperate = new FpgaBootOperate(pFpgaUpdateDlg);

    //开始进行FPGA升级
    pFpgaBootOperate->WriteFpgaBoot(FPGA_BOOT_FILE);

    delete pFpgaBootOperate;

    //关闭对话框
    pFpgaUpdateDlg->OnCancel();
    return 0;
}

CFpgaUpdateDlg::~CFpgaUpdateDlg()
{
    DeleteCriticalSection(&g_cs);
}
