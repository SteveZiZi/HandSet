#pragma once

#include "KBDefines.h"


#define KB_CHARSEL_WAIT_TIMEOUT  1000           // 输入选择字符时的超时等待时间

//
//-----------------------------------------------------------------------------
// 输入模式
//-----------------------------------------------------------------------------
//
enum KBInputMode
{
    KB_MODE_NUM,            // 数字输入模式
    KB_MODE_LET,            // 字母输入模式
    KB_MODE_UNDEF = -1
};

//
//-----------------------------------------------------------------------------
// 输入状态: 状态机
//-----------------------------------------------------------------------------
//
enum KBInputState
{
    KB_STATE_NORMAL,        // 当前正处于正常输入模式, 没有弹出字符选择框
    KB_STATE_CHARSEL,       // 当前正处于字符输入模式, 已经弹出字符选择框
    KB_STATE_UNDEF = -1
};


//
//-----------------------------------------------------------------------------
// 输入法与UI的接口, 通过该接口可以操作UI
//  本接口运行在非UI线程, 如果要操作UI, 则实现者要通过一定机制发消息给UI线程
//-----------------------------------------------------------------------------
//
class IKBInputMethodUI
{
public:
    virtual void  _ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel) = 0;
    virtual void  _HideCharSelView () = 0;
    virtual void  _UpdateInputMode (KBInputMode mode) = 0;

    // 电源管理方面函数
    virtual BOOL  _PowerIsBackLightOff () =0;
    virtual void  _PowerWakeupBackLight () =0;
};


//
//-----------------------------------------------------------------------------
// 输入法, 处理按键扫描码接收, 并按输入法的规则解析, 
//  最后把用户的输入发送给焦点窗口
//
// 线程模型: 
//-----------------------------------------------------------------------------
//
class CKBInputMethod
{
public:
    CKBInputMethod ();
    virtual ~CKBInputMethod ();

    static UINT AFX_CDECL  KBInputMethodMain (LPVOID param);
    void  StartInputMethod (CKBDrvOper* kbDrvOper, IKBInputMethodUI* imui = NULL);
    void  HandleKeyInputMain (UINT8 keyScancode);

public:
    KBInputMode  GetCurInputMode ();

protected:
    void  HandleNormalKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp);
    void  HandleNormalControlKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp);
    void  HandleNormalCharKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp, LPCTSTR charsOptional);
    void  HandleCharselKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp);

private:
    void  BeginCharSel (const KBScancodeInfo* scodeInfo, LPCTSTR charsOptional);
    void  OperCharSel (const KBScancodeInfo* scodeInfo, BOOL isKeyUp);
    BOOL  IsCharKey (const KBScancodeInfo* scodeInfo, LPCTSTR& retCharsOptional);
    void  ResetCurState (const KBScancodeInfo* curCharKeyInfo);
    BOOL  IsEditableWindow (HWND targetWnd);
    void  KeepInputTick (BOOL isKeyUp);

private:
    KBInputMode             m_curInputMode;     // 当前的输入模式
    KBInputState            m_curInputState;    // 当前的输入状态
    CKBDrvOper*             m_kbDrvOper;        // 键盘驱动操作
    IKBInputMethodUI*       m_imui;             // 与输入法框的UI接口, 通过该接口可以操作UI

    const KBScancodeInfo*   m_curCharKeyInfo;   // 如果处于KB_STATE_CHARSEL, 则存放当前的可选字符信息
    int                     m_curCharSel;       // 如果处于KB_STATE_CHARSEL, 当前正在选择的焦点字符
};