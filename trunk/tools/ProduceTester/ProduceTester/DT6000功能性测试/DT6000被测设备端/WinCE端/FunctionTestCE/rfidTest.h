#ifndef         __RFID_TEST_H__
#define         __RFID_TEST_H__

class CRfidTest:public CTestBase
{
public:
    CRfidTest();
    ~CRfidTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
private:
     int                m_iComIndex;                //RFIDģ����ʹ�õĴ��ڱ��
     TEST_INFO          m_ti;

};


#endif