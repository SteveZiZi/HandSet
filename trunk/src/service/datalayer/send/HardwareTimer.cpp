///
/// @brief
///     硬件定时器
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.12.23
///
/// 修改历史：
///

#include "StdAfx.h"
#include "HardwareTimer.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

///
/// @brief
///     构造函数
///
CHardwareTimer::CHardwareTimer(void)
: m_hTmrDev(INVALID_HANDLE_VALUE)
, m_hEvtExpired(INVALID_HANDLE_VALUE)
{
}

///
/// @brief
///     析构函数
///
CHardwareTimer::~CHardwareTimer(void)
{
    UnInit();
}

///
/// @brief
///     初始化硬件定时器
///
/// @param[in]  csName  硬件定时器名字
///
/// @return
///     bool    成功返回true
///
bool CHardwareTimer::Init( CString csName )
{
    // 复位
    UnInit();

    // 硬件定时器
    m_hTmrDev = CreateFile(csName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (m_hTmrDev == INVALID_HANDLE_VALUE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("CreateFile() failed.\n")));
        return false;
    }

    // 超时事件
    m_hEvtExpired = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hEvtExpired == INVALID_HANDLE_VALUE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("CreateEvent() failed.\n")));
        return false;
    }

    // 关联定时器超时事件对象
    if (!DeviceIoControl(m_hTmrDev, IOCTL_TMR_REGISTER, (PBYTE)&m_hEvtExpired, sizeof(HANDLE), NULL, 0, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("DeviceIoControl(IOCTL_TMR_SETPERIOD, ...) failed.\n")));
        return false;
    }

    return true;
}

///
/// @brief
///     卸载硬件定时器
///
void CHardwareTimer::UnInit()
{
    // 定时器超时事件
    if (INVALID_HANDLE_VALUE != m_hEvtExpired)
    {
        CloseHandle(m_hEvtExpired);
        m_hEvtExpired = INVALID_HANDLE_VALUE;
    }

    // 硬件定时器
    if (INVALID_HANDLE_VALUE != m_hTmrDev)
    {
        if (!DeviceIoControl(m_hTmrDev, IOCTL_TMR_UNREGISTER,  NULL, 0, NULL, 0, NULL, NULL))
        {
            RETAILMSG(OUTPUT_LOG_MSG, (TEXT("DeviceIoControl(IOCTL_TMR_UNREGISTER, ...) failed.\n")));
        }

        CloseHandle(m_hTmrDev);
        m_hTmrDev = INVALID_HANDLE_VALUE;
    }
}

///
/// @brief
///     等待指定时间或用户取消事件置位
///
/// @param[in]  dwDelay     等待超时（ms）
/// @param[in]  hEvtCancel  用户取消事件
///
/// @return
///     bool    等待成功返回true
///
bool CHardwareTimer::Wait( DWORD dwDelay, HANDLE hEvtCancel )
{
    // 设置超时时间
    DWORD dwSetTime = dwDelay * 1000;
    if (!DeviceIoControl(m_hTmrDev, IOCTL_TMR_SETPERIOD, (PBYTE)&dwSetTime, sizeof(dwSetTime), NULL, 0, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("DeviceIoControl(IOCTL_TMR_SETPERIOD, ...) failed.\r\n")));
        return false;
    }

    // 等待指定时间或用户取消事件置位
    HANDLE  hObjects[] = {m_hEvtExpired, hEvtCancel};
    DWORD   dwRet = WaitForMultipleObjects(sizeof(hObjects)/sizeof(hObjects[0]), hObjects, FALSE, dwDelay * 2);
    if ((WAIT_OBJECT_0 + 1)== dwRet)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("User cancel wait.\r\n")));
        return false;
    }
    else if (WAIT_TIMEOUT == dwRet)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("Hardware timer timeout.\r\n")));
        return false;
    }

    return true;
}
