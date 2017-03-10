#include "stdafx.h"

#include "KBAuxCore.h"

//
//-----------------------------------------------------------------------------
// 引入头文件未定义的API
//-----------------------------------------------------------------------------
//
extern "C" {
    typedef struct tagGetForegroundInfo
    {
        HWND    hwndActive;
        HWND    hwndFocus;
        HWND    hwndMenu;
        HWND    hwndKeyboardDest;
        DWORD    fdwConversion;
        DWORD    fdwSentence;
        BOOL    fOpen;
        DWORD    dwCompStrLen;
        HKL        KeyboardLayoutHandle;
    } GET_FOREGROUND_INFO;


    BOOL
        WINAPI
        GetForegroundInfo(
        GET_FOREGROUND_INFO    *pgfi
        );
};



CKBInputMethod::CKBInputMethod ()
{
    m_curInputMode = KB_MODE_NUM;
    m_curInputState = KB_STATE_NORMAL;
    m_kbDrvOper = NULL;
    m_imui = NULL;

    m_curCharKeyInfo = NULL;
    m_curCharSel = -1;
}
CKBInputMethod::~CKBInputMethod ()
{

}

//
//-----------------------------------------------------------------------------
// 启动输入法处理过程
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::StartInputMethod (CKBDrvOper* kbDrvOper, IKBInputMethodUI* imui)
{
    // 不能重复启动
    ASSERT(!m_kbDrvOper && kbDrvOper);
    m_kbDrvOper = kbDrvOper;
    m_imui = imui;

    AfxBeginThread(KBInputMethodMain, this);
}

//
//-----------------------------------------------------------------------------
// 输入法等待按键输入并分发主线程
//-----------------------------------------------------------------------------
//
UINT  CKBInputMethod::KBInputMethodMain (LPVOID param)
{
    CKBInputMethod*  kbInputMethod = (CKBInputMethod*)param;
    ASSERT(kbInputMethod);

    CKBDrvOper*  kbDrvOper = kbInputMethod->m_kbDrvOper;
    ASSERT(kbDrvOper);

    ::CeSetThreadPriority(::GetCurrentThread(), 109);

    UINT8  keyScancode = NULL;
    while (1)
    {
        // 处理输入法在用户选择状态时, 超时等待并默认用户的输入
        if (kbInputMethod->m_curInputState == KB_STATE_CHARSEL)
        {
            if (kbDrvOper->WaitForReadInputScancode(keyScancode, KB_CHARSEL_WAIT_TIMEOUT) == WAIT_TIMEOUT)
            {
                // 用户长时间不输入, 默认用户确认当前的输入, 模拟Enter按键发出
                kbInputMethod->HandleKeyInputMain(KB_SCODE_MAKE(KB_KEYID_ENTER, 0));
            }
            else
            {
                kbInputMethod->HandleKeyInputMain(keyScancode);
            }
        }
        else
        {
            // 非字符选择状态, 正常的无限等待用户输入
            kbDrvOper->WaitForReadInputScancode(keyScancode);
            if (kbInputMethod->m_imui->_PowerIsBackLightOff())
            {
                kbInputMethod->m_imui->_PowerWakeupBackLight();
                UINT8 uKey = keyScancode;
                while (WAIT_OBJECT_0 == kbDrvOper->WaitForReadInputScancode(keyScancode, 5000)) {
                    if (keyScancode != uKey) {
                        break;
                    }
                }
                kbDrvOper->WaitForReadInputScancode(keyScancode);
            }

            kbInputMethod->HandleKeyInputMain(keyScancode);
        }        
    }

    return 0;
}

