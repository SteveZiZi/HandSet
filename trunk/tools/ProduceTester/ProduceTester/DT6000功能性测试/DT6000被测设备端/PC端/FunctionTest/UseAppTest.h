#ifndef         __USE_APP_TEST_H__
#define         __USE_APP_TEST_H__

class CUseAppTest:public CTestBase
{
public:
    CUseAppTest();    

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const{return 1;} ;

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:

    BOOL        m_bShowInfo;                //�Ƿ��ڿ�ʼ����ǰ��ʾ��ʾ��Ϣ
    WCHAR       m_wszInfo[512];             //��ʼ����ǰ��ʾ����ʾ��Ϣ
    WCHAR       m_wszQuestion[512];         //���Խ���������ʾ������
    BOOL        m_bAskQuesion;              //���Խ������Ƿ�Ҫ������
    DWORD       m_dwCorrectExitCode;        //�����ѯ������ȷ���˳���Ӧ����

};


#endif