#pragma once
//#include "testBase.h"
#include ".\app_src\fiber.h"

class CSmvTest : public CTestBase
{
public:
    CSmvTest(void);
    ~CSmvTest(void);
    CSmvTest(HANDLE hDevice, UINT8 uDeviceItem);

    BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,const BYTE *pData,CConnect *pConnect);

    TEST_STATUS BeginSmvTest();
    TEST_STATUS ExcuteSmvTest();
    TEST_STATUS EndSmvTest();

    _SMV_92_c * GetSmvFiber();

private:
    TEST_INFO m_ti;
    IOCTRL_FIBER_IN_PARAM_t m_tFiberParam;
    _SMV_92_c *m_pFiber;
};
