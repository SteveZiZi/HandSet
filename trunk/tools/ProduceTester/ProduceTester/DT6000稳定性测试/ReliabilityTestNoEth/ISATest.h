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
    int             m_uiTestTimeStep;           //每一步测试中这步测试进行的次数
    BYTE            *m_pBufWirte,*m_pbufRead;   //指向读写缓冲区
    TEST_SITUATION  ExcuteOneMem16Test();       //执行一次16位内存测试
    TEST_SITUATION  ExcuteOneMem8Test();        //执行一次8位内存测试
    TEST_SITUATION  ExcuteOneIntTest();         //执行一次中断测试
    HANDLE          m_hIntEvent;                //中断的事件对象
};


#endif