// KeyboardAuxDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KeyboardAux.h"
#include "KeyboardAuxDlg.h"
#include <Pm.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//
//-----------------------------------------------------------------------------
// ���ڴ����뷨�߳�֪ͨUI�̵߳���Ϣ
//  ����WM_USER_SHOW_CHARSEL_VIEW ���ݲ���WMParamShowCharselView* ��wParam
//-----------------------------------------------------------------------------
//
#define WM_USER_SHOW_CHARSEL_VIEW   (PRIVATE_WND_USERID + 1)
#define WM_USER_HIDE_CHARSEL_VIEW   (PRIVATE_WND_USERID + 2)
#define WM_USER_UPDATE_INPUTMODE    (PRIVATE_WND_USERID + 3)
struct WMParamShowCharselView
{
    HWND                    targetWnd;          // ��ǰ���㴰��
    LPCTSTR                 charsOptional;      // ��ѡ�ַ�
    int                     curSel;             // ��ǰ��ѡ�е��ַ�����
};




CKeyboardAuxDlg::CKeyboardAuxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyboardAuxDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // ��������
    
#if 0  // ϵͳ�Ѿ����������, �����Լ����ü���
    VERIFY(AddFontResource(L"\\FlashDisk2\\MSYH.TTF"));
#endif
    
    m_popupBarFont.CreateFont(24, 0, 0, 0, 
        FW_NORMAL, FALSE, FALSE, 0, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, FF_ROMAN, 
        _T("΢���ź�"));

    m_powerIsBackLightOff = FALSE;
}


//
//=============================================================================
// ʵ��IKBTestKeyReciver�ӿ�
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// hook��ʵ���̵ĵĲ��԰�������
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_OnTestKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // ת��������Ϣ
    //keybd_event(nChar,     0, 0 ,0);

    const KBScancodeInfo*  scodeInfo = KBFindKeyFromMessage(nChar);
    if (scodeInfo)
    {
        UINT8  scancode = KB_SCODE_MAKE(scodeInfo->keyid, 0);
        m_kboper.TestWriteInputScancode(scancode);
    }
}

//
//-----------------------------------------------------------------------------
// hook��ʵ���̵ĵĲ��԰�������
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_OnTestKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // ת��������Ϣ
    //keybd_event(nChar,     0, KEYEVENTF_KEYUP ,0);

    const KBScancodeInfo*  scodeInfo = KBFindKeyFromMessage(nChar);
    if (scodeInfo)
    {
        UINT8  scancode = KB_SCODE_MAKE(scodeInfo->keyid, 1);
        m_kboper.TestWriteInputScancode(scancode);
    }
}


//
//=============================================================================
// ʵ��IKBInputMethodUI�ӿ�
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ��ʾ��ѡ���ִ���(���������뷨�߳�)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel)
{
    // ��Ϊ����ͬһ���߳�, ���Է���Ϣ֪ͨ
    WMParamShowCharselView  param;
    param.targetWnd = targetWnd;
    param.charsOptional = charsOptional;
    param.curSel = curSel;

    SendMessage(WM_USER_SHOW_CHARSEL_VIEW, (WPARAM)&param);
}

//
//-----------------------------------------------------------------------------
// ���غ�ѡ���ִ���(���������뷨�߳�)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_HideCharSelView ()
{
    // ��Ϊ����ͬһ���߳�, ���Է���Ϣ֪ͨ
    SendMessage(WM_USER_HIDE_CHARSEL_VIEW);
}

//
//-----------------------------------------------------------------------------
// ���µ�ǰ����ģʽ(���������뷨�߳�)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_UpdateInputMode (KBInputMode mode)
{
    SendMessage(WM_USER_UPDATE_INPUTMODE, mode);
}
//
//-----------------------------------------------------------------------------
// ��õ�ǰ����״̬, �Ƿ�����
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::_PowerIsBackLightOff ()
{
    return m_powerIsBackLightOff;
}
//
//-----------------------------------------------------------------------------
// ���ѱ���
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_PowerWakeupBackLight ()
{
    SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);

    m_powerIsBackLightOff = FALSE;

    KB_DEBUGLOG(L"CKeyboardAuxDlg::_PowerWakeupBackLight\r\n");
}

//
//=============================================================================
// ��Ϣ����
//=============================================================================
//
BEGIN_MESSAGE_MAP(CKeyboardAuxDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
    ON_WM_SIZE()
#endif
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_USER_SHOW_CHARSEL_VIEW, &CKeyboardAuxDlg::OnShowCharselView)
    ON_MESSAGE(WM_USER_HIDE_CHARSEL_VIEW, &CKeyboardAuxDlg::OnHideCharselView)
    ON_MESSAGE(WM_USER_UPDATE_INPUTMODE, &CKeyboardAuxDlg::OnUpdateInputMode)
    ON_WM_TIMER()
