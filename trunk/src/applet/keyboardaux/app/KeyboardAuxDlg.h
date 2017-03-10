#pragma once

#include "src/include/p_wndusermsg_def.h"
#include "src/include/p_filemapping_def.h"
#include "../kbauxcore/KBAuxCore.h"
#include "CharSelPopupBar.h"
#include "AppInfoIPC.h"
#include "../bklDetect/bklDetect.h"


//
//-----------------------------------------------------------------------------
// ���뷨������
//-----------------------------------------------------------------------------
//
class CKeyboardAuxDlg : public CDialog, public IKBTestKeyReciver, public IKBInputMethodUI
{
public:
    //
    // ��������
    //
    enum { IDD = IDD_KEYBOARDAUX_DIALOG };
    CKeyboardAuxDlg (CWnd* pParent = NULL);


public:
    //
    // ʵ��IKBTestKeyReciver�ӿ�
    //
    virtual void  _OnTestKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual void  _OnTestKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
public:
    //
    // ʵ��IKBInputMethodUI�ӿ�
    //
    virtual void  _ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel);
    virtual void  _HideCharSelView ();
    virtual void  _UpdateInputMode (KBInputMode mode);

    virtual BOOL  _PowerIsBackLightOff ();
    virtual void  _PowerWakeupBackLight ();


protected:
    //
    // ��Ϣ����
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
    // ����
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
    // �ڲ�����
    //
    HICON                   m_hIcon;            // ??
    CAppInfoExchgIPC        m_appipc;           // ����ǰ״̬��ȡ���ýӿ�
    CKBDrvOper              m_kboper;           // ������������
    CKBInputMethod          m_inputMethod;      // ���뷨����, ����ǰ����״̬���ַ�ת��
    CCharSelPopupBar        m_charselPopupBar;  // ��ѡ�ַ�����
    CFont                   m_popupBarFont;     // ��ǰ��ʾ������
    
public:
    DWORD                   m_powerIsBackLightOff;  // ��ǰ����״̬,
    CBKLMonitor             m_bklMonitor;
};
