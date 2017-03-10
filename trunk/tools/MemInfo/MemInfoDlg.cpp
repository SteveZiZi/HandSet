// MemInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MemInfo.h"
#include "MemInfoDlg.h"
#include "../lib_leakprobe/lib_leakprobe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMemInfoDlg 对话框

CMemInfoDlg::CMemInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemInfoDlg::IDD, pParent)
    , m_strMemusage(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMemInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_MEMUSAGE, m_strMemusage);
}

BEGIN_MESSAGE_MAP(CMemInfoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CMemInfoDlg 消息处理程序

BOOL CMemInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    // 定时清理临时对象
    SetTimer(1, 10000, NULL);

    // 打印cpu和内存信息
    SetTimer(2, 4123, NULL);

    // 第一次显示
    UpdateUsageInfo();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CMemInfoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MEMINFO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_MEMINFO_DIALOG));
	}
}
#endif


void CMemInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    //RETAILMSG(1, (L"CMemInfoDlg::OnTimer trigger...\r\n"));

    if (nIDEvent == 1)
    {
        // 清除临时对象
        // 参考kb 839859  105286 819635
        AfxGetApp()->OnIdle(1);
    }
    else if (nIDEvent == 2)
    {
        UpdateUsageInfo();
    }

    CDialog::OnTimer(nIDEvent);
}

void  CMemInfoDlg::UpdateUsageInfo ()
{
    // 获得当前时间
    SYSTEMTIME  systm;
    GetLocalTime(&systm);

    // 获得cpu和内存信息
    int  cpuUsage;
    GlobalMemoryUsageInfo  globalMemUsage;
    Lkp_GetCurrentUsageInfo(&cpuUsage, &globalMemUsage);

    // 计算最大值
    static  DWORD  maxLoad = 0;
    static  DWORD  maxUsedPhys = 0;
    maxLoad = max(maxLoad, globalMemUsage.dwMemoryLoad);
    maxUsedPhys = max(maxUsedPhys, globalMemUsage.dwUsedPhys);

    // 组合显示的字符串
    m_strMemusage.Format(L"cpu=%d%%, load=%u%%/%u%%, usedPhys=%uK/%uK", 
        cpuUsage, globalMemUsage.dwMemoryLoad, maxLoad, 
        globalMemUsage.dwUsedPhys/1024, maxUsedPhys/1024);

    // 在串口打印信息
    RETAILMSG(1, (L"MemInfo: [%04d-%02d-%02d %02d:%02d:%02d] %s\r\n",
        systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond,
        m_strMemusage));

    UpdateData(FALSE);  // 更新界面
}