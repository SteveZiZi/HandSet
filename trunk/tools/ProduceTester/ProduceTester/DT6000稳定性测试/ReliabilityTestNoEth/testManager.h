#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testBase.h"
#include "Ft3Tx.h"
#include <string>
using namespace std;
//定义CTestManager类，用于管理一个连接上的设备
//此类并未提供拷贝构造函数，请不要进行此类操作

#define     MAX_TEST_OBJ  64


//定义测试管理器向窗口发送的消息
#define  UM_TEST_COUNT_CHANGE         WM_USER +50             //测试次数改变

#define  UM_TEST_STATUS_CHANGE      WM_USER +51             //测试状态改变


class CTestManager
{
public:
    CTestManager();
    ~CTestManager();
    //初始化测试管理对象,strDeviceType为本设备类型
    BOOL Init(const WCHAR *strDeviceType,const WCHAR *strServerIP,HWND hMainWnd);

    BOOL    BeginTest();
    BOOL    StopTest();
    BOOL    WaitForStopComplete();
    BOOL    WriteLog(CTestBase *pTest,const WCHAR *strLog);
    BOOL    BuildTestReport(LPCWSTR strReprotFile);

    int     GetTestObjCount()const {return m_iTestObjNum;};
    const CTestBase *GetTestObj(int index) const{return m_pTestObj[index];};
    const  WCHAR *GetServerIp()const{return m_strServerIP.c_str();};

private:
    CTestBase   *m_pTestObj[MAX_TEST_OBJ];              //存储指向个测试项的对象的指针
    int         m_iTestObjNum;                          //有效的测试项的个数
    int         m_iCurTestObj;                          //当前正在测试的测试项在m_pTestObj的索引
    BOOL        m_pbTest[MAX_TEST_OBJ];                 //m_pTestObj各元素对应的测试项是否进行测试
    BOOL        m_bParallelTest;                        //是否并行测试

    //测试设备类型
    wchar_t     m_strDeviceType[32];
    wchar_t     m_strIniFilePath[MAX_PATH*2];

    //以下是一组与测试线程相关的变量和函数
    static DWORD CALLBACK TestProc(void *pVoid);        //测试线程函数
    static DWORD CALLBACK ParallelTestProc(void *pVoid);//并行线程函数
    HANDLE          m_hTestThread;                      //轮询测试线程句柄
    BOOL            m_bStopTest;                        //是否停止测试
    SYSTEMTIME      m_stBegin;                          //测试开始的时间
    HANDLE          m_phParallelHandle[MAX_TEST_OBJ];   //并行测试线程句柄


    //更换下一个测试项
    BOOL ChangeTest();
    
    //以下一组变量用来控制测试项的轮回
    BOOL            m_bChangTest;                       //是否更换测试项
    HANDLE          m_hTimmerThread;                    //定时器线程，用于决定何时调换下一个测试项
    HANDLE          m_hStopTimmerThread;                //决定是否停止定时器线程的事件对象
    static DWORD   CALLBACK TimerThreadProc(void *pVoid);//定时器线程函数
    

    HWND        m_hMainWnd;                             //发送测试消息的目标窗口
    wstring     m_strServerIP;
    //向测试管理器添加一个测试项
    BOOL        AddTestObj(CTestBase *pTest);
    BOOL        DisableTestObj(int index);              //禁用掉某个测试项

    CFt3Tx      *m_pFt3Tx;
    bool        m_bSendFt3;

};

#endif