#ifndef      __CAN_TEST_H__
#define      __CAN_TEST_H__

class CCanTest:public CTestBase
{
public:

    CCanTest ();
    ~CCanTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect)
    {
        return TEST_NORMAL;
    }
private:
    DWORD       m_dwCanDevId;               //使用CE上的第几路CAN

    HANDLE      m_hTestThread;              //测试线程句柄

    //测试线程函数
    static DWORD CALLBACK TestProc(void *pVoid);
    BOOL        m_bContinueTest;            //是否继续测试
  
};


#endif