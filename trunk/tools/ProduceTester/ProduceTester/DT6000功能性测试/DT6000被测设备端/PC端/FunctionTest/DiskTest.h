#ifndef         __DISK_TEST_H__
#define         __DISK_TEST_H__

#define     DISK_TEST_CMD_WRITE             0
#define     DISK_TEST_CMD_READ              1
#define     DISK_TEST_CMD_DELETE            2

class CDiskTest:public CTestBase
{
public:
    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const;

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:
    BOOL    m_bRemoveable;                  //是否要用一个设备来测试所有插口
    long    m_lSlotNum;                     //插口的数量
    long    m_lSlotNumTested;               //已经测试过的插口数量
};



#endif