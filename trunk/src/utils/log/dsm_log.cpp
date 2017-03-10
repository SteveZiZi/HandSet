/**
 * \file
 *      dsm_log.cpp
 *
 * \brief
 *      ��־ģ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/12
 */


#include "stdafx.h"
#include "dsm_log.h"

#include "src/utils/utility/dsm_utility.h"


#define DEFAULT_LOG_FILE                L"log.txt"                  ///< Ĭ����־�ļ���
#define DEFAULT_LOG_SIZE                512                         ///< ÿ����־�ļ�����ֽ���
#define DEFAULT_LOG_MAX_SIZE            DEFAULT_LOG_SIZE * 2048     ///< ��־�ļ���С1M



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
 *      ��ʼ����־ģ��
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

    // ������־�̵߳������ź���
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
 *       ����ʼ����־ģ�飬������Դ
 *
 * \return
 *      bool            ����ʼ���ɹ�����true��ʧ�ܷ���false
 */
bool dsm_log::uninitLog()
{
    dsm_log& rLog = dsm_log::getInstance();

    if(!rLog.m_bRun)
    {
        return false;
    }

    // ���ñ�־λΪfalse����־�̵߳õ���һ���ź������˳�
    rLog.m_bRun = false;

    ::ReleaseSemaphore(rLog.m_hSemaphore, 1, NULL);
    // 1000ms�ȴ��߳��˳�
    DWORD dwRet = ::WaitForSingleObject(rLog.m_hLogThread, 1000);

    ::CloseHandle(rLog.m_hSemaphore);
    rLog.m_hSemaphore = NULL;

    ::CloseHandle(rLog.m_hLogThread);
    rLog.m_hLogThread = NULL;

    ::DeleteCriticalSection(&rLog.m_criticalSection);

    return true;
}

/**
 *      д��һ����־
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

    // �ٽ���ֻ������־�б�Ķ�д
    ::EnterCriticalSection(&rLog.m_criticalSection);
    rLog.m_lstLog.push_back(data);
    ::LeaveCriticalSection(&rLog.m_criticalSection);

    // ��д��־�̹߳���
    ::ReleaseSemaphore(rLog.m_hSemaphore, 1, NULL);

    return true;
}


/**
 *      д��־�ļ��̺߳���
 *
 * \param
 *      arg             �̲߳���
 * \return
 *      unsigned int    �̷߳��ؽ��
 *      �����˳��߳�    1
 *      �쳣�˳��߳�    0
 */
DWORD dsm_log::logThread( void* arg )
{
    dsm_log& rLog = getInstance();
    CString csLogFile = DSM_GetAppDir() + rLog.m_csLogFile;

    // ���������־�ļ�
    HANDLE hFile = NULL;

    hFile = ::CreateFile(csLogFile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // ��ȡ�������ش���
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

    // ��ѭ��һֱ�ڵȴ��ź�����Ȼ�����־�б���Ϣд���ļ�
    while(rLog.m_bRun)
    {
        if(WAIT_FAILED == ::WaitForSingleObject(rLog.m_hSemaphore, INFINITE))
        {
            CloseHandle(hFile);
            ::OutputDebugString(L"failed to wait for semaphore object\n");
            return 1;
        }

        dwFileSize = ::GetFileSize(hFile, NULL);

        // ����ʧ�ܴ���ÿһ��д��־�����Գ��Բ���
        if(dwSize != 0xFFFFFFFF && dwSize >= DEFAULT_LOG_MAX_SIZE)
        {
            ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
            ::SetEndOfFile(hFile);
        }

        // д�ļ����ܽ�������Ӧ���ٽ����ڽ����ļ�����
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

