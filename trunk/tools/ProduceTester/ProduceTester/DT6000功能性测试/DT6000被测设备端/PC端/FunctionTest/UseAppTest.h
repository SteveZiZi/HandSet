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

    BOOL        m_bShowInfo;                //是否在开始测试前显示提示信息
    WCHAR       m_wszInfo[512];             //开始测试前显示的提示信息
    WCHAR       m_wszQuestion[512];         //测试结束后所提示的问题
    BOOL        m_bAskQuesion;              //测试结束后是否要问问题
    DWORD       m_dwCorrectExitCode;        //如果不询问则正确的退出码应该是

};


#endif