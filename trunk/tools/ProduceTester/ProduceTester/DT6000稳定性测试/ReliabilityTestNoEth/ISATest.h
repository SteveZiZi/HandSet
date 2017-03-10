#ifndef     __ISA_TEST_H__
#define     __ISA_TEST_H__

typedef enum _ISA_TEST_STEP
{
    ISA_TEST_MEM_16=0,
    ISA_TEST_MEM_8,
    ISA_TEST_INT
}ISA_TEST_STEP;

class CISATest:public CTestBase
{
public:
    CISATest();
    ~CISATest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);
private:
   
    ISA_TEST_STEP   m_testStep;
    int             m_uiTestTimeStep;           //ÿһ���������ⲽ���Խ��еĴ���
    BYTE            *m_pBufWirte,*m_pbufRead;   //ָ���д������
    TEST_SITUATION  ExcuteOneMem16Test();       //ִ��һ��16λ�ڴ����
    TEST_SITUATION  ExcuteOneMem8Test();        //ִ��һ��8λ�ڴ����
    TEST_SITUATION  ExcuteOneIntTest();         //ִ��һ���жϲ���
    HANDLE          m_hIntEvent;                //�жϵ��¼�����
};


#endif