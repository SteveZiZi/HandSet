����: ����ת��С����


ģ��˵��: �������븨������, �����ŽӼ���������Ӧ�ó���, �ó���ת����������Ϣ����׼�ļ���WM_KEYDOWN/UP,VK_��Ϣ, ����Ӧ�ò�
          ʹ�ñ�������밲װ����xxx-KeyTest.cab

��������������Debugģʽ, ���hook��׼��101������Ϣ, ���Ͳ��Ե�ɨ���������, ��������ת����������, ��������
������Ե�ʱ������Ҫ�������С��, Debug����Ե�����̵�Q���˳�


�÷�: ִ�г���


���:
========================================================================================
|
|   CKeyboardAuxApp: 
|       |
|       |--> CKeyboardAuxDlg (UI) ---- CCharSelPopupBar (UI)
|                 |                         |
|                 |                         |
|             CKBDrvOper --------> CKBInputMethod ---- KBKeyMap
|                 |
|                 |
|          xxx-KeyTest.cab (����) <---- DbgKBHook (Debugģʽ)
|                 |                             |
|                 |                             |
|             ����С��                    ��ʵ101-USB����
|
========================================================================================



������VK_XXX�Ķ�Ӧ��ϵ:
  0-9  .  ESC  Enter  F1-F6  Up/Down/Left/Right  ����ʵ���̵�VK_һһ��Ӧ
  ����: shift(VK_F9)  power(VK_F10)  menu(VK_TAB)  home(VK_F12)  

  ������С��� Shift+F4 �˳�������ͱ����̳���, 
  �����ǰ�����ǵ���ģʽ, ��ֻ�˳�������, ���˳����̳���




����:
1. ����һ������, WinCEϵͳ���ߺ��Ѻ�, �ᵼ�¼��̹����쳣, ���������, 
CKBDrvOper�еȴ������������ļ����¼�, �����ظ����յ����, ���������� 1�ΰ���1�ε����ɨ����, 
��ʵ���յ�����2�ΰ���1�ε���, ���߷�����, ���ּ��ʺܴ�, Ŀǰ����ȷ�ϲ��Ǽ���С����������������, ������ܳ�����������ceϵͳ


2. ���������˳�����
���ڱ��������ʱ, ���ְ������͵Ĳ��Ǳ�׼��KEYDOWN��Ϣ, ����OnChar��Ϣ, ���±���Ϩ��ʱ, �����ְ����޷�������Ļ
֮ǰ�õķ�ʽ���ڰ����ּ���ͬʱ����һ��VK_F24����, ��ģ�ⰴ������, �������ᵼ������һ������������õİ�������, ��������з�Ԥ�ڵķ�Ӧ
���ڸ�Ϊ��ʱͨ��GetSystemPowerState ���ϵͳ��Դ״̬, ������Ϩ��ʱ, ���Ұ����ְ���ʱ����SetSystemPowerState������Ļ




���̵�ͨ��:
����֪ͨ�������̵�ǰ���뷨�ı���, ��������������̵Ĺ��λ��
    ���̳���
    CAppInfoExchgIPC        m_appinfo;
    VERIFY(m_appipc.IPCInit());
    BOOL  CKeyboardAuxDlg::IPCGetFocusCaretPos (HWND curActiveWnd, CPoint& retpos)
    {
            // ��ǰ�����, ���ͻ�ȡ���λ����Ϣ
            if (::SendMessage(curActiveWnd, PWID_GET_CARETPOS, NULL, NULL) == PWID_RET_CARETPOS_OK)
                // ��ȷ��Ӧ��, ͨ��ipc��ȡԶ�˴������Ľ��
                DSM800AppInfo  appInfo;
                if (m_appipc.GetDSM800APPInfo(&appInfo))
    }
    BOOL  CKeyboardAuxDlg::IPCUpdateCurInputMode (HWND curActiveWnd, KBInputMode mode)
    {
        // ͨ��ipc��״̬���ý��빲���ڴ�
        if (m_appipc.SetInputMode(afmMode))
            // ֪ͨԶ�˴��ڵ�ǰ״̬�Ѿ��ı�
                ::SendMessage(curActiveWnd, PWID_UPDATE_INPUTMODE, afmMode, NULL);
    }



    ��������
    CAppInfoExchgIPC        m_appinfo;
    m_appinfo.IPCInit();
        ON_MESSAGE(PWID_GET_CARETPOS, &CReportControlDemoDlg::OnGetCaretPos)
        ON_MESSAGE(PWID_UPDATE_INPUTMODE, &CReportControlDemoDlg::OnUpdateInputMode)
    LRESULT  CReportControlDemoDlg::OnGetCaretPos (WPARAM, LPARAM)
    {
        POINT  caretPos;
        if (::GetCaretPos(&caretPos))
        {
            if (m_appinfo.SetCaretPos(caretPos))
            {
                return PWID_RET_CARETPOS_OK;
            }
        }
        return 0;
    }
    LRESULT  CReportControlDemoDlg::OnUpdateInputMode (WPARAM wParam, LPARAM)
    {
        CString  str;
        str.Format(L"���뷨�л� %d", wParam);
        EL_DEBUGLOG(L"%s\n", str);
        return 0;
    }

