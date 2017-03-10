#pragma once

#include "KBDefines.h"


#define KB_CHARSEL_WAIT_TIMEOUT  1000           // ����ѡ���ַ�ʱ�ĳ�ʱ�ȴ�ʱ��

//
//-----------------------------------------------------------------------------
// ����ģʽ
//-----------------------------------------------------------------------------
//
enum KBInputMode
{
    KB_MODE_NUM,            // ��������ģʽ
    KB_MODE_LET,            // ��ĸ����ģʽ
    KB_MODE_UNDEF = -1
};

//
//-----------------------------------------------------------------------------
// ����״̬: ״̬��
//-----------------------------------------------------------------------------
//
enum KBInputState
{
    KB_STATE_NORMAL,        // ��ǰ��������������ģʽ, û�е����ַ�ѡ���
    KB_STATE_CHARSEL,       // ��ǰ�������ַ�����ģʽ, �Ѿ������ַ�ѡ���
    KB_STATE_UNDEF = -1
};


//
//-----------------------------------------------------------------------------
// ���뷨��UI�Ľӿ�, ͨ���ýӿڿ��Բ���UI
//  ���ӿ������ڷ�UI�߳�, ���Ҫ����UI, ��ʵ����Ҫͨ��һ�����Ʒ���Ϣ��UI�߳�
//-----------------------------------------------------------------------------
//
class IKBInputMethodUI
{
public:
    virtual void  _ShowCharSelView (HWND targetWnd, LPCTSTR charsOptional, int curSel) = 0;
    virtual void  _HideCharSelView () = 0;
    virtual void  _UpdateInputMode (KBInputMode mode) = 0;

    // ��Դ�����溯��
    virtual BOOL  _PowerIsBackLightOff () =0;
    virtual void  _PowerWakeupBackLight () =0;
};


//
//-----------------------------------------------------------------------------
// ���뷨, ������ɨ�������, �������뷨�Ĺ������, 
//  �����û������뷢�͸����㴰��
//
// �߳�ģ��: 
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
    KBInputMode             m_curInputMode;     // ��ǰ������ģʽ
    KBInputState            m_curInputState;    // ��ǰ������״̬
    CKBDrvOper*             m_kbDrvOper;        // ������������
    IKBInputMethodUI*       m_imui;             // �����뷨���UI�ӿ�, ͨ���ýӿڿ��Բ���UI

    const KBScancodeInfo*   m_curCharKeyInfo;   // �������KB_STATE_CHARSEL, ���ŵ�ǰ�Ŀ�ѡ�ַ���Ϣ
    int                     m_curCharSel;       // �������KB_STATE_CHARSEL, ��ǰ����ѡ��Ľ����ַ�
};