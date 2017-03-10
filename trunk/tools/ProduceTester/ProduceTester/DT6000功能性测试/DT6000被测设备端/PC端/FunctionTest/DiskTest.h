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
    BOOL    m_bRemoveable;                  //�Ƿ�Ҫ��һ���豸���������в��
    long    m_lSlotNum;                     //��ڵ�����
    long    m_lSlotNumTested;               //�Ѿ����Թ��Ĳ������
};



#endif