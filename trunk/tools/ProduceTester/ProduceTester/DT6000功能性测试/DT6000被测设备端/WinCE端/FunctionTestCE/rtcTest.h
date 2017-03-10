#ifndef     __RTC_TEST_H__
#define     __RTC_TEST_H__

class CRTCTest:public CTestBase
{
public:
    CRTCTest();
    ~CRTCTest();
    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);

private:   
};


#endif