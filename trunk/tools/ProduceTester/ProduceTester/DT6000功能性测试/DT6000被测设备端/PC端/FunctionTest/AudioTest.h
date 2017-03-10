#ifndef     __AUDIO_TEST_H__
#define     __AUDIO_TEST_H__


//������Ƶ���Բ�������
#define    PARAM_BEGIN_RECORD           0       //��ʼ¼��
#define    PARAM_STOP_RECORD            1       //ֹͣ¼��
#define    PARAM_BEGIN_PLAY             2       //��ʼ��������
#define    PARAM_STOP_PLAY              3       //ֹͣ��������
#define    PARAM_GET_POS_RECORD         4       //��ȡ¼�Ƶ�����λ����Ϣ
#define    PARAM_GET_POS_PLAY           5       //��ȡ������λ����Ϣ

class CAudioTest:public CTestBase
{
public:
    CAudioTest();
    ~CAudioTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const;

    TEST_STATUS OnBeginTest(CConnect *pConnect) ;
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:
    BOOL    m_bTestRecord;                  //�Ƿ����¼��
    

};


#endif