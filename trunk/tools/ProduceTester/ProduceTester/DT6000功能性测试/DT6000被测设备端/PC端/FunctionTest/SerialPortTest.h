#ifndef     __SERIAL_PORT_TEST_H__
#define     __SERIAL_PORT_TEST_H__

//串口测试的步骤
#define     SERIAL_PORT_TEST_CTL_LINE       0       //测试控制信号线
#define     SERIAL_PORT_TEST_RECV           1       //接收
#define     SERIAL_PORT_TEST_SEND           2       //测试发送
#define     SERIAL_PORT_TEST_OPEN           3       //打开串口
#define     SERIAL_PORT_TEST_CLOSE          4       //关闭串口

//定义串口类型
#define     SERIAL_PORT_TYPE_SIMPLE         0       //普通三线串口
#define     SERIAL_PORT_TYPE_FULL_FUNC      1       //全功能串口

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

    int         m_iTestStep;                        //当前测试步骤
    int         m_iTestStepCount;                   //当前测试步骤已经进行的次数

    UINT32      m_uiErrMask;                        //按SIGNAL_LINE_INDEX各信号线是否出错
    int         m_iDataLen;                         //发送和接收串口传输的数据包长度

    int         m_iSerialType;                      //串口类型
    int         m_iLocalPort;                       //本地串口编号
    CxCom       m_xCom;                             //操纵本地串口
    int         m_iBraudRate;

    TEST_STATUS ExcuteOneCTLLinTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneRecvTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneSendTest(CConnect *pConnect);
};



#endif