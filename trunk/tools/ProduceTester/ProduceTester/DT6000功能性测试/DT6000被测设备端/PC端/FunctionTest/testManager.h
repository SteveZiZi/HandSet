#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testCommon.h"
#include "connect.h"
#include "testBase.h"

//定义需要时，TestManager对象向窗口发送的消息
#define  UM_NEED_DISP           WM_USER +200                //需要更新显示，
#define  UM_CONECT_DOWN         WM_USER +201                //连接断开  
#define  UM_SHOW_STATUS         WM_USER +203                //显示特定的状态信息
#define  UM_TEST_BEGIN          WM_USER +204                //测试开始
#define  UM_TEST_END            WM_USER +205                //测试结束
#define  UM_TEST_INIT_ERROR     WM_USER +206                //测试项初始化失败
#define  UM_TEST_DEINIT_ERROR   WM_USER +207                //测试项释放资源失败
#define  UM_CONNECT_FINISH      WM_USER +208                //PC连接到工控机的操作完成

//定义CTestManager类，用于管理一个连接上的设备
//此类并未提供拷贝构造函数，请不要进行此类操作
class CTestBase;
class CTestManager
{
public:
    
    CTestManager();
    ~CTestManager();

    //用于初始化此测试管理对象
    //pConnect是一个已经连接上的对象指针
    //hMainWnd是用于处理各种连接消息的窗口
    BOOL    Init(CConnect *pConnect,HWND hMainWnd);
    void    showTestStatus(LPCWSTR strTestStatus);

    const  DEVICE_INFO *GetDeviceInfo()const{return &m_devInfo;};
    //获取测试项的数目
    int GetTestCount()const{return m_iTestObjNum;};
    //将CConnect对象与测试管理对象分离，在连接断开时调用
    CConnect *DettachConnect();
    CConnect *ReplaceConnect(CConnect *pConnect);
    CConnect *GetConnect()      {return m_pConnect; };

    HWND    GetMainWnd()const {return m_hMainWnd; };
    //由索引获取测试对象
    CTestBase *GetTestIndex(int index){return m_pTestObj[index];};
    int MessageBox(LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)
    {
        return ::MessageBox(m_hMainWnd,lpText,lpCaption,uType);
    }

    BOOL    WriteLog(CTestBase *pTest,const WCHAR *strLog);

private:
    CConnect        *m_pConnect;
    DEVICE_INFO     m_devInfo;

    //定义需要测试的项目
    CTestBase       *m_pTestObj[MAX_TEST_OBJ];  
    int             m_iTestObjNum;                  //需要测试的项目数量
    HWND    m_hMainWnd;                             //用来相应消息的窗口
    //添加测试项
    BOOL AddTest(CTestBase *pTest);  

    //通过远程连接获取设备信息,如果成功，返回值大于0，连接对象响应超时返回0，
    //连接对象出错返回值小于0
    int     GetDeviceInfo(CConnect *pConnect,DEVICE_INFO *pDevInfo);
};


#endif