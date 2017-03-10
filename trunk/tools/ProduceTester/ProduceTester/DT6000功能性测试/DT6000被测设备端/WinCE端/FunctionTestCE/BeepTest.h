#ifndef  __BEEP_TEST_H__
#define  __BEEP_TEST_H__

class CBeepTest:public CTestBase
{
public:
    CBeepTest();
    ~CBeepTest();
    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
private:
    BOOL    (* epcBuzzerOn)         (DWORD dwOntime);
    BOOL    (* epcBuzzerOff)        (void);
    DWORD   (* epcBuzzerGetStatus)  (void);
    HINSTANCE       m_hInstBeepDll;             //º”‘ÿgpioøÿ÷∆dll
};


#endif