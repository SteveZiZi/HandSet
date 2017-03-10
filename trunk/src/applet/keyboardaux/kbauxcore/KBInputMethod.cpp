#include "stdafx.h"

#include "KBAuxCore.h"

//
//-----------------------------------------------------------------------------
// ����ͷ�ļ�δ�����API
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
// �������뷨�������
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::StartInputMethod (CKBDrvOper* kbDrvOper, IKBInputMethodUI* imui)
{
    // �����ظ�����
    ASSERT(!m_kbDrvOper && kbDrvOper);
    m_kbDrvOper = kbDrvOper;
    m_imui = imui;

    AfxBeginThread(KBInputMethodMain, this);
}

//
//-----------------------------------------------------------------------------
// ���뷨�ȴ��������벢�ַ����߳�
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
        // �������뷨���û�ѡ��״̬ʱ, ��ʱ�ȴ���Ĭ���û�������
        if (kbInputMethod->m_curInputState == KB_STATE_CHARSEL)
        {
            if (kbDrvOper->WaitForReadInputScancode(keyScancode, KB_CHARSEL_WAIT_TIMEOUT) == WAIT_TIMEOUT)
            {
                // �û���ʱ�䲻����, Ĭ���û�ȷ�ϵ�ǰ������, ģ��Enter��������
                kbInputMethod->HandleKeyInputMain(KB_SCODE_MAKE(KB_KEYID_ENTER, 0));
            }
            else
            {
                kbInputMethod->HandleKeyInputMain(keyScancode);
            }
        }
        else
        {
            // ���ַ�ѡ��״̬, ���������޵ȴ��û�����
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
// ���밴������������
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleKeyInputMain (UINT8 keyScancode)
{
    const KBScancodeInfo*  scodeInfo = KBFindKeyFromScancode(keyScancode);
    ASSERT(scodeInfo);
    BOOL  isKeyUp = KB_SCODE_ISKEYUP(keyScancode);

    // ���ǵ�ǰ�����İ�ѹ״̬
    if (isKeyUp)
    {
        KBSetKeyCurPressState(keyScancode, KB_PRESS_UP);
    }
    else
    {
        KBSetKeyCurPressState(keyScancode, KB_PRESS_DOWN);
    }

    // ������Ϣ�ַ�
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
// ��õ�ǰ������ģʽ
//-----------------------------------------------------------------------------
//
KBInputMode  CKBInputMethod::GetCurInputMode ()
{
    return m_curInputMode;
}


//
//-----------------------------------------------------------------------------
// ��������ģʽ�µİ�������
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
// ������ư���������
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleNormalControlKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    
    if (isKeyUp)
    {
        // ģ����̰���
        keybd_event(scodeInfo->nChar,     0, KEYEVENTF_KEYUP ,0);
    }
    else
    {
        //
        // ������ϰ���
        //
        if (scodeInfo->keyid == KB_KEYID_F4)
        {
            // ���û�ͬʱ����Shift+F4, �˳�������, ���˳����̳���(����״̬���˳����̳���)
            const KBScancodeInfo*  kbF1Info = KBFindKeyFromScancode(KB_SCODE_MAKE(KB_KEYID_SHIFT, 0));
            ASSERT(kbF1Info);

            if (kbF1Info->curPressState == KB_PRESS_DOWN)
            {

                KB_DEBUGLOG(L"kbaux trigger app quit\n");

                // ģ����̰���VK_F7����, Ŀǰ�������ǿ�F7�����˳�
                keybd_event(VK_F7,     0, 0 ,0);

#ifndef _DEBUG
                // �˳������̳���
                ::ExitProcess(0);
#endif
            }
        }
        else if (scodeInfo->keyid == KB_KEYID_HOME)
        {
            // ���û�ͬʱ����Shift+HOME, ���п��ٽ���
            const KBScancodeInfo*  kbF1Info = KBFindKeyFromScancode(KB_SCODE_MAKE(KB_KEYID_SHIFT, 0));
            ASSERT(kbF1Info);

            if (kbF1Info->curPressState == KB_PRESS_DOWN)
            {

                KB_DEBUGLOG(L"kbaux print screen\n");

                // ģ����̰���VK_SNAPSHOT��������, 
                keybd_event(VK_SNAPSHOT,     0, 0 ,0);
                return;
            }
        }


        //
        // ������������Ϣ, ���������ⰴ����Ϣ
        //
        if (scodeInfo->keyid == KB_KEYID_SHIFT)
        {
            // �л�����ģʽ
            if (m_curInputMode == KB_MODE_NUM)
            {
                m_curInputMode = KB_MODE_LET;
            }
            else
            {
                m_curInputMode = KB_MODE_NUM;
            }

            // ֪ͨUI����
            if (m_imui)
            {
                m_imui->_UpdateInputMode(m_curInputMode);
            }
        }
        else if (scodeInfo->keyid == KB_KEYID_TESTQUIT)
        {
            // ���û�����101���̵�Q��ʱ, �˳�����
            ::ExitProcess(0);
        }
        else
        {
            // ģ����̰���
            keybd_event(scodeInfo->nChar,     0, 0 ,0);
        }
    }
}
//
//-----------------------------------------------------------------------------
// �����ַ�����
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleNormalCharKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp, LPCTSTR charsOptional)
{
    if (isKeyUp)
    {
    }
    else
    {
//         // ���û������ַ�������ʱ��, ��������ϵͳ����keybd_event
//         // Ϊ��ά��ϵͳ��������, ��ֹ��Դ����رձ���, �����������ά�ֻ���
//         KeepInputTick(isKeyUp);

        BeginCharSel(scodeInfo, charsOptional);
    }
}


