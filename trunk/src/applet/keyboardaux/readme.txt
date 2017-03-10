名称: 按键转换小程序


模块说明: 键盘输入辅助程序, 用于桥接键盘驱动与应用程序, 该程序转换驱动的消息到标准的键盘WM_KEYDOWN/UP,VK_消息, 发给应用层
          使用本程序必须安装驱动xxx-KeyTest.cab

本程序如果编译成Debug模式, 则会hook标准的101键盘消息, 发送测试的扫描码给驱动, 再由驱动转发按键回来, 做测试用
避免测试的时候总是要接入键盘小板, Debug版可以点击键盘的Q键退出


用法: 执行程序


框架:
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
|          xxx-KeyTest.cab (驱动) <---- DbgKBHook (Debug模式)
|                 |                             |
|                 |                             |
|             按键小板                    真实101-USB键盘
|
========================================================================================



按键与VK_XXX的对应关系:
  0-9  .  ESC  Enter  F1-F6  Up/Down/Left/Right  与真实键盘的VK_一一对应
  其中: shift(VK_F9)  power(VK_F10)  menu(VK_TAB)  home(VK_F12)  

  按键盘小板的 Shift+F4 退出主程序和本键盘程序, 
  如果当前程序是调试模式, 则只退出主程序, 不退出键盘程序




问题:
1. 碰到一个问题, WinCE系统休眠后唤醒后, 会导致键盘功能异常, 具体表现是, 
CKBDrvOper中等待驱动发过来的键盘事件, 会有重复接收的情况, 比如正常是 1次按下1次弹起的扫描码, 
但实际收到的是2次按下1次弹起, 或者反过来, 出现几率很大, 目前可以确认不是键盘小程序和主程序的问题, 问题可能出现在驱动或ce系统


2. 关于屏保退出问题
由于本程序设计时, 数字按键发送的不是标准的KEYDOWN消息, 而是OnChar消息, 导致背光熄灭时, 按数字按键无法唤醒屏幕
之前用的方式是在按数字键是同时发送一个VK_F24按键, 来模拟按键输入, 但这样会导致另外一个问题就是无用的按键发送, 程序可能有非预期的反应
现在改为定时通过GetSystemPowerState 获得系统电源状态, 当背光熄灭时, 并且按数字按键时调用SetSystemPowerState唤醒屏幕




进程的通信:
可以通知其他进程当前输入法改变了, 获得其他其他进程的光标位置
    键盘程序
    CAppInfoExchgIPC        m_appinfo;
    VERIFY(m_appipc.IPCInit());
    BOOL  CKeyboardAuxDlg::IPCGetFocusCaretPos (HWND curActiveWnd, CPoint& retpos)
    {
            // 向当前活动窗口, 发送获取光标位置消息
            if (::SendMessage(curActiveWnd, PWID_GET_CARETPOS, NULL, NULL) == PWID_RET_CARETPOS_OK)
                // 正确响应后, 通过ipc获取远端传过来的结果
                DSM800AppInfo  appInfo;
                if (m_appipc.GetDSM800APPInfo(&appInfo))
    }
    BOOL  CKeyboardAuxDlg::IPCUpdateCurInputMode (HWND curActiveWnd, KBInputMode mode)
    {
        // 通过ipc将状态设置进入共享内存
        if (m_appipc.SetInputMode(afmMode))
            // 通知远端窗口当前状态已经改变
                ::SendMessage(curActiveWnd, PWID_UPDATE_INPUTMODE, afmMode, NULL);
    }



    其他进程
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
        str.Format(L"输入法切换 %d", wParam);
        EL_DEBUGLOG(L"%s\n", str);
        return 0;
    }

