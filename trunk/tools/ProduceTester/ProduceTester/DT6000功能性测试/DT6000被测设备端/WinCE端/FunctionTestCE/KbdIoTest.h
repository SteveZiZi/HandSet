#ifndef     __KBD_IO_TEST_H__
#define     __KBD_IO_TEST_H__

//��չ����IO��ͨ�����ú�8000ϵ��һ����gpio�ӿڣ�����������,��������CPU�����IO��
//��������չ����IO�������

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

    HINSTANCE       m_hInstGpioDll;             //����gpio����dll
};



#endif