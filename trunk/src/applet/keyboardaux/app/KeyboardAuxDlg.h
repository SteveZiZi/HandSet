#pragma once

#include "src/include/p_wndusermsg_def.h"
#include "src/include/p_filemapping_def.h"
#include "../kbauxcore/KBAuxCore.h"
#include "CharSelPopupBar.h"
#include "AppInfoIPC.h"
#include "../bklDetect/bklDetect.h"


//
//-----------------------------------------------------------------------------
// 输入法主窗口
//-----------------------------------------------------------------------------
//
class CKeyboardAuxDlg : public CDialog, public IKBTestKeyReciver, public IKBInputMethodUI
{
public:
    //
    // 创建构造
    //
    enum { IDD = IDD_KEYBOARDAUX_DIALOG };
    CKeyboardAuxDlg (CWnd* pParent = NULL);


public:
    //
    // 实现IKBTestKeyReciver接口
    //
    virtual void  _OnTestKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void  _OnTestKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
public:
    //
    // 实现IKBInputMethodUI接口
    //
    virtual void  _ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel);
    virtual void  _HideCharSelView ();
    virtual void  _UpdateInputMode (KBInputMode mode);

    virtual BOOL  _PowerIsBackLightOff ();
    virtual void  _PowerWakeupBackLight ();


protected:
    //
    // 消息处理
    //
	DECLARE_MESSAGE_MAP()

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
    afx_msg void  OnSize (UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif

    afx_msg void  OnTimer (UINT_PTR nIDEvent);
    afx_msg LRESULT  OnShowCharselView (WPARAM, LPARAM);
    afx_msg LRESULT  OnHideCharselView (WPARAM, LPARAM);
    afx_msg LRESULT  OnUpdateInputMode (WPARAM, LPARAM);

    virtual BOOL  OnInitDialog ();
    virtual void  DoDataExchange (CDataExchange* pDX);

    static UINT BackLightOffDetectProc(LPVOID lpArg);


private:
    //
    // 辅助
    //
    void  KBInitMainGUI ();
    void  KBInitCharselPopupBar ();
    BOOL  IPCGetFocusCaretPos (HWND curActiveWnd, CPoint& retpos);
    BOOL  IPCUpdateCurInputMode (HWND curActiveWnd, KBInputMode mode);
    void  IPCInitData ();
    HWND  GetTopLevelForegroundWindow ();
    void  AdjustWindowRectInsideScreen(const CRect& surroundRect, CRect& retRectFix);
    

private:
    //
    // 内部数据
    //
    HICON                   m_hIcon;            // ??
    CAppInfoExchgIPC        m_appipc;           // 程序当前状态获取设置接口
    CKBDrvOper              m_kboper;           // 驱动操作对象
    CKBInputMethod          m_inputMethod;      // 输入法对象, 管理当前输入状态及字符转换
    CCharSelPopupBar        m_charselPopupBar;  // 候选字符窗口
    CFont                   m_popupBarFont;     // 当前显示的字体
    
public:
    DWORD                   m_powerIsBackLightOff;  // 当前背光状态,
    CBKLMonitor             m_bklMonitor;
};
