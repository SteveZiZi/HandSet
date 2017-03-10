#include "stdafx.h"

#include "winioctl.h"
#include "KBDrvOper.h"



#define IOCTL_KEY_READSCANCODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0301, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_KEY_TEST            \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define KB_KEYEVENT_NAME    L"GetKeyCode_Event"



CKBDrvOper::CKBDrvOper ()
{
    m_hDevice = INVALID_HANDLE_VALUE;
    m_hKeyNotifyEvent = INVALID_HANDLE_VALUE;
}


CKBDrvOper::~CKBDrvOper ()
{
    DeviceDeinit();
}


BOOL  CKBDrvOper::DeviceInit ()
{
    ASSERT(m_hKeyNotifyEvent == INVALID_HANDLE_VALUE);
    ASSERT(m_hDevice == INVALID_HANDLE_VALUE);


    m_hDevice = CreateFile(L"ZLK1:", GENERIC_READ|GENERIC_WRITE, 0, 0,OPEN_ALWAYS, 0, NULL);
    if (m_hDevice == INVALID_HANDLE_VALUE)
    {
        KB_DEBUGLOG(L"Open GPM Device failed");
        ASSERT(0);
        return FALSE;
    }
    else
    {
        KB_DEBUGLOG(_T("Open GPM Device\r\n"));
    }

    m_hKeyNotifyEvent = CreateEvent(NULL, FALSE, FALSE, KB_KEYEVENT_NAME);
    if (m_hKeyNotifyEvent == INVALID_HANDLE_VALUE)
    {
        KB_DEBUGLOG(_T("Open Key Notify Event Event failed,error code is %d\r\n"), GetLastError());
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}

void  CKBDrvOper::DeviceDeinit ()
{
    if (m_hKeyNotifyEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hKeyNotifyEvent);
        m_hKeyNotifyEvent = INVALID_HANDLE_VALUE;
    }

    if (m_hDevice != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
    }
}


BOOL  CKBDrvOper::TestWriteInputScancode (UINT8 scancode)
{
    ASSERT(m_hKeyNotifyEvent != INVALID_HANDLE_VALUE);
    ASSERT(m_hDevice != INVALID_HANDLE_VALUE);

    if (DeviceIoControl(m_hDevice, IOCTL_KEY_TEST, &scancode, sizeof(scancode), 
        NULL,NULL,NULL,NULL))
    {
        return TRUE;
    }
    else
    {
        KB_DEBUGLOG(_T("Last Error Cord(0x%x)\r\n"), GetLastError());
        ASSERT(0);
        return FALSE;
    }
}

//
//-----------------------------------------------------------------------------
// 等待键盘输入, 可以指定超时时间ms, 或INFINITE
//  返回值的意义同WaitForSingleObject
//-----------------------------------------------------------------------------
//
DWORD  CKBDrvOper::WaitForReadInputScancode (UINT8& retScancode, DWORD dwMilliseconds)
{
    ASSERT(m_hKeyNotifyEvent != INVALID_HANDLE_VALUE);
    ASSERT(m_hDevice != INVALID_HANDLE_VALUE);

    if (WaitForSingleObject(m_hKeyNotifyEvent, dwMilliseconds) == WAIT_TIMEOUT)
    {
        return WAIT_TIMEOUT;
    }


    if (DeviceIoControl(m_hDevice, IOCTL_KEY_READSCANCODE, NULL, NULL, &retScancode, sizeof(retScancode), NULL, NULL))
    {
        KB_DEBUGLOG(_T("Read Scan Code(0x%02x)\r\n"), retScancode);
        return WAIT_OBJECT_0;
    }
    else
    {
        KB_DEBUGLOG(_T("Last Error Cord(0x%x)\r\n"), GetLastError());
        ASSERT(0);
        return WAIT_FAILED;
    }

    
}