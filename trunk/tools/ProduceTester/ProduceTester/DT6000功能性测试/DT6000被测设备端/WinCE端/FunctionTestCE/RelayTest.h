#ifndef         __RELAY_TEST_H__
#define         __RELAY_TEST_H__

class   CRelayTest:public CTestBase
{
public:
    CRelayTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:
    BOOL (*epcRelayInit)();
    BOOL (*epcRelayOn)(DWORD  dwIndex);
    BOOL (*epcRelayOff)(DWORD  dwIndex);
    DWORD (*epcGetOpenInputState)(DWORD  dwIndex);

    BOOL    LoadRelayTestLib();

    unsigned int     m_iRelayNum;            //继电器数目
    unsigned int     m_iOpenInputNum;        //开漏输入数目

};





#endif