END_MESSAGE_MAP()


//
//-----------------------------------------------------------------------------
// ��ʾ��ѡ���ִ���(������UI�߳�)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnShowCharselView (WPARAM wParam, LPARAM lParam)
{
    WMParamShowCharselView*  showViewParam = (WMParamShowCharselView*)wParam;
    ASSERT(showViewParam);

    // ����ѡ�ַ������봰��
    m_charselPopupBar.SetCharsOptional(showViewParam->charsOptional, showViewParam->curSel);


    // ���ݵ�ǰ���λ�õ�������λ��, Ŀǰ�޷�����������̵Ĺ��λ��
    // ֻ�����ô����������, ��ʾ��Ŀ�괰�ڵ����е�λ��
    CRect  targetRect;
    CRect  popupBarRect;
    ::GetWindowRect(showViewParam->targetWnd, &targetRect);
    m_charselPopupBar.GetWindowRect(&popupBarRect);

    CRect  popupBarRectFix;
    popupBarRectFix.left = targetRect.Width() / 2 + targetRect.left - popupBarRect.Width() / 2;
    popupBarRectFix.top = targetRect.bottom;
    popupBarRectFix.right = popupBarRectFix.left + popupBarRect.Width()+1;
    popupBarRectFix.bottom = popupBarRectFix.top + popupBarRect.Height();
    

#if 0 // ��ʱ��ʹ�ø÷���������, �ƺ�̫������cpu

    // ������Ի�õ�ǰ���λ��, ����ݹ��λ�õ���
    CPoint  caretPos;
    if (IPCGetFocusCaretPos(GetTopLevelForegroundWindow(), caretPos))
    {
        ::ClientToScreen(showViewParam->targetWnd, &caretPos);
        popupBarX = caretPos.x;
    }

#endif

    // �������յĺ�ѡ����λ��, ��֤��������Ļ���ӷ�Χ��
    AdjustWindowRectInsideScreen(targetRect, popupBarRectFix);

    
    // ��������λ��
    m_charselPopupBar.SetWindowPos(&CWnd::wndTopMost, popupBarRectFix.left, popupBarRectFix.top, 1, 1, 
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    
	// �����޸�
	/********************************************************/
    return 0;
}

//
//-----------------------------------------------------------------------------
// ���غ�ѡ���ִ���(������UI�߳�)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnHideCharselView (WPARAM wParam, LPARAM lParam)
{
    m_charselPopupBar.ShowWindow(SW_HIDE);

    return 0;
}

//
//-----------------------------------------------------------------------------
// ���µ�ǰ����ģʽ(������UI�߳�)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnUpdateInputMode (WPARAM wParam, LPARAM lParam)
{
    // ֪ͨӦ�ó����������л�����ģʽ
    HWND  curActiveWnd = GetTopLevelForegroundWindow();
    if (curActiveWnd)
    {
        IPCUpdateCurInputMode(curActiveWnd, (KBInputMode)wParam);
    }

    return 0;
}

//
//-----------------------------------------------------------------------------
// ��ʱ������ʱCDC����
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::OnTimer (UINT_PTR nIDEvent)
{
    KB_DEBUGLOG(L"CKeyboardAuxDlg::OnTimer trigger...\r\n");

    if (nIDEvent == 1)
    {
        // �����ʱ����
        // �ο�kb 839859  105286 819635
        AfxGetApp()->OnIdle(1);


        // ��ʱ��õ�Դ״̬
// #define SIZEOFARRAY(a)  (sizeof(a) / sizeof(a[0]))
//         TCHAR szPowerState[32];
//         DWORD dwPwrFlags;
//         GetSystemPowerState(szPowerState, SIZEOFARRAY(szPowerState), &dwPwrFlags);
//         KB_DEBUGLOG(L"GetSystemPowerState=%s flag=0x%x \r\n", szPowerState, dwPwrFlags);
//         if (dwPwrFlags & POWER_STATE_BACKLIGHTON)
//         {
//             m_powerIsBackLightOff = FALSE;
//         }
//         else
//         {
//             m_powerIsBackLightOff = TRUE;
//         }

    }
    else if (nIDEvent == 2)
    {
        // һ���������ش���, ����ʱ������ִֻ��һ��
        KillTimer(2);
        ShowWindow(SW_HIDE);
        
        IPCInitData();
    }

    CDialog::OnTimer(nIDEvent);
}

UINT CKeyboardAuxDlg::BackLightOffDetectProc(LPVOID lpArg)
{
    CKeyboardAuxDlg *pKeyAuxDlg = (CKeyboardAuxDlg *)lpArg;

    CeSetThreadPriority(::GetCurrentThread(), 120);

    while(1) {
        if (pKeyAuxDlg->m_bklMonitor.waitBklOff()) {
            pKeyAuxDlg->m_powerIsBackLightOff = TRUE;
        }
        else {
            break;
        }
    }

    return 0;
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CKeyboardAuxDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
    if (AfxIsDRAEnabled())
    {
        DRA::RelayoutDialog(
            AfxGetResourceHandle(), 
            this->m_hWnd, 
            DRA::GetDisplayMode() != DRA::Portrait ? 
            MAKEINTRESOURCE(IDD_KEYBOARDAUX_DIALOG_WIDE) : 
        MAKEINTRESOURCE(IDD_KEYBOARDAUX_DIALOG));
    }
}
#endif


//
//=============================================================================
// ��Ϣ����-���⺯��
//=============================================================================
//

BOOL CKeyboardAuxDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������

    // gui
    KBInitMainGUI();
    KBInitCharselPopupBar();

    // other
    if (m_kboper.DeviceInit())
    {
        // ��װ���̹���, ���ڲ�����
        InstallDbgHookKeyboard(this);

        // ��ʼ��IPCͨ��
        VERIFY(m_appipc.IPCInit());

        // ���뷨�����߳�����
        m_inputMethod.StartInputMethod(&m_kboper, this);
    }
    else
    {
        // ��ʼ��ʧ��
        AfxMessageBox(L"Maybe another kbaux running!\nPress Q exit");
        ::ExitProcess(0);
        return FALSE;
    }


    // ��ʱ������ʱ����
    SetTimer(1, 10000, NULL);

    // һ���������ش���, ����ʱ������ִֻ��һ��, Ҫ��OnTimer��Kill
    SetTimer(2, 0, NULL);

    AfxBeginThread(BackLightOffDetectProc, this);

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CKeyboardAuxDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ��ʼ��������
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::KBInitMainGUI ()
{
    //����������ȥ������ͼ��
    ModifyStyleEx(WS_EX_APPWINDOW |WS_EX_NOACTIVATE, WS_EX_TOOLWINDOW, SWP_NOACTIVATE);


#if 0  // ��ʱ���ƶ�����, �����Ժ��Ҫ�õ�

    //�������ƶ����������ϡ��������������м䣬���Ϊ��������1/10���߶���С���������ĸ߶�
    HWND  hTaskBarWnd   =::FindWindow(TEXT("HHTaskBar"),   NULL);       // ����������ľ�� 
    CRect  tasKBarRect; 
    ::GetWindowRect(hTaskBarWnd, &tasKBarRect);                         // �������ĳߴ� 
    int width = tasKBarRect.Width()/10;
    int x1 = tasKBarRect.CenterPoint().x - width/2;
    int y1 = tasKBarRect.top;
    int x2 = x1 + width;
    int y2 = tasKBarRect.bottom;
    CRect showPosition;                                                 // ������ʾλ��
    showPosition.left = x1+2;                                           // ����ʾλ�ý���΢��
    showPosition.top = y1+2;
    showPosition.right = x2;
    showPosition.bottom = y2-2;
    //showPosition.MoveToY(tasKBarRect.bottom);                           // �Ƴ���������, ����ʾ

    SetWindowPos(&CWnd::wndTopMost, showPosition.left, showPosition.top, 
    showPosition.Width(), showPosition.Height(), SWP_SHOWWINDOW | SWP_FRAMECHANGED);

#endif


    // һ���������ش���
    SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, NULL);

    //��ֹ���ڻ�ý���
    EnableWindow(FALSE);
    ShowWindow(SW_HIDE);
}

