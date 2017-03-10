/// @file
///
/// @brief
///     解析XML文件，获取有用的数据
///
/// @note
///     从XML文件中获取进程信息列表
///
/// Copyright (c) 2013 IUnknownBase Inc.
///
/// 作者:
///
///     chao 2013.7.1
///
/// 修改历史: V1.1
///
/// 修改时间: 20113.8.2
///
#pragma once
#include "Markup.h"
#include <vector>

///
/// @brief
///     进程信息单元
///
typedef struct 
{
    CString     strProcessName;     ///<启动进程的路径，可以是相对路径或绝对路径
    CString     strProcessParam;    ///<启动进程的参数
    int         uDelayTime;         ///<相对上一个进程要延迟启动的时间
    bool        isProtected;        ///<标记为是否需要进行监控保护
    bool        bAbsolutePath;      ///<标记进程路径名是相对路径还是绝对路径
}_PROCESS_INFO;

class CReadProcess
{
public:
    CReadProcess(void);
    ~CReadProcess(void);
    CReadProcess(CString strFileName);
    BOOL GetProcessList(std::vector<_PROCESS_INFO> &vecProcessInfo);
    CString GetLogoName();
private:
    //CMarkup类的一个对象，用来操纵XML文件
    CMarkup m_markup;

    //要操作的XML文件名
    CString m_strFileName;

    //要启动的logo图片路径
    CString m_strLogoName;

    bool JudgeFilePath(CString strFilePath);
};
