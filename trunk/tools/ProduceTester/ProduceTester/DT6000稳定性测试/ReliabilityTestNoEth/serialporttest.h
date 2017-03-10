#ifndef     __SERIAL_PORT_TEST_H__
#define     __SERIAL_PORT_TEST_H__

class   CSerialPortTest:public CTestBase
{
public:
    CSerialPortTest();
    ~CSerialPortTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);
private:
    static const int __SERIAL_DATA_LEN=100;

    
    UCHAR           m_ucPort1;                  //���ڲ��ԵĴ��ڱ��1
    UCHAR           m_ucPort2;                  //���ڲ��ԵĴ��ڱ��2
    BOOL            m_bDir;                     //���ڷ������ݵķ������ʾ�Ӵ��ڱ��1
                                                //���͵����ڱ��2�������෴
    //���崮�ڵ���������
    typedef struct _SERIALDATA {
        char cBuf[1024];
        DWORD dwBufCnt;       
        HANDLE hRecvEvent;
    } SERIALDATA;

    SERIALDATA      m_serData[2];               //�洢Ҫ���ԵĴ��ڱ��1��2������
    BOOL            InitSerialDev();            //��ʼ��Ҫ���ԵĴ����豸
    TEST_SITUATION  ExcuteOneTestLoopBack();    //ִ��һ���Է����յĲ���
    TEST_SITUATION  ExcuteOneTestEachOther();   //ִ��һ�ζԷ����յĲ���
    BOOL            m_bDevInitSucess;           //�����豸��ʼ���Ƿ�ɹ�

    //���1�Ĵ��ڽ��յ����ݵĻص�����
    static void  CALLBACK OnCom1Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);
    //���2�Ĵ��ڽ��յ����ݵĻص�����
    static void  CALLBACK OnCom2Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);
    BYTE            m_pDatabuf[1024];
};


#endif