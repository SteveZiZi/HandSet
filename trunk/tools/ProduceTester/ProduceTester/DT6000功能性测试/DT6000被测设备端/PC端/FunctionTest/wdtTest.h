#ifndef     __WDT_TEST_H__
#define     __WDT_TEST_H__

//ø¥√≈π∑≤‚ ‘
class CWdtTest:public CTestBase
{
public: 
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 1;};

    TEST_STATUS OnBeginTest(CConnect *pConnect) {return TEST_NORMAL;};
    TEST_STATUS OnEndTest(CConnect *pConnect)   {return TEST_NORMAL; };
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:


};

#endif