//
//-----------------------------------------------------------------------------
// 输入按键处理主过程
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleKeyInputMain (UINT8 keyScancode)
{
    const KBScancodeInfo*  scodeInfo = KBFindKeyFromScancode(keyScancode);
    ASSERT(scodeInfo);
    BOOL  isKeyUp = KB_SCODE_ISKEYUP(keyScancode);

    // 簿记当前按键的按压状态
    if (isKeyUp)
    {
        KBSetKeyCurPressState(keyScancode, KB_PRESS_UP);
    }
    else
    {
        KBSetKeyCurPressState(keyScancode, KB_PRESS_DOWN);
    }

    // 按键消息分发
    switch (m_curInputState)
    {
    case KB_STATE_NORMAL:
        HandleNormalKeyInput(scodeInfo, isKeyUp);
        break;
    case KB_STATE_CHARSEL:
        HandleCharselKeyInput(scodeInfo, isKeyUp);
        break;
    default:
        ASSERT(0);
        break;
    }

    KB_DEBUGLOG(L"ReadKBInputProc: %d\n", keyScancode);
}

//
//-----------------------------------------------------------------------------
// 获得当前的输入模式
//-----------------------------------------------------------------------------
//
KBInputMode  CKBInputMethod::GetCurInputMode ()
{
    return m_curInputMode;
}


//
//-----------------------------------------------------------------------------
// 处理正常模式下的按键输入
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleNormalKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{

    LPCTSTR  charsOptional;
    if (IsCharKey(scodeInfo, charsOptional))
    {
        HandleNormalCharKeyInput(scodeInfo, isKeyUp, charsOptional);
    }
    else
    {
        HandleNormalControlKeyInput(scodeInfo, isKeyUp);
    }
}
//
//-----------------------------------------------------------------------------
// 处理控制按键的输入
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleNormalControlKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    
    if (isKeyUp)
    {
        // 模拟键盘按键
        keybd_event(scodeInfo->nChar,     0, KEYEVENTF_KEYUP ,0);
    }
    else
    {
        //
        // 处理组合按键
        //
        if (scodeInfo->keyid == KB_KEYID_F4)
        {
            // 当用户同时按下Shift+F4, 退出主程序, 并退出键盘程序(调试状态不退出键盘程序)
            const KBScancodeInfo*  kbF1Info = KBFindKeyFromScancode(KB_SCODE_MAKE(KB_KEYID_SHIFT, 0));
            ASSERT(kbF1Info);

            if (kbF1Info->curPressState == KB_PRESS_DOWN)
            {

                KB_DEBUGLOG(L"kbaux trigger app quit\n");

                // 模拟键盘按键VK_F7按下, 目前主程序是靠F7按键退出
                keybd_event(VK_F7,     0, 0 ,0);

#ifndef _DEBUG
                // 退出当键盘程序
                ::ExitProcess(0);
#endif
            }
        }
        else if (scodeInfo->keyid == KB_KEYID_HOME)
        {
            // 当用户同时按下Shift+HOME, 进行快速截屏
            const KBScancodeInfo*  kbF1Info = KBFindKeyFromScancode(KB_SCODE_MAKE(KB_KEYID_SHIFT, 0));
            ASSERT(kbF1Info);

            if (kbF1Info->curPressState == KB_PRESS_DOWN)
            {

                KB_DEBUGLOG(L"kbaux print screen\n");

                // 模拟键盘按键VK_SNAPSHOT按键按下, 
                keybd_event(VK_SNAPSHOT,     0, 0 ,0);
                return;
            }
        }


        //
        // 处理单个按键消息, 并处理特殊按键消息
        //
        if (scodeInfo->keyid == KB_KEYID_SHIFT)
        {
            // 切换输入模式
            if (m_curInputMode == KB_MODE_NUM)
            {
                m_curInputMode = KB_MODE_LET;
            }
            else
            {
                m_curInputMode = KB_MODE_NUM;
            }

            // 通知UI窗口
            if (m_imui)
            {
                m_imui->_UpdateInputMode(m_curInputMode);
            }
        }
        else if (scodeInfo->keyid == KB_KEYID_TESTQUIT)
        {
            // 当用户按下101键盘的Q键时, 退出程序
            ::ExitProcess(0);
        }
        else
        {
            // 模拟键盘按键
            keybd_event(scodeInfo->nChar,     0, 0 ,0);
        }
    }
}
//
//-----------------------------------------------------------------------------
// 处理字符输入
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleNormalCharKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp, LPCTSTR charsOptional)
{
    if (isKeyUp)
    {
    }
    else
    {
//         // 当用户输入字符按键的时候, 并不会向系统发送keybd_event
//         // 为了维持系统输入心跳, 防止电源管理关闭背光, 这里加入心跳维持机制
//         KeepInputTick(isKeyUp);

        BeginCharSel(scodeInfo, charsOptional);
    }
}


