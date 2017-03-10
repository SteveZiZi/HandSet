#ifndef      __ZIG_BEE_TEST__
#define     __ZIG_BEE_TEST__

class CZigBeeTest:public CTestBase
{
public:
    CZigBeeTest();
    ~CZigBeeTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 1;};

    TEST_STATUS OnBeginTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS OnEndTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
};


#endif