#ifndef  __SCREEN_TEST_H__
#define  __SCREEN_TEST_H__


#define  SCREEN_PARAM_INIT                  0
#define  SCREEN_PARAM_END                   1
#define  SCREEN_PARAM_TEST_COLOR_LEVEL      2
#define  SCREEN_PARAM_TEST_IMAGE            3

class CSreenTest:public CTestBase
{
public:
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 2;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
    void OnConnectDown();
};




#endif