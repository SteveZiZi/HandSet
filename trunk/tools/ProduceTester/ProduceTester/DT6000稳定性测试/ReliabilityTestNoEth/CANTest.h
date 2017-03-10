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
    UINT16          m_uiCmd;                    //��������Ҫ���������   
    BOOL    InitDevCan();
    BOOL    m_bInitError;                       //�豸��ʼ���Ƿ�ɹ�
    BOOL    m_bDirection;                       //���ݷ��͵ķ���

    DWORD           m_testCanID1;               //�����Եĵ�һ·CAN��ID
    DWORD           m_testCanID2;               //�����Եĵ�һ·CAN��ID
};

#endif