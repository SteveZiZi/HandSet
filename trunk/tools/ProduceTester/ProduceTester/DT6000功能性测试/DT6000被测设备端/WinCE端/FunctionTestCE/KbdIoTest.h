#ifndef     __KBD_IO_TEST_H__
#define     __KBD_IO_TEST_H__

//扩展键盘IO（通常采用和8000系列一样的gpio接口，这里适用于,既引出了CPU本身的IO，
//又另外扩展出的IO）的情况

class CKbdIoTest:public CTestBase
{
public:
    CKbdIoTest();
    ~CKbdIoTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:

    int (*epcIoWrite)(DWORD dwPinState);
    int (*epcIoRead)(DWORD *dwPinState);

    HINSTANCE       m_hInstGpioDll;             //加载gpio控制dll
};



#endif