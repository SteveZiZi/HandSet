// MemInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MemInfo.h"
#include "MemInfoDlg.h"
#include "../lib_leakprobe/lib_leakprobe.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMemInfoDlg �Ի���

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


// CMemInfoDlg ��Ϣ�������

BOOL CMemInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

    // ��ʱ������ʱ����
    SetTimer(1, 10000, NULL);

    // ��ӡcpu���ڴ���Ϣ
    SetTimer(2, 4123, NULL);

    // ��һ����ʾ
    UpdateUsageInfo();
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    //RETAILMSG(1, (L"CMemInfoDlg::OnTimer trigger...\r\n"));

    if (nIDEvent == 1)
    {
        // �����ʱ����
        // �ο�kb 839859  105286 819635
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
    // ��õ�ǰʱ��
    SYSTEMTIME  systm;
    GetLocalTime(&systm);

    // ���cpu���ڴ���Ϣ
    int  cpuUsage;
    GlobalMemoryUsageInfo  globalMemUsage;
    Lkp_GetCurrentUsageInfo(&cpuUsage, &globalMemUsage);

    // �������ֵ
    static  DWORD  maxLoad = 0;
    static  DWORD  maxUsedPhys = 0;
    maxLoad = max(maxLoad, globalMemUsage.dwMemoryLoad);
    maxUsedPhys = max(maxUsedPhys, globalMemUsage.dwUsedPhys);

    // �����ʾ���ַ���
    m_strMemusage.Format(L"cpu=%d%%, load=%u%%/%u%%, usedPhys=%uK/%uK", 
        cpuUsage, globalMemUsage.dwMemoryLoad, maxLoad, 
        globalMemUsage.dwUsedPhys/1024, maxUsedPhys/1024);

    // �ڴ��ڴ�ӡ��Ϣ
    RETAILMSG(1, (L"MemInfo: [%04d-%02d-%02d %02d:%02d:%02d] %s\r\n",
        systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond,
        m_strMemusage));

    UpdateData(FALSE);  // ���½���
}