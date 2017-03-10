#ifndef         __SPI_FLASH_TEST_H__
#define         __SPI_FLASH_TEST_H__

class CSpiFlash:public CTestBase
{
public:
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 3;};

    TEST_STATUS OnBeginTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS OnEndTest(CConnect *pConnect) {return TEST_NORMAL; };
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

};



#endif