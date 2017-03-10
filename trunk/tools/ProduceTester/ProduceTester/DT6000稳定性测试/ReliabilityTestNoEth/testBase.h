#ifndef __TEST_BASE_H__
#define __TEST_BASE_H__

//�������в�����Ŀ�Ļ���CTestBase

//����������
typedef enum _TEST_SITUATION
{
    SITUATION_ERROR=0,                  //������ ���Դ���
    SITUATION_NORMAL,                   //������ ��������
    SITUATION_DEAD,                     //������ ���Ե�ʱ�� ִ�в��Եĺ������߳������ˣ�û����Ӧ
    
}TEST_SITUATION;


//���������Ĳ���״̬
typedef enum _TEST_STATUS
{
    STATUS_WAITING=0,                   //������
    STATUS_TESTING,                     //���ڲ���
    STATUS_STOP,                        //��ֹͣ��
    STATUS_BE_STOPPED,                  //��ΪĳЩԭ��ֹͣ
    STATUS_DEAD,                        //����������
    
}TEST_STATUS;

//������Ա���ı���һ���У�Ҳ����һ���Ӽ�¼��
struct SReportNape
{
    WCHAR strNape[32];                  //�Ӽ�¼�������
    WCHAR strNapeDescriptor[128];       //�Ӽ�¼�������
    BOOL  bHighLight;                   //�Ӽ�¼���Ƿ������ʾ
};

class CTestManager;

class CTestBase
{
public:
    CTestBase():
      m_uiTestCount(0),
      m_uiErrCount(0),
      m_uiTestTime(0),
      m_testStatus(STATUS_WAITING),
      m_pManager(NULL)
    {
        m_strTestName[0]=0;
        m_strLastError[0]=0;
    }

    virtual ~CTestBase()    {};
    virtual BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager)=0;     
    virtual TEST_SITUATION ExcuteOneTest()=0; 

    virtual const WCHAR *GetTestObjName()const  {return m_strTestName;};   
    virtual unsigned int GetTestCount()const    {return m_uiTestCount;};
    virtual unsigned int GetErrCount()const     {return m_uiErrCount; };
    virtual const WCHAR *GetLastErrorMsg()const {return m_strLastError;};  


    

    //��ȡ����Ĳ���״̬
    const TEST_STATUS GetTestSTatus()const      {return m_testStatus;};
    void SetTestStatus(TEST_STATUS status)      {m_testStatus=status;};

    //��Ӳ���ʱ�䣬һ���������ÿ���ֻ��Ĳ���ʱ���ɲ��Թ�������ͳ�ƣ�
    //������AddTestTime��ʱ���ۼӵ�ÿ��������Ĳ���ʱ���ܺ���
    void AddTestTime(unsigned int uiTestTime)   {m_uiTestTime+=uiTestTime;};
    //��ȡ���Ա������������������ؼ�¼
    virtual const SReportNape *GetReportNapes(int &count)=0;

protected:
    unsigned int    m_uiTestCount;                      //�Ѿ����ԵĴ���
    unsigned int    m_uiErrCount;                       //����Ĵ���
    WCHAR           m_strTestName[32];                  //�������������
    WCHAR           m_strLastError[256];                //�������һ�γ�����ı�����    
    //���±����������ɲ��Ա�������Ҫ����
    SReportNape     m_reportNape[8];
    UINT64          m_uiTestTime;                       //��������һ�����Ե�ʱ��,msΪ��λ
    //m_testStatus������ʾ�˲�����Ĳ���״̬������״̬������Ϊ��������ȡ״̬��Ϣ
    //��ʹ�ǲ��Թ�����Ҳ������Ϊ����ֵ�������Ƿ�������������Ĳ���
    TEST_STATUS     m_testStatus;    

     CTestManager    *m_pManager;
};  




#endif