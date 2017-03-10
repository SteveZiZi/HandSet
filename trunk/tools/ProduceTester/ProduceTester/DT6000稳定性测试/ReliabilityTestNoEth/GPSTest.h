#ifndef     __GPS_TEST_H__
#define     __GPS_TEST_H__

#include "xComCE.h"
class CGpsTest:public CTestBase
{
public:
    CGpsTest();
    BOOL    Init(const WCHAR *strIniFile, int index,CTestManager *pManager);    
    TEST_SITUATION      ExcuteOneTest();
    const SReportNape *GetReportNapes(int &count);



private:

    int             m_nPort;                //GPS´®¿Ú±àºÅ
    CxComCE         m_xCom;

    BYTE buf[2048];

    
    
};




#endif