//
//-----------------------------------------------------------------------------
// ��ʼ����ѡ���ִ���
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::KBInitCharselPopupBar ()
{
    m_charselPopupBar.CreateEx(WS_EX_TOOLWINDOW,  WS_POPUP, CRect(0, 0, 0, 0), this, 0);
    m_charselPopupBar.SetOwner(this);

    m_charselPopupBar.SetFont(&m_popupBarFont);

    //��ֹ���ڻ�ý���
    m_charselPopupBar.ModifyStyleEx(0, WS_EX_NOACTIVATE);
    m_charselPopupBar.EnableWindow(FALSE);
    m_charselPopupBar.ShowWindow(SW_HIDE);
}

//
//-----------------------------------------------------------------------------
// ͨ��IPC����, ��õ�ǰ���λ��, ����ڵ�ǰ���㴰��
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::IPCGetFocusCaretPos (HWND curActiveWnd, CPoint& retpos)
{
    if (curActiveWnd)
    {
        // ��ǰ�����, ���ͻ�ȡ���λ����Ϣ
        if (::SendMessage(curActiveWnd, PWID_GET_CARETPOS, NULL, NULL) == PWID_RET_CARETPOS_OK)
        {
            // ��ȷ��Ӧ��, ͨ��ipc��ȡԶ�˴������Ľ��
            DSM800AppInfo  appInfo;
            if (m_appipc.GetDSM800APPInfo(&appInfo))
            {
                retpos = appInfo.caretPos;
                return TRUE;
            }
        }
    }

    return FALSE;
}

