#pragma once

#include "KBDefines.h"




class CKBDrvOper
{
public:
    CKBDrvOper ();
    virtual ~CKBDrvOper ();

    BOOL  DeviceInit ();
    void  DeviceDeinit ();

    BOOL  TestWriteInputScancode (UINT8 scancode);
    DWORD  WaitForReadInputScancode (UINT8& retScancode, DWORD dwMilliseconds = INFINITE);

private:
    HANDLE                  m_hDevice;
    HANDLE                  m_hKeyNotifyEvent;
};