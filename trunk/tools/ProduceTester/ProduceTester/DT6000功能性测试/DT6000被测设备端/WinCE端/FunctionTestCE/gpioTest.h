#ifndef     __GPIO_TEST_H
#define     __GPIO_TEST_H

class   CGpioTest:public CTestBase
{
public:
    CGpioTest();
    ~CGpioTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:    
 
};



#endif