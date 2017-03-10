#ifndef     __KBD_IO_TEST_H__
#define     __KBD_IO_TEST_H__

//��չ����IO����
class CKbdIoTest:public CTestBase
{
public:
    CKbdIoTest();
    ~CKbdIoTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return m_uiShouldTestCount;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:

    unsigned int        m_uiOutPinNum;              //������ŵĸ���
    unsigned int        m_uiInPinNum;               //�������ŵĸ���
    unsigned int        m_uiShouldTestCount;        //Ӧ�ò��ԵĴ���

    unsigned int        m_uiOutMask;                //�������������
    unsigned int        m_uiInMask;                 //�������������
    unsigned int        m_uiErrMask;                //���ڱ����ڼ������ų��������(������Ϊ��׼)
};


#endif