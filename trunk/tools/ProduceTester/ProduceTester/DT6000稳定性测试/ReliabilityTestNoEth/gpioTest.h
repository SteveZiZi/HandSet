#ifndef     __GPIO_TEST_H
#define     __GPIO_TEST_H

class   CGpioTest:public CTestBase
{
public:
    CGpioTest();
    ~CGpioTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:

    int (*epcIoWrite)(DWORD dwPinState);
    int (*epcIoRead)(DWORD *dwPinState);

    HINSTANCE       m_hInstGpioDll;             //º”‘ÿgpioøÿ÷∆dll
    int             m_iInPinNum;
    int             m_iOutPinNum;  
};



#endif