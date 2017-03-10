#ifndef     __KBD_TEST_H__
#define     __KBD_TEST_H__

class CKbdTest:public CTestBase
{
public:
    CKbdTest();
    ~CKbdTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
        const BYTE *pData,CConnect *pConnect);
private:

    void        *m_pDlg;

    WCHAR       m_strExtFilePath[MAX_PATH*2];

};

#endif