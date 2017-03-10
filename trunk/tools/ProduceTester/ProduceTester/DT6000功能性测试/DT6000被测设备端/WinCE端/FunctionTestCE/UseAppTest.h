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

    wstring     m_wstrApp;              //欲执行可执行文件路径
    int         m_iTerminateDemoWay;    //终止Demo程序的方式
    HANDLE      m_hDemoProcesss;
    DWORD       m_dwDemoProcessID;      //Demo程序的进程ID

    wstring     m_wstrDemoWindowtitle;  //Demo程序主窗口的标题
    UINT        m_uiMessage;            //要关闭Demo程序需要发送的消息
    DWORD       m_dwParam1;
    DWORD       m_dwParam2;
    HWND        m_hDemoMainWnd;         //Demo程序的主窗口句柄

    wstring     m_wstrTerminateCmd;     //要关闭demo程序所需要执行的命令

    static BOOL CALLBACK __EnumWindowProc(HWND hwnd, LPARAM lParam);
};

#endif