#ifndef     __GPS_TEST_H__
#define     __GPS_TEST_H__

#include "xComCE.h"
class CGpsTest:public CTestBase
{
public:
    BOOL            Init(const WCHAR *strIniFile, int index,
        UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS     ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
        const BYTE *pData,CConnect *pConnect);

private:

    TEST_INFO       m_ti;

    CxComCE         m_xComCE;

    int             m_iComIndex;                //GPRS模块所使用的串口编号
    int             m_iBraudRate;
};



#endif