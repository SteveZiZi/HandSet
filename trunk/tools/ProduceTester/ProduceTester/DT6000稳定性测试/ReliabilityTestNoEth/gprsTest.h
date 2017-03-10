#ifndef     __GPRS_TEST_H__
#define     __GPRS_TEST_H__

#include "GPRS.h"
class CGPRSTest:public CTestBase
{
public:
    CGPRSTest();

    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);

private:
    CGPRS           m_GPRS;
    int             m_nPort;            //¶Ë¿ÚºÅ
    unsigned int    m_uiBraudRate;      //²¨ÌØÂÊ
};




#endif