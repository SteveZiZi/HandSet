#ifndef     __SCREEN_TEST_H__
#define     __SCREEN_TEST_H__


#define  SCREEN_PARAM_INIT                  0
#define  SCREEN_PARAM_END                   1
#define  SCREEN_PARAM_TEST_COLOR_LEVEL      2
#define  SCREEN_PARAM_TEST_IMAGE            3

class CScreenTest:public CTestBase
{
public:
    CScreenTest();
    ~CScreenTest(void);

    BOOL    Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);
     TEST_STATUS OnConnectDown();
private:
    TEST_INFO       m_ti;
    void            *m_pFullDlgWnd;         //����ʹ��void *����Ϊ�˷�ֹ������Ҫ
                                            //ʹ�ô�ͷ�ļ����ļ�����stdafx.h
    //��ʾȫ�����ڵ��߳�
    static DWORD CALLBACK ScreenTestThread(void *pVoid);

    BOOL InitScreenTest();
};


#endif