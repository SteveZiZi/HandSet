///
/// @brief
///     Ӳ����ʱ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.12.23
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "HardwareTimer.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\include\ctrlcode_def.h"

///
/// @brief
///     ���캯��
///
CHardwareTimer::CHardwareTimer(void)
: m_hTmrDev(INVALID_HANDLE_VALUE)
, m_hEvtExpired(INVALID_HANDLE_VALUE)
{
}

///
/// @brief
///     ��������
///
CHardwareTimer::~CHardwareTimer(void)
{
    UnInit();
}

///
/// @brief
///     ��ʼ��Ӳ����ʱ��
///
/// @param[in]  csName  Ӳ����ʱ������
///
/// @return
///     bool    �ɹ�����true
///
bool CHardwareTimer::Init( CString csName )
{
    // ��λ
    UnInit();

    // Ӳ����ʱ��
    m_hTmrDev = CreateFile(csName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (m_hTmrDev == INVALID_HANDLE_VALUE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("CreateFile() failed.\n")));
        return false;
    }

    // ��ʱ�¼�
    m_hEvtExpired = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hEvtExpired == INVALID_HANDLE_VALUE)
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("CreateEvent() failed.\n")));
        return false;
    }

    // ������ʱ����ʱ�¼�����
    if (!DeviceIoControl(m_hTmrDev, IOCTL_TMR_REGISTER, (PBYTE)&m_hEvtExpired, sizeof(HANDLE), NULL, 0, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("DeviceIoControl(IOCTL_TMR_SETPERIOD, ...) failed.\n")));
        return false;
    }

    return true;
}

///
/// @brief
///     ж��Ӳ����ʱ��
///
void CHardwareTimer::UnInit()
{
    // ��ʱ����ʱ�¼�
    if (INVALID_HANDLE_VALUE != m_hEvtExpired)
    {
        CloseHandle(m_hEvtExpired);
        m_hEvtExpired = INVALID_HANDLE_VALUE;
    }

    // Ӳ����ʱ��
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
///     �ȴ�ָ��ʱ����û�ȡ���¼���λ
///
/// @param[in]  dwDelay     �ȴ���ʱ��ms��
/// @param[in]  hEvtCancel  �û�ȡ���¼�
///
/// @return
///     bool    �ȴ��ɹ�����true
///
bool CHardwareTimer::Wait( DWORD dwDelay, HANDLE hEvtCancel )
{
    // ���ó�ʱʱ��
    DWORD dwSetTime = dwDelay * 1000;
    if (!DeviceIoControl(m_hTmrDev, IOCTL_TMR_SETPERIOD, (PBYTE)&dwSetTime, sizeof(dwSetTime), NULL, 0, NULL, NULL))
    {
        RETAILMSG(OUTPUT_LOG_MSG, (TEXT("DeviceIoControl(IOCTL_TMR_SETPERIOD, ...) failed.\r\n")));
        return false;
    }

    // �ȴ�ָ��ʱ����û�ȡ���¼���λ
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
