#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testCommon.h"
#include "connect.h"
#include "testBase.h"

//������Ҫʱ��TestManager�����򴰿ڷ��͵���Ϣ
#define  UM_NEED_DISP           WM_USER +200                //��Ҫ������ʾ��
#define  UM_CONECT_DOWN         WM_USER +201                //���ӶϿ�  
#define  UM_SHOW_STATUS         WM_USER +203                //��ʾ�ض���״̬��Ϣ
#define  UM_TEST_BEGIN          WM_USER +204                //���Կ�ʼ
#define  UM_TEST_END            WM_USER +205                //���Խ���
#define  UM_TEST_INIT_ERROR     WM_USER +206                //�������ʼ��ʧ��
#define  UM_TEST_DEINIT_ERROR   WM_USER +207                //�������ͷ���Դʧ��
#define  UM_CONNECT_FINISH      WM_USER +208                //PC���ӵ����ػ��Ĳ������

//����CTestManager�࣬���ڹ���һ�������ϵ��豸
//���ಢδ�ṩ�������캯�����벻Ҫ���д������
class CTestBase;
class CTestManager
{
public:
    
    CTestManager();
    ~CTestManager();

    //���ڳ�ʼ���˲��Թ������
    //pConnect��һ���Ѿ������ϵĶ���ָ��
    //hMainWnd�����ڴ������������Ϣ�Ĵ���
    BOOL    Init(CConnect *pConnect,HWND hMainWnd);
    void    showTestStatus(LPCWSTR strTestStatus);

    const  DEVICE_INFO *GetDeviceInfo()const{return &m_devInfo;};
    //��ȡ���������Ŀ
    int GetTestCount()const{return m_iTestObjNum;};
    //��CConnect��������Թ��������룬�����ӶϿ�ʱ����
    CConnect *DettachConnect();
    CConnect *ReplaceConnect(CConnect *pConnect);
    CConnect *GetConnect()      {return m_pConnect; };

    HWND    GetMainWnd()const {return m_hMainWnd; };
    //��������ȡ���Զ���
    CTestBase *GetTestIndex(int index){return m_pTestObj[index];};
    int MessageBox(LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)
    {
        return ::MessageBox(m_hMainWnd,lpText,lpCaption,uType);
    }

    BOOL    WriteLog(CTestBase *pTest,const WCHAR *strLog);

private:
    CConnect        *m_pConnect;
    DEVICE_INFO     m_devInfo;

    //������Ҫ���Ե���Ŀ
    CTestBase       *m_pTestObj[MAX_TEST_OBJ];  
    int             m_iTestObjNum;                  //��Ҫ���Ե���Ŀ����
    HWND    m_hMainWnd;                             //������Ӧ��Ϣ�Ĵ���
    //��Ӳ�����
    BOOL AddTest(CTestBase *pTest);  

    //ͨ��Զ�����ӻ�ȡ�豸��Ϣ,����ɹ�������ֵ����0�����Ӷ�����Ӧ��ʱ����0��
    //���Ӷ��������ֵС��0
    int     GetDeviceInfo(CConnect *pConnect,DEVICE_INFO *pDevInfo);
};


#endif