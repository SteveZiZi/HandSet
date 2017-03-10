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

    
    UCHAR           m_ucPort1;                  //用于测试的串口编号1
    UCHAR           m_ucPort2;                  //用于测试的串口编号2
    BOOL            m_bDir;                     //串口发送数据的方向，真表示从串口编号1
                                                //发送到串口编号2，假则相反
    //定义串口的数据描述
    typedef struct _SERIALDATA {
        char cBuf[1024];
        DWORD dwBufCnt;       
        HANDLE hRecvEvent;
    } SERIALDATA;

    SERIALDATA      m_serData[2];               //存储要测试的串口编号1和2的数据
    BOOL            InitSerialDev();            //初始化要测试的串口设备
    TEST_SITUATION  ExcuteOneTestLoopBack();    //执行一次自发自收的测试
    TEST_SITUATION  ExcuteOneTestEachOther();   //执行一次对发对收的测试
    BOOL            m_bDevInitSucess;           //串口设备初始化是否成功

    //编号1的串口接收到数据的回调函数
    static void  CALLBACK OnCom1Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);
    //编号2的串口接收到数据的回调函数
    static void  CALLBACK OnCom2Recv(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);
    BYTE            m_pDatabuf[1024];
};


#endif