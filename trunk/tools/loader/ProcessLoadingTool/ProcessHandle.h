/// @file
///
/// @brief
///     ��������̲߳�����ĳЩ����
///
/// @note
///        
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// ����:
///
///     chao 2013.7.17
///
/// �޸���ʷ: V1.1
///
/// �޸�ʱ��: 20113.8.2
///
#pragma once
#include "ReadProcess.h"
#include <map>

const UINT PROCESS_SIZE = 64;    ///<Ĭ�Ͽ��Խ��м����Ľ��̸���

class CProcessHandle
{
public:
    CProcessHandle(void);
    ~CProcessHandle(void);
    bool RunProcess(_PROCESS_INFO pszApplicationName);
    bool StartProcess();
    bool StartMonitoring();
    void RemoveProcessHandle(DWORD dwIndex);
    bool DisplayLogo(CString strLogoName);
private:
    //������XML�ж�ȡ����
    CReadProcess* m_pReadProcess;

    //Ҫ���ӵĽ��̸���
    UINT m_nProcessCnt;

    //�������̵ľ��
    HANDLE m_hProcess[PROCESS_SIZE];

    //������̵������Ϣ
    std::vector<_PROCESS_INFO> m_vecProcessInfo;

    //���о�����±�ֵ��ӳ�䣬������ݽ��̾����ȡ���������Ϣ
    std::map <HANDLE, int> m_mapHandle;

    //Ӧ�ó�������Ŀ¼�ĸ�Ŀ¼
    CString strProcessPath;

    //��ǰ�������̵���һ�����̾��
    HANDLE m_hPreProcess;

};
