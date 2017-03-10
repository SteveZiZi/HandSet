#ifndef     __7290_TEST_H__
#define     __7290_TEST_H__

#include "FunctionTestCE.h"
#include "7290TestDlg.h"
class  C7290Test:public CTestBase
{
public:
    C7290Test();
    ~C7290Test();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);
    virtual TEST_STATUS OnConnectDown();
    
private:

    C7290TestDlg    *m_pTestDlg;
    HANDLE          m_hTestThread;              //²âÊÔÏß³Ì¾ä±ú

    static DWORD CALLBACK TestProc(LPVOID pVoid);
};



#endif