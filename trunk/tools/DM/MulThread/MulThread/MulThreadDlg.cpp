// MulThreadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MulThread.h"
#include "MulThreadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMulThreadDlg 对话框

CMulThreadDlg::CMulThreadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMulThreadDlg::IDD, pParent)
    , m_hThread(NULL)
    , m_bThreadRunning(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMulThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMulThreadDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_START_THREAD, &CMulThreadDlg::OnBnClickedButtonStartThread)
    ON_BN_CLICKED(IDC_BUTTON_STOP_THREAD, &CMulThreadDlg::OnBnClickedButtonStopThread)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CMulThreadDlg 消息处理程序

BOOL CMulThreadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
    SetTimer(1, 1000, NULL);

//    MoveWindow(CRect(0,0,640,480));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CMulThreadDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MULTHREAD_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_MULTHREAD_DIALOG));
	}
}
#endif


void CMulThreadDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认
    Invalidate(TRUE);

    CDialog::OnTimer(nIDEvent);
}

void CMulThreadDlg::OnBnClickedButtonStartThread()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_hThread == NULL)
    {
        m_bThreadRunning = TRUE;
        DWORD dwThreadId;
        m_hThread = CreateThread(NULL, 0, FloatThread, this, NULL, &dwThreadId);
        if (m_hThread == NULL)
        {
            RETAILMSG(1, (_T("Create Thread Failed \r\n")));
            return ;
        }
        else 
        {
            //创建线程成功，设置优先级
            if(CeSetThreadPriority(m_hThread, 108))
            {
                RETAILMSG(1, (_T("Set Thread Prio to %d \r\n"), CeGetThreadPriority(m_hThread)));
            }
            else
            {
                RETAILMSG(1, (_T("Set Thread Prio Failed \r\n")));
            }
        }
    }
}

float m = 2.0f;
int   n = 2;

DWORD FloatThread( PVOID pArg )
{
    CMulThreadDlg* pDlg = static_cast<CMulThreadDlg*>(pArg);
    srand(GetTickCount());
    while(pDlg->m_bThreadRunning)
    {
        for (int i = 0; i < 3000; ++i)
        {
//            m = static_cast<float>( rand()) * i;
            n = rand() * i;
        }
        Sleep(1);
    }
    return 1;
}

void CMulThreadDlg::OnBnClickedButtonStopThread()
{
    // TODO: 在此添加控件通知处理程序代码
    m_bThreadRunning = FALSE;
    DWORD dwRet = WaitForSingleObject(m_hThread, 1000);
    if (dwRet == WAIT_OBJECT_0)
    {
        RETAILMSG(1, (_T("Thread Exit Success ! \r\n")));
    }
    else
    {
        RETAILMSG(1, (_T("Thread Exit Failed ! \r\n")));
        TerminateThread(m_hThread, 44);
    }
    m_hThread = NULL;
}

void CMulThreadDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()
//     for (int i = 0; i < 8; ++i)
//     {
//         for (int j = 0; j < 6; ++j)
//         {
//             dc.FillSolidRect(i * 80,j*80, (i+1)*80,(j+1)*80,RGB(rand()%256, rand()%256, rand()%256));
//         }
//     }
    dc.FillSolidRect(0,0, 640,240,RGB(rand()%256, rand()%256, rand()%256));

    dc.FillSolidRect(0,240, 640,240,RGB(rand()%256, rand()%256, rand()%256));

    CString str;
    str.Format(L"Float: %f \rInt: %d", m, n);
    SetDlgItemText(IDC_STATIC_FLOAT, str);
}
