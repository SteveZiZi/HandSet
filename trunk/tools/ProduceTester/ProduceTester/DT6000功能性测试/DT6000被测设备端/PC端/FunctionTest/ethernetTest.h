#ifndef  __ETHERNET_TEST__H__
#define  __ETHERNET_TEST__H__


//ÒÔÌ«Íø²âÊÔ
class CEthernetTest:public CTestBase
{  
public:
    CEthernetTest();
    ~CEthernetTest();

    BOOL Init(const WCHAR *strIniFile, int index,CTestManager *pManager);
    int  GetShouldTestTimes()const {return 100;};

    TEST_STATUS OnBeginTest(CConnect *pConnect);
    TEST_STATUS OnEndTest(CConnect *pConnect);
    TEST_STATUS ExcuteOneTest(CConnect *pConnect);
private:

    TSocket     m_comSocket;
};

#endif