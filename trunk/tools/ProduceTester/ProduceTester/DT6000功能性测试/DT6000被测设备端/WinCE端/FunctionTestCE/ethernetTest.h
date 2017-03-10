#ifndef  __ETHERNET_TEST__H__
#define  __ETHERNET_TEST__H__

//以太网测试
class CEthernetTest:public CTestBase
{  
public:
    CEthernetTest();
    ~CEthernetTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:
    WCHAR       m_strEthName[64];                       //要测试的网卡名称

    TSocket     m_socketServer;                         //服务器套接字
    TSocket     *m_pSocketComm;                         //用于通信的套接字

    struct in_addr  m_addLocalEth;                      //要测试的网卡的IP地址
    BOOL        m_bFindEth;                             //是否找到要测试的网卡
    BOOL        m_bEthLinked;                           //要测试的网卡是否已经连接网线

    //定义服务器线程相关
    static DWORD CALLBACK ServerThreadProc(void *pVoid);
    HANDLE      m_hServerThread;
    BOOL        m_bKillServerThread;                    //是否杀死服务器线程

    //定义测试线程相关
    static DWORD CALLBACK TestThreadProc(void *pVoid);  //测试线程函数
    HANDLE      m_hTestThread;                          //测试线程句柄
};


#endif