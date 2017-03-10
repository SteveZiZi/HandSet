#pragma once
#include ".\app_src\fiber.h"
class CFt3Tx
{
public:
    CFt3Tx(void);
    ~CFt3Tx(void);
    CFt3Tx(HANDLE hDevice, UINT16 uDeviceItem);
    bool Ft3Send();
    bool Ft3Stop();
private:
    Ft3SendCtlWord_t m_tCtrlW;
    _FT3_c           *m_pFt3;
     WCHAR       m_strFt3FilePath[MAX_PATH*2];
};
