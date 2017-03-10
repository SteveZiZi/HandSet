#ifndef     __WDT_TEST_H__
#define     __WDT_TEST_H__

//ø¥√≈π∑≤‚ ‘
class CWdtTest:public CTestBase
{
public:
    CWdtTest();
    ~CWdtTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);

  
};


#endif