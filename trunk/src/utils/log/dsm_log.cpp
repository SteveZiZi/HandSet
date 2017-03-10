/**
 * \file
 *      dsm_log.cpp
 *
 * \brief
 *      日志模块实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/12
 */


#include "stdafx.h"
#include "dsm_log.h"

#include "src/utils/utility/dsm_utility.h"


#define DEFAULT_LOG_FILE                L"log.txt"                  ///< 默认日志文件名
#define DEFAULT_LOG_SIZE                512                         ///< 每条日志文件最大字节数
#define DEFAULT_LOG_MAX_SIZE            DEFAULT_LOG_SIZE * 2048     ///< 日志文件大小1M



dsm_log::dsm_log()
: m_bInited(false)
, m_bRun(true)
, m_hLogThread(NULL)
, m_hSemaphore(NULL)
, m_csLogFile(DEFAULT_LOG_FILE)
{

}

dsm_log::~dsm_log()
{
    dsm_log::uninitLog();
}


/**
 *      初始化日志模块
 */
bool dsm_log::Init( const wchar_t* pszLogFile /*= L"log.txt"*/ )
{
    if(!pszLogFile)
    {
        ::OutputDebugString(L"null pointer for log file name\n");
        return false;
    }

    dsm_log& rLog = dsm_log::getInstance();

    if(rLog.m_bInited)
    {
        ::OutputDebugString(L"already init log module\n");
        return false;
    }

    rLog.m_csLogFile = pszLogFile;

    // 创建日志线程的匿名信号量
    rLog.m_hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

    if(!rLog.m_hSemaphore)
    {
        ::OutputDebugString(L"failed to create semaphore\n");
        return false;
    }

    ::InitializeCriticalSection(&rLog.m_criticalSection);

    rLog.m_hLogThread = CreateThread(NULL, 0, logThread, NULL, 
        STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);

    if(!rLog.m_hLogThread)
    {
        ::OutputDebugString(L"failed to create thread\n");
        return false;
    }

    rLog.m_bInited = true;

    return true;
}


/**
 *       反初始化日志模块，清理资源
 *
 * \return
 *      bool            反初始化成功返回true，失败返回false
 */
bool dsm_log::uninitLog()
{
    dsm_log& rLog = dsm_log::getInstance();

    if(!rLog.m_bRun)
    {
        return false;
    }

    // 设置标志位为false，日志线程得到下一次信号量后退出
    rLog.m_bRun = false;

    ::ReleaseSemaphore(rLog.m_hSemaphore, 1, NULL);
    // 1000ms等待线程退出
    DWORD dwRet = ::WaitForSingleObject(rLog.m_hLogThread, 1000);

    ::CloseHandle(rLog.m_hSemaphore);
    rLog.m_hSemaphore = NULL;

    ::CloseHandle(rLog.m_hLogThread);
    rLog.m_hLogThread = NULL;

    ::DeleteCriticalSection(&rLog.m_criticalSection);

    return true;
}

/**
 *      写入一条日志
 */
bool dsm_log::Write( const wchar_t* pszMsg, LOG_LEVEL level /*= LEVEL_INFO*/ )
{
    if(!pszMsg)
    {
        ::OutputDebugString(L"null pointer for log msg\n");
        return false;
    }

    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);

    _LOGDATA data = {level, sysTime, pszMsg};

    dsm_log& rLog = dsm_log::getInstance();

    // 临界区只负责日志列表的读写
    ::EnterCriticalSection(&rLog.m_criticalSection);
    rLog.m_lstLog.push_back(data);
    ::LeaveCriticalSection(&rLog.m_criticalSection);

    // 让写日志线程工作
    ::ReleaseSemaphore(rLog.m_hSemaphore, 1, NULL);

    return true;
}


/**
 *      写日志文件线程函数
 *
 * \param
 *      arg             线程参数
 * \return
 *      unsigned int    线程返回结果
 *      正常退出线程    1
 *      异常退出线程    0
 */
DWORD dsm_log::logThread( void* arg )
{
    dsm_log& rLog = getInstance();
    CString csLogFile = DSM_GetAppDir() + rLog.m_csLogFile;

    // 创建或打开日志文件
    HANDLE hFile = NULL;

    hFile = ::CreateFile(csLogFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // 获取操作返回代码
    DWORD dwErrorCode = GetLastError();

    if(INVALID_HANDLE_VALUE == hFile)
    {
        ::OutputDebugString(L"failed to open log file\n");
        return 1;
    }

    wchar_t szLog[DEFAULT_LOG_SIZE] = {0};
    DWORD dwFileSize, dwSize, dwWritedLen;
    dwFileSize = dwSize = dwWritedLen = 0;

    listlog lstLog;
    listlog::iterator iter;
    listlog::iterator iterEnd;

    // 该循环一直在等待信号量，然后把日志列表信息写入文件
    while(rLog.m_bRun)
    {
        if(WAIT_FAILED == ::WaitForSingleObject(rLog.m_hSemaphore, INFINITE))
        {
            CloseHandle(hFile);
            ::OutputDebugString(L"failed to wait for semaphore object\n");
            return 1;
        }

        dwFileSize = ::GetFileSize(hFile, NULL);

        // 不做失败处理，每一次写日志都可以尝试操作
        if(dwSize != 0xFFFFFFFF && dwSize >= DEFAULT_LOG_MAX_SIZE)
        {
            ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
            ::SetEndOfFile(hFile);
        }

        // 写文件可能较慢，不应在临界区内进行文件操作
        ::EnterCriticalSection(&rLog.m_criticalSection);
        iter = rLog.m_lstLog.begin();
        iterEnd = rLog.m_lstLog.end();
        for(; iter != iterEnd; iter++)
        {
            lstLog.push_back(*iter);
        }

        rLog.m_lstLog.clear();
        ::LeaveCriticalSection(&rLog.m_criticalSection);

        iter = lstLog.begin();
        iterEnd = lstLog.end();

        for(; iter != iterEnd; iter++)
        {
            _LOGDATA pData = *iter;

            ::OutputDebugString(pData.logmsg.c_str());
            ::OutputDebugString(L"\n");
            swprintf(szLog, L"{%d}%d/%02d/%02d %02d:%02d:%02d:%03d\t[%s]\n", 
                pData.level, pData.systemtime.wYear, pData.systemtime.wMonth, 
                pData.systemtime.wDay, pData.systemtime.wHour, 
                pData.systemtime.wMinute, pData.systemtime.wSecond, 
                pData.systemtime.wMilliseconds, pData.logmsg.c_str());

            ::SetFilePointer(hFile, 0, NULL, FILE_END);

            dwWritedLen = 0;
            dwSize = (DWORD)wcslen(szLog) * sizeof(szLog[0]);
            if(!::WriteFile(hFile, szLog, dwSize, &dwWritedLen, NULL))
            {
                ::OutputDebugString(L"failed to write\n");
            }
        }

        lstLog.clear();
    }

    CloseHandle(hFile);

    return 0;
}

