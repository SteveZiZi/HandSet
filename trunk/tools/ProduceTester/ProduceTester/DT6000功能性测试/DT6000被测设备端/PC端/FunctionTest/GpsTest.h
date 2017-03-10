#ifndef         __GPS_TEST_H__
#define         __GPS_TEST_H__

class CGpsTest:public CTestBase
{
public:
    CGpsTest();
    ~CGpsTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 1;};

    TEST_STATUS OnBeginTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS OnEndTest(CConnect *pConnect) {return TEST_NORMAL; };
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:

};


#endif