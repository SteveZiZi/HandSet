#ifndef         __RELAY_TEST_H__
#define         __RELAY_TEST_H__

class CRelayTest:public CTestBase
{
public:
    CRelayTest();
    ~CRelayTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 4;};

    TEST_STATUS OnBeginTest(CConnect *pConnect) ;
    TEST_STATUS OnEndTest(CConnect *pConnect){return TEST_NORMAL;};
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:

    unsigned int     m_iRelayNum;            //继电器数目
    unsigned int     m_iOpenInputNum;        //开漏输入数目
};


#endif