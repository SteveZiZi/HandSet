#ifndef     __LED_TEST_H__
#define     __LED_TEST_H__

class CLedTest:public CTestBase
{
public:
    CLedTest();
    ~CLedTest();

    BOOL            Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS     ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                        const BYTE *pData,CConnect *pConnect);

private:
    void (*epcSetLedMask)(DWORD    dwMask);
    void (*epcGetLedMask)(DWORD    *dwMask);

    HINSTANCE       m_hInstLedDll;             //º”‘ÿledøÿ÷∆dll
    TEST_INFO       m_ti;
    int             m_iLedNum;

    DWORD           m_dwTestCount;
};



#endif