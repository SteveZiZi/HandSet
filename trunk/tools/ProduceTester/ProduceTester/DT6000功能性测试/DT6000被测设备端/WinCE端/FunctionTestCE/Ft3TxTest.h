#pragma once
#include ".\app_src\fiber.h"
#include "Ft3Test.h"
class CFt3TxTest:public CTestBase
{
public:
    CFt3TxTest(void);
    ~CFt3TxTest(void);
    CFt3TxTest(HANDLE hDevice, UINT8 uDeviceItem);
    BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
    TEST_STATUS BeginFt3Test();
    TEST_STATUS ExcuteFt3Test();
    TEST_STATUS EndFt3Test();
    void SetRxFt3(_FT3_c *pRx1Ft3, _FT3_c *pRx2Ft3);

private:
    TEST_INFO m_ti;
    _FT3_c    *m_pFt3;
    _FT3_c    *m_pRx1Ft3;
    _FT3_c    *m_pRx2Ft3;
    Ft3SendCtlWord_t m_tCtrlW;
};
