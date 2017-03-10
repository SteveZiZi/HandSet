#ifndef  __ETHERNET_TEST__H__
#define  __ETHERNET_TEST__H__

//��̫������
class CEthernetTest:public CTestBase
{  
public:
    CEthernetTest();
    ~CEthernetTest();

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

private:
    WCHAR       m_strEthName[64];                       //Ҫ���Ե���������

    TSocket     m_socketServer;                         //�������׽���
    TSocket     *m_pSocketComm;                         //����ͨ�ŵ��׽���

    struct in_addr  m_addLocalEth;                      //Ҫ���Ե�������IP��ַ
    BOOL        m_bFindEth;                             //�Ƿ��ҵ�Ҫ���Ե�����
    BOOL        m_bEthLinked;                           //Ҫ���Ե������Ƿ��Ѿ���������

    //����������߳����
    static DWORD CALLBACK ServerThreadProc(void *pVoid);
    HANDLE      m_hServerThread;
    BOOL        m_bKillServerThread;                    //�Ƿ�ɱ���������߳�

    //��������߳����
    static DWORD CALLBACK TestThreadProc(void *pVoid);  //�����̺߳���
    HANDLE      m_hTestThread;                          //�����߳̾��
};


#endif