//
//-----------------------------------------------------------------------------
// �����ַ�ѡ�񰴼�����
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::HandleCharselKeyInput (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    if (isKeyUp)
    {
    }
    else
    {
//         // ���û������ַ�������ʱ��, ��������ϵͳ����keybd_event
//         // Ϊ��ά��ϵͳ��������, ��ֹ��Դ����رձ���, �����������ά�ֻ���
//         KeepInputTick(isKeyUp);

        OperCharSel(scodeInfo, isKeyUp);
    }
}



//
//-----------------------------------------------------------------------------
// �����ַ�ѡ������, ���û��ṩѡ��
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::BeginCharSel (const KBScancodeInfo* scodeInfo, LPCTSTR charsOptional)
{
    GET_FOREGROUND_INFO  gif;
    GetForegroundInfo(&gif);

    int  charsNum = wcslen(charsOptional);
    if (charsNum > 1)
    {
        // �����Edit����, �Ž���༭״̬
        if (IsEditableWindow(gif.hwndFocus))
        {
            // ����UIѡ�񴰿�, ���û�ѡ��
            ResetCurState(scodeInfo);

            if (m_imui)
            {
                m_imui->_ShowCharSelView(gif.hwndFocus, charsOptional, m_curCharSel);
            }
        }
    }
    else
    {
        // ��ǰ���㴰�ڷ����ַ�
        if (gif.hwndFocus)
        {
            //--bug4.4 �����ּ�����SCL����ҳ���, ����С�����ƺ�������, ���κΰ�������Ч, ������ͨ���̿��Լ���, �����С������������, 
            //    ԭ�����ҿ����ʹ��SendMessage WM_CHAR, ��Ŀ���������WM_CHAR�н�����ģ̬, ����SendMessage�����˳�, �ĳ�PostMessage�ͺ���
            ::PostMessage(gif.hwndFocus, WM_CHAR, WPARAM(charsOptional[0]), 0);
        }
    }
}
//
//-----------------------------------------------------------------------------
// �����ַ�ѡ��
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::OperCharSel (const KBScancodeInfo* scodeInfo, BOOL isKeyUp)
{
    GET_FOREGROUND_INFO  gif;
    GetForegroundInfo(&gif);

    LPCTSTR  curCharsOptional;  // ��õ�ǰ��ѡ�ַ���
    VERIFY(IsCharKey(m_curCharKeyInfo, curCharsOptional));
    int  curCharsNum = wcslen(curCharsOptional);

    LPCTSTR  tempCharsOptional;
    if (IsCharKey(scodeInfo, tempCharsOptional) && scodeInfo != m_curCharKeyInfo)
    {
        //
        // ����û������ַ�����, ��������һ������İ�����ͬ
        //
        // ȷ�ϵ�ǰ������ǰ���㴰�ڷ����ַ�
        if (gif.hwndFocus)
        {
            ASSERT(m_curCharSel >= 0 && m_curCharSel < curCharsNum);
            // ��ֹSendMessage���������̿��� bug4.4, 
            // ���ֿ��ǵ����ܻᷢ���ַ�����ȥ, ��˻��ǸĻ�SendMessage, ��Ϊͨ�������ַ�ѡ��ģʽ��, ������Edit�ؼ���, ��֪���Ժ�᲻��������
            ::SendMessage(gif.hwndFocus, WM_CHAR, WPARAM(curCharsOptional[m_curCharSel]), 0);
        }

        // ����״̬
        ResetCurState(NULL);

        // ����һ���µ��ַ�����
        HandleNormalKeyInput(scodeInfo, isKeyUp);
    }
    else
    {
        //
        // ����û����������Ŀ��ư���
        //
        // ���ư�������, ��ǰ�����ַ�����(�൱�ڿ���ѡ����һ���ַ�)
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
            // ȷ�ϵ�ǰ������ǰ���㴰�ڷ����ַ�
            if (scodeInfo->keyid == KB_KEYID_ENTER && gif.hwndFocus)
            {
                ASSERT(m_curCharSel >= 0 && m_curCharSel < curCharsNum);
                // ��ֹSendMessage���������̿��� bug4.4
                // ���ֿ��ǵ����ܻᷢ���ַ�����ȥ, ��˻��ǸĻ�SendMessage, ��Ϊͨ�������ַ�ѡ��ģʽ��, ������Edit�ؼ���, ��֪���Ժ�᲻��������
                ::SendMessage(gif.hwndFocus, WM_CHAR, WPARAM(curCharsOptional[m_curCharSel]), 0);
            }

            // ��������, ����ESC�������Ŀ��ư���, �൱��ȡ����ǰ����
            if (m_imui)
            {
                m_imui->_HideCharSelView();
            }

            // ����״̬
            ResetCurState(NULL);
        }
    }
}
//
//-----------------------------------------------------------------------------
// ����ڵ�ǰģʽ��, �����Ƿ�һ���ַ�����, �������һ�����ư���
//  ������ַ�����, �򷵻ؿ�ѡ�ַ���
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
// �������õ�ǰ������״̬, 
// �������NULL, ���˳��ַ�ѡ��״̬
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
// �ж�Ŀ�괰���Ƿ�ɱ༭�Ĵ���
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
// ͨ��ĳ�ַ�ʽά��ϵͳ��������, ��ֹ����Ϩ��
//-----------------------------------------------------------------------------
//
void  CKBInputMethod::KeepInputTick (BOOL isKeyUp)
{
    /* ����ʹ�����ַ�ʽ���ѱ���, ӦΪ��Ӱ����̹���, �෢һЩû�õ�VK
    // ����û��ʹ�õİ���ά������
    if (isKeyUp)
    {
        // ģ����̰���
        keybd_event(VK_F24,     0, KEYEVENTF_KEYUP ,0);
    }
    else
    {
        // ģ����̰���
        keybd_event(VK_F24,     0, 0 ,0);
    }
    */

    ASSERT(m_imui);
    
    if (m_imui->_PowerIsBackLightOff())
    {
        m_imui->_PowerWakeupBackLight();
    }
}