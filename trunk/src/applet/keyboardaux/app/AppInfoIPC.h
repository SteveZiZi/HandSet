#pragma once

#include "src/utils/ipc/filemapping.h"
#include "src/include/p_filemapping_def.h"

//
//-----------------------------------------------------------------------------
// ��������ͨ�ŵĸ�����
//  ���Ի�õ�ǰ���������״̬
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
    CShareMemory            m_fileMapping;      // �ڴ�ӳ���ļ�������������
};



//
//-----------------------------------------------------------------------------
// ��ʼ��IPCͨ��, �������0 ��������
//  ��0�������ɹ�
//-----------------------------------------------------------------------------
//
inline DWORD  CAppInfoExchgIPC::IPCInit ()
{
    return m_fileMapping.Open(DSM800_APPINFOEXCHG_MAPPING, sizeof(DSM800AppInfo));
}

//
//-----------------------------------------------------------------------------
// ���Ӧ�ó���ǰ��״̬
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::GetDSM800APPInfo (DSM800AppInfo* retInfo)
{
    ASSERT(retInfo);
    return m_fileMapping.Read(retInfo, sizeof(DSM800AppInfo));
}

//
//-----------------------------------------------------------------------------
// ���õ�ǰ����ģʽ
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::SetInputMode (APP_KBInputMode inputMode)
{
    return m_fileMapping.Write(&inputMode, sizeof(APP_KBInputMode), 
        offsetof(DSM800AppInfo, inputMode));
}

//
//-----------------------------------------------------------------------------
// ���õ�ǰ�����뽹��λ��
//-----------------------------------------------------------------------------
//
inline BOOL  CAppInfoExchgIPC::SetCaretPos (POINT pos)
{
    return m_fileMapping.Write(&pos, sizeof(POINT), 
        offsetof(DSM800AppInfo, caretPos));
}