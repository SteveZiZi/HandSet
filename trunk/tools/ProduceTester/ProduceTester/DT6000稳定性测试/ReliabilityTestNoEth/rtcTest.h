#ifndef  __RTC_TEST_H__
#define  __RTC_TEST_H__

class CRTCTest:public CTestBase
{
public:
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);
};


#endif