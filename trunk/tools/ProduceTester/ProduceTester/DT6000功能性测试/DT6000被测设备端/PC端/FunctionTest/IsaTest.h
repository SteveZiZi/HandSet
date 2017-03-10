#ifndef     __ISA_TEST_H__
#define     __ISA_TEST_H__

//ISA����

//ISA���ԵĲ���
#define     ISA_TEST_MEM_16         0           //16λ�ڴ����
#define     ISA_TEST_MEM_8          1           //8λ�ڴ����
#define     ISA_TEST_INT            2           //ISA�жϲ���

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
    int             m_iTestStepCount;           //ÿһ�����Եļ���
};



#endif