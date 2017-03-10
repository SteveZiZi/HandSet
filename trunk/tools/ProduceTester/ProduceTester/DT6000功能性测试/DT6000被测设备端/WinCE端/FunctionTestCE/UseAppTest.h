#ifndef         __USE_APP_TEST_H__
#define         __USE_APP_TEST_H__

#include <string>
using namespace std;

class CUseAppTest:public CTestBase
{
public:

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);
private:

    wstring     m_wstrApp;              //��ִ�п�ִ���ļ�·��
    int         m_iTerminateDemoWay;    //��ֹDemo����ķ�ʽ
    HANDLE      m_hDemoProcesss;
    DWORD       m_dwDemoProcessID;      //Demo����Ľ���ID

    wstring     m_wstrDemoWindowtitle;  //Demo���������ڵı���
    UINT        m_uiMessage;            //Ҫ�ر�Demo������Ҫ���͵���Ϣ
    DWORD       m_dwParam1;
    DWORD       m_dwParam2;
    HWND        m_hDemoMainWnd;         //Demo����������ھ��

    wstring     m_wstrTerminateCmd;     //Ҫ�ر�demo��������Ҫִ�е�����

    static BOOL CALLBACK __EnumWindowProc(HWND hwnd, LPARAM lParam);
};

#endif