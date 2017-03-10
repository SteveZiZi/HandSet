#ifndef     __BEEP_TEST_H__
#define     __BEEP_TEST_H__
class   CBeepTest:public CTestBase
{
public:
    CBeepTest();
    ~CBeepTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:

    BOOL    (* epcBuzzerOn)         (DWORD dwOntime);
    BOOL    (* epcBuzzerOff)        (void);
    DWORD   (* epcBuzzerGetStatus)  (void);
    HINSTANCE       m_hInstBeepDll;             //º”‘ÿLEDøÿ÷∆dll
};


#endif