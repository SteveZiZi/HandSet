#include "stdafx.h"

#include "DbgKBHook.h"




#define WH_JOURNALRECORD    0 
#define WH_JOURNALPLAYBACK  1 
#define WH_KEYBOARD_LL      20 
#define HC_ACTION           0 

typedef struct {
    DWORD                   vkCode;
    DWORD                   scanCode;
    DWORD                   flags;
    DWORD                   time;
    ULONG_PTR               dwExtraInfo;
}   KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;

typedef LRESULT  (CALLBACK* HOOKPROC) (int code, WPARAM wParam, LPARAM lParam);
typedef HHOOK  (WINAPI *_SetWindowsHookExW) (int, HOOKPROC, HINSTANCE, DWORD);
typedef LRESULT  (WINAPI *_CallNextHookEx) (HHOOK, int, WPARAM, LPARAM);
typedef LRESULT  (WINAPI *_UnhookWindowsHookEx) (HHOOK);

static _SetWindowsHookExW   SetHook = NULL; 
static _UnhookWindowsHookEx UnhookHook = NULL; 
static _CallNextHookEx      CallNextHook = NULL; 

static HINSTANCE            g_hHookApiDll   = NULL;
static HHOOK                g_hKbdHook      = NULL;
static IKBTestKeyReciver*   g_keyReciver    = NULL;

LRESULT CALLBACK  KbdHookCallback (int nCode, WPARAM wParam, LPARAM lParam);
BOOL  KbdActivateHook (HOOKPROC HookCallback); 
BOOL  KbdDeactivateHook (); 


BOOL  KbdActivateHook (HOOKPROC hookProc) 
{
    SetHook          = NULL; 
    CallNextHook     = NULL; 
    UnhookHook        = NULL; 


    g_hHookApiDll = LoadLibrary(_T("coredll.dll")); 
    if (NULL == g_hHookApiDll) 
    { 
        return FALSE; 
    } 

    SetHook = (_SetWindowsHookExW)GetProcAddress(g_hHookApiDll, _T("SetWindowsHookExW")); 
    if (NULL == SetHook) 
    { 
        return FALSE; 
    } 

    CallNextHook = (_CallNextHookEx)GetProcAddress(g_hHookApiDll, _T("CallNextHookEx")); 
    if (NULL == CallNextHook) 
    { 
        return FALSE; 
    } 

    UnhookHook = (_UnhookWindowsHookEx)GetProcAddress(g_hHookApiDll, _T("UnhookWindowsHookEx")); 
    if (NULL == UnhookHook) 
    { 
        return FALSE; 
    } 


    g_hKbdHook = SetHook(WH_KEYBOARD_LL, hookProc, NULL, 0);

    return TRUE; 
} 

BOOL  KbdDeactivateHook () 
{ 

    if (g_hKbdHook != NULL) 
    { 
        UnhookHook(g_hKbdHook); 
        g_hKbdHook = NULL; 
    } 

    if (g_hHookApiDll != NULL) 
    { 
        FreeLibrary(g_hHookApiDll); 
        g_hHookApiDll = NULL; 
    } 
    return TRUE; 
} 

LRESULT CALLBACK  KbdHookCallback (int nCode, WPARAM wParam, LPARAM lParam) 
{ 
    if (nCode >= HC_ACTION) 
    { 
        if (wParam == WM_KEYDOWN || wParam == WM_KEYUP)
        { 
            DWORD dwKey = ((KBDLLHOOKSTRUCT*)lParam)->vkCode; 
            if (((KBDLLHOOKSTRUCT*)lParam)->scanCode &&  // 用scanCode保证按键是物理按键按下, 而不是keybd_event产生的
                dwKey != VK_NUMLOCK)  
            { 
                //在这里对按键操作进行处理 
                KB_DEBUGLOG(L"hook hit\n");
                
                // 向host窗口发送测试按键消息
                ASSERT(g_keyReciver);
                if (wParam == WM_KEYDOWN)
                {
                    g_keyReciver->_OnTestKeyDown(dwKey, 0, 0);
                }
                else if (wParam == WM_KEYUP)
                {
                    g_keyReciver->_OnTestKeyUp(dwKey, 0, 0);
                }

                /*
                if (wParam == WM_KEYDOWN) keybd_event('R',     0, 0 ,0);  // 可以发送虚拟键, 不会造成递归
                if (wParam == WM_KEYUP) keybd_event('R',     0, KEYEVENTF_KEYUP ,0);
                */

                return 1;  // 该消息已经被处理过
            } 
        } 
    } 

    return CallNextHook(g_hKbdHook, nCode, wParam, lParam);
}


BOOL  InstallDbgHookKeyboard (IKBTestKeyReciver* keyReciver)
{
#ifdef _DEBUG

    ASSERT(!g_keyReciver);

    if (KbdActivateHook(KbdHookCallback))
    {
        g_keyReciver = keyReciver;
        return TRUE;
    }
    else
    {
        return FALSE;
    }

#endif // _DEBUG

    return TRUE;
}

void  UninstallDbgHookKeyboard ()
{
#ifdef _DEBUG

    ASSERT(g_keyReciver);

    g_keyReciver = NULL;
    KbdDeactivateHook();

#endif // _DEBUG
}