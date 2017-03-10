#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testBase.h"
#include "Ft3Tx.h"
#include <string>
using namespace std;
//����CTestManager�࣬���ڹ���һ�������ϵ��豸
//���ಢδ�ṩ�������캯�����벻Ҫ���д������

#define     MAX_TEST_OBJ  64


//������Թ������򴰿ڷ��͵���Ϣ
#define  UM_TEST_COUNT_CHANGE         WM_USER +50             //���Դ����ı�

#define  UM_TEST_STATUS_CHANGE      WM_USER +51             //����״̬�ı�


class CTestManager
{
public:
    CTestManager();
    ~CTestManager();
    //��ʼ�����Թ������,strDeviceTypeΪ���豸����
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
    CTestBase   *m_pTestObj[MAX_TEST_OBJ];              //�洢ָ���������Ķ����ָ��
    int         m_iTestObjNum;                          //��Ч�Ĳ�����ĸ���
    int         m_iCurTestObj;                          //��ǰ���ڲ��ԵĲ�������m_pTestObj������
    BOOL        m_pbTest[MAX_TEST_OBJ];                 //m_pTestObj��Ԫ�ض�Ӧ�Ĳ������Ƿ���в���
    BOOL        m_bParallelTest;                        //�Ƿ��в���

    //�����豸����
    wchar_t     m_strDeviceType[32];
    wchar_t     m_strIniFilePath[MAX_PATH*2];

    //������һ��������߳���صı����ͺ���
    static DWORD CALLBACK TestProc(void *pVoid);        //�����̺߳���
    static DWORD CALLBACK ParallelTestProc(void *pVoid);//�����̺߳���
    HANDLE          m_hTestThread;                      //��ѯ�����߳̾��
    BOOL            m_bStopTest;                        //�Ƿ�ֹͣ����
    SYSTEMTIME      m_stBegin;                          //���Կ�ʼ��ʱ��
    HANDLE          m_phParallelHandle[MAX_TEST_OBJ];   //���в����߳̾��


    //������һ��������
    BOOL ChangeTest();
    
    //����һ������������Ʋ�������ֻ�
    BOOL            m_bChangTest;                       //�Ƿ����������
    HANDLE          m_hTimmerThread;                    //��ʱ���̣߳����ھ�����ʱ������һ��������
    HANDLE          m_hStopTimmerThread;                //�����Ƿ�ֹͣ��ʱ���̵߳��¼�����
    static DWORD   CALLBACK TimerThreadProc(void *pVoid);//��ʱ���̺߳���
    

    HWND        m_hMainWnd;                             //���Ͳ�����Ϣ��Ŀ�괰��
    wstring     m_strServerIP;
    //����Թ��������һ��������
    BOOL        AddTestObj(CTestBase *pTest);
    BOOL        DisableTestObj(int index);              //���õ�ĳ��������

    CFt3Tx      *m_pFt3Tx;
    bool        m_bSendFt3;

};

#endif