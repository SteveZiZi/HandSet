#pragma once
#include ".\app_src\fiber.h"

class CFt3Test:public CTestBase
{
public:
    CFt3Test(void);
    ~CFt3Test(void);
    CFt3Test(HANDLE hDevice, UINT16 uDeviceItem);

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:
    Ft3SendCtlWord_t m_tCtrlW;
    _FT3_c           *m_pFt3;
    int              m_iTimeOut;

    //接收的FT3报文总帧数
    UINT32           m_uTotalFrame;

    //接收的错误报文帧数
    UINT32           m_uErrorFrame;
};
