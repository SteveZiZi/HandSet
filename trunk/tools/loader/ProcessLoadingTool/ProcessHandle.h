/// @file
///
/// @brief
///     启动相关线程并监视某些进程
///
/// @note
///        
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// 作者:
///
///     chao 2013.7.17
///
/// 修改历史: V1.1
///
/// 修改时间: 20113.8.2
///
#pragma once
#include "ReadProcess.h"
#include <map>

const UINT PROCESS_SIZE = 64;    ///<默认可以进行监听的进程个数

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
    //用来从XML中读取数据
    CReadProcess* m_pReadProcess;

    //要监视的进程个数
    UINT m_nProcessCnt;

    //监听进程的句柄
    HANDLE m_hProcess[PROCESS_SIZE];

    //保存进程的相关信息
    std::vector<_PROCESS_INFO> m_vecProcessInfo;

    //进行句柄和下标值的映射，方便根据进程句柄获取进程相关信息
    std::map <HANDLE, int> m_mapHandle;

    //应用程序所在目录的父目录
    CString strProcessPath;

    //当前启动进程的上一个进程句柄
    HANDLE m_hPreProcess;

};
