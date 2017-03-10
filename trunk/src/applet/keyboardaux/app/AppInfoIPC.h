#pragma once

#include "src/utils/ipc/filemapping.h"
#include "src/include/p_filemapping_def.h"

//
//-----------------------------------------------------------------------------
// 与主程序通信的辅助类
//  可以获得当前程序的运行状态
//-----------------------------------------------------------------------------
//
class CAppInfoExchgIPC
{
public:
    DWORD  IPCInit ();

public:
    BOOL  GetDSM800APPInfo (DSM800AppInfo* retInfo);
    BOOL  SetInputMode (APP_KBInputMode inputMode);
    BOOL  SetCaretPos (POINT pos);

private:
    CShareMemory            m_fileMapping;      // 内存映射文件操作辅助对象
};



//
//-----------------------------------------------------------------------------
// 初始化IPC通信, 如果返回0 则代表错误
//  非0否则代表成功
//-----------------------------------------------------------------------------
//
inline DWORD  CAppInfoExchgIPC::IPCInit ()
{
    return m_fileMapping.Open(DSM800_APPINFOEXCHG_MAPPING, sizeof(DSM800AppInfo));
}

//
//-----------------------------------------------------------------------------
// 获得应用程序当前的状态
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::GetDSM800APPInfo (DSM800AppInfo* retInfo)
{
    ASSERT(retInfo);
    return m_fileMapping.Read(retInfo, sizeof(DSM800AppInfo));
}

//
//-----------------------------------------------------------------------------
// 设置当前输入模式
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::SetInputMode (APP_KBInputMode inputMode)
{
    return m_fileMapping.Write(&inputMode, sizeof(APP_KBInputMode), 
        offsetof(DSM800AppInfo, inputMode));
}

//
//-----------------------------------------------------------------------------
// 设置当前的输入焦点位置
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::SetCaretPos (POINT pos)
{
    return m_fileMapping.Write(&pos, sizeof(POINT), 
        offsetof(DSM800AppInfo, caretPos));
}