//
//-----------------------------------------------------------------------------
// 处理字符选择按键输入
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleCharselKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    if (isKeyUp)
    {
    }
    else
    {
//         // 当用户输入字符按键的时候, 并不会向系统发送keybd_event
//         // 为了维持系统输入心跳, 防止电源管理关闭背光, 这里加入心跳维持机制
//         KeepInputTick(isKeyUp);

        OperCharSel(scodeInfo, isKeyUp);
    }
}



//
//-----------------------------------------------------------------------------
// 处理字符选择输入, 给用户提供选择
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::BeginCharSel (const KBScancodeInfo* scodeInfo, LPCTSTR charsOptional)
{
    GET_FOREGROUND_INFO  gif;
    GetForegroundInfo(&gif);

    int  charsNum = wcslen(charsOptional);
    if (charsNum > 1)
    {
        // 如果是Edit窗口, 才进入编辑状态
        if (IsEditableWindow(gif.hwndFocus))
        {
            // 弹出UI选择窗口, 给用户选择
            ResetCurState(scodeInfo);

            if (m_imui)
            {
                m_imui->_ShowCharSelView(gif.hwndFocus, charsOptional, m_curCharSel);
            }
        }
    }
    else
    {
        // 向当前焦点窗口发送字符
        if (gif.hwndFocus)
        {
            //--bug4.4 用数字键进入SCL导入页面后, 键盘小程序似乎挂起了, 按任何按键都无效, 但用普通键盘可以激活, 激活后小键盘又能用了, 
            //    原因是我跨进程使用SendMessage WM_CHAR, 但目标进程又在WM_CHAR中进入了模态, 导致SendMessage不能退出, 改成PostMessage就好了
            ::PostMessage(gif.hwndFocus, WM_CHAR, WPARAM(charsOptional[0]), 0);
        }
    }
}
//
//-----------------------------------------------------------------------------
// 操作字符选择
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::OperCharSel (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    GET_FOREGROUND_INFO  gif;
    GetForegroundInfo(&gif);

    LPCTSTR  curCharsOptional;  // 获得当前可选字符集
    VERIFY(IsCharKey(m_curCharKeyInfo, curCharsOptional));
    int  curCharsNum = wcslen(curCharsOptional);

    LPCTSTR  tempCharsOptional;
    if (IsCharKey(scodeInfo, tempCharsOptional) && scodeInfo != m_curCharKeyInfo)
    {
        //
        // 如果用户按下字符按键, 并且与上一个输入的按键不同
        //
        // 确认当前输入向当前焦点窗口发送字符
        if (gif.hwndFocus)
        {
            ASSERT(m_curCharSel >= 0 && m_curCharSel < curCharsNum);
            // 防止SendMessage被其他进程卡死 bug4.4, 
            // 但又考虑到可能会发送字符串过去, 因此还是改回SendMessage, 因为通常进入字符选择模式后, 都是在Edit控件内, 不知道以后会不会有问题
            ::SendMessage(gif.hwndFocus, WM_CHAR, WPARAM(curCharsOptional[m_curCharSel]), 0);
        }

        // 重置状态
        ResetCurState(NULL);

        // 进入一个新的字符输入
        HandleNormalKeyInput(scodeInfo, isKeyUp);
    }
    else
    {
        //
        // 如果用户按下其他的控制按键
        //
        // 控制按键输入, 或当前输入字符按键(相当于控制选择下一个字符)
        if (scodeInfo == m_curCharKeyInfo || 
            scodeInfo->keyid == KB_KEYID_DOWN ||
            scodeInfo->keyid == KB_KEYID_RIGHT)
        {
            m_curCharSel++;
            m_curCharSel = m_curCharSel % curCharsNum;

            if (m_imui)
            {
                m_imui->_ShowCharSelView(gif.hwndFocus, curCharsOptional, m_curCharSel);
            }
        }
        else if (scodeInfo->keyid == KB_KEYID_UP ||
            scodeInfo->keyid == KB_KEYID_LEFT)
        {
            m_curCharSel--;
            if (m_curCharSel < 0)
            {
                m_curCharSel = curCharsNum - 1;
            }

            if (m_imui)
            {
                m_imui->_ShowCharSelView(gif.hwndFocus, curCharsOptional, m_curCharSel);
            }
        }
        else
        {
            // 确认当前输入向当前焦点窗口发送字符
            if (scodeInfo->keyid == KB_KEYID_ENTER && gif.hwndFocus)
            {
                ASSERT(m_curCharSel >= 0 && m_curCharSel < curCharsNum);
                // 防止SendMessage被其他进程卡死 bug4.4
                // 但又考虑到可能会发送字符串过去, 因此还是改回SendMessage, 因为通常进入字符选择模式后, 都是在Edit控件内, 不知道以后会不会有问题
                ::SendMessage(gif.hwndFocus, WM_CHAR, WPARAM(curCharsOptional[m_curCharSel]), 0);
            }

            // 其他按键, 包括ESC或其他的控制按键, 相当于取消当前输入
            if (m_imui)
            {
                m_imui->_HideCharSelView();
            }

            // 重置状态
            ResetCurState(NULL);
        }
    }
}
//
//-----------------------------------------------------------------------------
// 检查在当前模式下, 按键是否一个字符按键, 否则就是一个控制按键
//  如果是字符按键, 则返回可选字符集
//-----------------------------------------------------------------------------
//
BOOL  CKBInputMethod::IsCharKey (const KBScancodeInfo* scodeInfo, LPCTSTR& retCharsOptional)
{
    ASSERT(scodeInfo);

    if (m_curInputMode == KB_MODE_LET && scodeInfo->letModeChars)
    {
        retCharsOptional = scodeInfo->letModeChars;
        return TRUE;
    }
    else if (m_curInputMode == KB_MODE_NUM && scodeInfo->numModeChars)
    {
        retCharsOptional = scodeInfo->numModeChars;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//
//-----------------------------------------------------------------------------
// 重新设置当前的输入状态, 
// 如果传入NULL, 则退出字符选择状态
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::ResetCurState (const KBScancodeInfo* curCharKeyInfo)
{
    if (curCharKeyInfo)
    {
        m_curCharKeyInfo = curCharKeyInfo;
        m_curCharSel = 0;
        m_curInputState = KB_STATE_CHARSEL;
    }
    else
    {
        m_curCharKeyInfo = NULL;
        m_curCharSel = -1;
        m_curInputState = KB_STATE_NORMAL;
    }
}
//
//-----------------------------------------------------------------------------
// 判断目标窗口是否可编辑的窗口
//-----------------------------------------------------------------------------
//
BOOL  CKBInputMethod::IsEditableWindow (HWND targetWnd)
{
    WCHAR  tgrWndClassName[20];
    ::GetClassName(targetWnd, tgrWndClassName, 20);
    if (wcscmp(tgrWndClassName, L"Edit") == 0 || 
        wcscmp(tgrWndClassName, L"WCE_EDIT") == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//
//-----------------------------------------------------------------------------
// 通过某种方式维持系统输入心跳, 防止背光熄灭
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::KeepInputTick (BOOL isKeyUp)
{
    /* 不再使用这种方式唤醒背光, 应为会影响键盘功能, 多发一些没用的VK
    // 发送没有使用的按键维持心跳
    if (isKeyUp)
    {
        // 模拟键盘按键
        keybd_event(VK_F24,     0, KEYEVENTF_KEYUP ,0);
    }
    else
    {
        // 模拟键盘按键
        keybd_event(VK_F24,     0, 0 ,0);
    }
    */

    ASSERT(m_imui);
    
    if (m_imui->_PowerIsBackLightOff())
    {
        m_imui->_PowerWakeupBackLight();
    }
}