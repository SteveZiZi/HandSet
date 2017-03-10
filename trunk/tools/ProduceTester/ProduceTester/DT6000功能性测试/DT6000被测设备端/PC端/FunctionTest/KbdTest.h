#ifndef         __KBD_TEST_H__
#define         __KBD_TEST_H__

class CKbdTest:public CTestBase
{
public:
    CKbdTest();
    ~CKbdTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 1;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:

};



#endif