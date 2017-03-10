#pragma once
//#include "testBase.h"
#include ".\app_src\fiber.h"
class CBcodeTest : public CTestBase
{
public:
    CBcodeTest(void);
    ~CBcodeTest(void);
    CBcodeTest(_SMV_92_c * pFiber);
    BOOL Init(const WCHAR *strIniFile, int index,UINT16  *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS ProcessExcuteCmd(UINT16 param, UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);

    TEST_STATUS BeginSmvTest();
    TEST_STATUS ExcuteSmvTest();
    TEST_STATUS EndSmvTest();
private:
    TEST_INFO m_ti;
    _SMV_92_c * m_pFiber;
    IOCTRL_FIBER_IN_PARAM_t m_tFiberParam;
};
