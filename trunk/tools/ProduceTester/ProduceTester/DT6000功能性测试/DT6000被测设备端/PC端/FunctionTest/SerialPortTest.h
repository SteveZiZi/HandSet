#ifndef     __SERIAL_PORT_TEST_H__
#define     __SERIAL_PORT_TEST_H__

//���ڲ��ԵĲ���
#define     SERIAL_PORT_TEST_CTL_LINE       0       //���Կ����ź���
#define     SERIAL_PORT_TEST_RECV           1       //����
#define     SERIAL_PORT_TEST_SEND           2       //���Է���
#define     SERIAL_PORT_TEST_OPEN           3       //�򿪴���
#define     SERIAL_PORT_TEST_CLOSE          4       //�رմ���

//���崮������
#define     SERIAL_PORT_TYPE_SIMPLE         0       //��ͨ���ߴ���
#define     SERIAL_PORT_TYPE_FULL_FUNC      1       //ȫ���ܴ���

#include "xCom.h"
class CSerialPortTest:public CTestBase
{
public:
    CSerialPortTest();
    ~CSerialPortTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const;

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:

    int         m_iTestStep;                        //��ǰ���Բ���
    int         m_iTestStepCount;                   //��ǰ���Բ����Ѿ����еĴ���

    UINT32      m_uiErrMask;                        //��SIGNAL_LINE_INDEX���ź����Ƿ����
    int         m_iDataLen;                         //���ͺͽ��մ��ڴ�������ݰ�����

    int         m_iSerialType;                      //��������
    int         m_iLocalPort;                       //���ش��ڱ��
    CxCom       m_xCom;                             //���ݱ��ش���
    int         m_iBraudRate;

    TEST_STATUS ExcuteOneCTLLinTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneRecvTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneSendTest(CConnect *pConnect);
};



#endif