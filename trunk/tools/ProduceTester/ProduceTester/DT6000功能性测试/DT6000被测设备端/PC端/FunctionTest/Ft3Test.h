#pragma once

class CFt3Test:public CTestBase
{
public:
    CFt3Test(void);
    ~CFt3Test(void);
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 5;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
};
