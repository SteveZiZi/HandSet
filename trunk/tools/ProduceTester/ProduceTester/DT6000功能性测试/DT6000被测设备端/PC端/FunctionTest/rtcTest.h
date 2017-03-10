#ifndef     __RTC_TEST_H__
#define     __RTC_TEST_H__

class  CRtcTest:public CTestBase
{
public:
    CRtcTest();
    ~CRtcTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 3;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:
};



#endif