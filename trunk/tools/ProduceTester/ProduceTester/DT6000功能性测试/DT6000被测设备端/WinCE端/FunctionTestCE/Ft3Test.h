#pragma once
//#include "testBase.h"
#include ".\app_src\fiber.h"
class CFt3Test :public CTestBase
{
public:
    CFt3Test(void);
    ~CFt3Test(void);
    CFt3Test(HANDLE hDevice, UINT8 uDeviceItem);
    BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
    TEST_STATUS BeginFt3Test();
    TEST_STATUS ExcuteFt3Test();
    TEST_STATUS EndFt3Test();
    _FT3_c * GetRxFt3();

private:
    TEST_INFO m_ti;
    _FT3_c    *m_pFt3;
    Ft3SendCtlWord_t m_tCtrlW;
};
