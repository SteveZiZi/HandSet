#ifndef     __ISA_TEST_H__
#define     __ISA_TEST_H__

//ISA测试

//ISA测试的步骤
#define     ISA_TEST_MEM_16         0           //16位内存测试
#define     ISA_TEST_MEM_8          1           //8位内存测试
#define     ISA_TEST_INT            2           //ISA中断测试

class CIsaTest:public CTestBase
{
public:
    CIsaTest();
    ~CIsaTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const ;

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);

private:

    unsigned int    m_isaTestStep;
    int             m_iTestStepCount;           //每一步测试的计数
};



#endif