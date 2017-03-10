#ifndef     __WIFI_TEST_H__
#define     __WIFI_TEST_H__

class CWifiTest:public CTestBase
{
public:
    CWifiTest();
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);
private:
    
    WCHAR   m_strDestIp[64];
};




#endif