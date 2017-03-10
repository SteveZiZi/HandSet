#ifndef     __LED_TEST_H__
#define     __LED_TEST_H__

#define  LED1   1
#define  LED2   2

class CLedTest:public CTestBase
{
public:
    CLedTest();
    ~CLedTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:

    BOOL    (*epcLedOn) (DWORD dwLedNum, DWORD dwOntime);
    BOOL    (*epcLedOff) (DWORD dwLedNum);
    DWORD   (*epcLedGetStatus) (DWORD dwLedNum);
    HINSTANCE       m_hInstLedDll;             //º”‘ÿgpioøÿ÷∆dll
};


#endif