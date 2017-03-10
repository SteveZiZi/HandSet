// KbdTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FunctionTestCE.h"
#include "KbdTestDlg.h"

#define  VK_OEM_PERIOD 0xBE
// CKbdTestDlg dialog

IMPLEMENT_DYNAMIC(CKbdTestDlg, CDialog)

CKbdTestDlg::CKbdTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKbdTestDlg::IDD, pParent)
    , m_strPressKey(_T(""))
{

}

CKbdTestDlg::~CKbdTestDlg()
{
}

void CKbdTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_KEY_NAME, m_strPressKey);
}


BEGIN_MESSAGE_MAP(CKbdTestDlg, CDialog)
    ON_WM_CHAR()
END_MESSAGE_MAP()


// CKbdTestDlg message handlers




bool    g_bThreadKill;
static UINT    KbdTestThread(LPVOID pParam);

static const CString gs_strKeyNameMap[]=
{
    L"Shift",
    L"0",
    L".",
    L"Home",
    L"DOWN",
    L"7",
    L"8",
    L"9",
    L"Menu",
    L"RIGHT",
    L"4",
    L"5",
    L"6",
    L"LEFT",
    L"UP",
    L"1",
    L"2",
    L"3",
    L"Esc",
    L"Enter",
    L"F1",
    L"F2",
    L"F3",
    L"F4",
    L"F5",
    L"F6",
    L"开机键"  
    
};

volatile BOOL    g_bKeyPressed[sizeof(gs_strKeyNameMap)/sizeof(gs_strKeyNameMap[0])]={0};


BOOL CKbdTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    //将本窗口全屏显示，并置于最顶端
    DWORD       cx = GetSystemMetrics(SM_CXSCREEN);
    DWORD       cy = GetSystemMetrics(SM_CYSCREEN);

    MoveWindow(0,0,cx,cy,TRUE);
    ::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

    for(int i=0;i<sizeof(gs_strKeyNameMap)/sizeof(gs_strKeyNameMap[0]);i++)
    {
        g_bKeyPressed[i]=FALSE;
    }

    g_bKeyPressed[KB_KEYID_MENU] = TRUE;
    SetFocus();

    m_pCkbDrv = new CKBDrvOper();
    g_bThreadKill = FALSE;
    AfxBeginThread(KbdTestThread, this);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CKbdTestDlg::OnOK()
{
    BOOL bFailed =FALSE;
    for (int i=0;i<sizeof(g_bKeyPressed)/sizeof(g_bKeyPressed[0]);i++ )
    {
        if (!g_bKeyPressed[i])
        {
            bFailed = TRUE;
            break;
        }
    }

    if (!bFailed)
    {
        CDialog::OnOK();
    }
}

void CKbdTestDlg::OnCancel()
{
    BOOL        bFailed =FALSE;
    CString     str =L"你还有标志为 ";
    unsigned int         i;

    for (i=0;i<sizeof(g_bKeyPressed)/sizeof(g_bKeyPressed[0]);i++ )
    {
        if (!g_bKeyPressed[i])
        {
            bFailed = TRUE;
            str +=gs_strKeyNameMap[i];
            str += L' ';
        }
    }

    if (bFailed)
    {
        str += L"的按键还没有检测到按下，如果已经按下了这些键，请点\"是\"，点\"否\"返回继续测试 ";
        if (MessageBox(str,L"错误",MB_ICONQUESTION|MB_YESNO)!=IDYES)
        {
            return;
        }
        g_bThreadKill = TRUE;
        Sleep(200);
        CDialog::OnCancel();
        return ;
    }


    OnOK();
}

static UINT KbdTestThread( LPVOID pParam )
{
    CKbdTestDlg * pDlg = (CKbdTestDlg *)pParam;

    UINT8 uScanCode;
    UINT8 uKeyID = 0;
    while(!g_bThreadKill)
    {
        UINT uRet = pDlg->m_pCkbDrv->WaitForReadInputScancode(uScanCode, 100);
        if(uRet == WAIT_TIMEOUT)
        {
            continue;
        }
        uKeyID = KB_SCODE_GETKEYID(uScanCode);
        pDlg->SetWindowText(gs_strKeyNameMap[uKeyID]);
        g_bKeyPressed[uKeyID] = TRUE;

        bool bFailed =FALSE;
        for (int i=0;i<sizeof(g_bKeyPressed)/sizeof(g_bKeyPressed[0]);i++ )
        {
            if (!g_bKeyPressed[i])
            {
                bFailed = TRUE;
                break;
            }
        }

        if (!bFailed)
        {
            g_bThreadKill = TRUE;
            pDlg->PostMessage(WM_COMMAND,IDOK,0);
            
        }
    }
    
    return 0;
}