//
//-----------------------------------------------------------------------------
// ͨ��IPC����, ���µ�ǰ���뷨״̬, �Ա��������֪
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::IPCUpdateCurInputMode (HWND curActiveWnd, KBInputMode mode)
{
    // ͨ��ipc��״̬���ý��빲���ڴ�
    APP_KBInputMode  afmMode = APP_KB_MODE_UNDEF;
    if (mode == KB_MODE_NUM)
    {
        afmMode = APP_KB_MODE_NUM;
    }
    else if (mode == KB_MODE_LET)
    {
        afmMode = APP_KB_MODE_LET;
    }

    if (m_appipc.SetInputMode(afmMode))
    {
        // ֪ͨԶ�˴��ڵ�ǰ״̬�Ѿ��ı�
        if (curActiveWnd)
        {
            ::SendMessage(curActiveWnd, PWID_UPDATE_INPUTMODE, afmMode, NULL);
        }

        return TRUE;
    }

    return FALSE;
}

//
//-----------------------------------------------------------------------------
// ��ʼ��IPC����
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::IPCInitData ()
{
    // ��һ������, ��ʼ��app��Ϣ��������
    IPCUpdateCurInputMode(GetTopLevelForegroundWindow(), m_inputMethod.GetCurInputMode());
}

//
//-----------------------------------------------------------------------------
// ������ĵ�ǰ���������(���洰�ڵ��Ӵ���)
//-----------------------------------------------------------------------------
//
HWND  CKeyboardAuxDlg::GetTopLevelForegroundWindow ()
{
    HWND  prevWnd;
    HWND  hwnd = ::GetForegroundWindow();
    if (!hwnd)
    {
        return NULL;
    }

    do
    {
        prevWnd = hwnd;
        hwnd = ::GetParent(hwnd);
    } while (hwnd);

    return prevWnd;
}

//
//-----------------------------------------------------------------------------
// ����retRectFix, ʹ��retRectFix����Ļ��Χ�ڿɼ�, 
//  �����Ҫ����retRectFix��λ��, ����뻷��surroundRect,
//  ������ĳ��λ�õ����Ҽ��˵�, ���ߵ������뷨��ѡ����
//
//  retRectFix -- In,Out���Ͳ���, ��Ҫ�����ľ���
//-----------------------------------------------------------------------------
//
static CRect  __GetScreenArea (HMONITOR hMonitor)
{
    MONITORINFO  info;

    if (::GetMonitorInfo(hMonitor, &info))
    {
        return info.rcMonitor;
    }

    return CRect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
}
static CRect  __GetScreenArea (LPCRECT lprcScreenCoords)
{
    return __GetScreenArea(MonitorFromRect(lprcScreenCoords, 0));
}
void  CKeyboardAuxDlg::AdjustWindowRectInsideScreen(const CRect& surroundRect, CRect& retRectFix)
{
    // ������Ļ��������С, ��������λ��, ����պ�����Ļ��Եʱ, �ô�����ʾ����
    CRect  rcWork = __GetScreenArea(retRectFix);
    if (retRectFix.bottom > rcWork.bottom && retRectFix.top > rcWork.CenterPoint().y)
    {
        retRectFix.OffsetRect(0, - retRectFix.Height() - surroundRect.Height());
    }
    if (retRectFix.left < rcWork.left)
    {
        retRectFix.OffsetRect(rcWork.left - retRectFix.left, 0);
    }
    if (retRectFix.right > rcWork.right)
    {
        retRectFix.OffsetRect(rcWork.right - retRectFix.right, 0);
    }
}