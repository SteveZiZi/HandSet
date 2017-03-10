// KeyboardAuxDlg.cpp : 实现文件
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
// 用于从输入法线程通知UI线程的消息
//  其中WM_USER_SHOW_CHARSEL_VIEW 传递参数WMParamShowCharselView* 到wParam
//-----------------------------------------------------------------------------
//
#define WM_USER_SHOW_CHARSEL_VIEW   (PRIVATE_WND_USERID + 1)
#define WM_USER_HIDE_CHARSEL_VIEW   (PRIVATE_WND_USERID + 2)
#define WM_USER_UPDATE_INPUTMODE    (PRIVATE_WND_USERID + 3)
struct WMParamShowCharselView
{
    HWND                    targetWnd;          // 当前焦点窗口
    LPCTSTR                 charsOptional;      // 候选字符
    int                     curSel;             // 当前被选中的字符索引
};




CKeyboardAuxDlg::CKeyboardAuxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyboardAuxDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // 创建字体
    
#if 0  // 系统已经加入该字体, 无需自己调用加载
    VERIFY(AddFontResource(L"\\FlashDisk2\\MSYH.TTF"));
#endif
    
    m_popupBarFont.CreateFont(24, 0, 0, 0, 
        FW_NORMAL, FALSE, FALSE, 0, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, FF_ROMAN, 
        _T("微软雅黑"));

    m_powerIsBackLightOff = FALSE;
}


//
//=============================================================================
// 实现IKBTestKeyReciver接口
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// hook真实键盘的的测试按键按下
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_OnTestKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 转发按键消息
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
// hook真实键盘的的测试按键弹起
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_OnTestKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // 转发按键消息
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
// 实现IKBInputMethodUI接口
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 显示候选文字窗口(运行在输入法线程)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel)
{
    // 因为不在同一个线程, 所以发消息通知
    WMParamShowCharselView  param;
    param.targetWnd = targetWnd;
    param.charsOptional = charsOptional;
    param.curSel = curSel;

    SendMessage(WM_USER_SHOW_CHARSEL_VIEW, (WPARAM)&param);
}

//
//-----------------------------------------------------------------------------
// 隐藏候选文字窗口(运行在输入法线程)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_HideCharSelView ()
{
    // 因为不在同一个线程, 所以发消息通知
    SendMessage(WM_USER_HIDE_CHARSEL_VIEW);
}

