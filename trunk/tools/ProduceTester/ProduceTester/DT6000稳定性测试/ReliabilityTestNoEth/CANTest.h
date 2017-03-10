#ifndef __CAN_TEST_H__
#define __CAN_TEST_H__

class CCANTest:public CTestBase
{
public:
    CCANTest();
    ~CCANTest();
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);
private:
    UINT16          m_uiCmd;                    //本测试项要求的命令码   
    BOOL    InitDevCan();
    BOOL    m_bInitError;                       //设备初始化是否成功
    BOOL    m_bDirection;                       //数据发送的方向

    DWORD           m_testCanID1;               //本测试的第一路CAN的ID
    DWORD           m_testCanID2;               //本测试的第一路CAN的ID
};

#endif