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
    DWORD       m_dwCanDevId;               //ʹ��CE�ϵĵڼ�·CAN

    HANDLE      m_hTestThread;              //�����߳̾��

    //�����̺߳���
    static DWORD CALLBACK TestProc(void *pVoid);
    BOOL        m_bContinueTest;            //�Ƿ��������
  
};


#endif