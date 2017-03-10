#ifndef     __SERIAL_PORT_TEST_H__
#define     __SERIAL_PORT_TEST_H__

#include "xComCE.h"
class CSerialPortTest:public CTestBase
{
public:
    CSerialPortTest() ;
    ~CSerialPortTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);

    TEST_STATUS OnConnectDown(){m_serialCom.CloseCom();return TEST_NORMAL;};

private:
    CxComCE     m_serialCom;
    int         m_iSerialPort;              //本测试项对应的串口编号
    int         m_iBraudRate;
    TEST_INFO   m_ti;
    int         m_iDataLen;                 //发送和接收串口传输的数据包长度
    
    TEST_STATUS ExcuteOneCTLLinTest(UINT16 uiDatalen,const BYTE *pData,CConnect *pConnect);
    TEST_STATUS ExcuteOneRecvTest(UINT16 uiDatalen,const BYTE *pData,CConnect *pConnect);
    TEST_STATUS ExcuteOneSendTest(UINT16 uiDatalen,const BYTE *pData,CConnect *pConnect);
};



#endif