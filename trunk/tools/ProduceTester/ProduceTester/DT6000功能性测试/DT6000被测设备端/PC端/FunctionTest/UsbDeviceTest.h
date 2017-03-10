#ifndef  __ACTIVESYNC_TEST_H__
#define  __ACTIVESYNC_TEST_H__

//ActiveSync≤‚ ‘
class CActiveSyncTest:public CTestBase
{
public:  

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 3;};

    TEST_STATUS OnBeginTest(CConnect *pConnect) ;
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:
    TEST_STATUS    WriteRemoteTestFile(BYTE *pWriteBuf);
    TEST_STATUS    ReadRemoteTestFile(BYTE *pReadBuf);
};


#endif