#ifndef     __GPRS_TEST_H__
#define     __GPRS_TEST_H__

#include "GPRS.h"
#include "xComCE.h"

class CGPRSTest:public CTestBase
{
public:

    BOOL            Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS     ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);

private:
    
    TEST_INFO       m_ti;
    CGPRS           m_gprs;

    int             m_iComIndex;                //GPRS模块所使用的串口编号
    int             m_iBraudRate;
};



#endif