//
//-----------------------------------------------------------------------------
// 更新当前输入模式(运行在输入法线程)
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::_UpdateInputMode (KBInputMode mode)
{
    SendMessage(WM_USER_UPDATE_INPUTMODE, mode);
}
//
//-----------------------------------------------------------------------------
// 获得当前背光状态, 是否亮灭
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::_PowerIsBackLightOff ()
{
    return m_powerIsBackLightOff;
}
//
//-----------------------------------------------------------------------------
// 唤醒背光
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
// 消息处理
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
// 显示候选文字窗口(运行在UI线程)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnShowCharselView (WPARAM wParam, LPARAM lParam)
{
    WMParamShowCharselView*  showViewParam = (WMParamShowCharselView*)wParam;
    ASSERT(showViewParam);

    // 将候选字符设置入窗口
    m_charselPopupBar.SetCharsOptional(showViewParam->charsOptional, showViewParam->curSel);


    // 根据当前光标位置调整窗口位置, 目前无法获得其他进程的光标位置
    // 只能先用窗口坐标代替, 显示在目标窗口的下中的位置
    CRect  targetRect;
    CRect  popupBarRect;
    ::GetWindowRect(showViewParam->targetWnd, &targetRect);
    m_charselPopupBar.GetWindowRect(&popupBarRect);

    CRect  popupBarRectFix;
    popupBarRectFix.left = targetRect.Width() / 2 + targetRect.left - popupBarRect.Width() / 2;
    popupBarRectFix.top = targetRect.bottom;
    popupBarRectFix.right = popupBarRectFix.left + popupBarRect.Width()+1;
    popupBarRectFix.bottom = popupBarRectFix.top + popupBarRect.Height();
    

#if 0 // 暂时不使用该方法跟随光标, 似乎太过消耗cpu

    // 如果可以获得当前光标位置, 则根据光标位置调整
    CPoint  caretPos;
    if (IPCGetFocusCaretPos(GetTopLevelForegroundWindow(), caretPos))
    {
        ::ClientToScreen(showViewParam->targetWnd, &caretPos);
        popupBarX = caretPos.x;
    }

#endif

    // 调整最终的候选窗口位置, 保证窗口在屏幕可视范围内
    AdjustWindowRectInsideScreen(targetRect, popupBarRectFix);

    
    // 调整窗口位置
    m_charselPopupBar.SetWindowPos(&CWnd::wndTopMost, popupBarRectFix.left, popupBarRectFix.top, 1, 1, 
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    
	// 界面修改
	/********************************************************/
    return 0;
}

//
//-----------------------------------------------------------------------------
// 隐藏候选文字窗口(运行在UI线程)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnHideCharselView (WPARAM wParam, LPARAM lParam)
{
    m_charselPopupBar.ShowWindow(SW_HIDE);

    return 0;
}

//
//-----------------------------------------------------------------------------
// 更新当前输入模式(运行在UI线程)
//-----------------------------------------------------------------------------
//
LRESULT  CKeyboardAuxDlg::OnUpdateInputMode (WPARAM wParam, LPARAM lParam)
{
    // 通知应用程序主窗口切换输入模式
    HWND  curActiveWnd = GetTopLevelForegroundWindow();
    if (curActiveWnd)
    {
        IPCUpdateCurInputMode(curActiveWnd, (KBInputMode)wParam);
    }

    return 0;
}

//
//-----------------------------------------------------------------------------
// 定时清理临时CDC对象
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::OnTimer (UINT_PTR nIDEvent)
{
    KB_DEBUGLOG(L"CKeyboardAuxDlg::OnTimer trigger...\r\n");

    if (nIDEvent == 1)
    {
        // 清除临时对象
        // 参考kb 839859  105286 819635
        AfxGetApp()->OnIdle(1);


        // 定时获得电源状态
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
        // 一启动就隐藏窗口, 本定时器过程只执行一次
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
// 消息处理-虚拟函数
//=============================================================================
//

BOOL CKeyboardAuxDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    // gui
    KBInitMainGUI();
    KBInitCharselPopupBar();

    // other
    if (m_kboper.DeviceInit())
    {
        // 安装键盘钩子, 用于测试用
        InstallDbgHookKeyboard(this);

        // 初始化IPC通信
        VERIFY(m_appipc.IPCInit());

        // 输入法处理线程启动
        m_inputMethod.StartInputMethod(&m_kboper, this);
    }
    else
    {
        // 初始化失败
        AfxMessageBox(L"Maybe another kbaux running!\nPress Q exit");
        ::ExitProcess(0);
        return FALSE;
    }


    // 定时清理临时对象
    SetTimer(1, 10000, NULL);

    // 一启动就隐藏窗口, 本定时器过程只执行一次, 要在OnTimer中Kill
    SetTimer(2, 0, NULL);

    AfxBeginThread(BackLightOffDetectProc, this);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CKeyboardAuxDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 初始化主窗口
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::KBInitMainGUI ()
{
    //从任务栏上去掉程序图标
    ModifyStyleEx(WS_EX_APPWINDOW |WS_EX_NOACTIVATE, WS_EX_TOOLWINDOW, SWP_NOACTIVATE);


#if 0  // 暂时不移动窗口, 可能以后会要用到

    //将窗口移动在任务栏上。窗口在任务栏中间，宽度为任务栏的1/10，高度略小于任务栏的高度
    HWND  hTaskBarWnd   =::FindWindow(TEXT("HHTaskBar"),   NULL);       // 获得任务栏的句柄 
    CRect  tasKBarRect; 
    ::GetWindowRect(hTaskBarWnd, &tasKBarRect);                         // 任务栏的尺寸 
    int width = tasKBarRect.Width()/10;
    int x1 = tasKBarRect.CenterPoint().x - width/2;
    int y1 = tasKBarRect.top;
    int x2 = x1 + width;
    int y2 = tasKBarRect.bottom;
    CRect showPosition;                                                 // 窗口显示位置
    showPosition.left = x1+2;                                           // 对显示位置进行微调
    showPosition.top = y1+2;
    showPosition.right = x2;
    showPosition.bottom = y2-2;
    //showPosition.MoveToY(tasKBarRect.bottom);                           // 移出窗口外面, 不显示

    SetWindowPos(&CWnd::wndTopMost, showPosition.left, showPosition.top, 
    showPosition.Width(), showPosition.Height(), SWP_SHOWWINDOW | SWP_FRAMECHANGED);

#endif


    // 一启动就隐藏窗口
    SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, NULL);

    //禁止窗口获得焦点
    EnableWindow(FALSE);
    ShowWindow(SW_HIDE);
}

//
//-----------------------------------------------------------------------------
// 初始化候选文字窗口
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::KBInitCharselPopupBar ()
{
    m_charselPopupBar.CreateEx(WS_EX_TOOLWINDOW,  WS_POPUP, CRect(0, 0, 0, 0), this, 0);
    m_charselPopupBar.SetOwner(this);

    m_charselPopupBar.SetFont(&m_popupBarFont);

    //禁止窗口获得焦点
    m_charselPopupBar.ModifyStyleEx(0, WS_EX_NOACTIVATE);
    m_charselPopupBar.EnableWindow(FALSE);
    m_charselPopupBar.ShowWindow(SW_HIDE);
}

//
//-----------------------------------------------------------------------------
// 通过IPC机制, 获得当前光标位置, 相对于当前焦点窗口
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::IPCGetFocusCaretPos (HWND curActiveWnd, CPoint& retpos)
{
    if (curActiveWnd)
    {
        // 向当前活动窗口, 发送获取光标位置消息
        if (::SendMessage(curActiveWnd, PWID_GET_CARETPOS, NULL, NULL) == PWID_RET_CARETPOS_OK)
        {
            // 正确响应后, 通过ipc获取远端传过来的结果
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
// 通过IPC机制, 更新当前输入法状态, 以便主程序获知
//-----------------------------------------------------------------------------
//
BOOL  CKeyboardAuxDlg::IPCUpdateCurInputMode (HWND curActiveWnd, KBInputMode mode)
{
    // 通过ipc将状态设置进入共享内存
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
        // 通知远端窗口当前状态已经改变
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
// 初始化IPC数据
//-----------------------------------------------------------------------------
//
void  CKeyboardAuxDlg::IPCInitData ()
{
    // 第一次启动, 初始化app信息交换数据
    IPCUpdateCurInputMode(GetTopLevelForegroundWindow(), m_inputMethod.GetCurInputMode());
}

//
//-----------------------------------------------------------------------------
// 获得最顶层的当前活动的主窗口(桌面窗口的子窗口)
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
// 调整retRectFix, 使得retRectFix在屏幕范围内可见, 
//  如果需要调整retRectFix的位置, 则必须环绕surroundRect,
//  比如在某个位置弹出右键菜单, 或者弹出输入法候选窗口
//
//  retRectFix -- In,Out类型参数, 需要修正的矩形
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
    // 根据屏幕工作区大小, 调整窗口位置, 比如刚好在屏幕边缘时, 让窗口显示完整
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