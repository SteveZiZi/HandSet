#ifndef  __BEEP_TEST_H__
#define  __BEEP_TEST_H__

class CBeepTest:public CTestBase
{
public:
    CBeepTest();
    ~CBeepTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 32;};

    TEST_STATUS OnBeginTest(CConnect *pConnect) ;
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:   
};


#endif