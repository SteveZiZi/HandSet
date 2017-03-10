#ifndef _TEST_MANAGER_H__
#define _TEST_MANAGER_H__

#include "testCommon.h"
#include "testBase.h"
//����CTestManager�࣬���ڹ���һ�������ϵ��豸
//���ಢδ�ṩ�������캯�����벻Ҫ���д������


class CTestManager
{
public:
    CTestManager();
    ~CTestManager();

    //��ʼ�����Թ������,strDeviceTypeΪ���豸����
    BOOL Init(const WCHAR *strDeviceType,HWND  hMainWnd);
    BOOL OnConnectDown();

    BOOL GetDeviceInfo(DEVICE_INFO *pInfo);
    int     ProcessOnePacket();

    //��һ�����Ӷ������ӵ�����ʱ��Ҫ�����������
    BOOL OnConnect(CConnect *pConnect);
    CConnect *DetachConnect()
    {
        CConnect *p=m_pConnect;
        m_pConnect=NULL;
        return p;
    }
    CConnect *GetConnect()const{return m_pConnect;};
    int TestConnect();


//     //����CTestManager������Ҫʹ�õ�CConnect����
//     //CTestManager��������CConnect����ķ�����ͷ�
//     //�����뱣֤һ�������ڱ�CTestManager������ʹ�õĹ����е���Ч��
//     void SetConnect(CConnect *pConnect)
//     {
//         m_pConnect=pConnect;
//     }
    HWND   GetMainWnd(){return m_hMainWnd;};

private:

    //���������Ƕ������������������ʵ��ִ�в��Ե�CTestBase����֮���ӳ���ϵ
    //ʵ�ʵ�ӳ�䷽����ִ������������i��CTestBase�����ָ��Ϊm_pTestObj���Ϊi��Ԫ�ص�ֵ
    CTestBase   *m_pTestObj[MAX_TEST_OBJ];   
    CConnect     *m_pConnect;

    PACKET      m_pa;
    //�����豸����
    wchar_t     m_strDeviceType[32];
    wchar_t     m_strIniFilePath[MAX_PATH*2];

    HWND        m_hMainWnd;             //���Գ���CE�˵������ھ��
};

#endif