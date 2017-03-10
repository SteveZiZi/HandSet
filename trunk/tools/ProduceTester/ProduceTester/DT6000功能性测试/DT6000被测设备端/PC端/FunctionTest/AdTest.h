#ifndef     __AD_TEST_H__
#define     __AD_TEST_H__

class CADTest:public CTestBase
{
public:
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 32;};

    TEST_STATUS OnBeginTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS OnEndTest(CConnect *pConnect){return TEST_NORMAL; };
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:

};

#endif