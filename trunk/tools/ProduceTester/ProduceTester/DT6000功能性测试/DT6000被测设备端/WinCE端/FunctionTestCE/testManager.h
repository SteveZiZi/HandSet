#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testCommon.h"
#include "testBase.h"
//定义CTestManager类，用于管理一个连接上的设备
//此类并未提供拷贝构造函数，请不要进行此类操作


class CTestManager
{
public:
    CTestManager();
    ~CTestManager();

    //初始化测试管理对象,strDeviceType为本设备类型
    BOOL Init(const WCHAR *strDeviceType,HWND  hMainWnd);
    BOOL OnConnectDown();

    BOOL GetDeviceInfo(DEVICE_INFO *pInfo);
    int     ProcessOnePacket();

    //当一个连接对象连接到主机时需要调用这个函数
    BOOL OnConnect(CConnect *pConnect);
    CConnect *DetachConnect()
    {
        CConnect *p=m_pConnect;
        m_pConnect=NULL;
        return p;
    }
    CConnect *GetConnect()const{return m_pConnect;};
    int TestConnect();


//     //设置CTestManager对象所要使用的CConnect对象
//     //CTestManager并不负责CConnect对象的分配和释放
//     //所以请保证一个连接在被CTestManager对象所使用的过程中的有效性
//     void SetConnect(CConnect *pConnect)
//     {
//         m_pConnect=pConnect;
//     }
    HWND   GetMainWnd(){return m_hMainWnd;};

private:

    //在这里我们定义测试命令索引码与实际执行测试的CTestBase对象之间的映射关系
    //实际的映射方法是执行命令索引码i的CTestBase对象的指针为m_pTestObj序号为i的元素的值
    CTestBase   *m_pTestObj[MAX_TEST_OBJ];   
    CConnect     *m_pConnect;

    PACKET      m_pa;
    //测试设备类型
    wchar_t     m_strDeviceType[32];
    wchar_t     m_strIniFilePath[MAX_PATH*2];

    HWND        m_hMainWnd;             //测试程序CE端的主窗口句柄
};

#endif