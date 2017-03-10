#ifndef     __AUDIO_TEST_H__
#define     __AUDIO_TEST_H__

#include "WavOut.h"
//“Ù∆µ≤‚ ‘¿‡
class CAudioTest:public CTestBase
{
public:
    BOOL    Init(const WCHAR *strIniFile, 
        int index,UINT16 *uipCmdIndex,CTestManager *pManager=NULL);
    TEST_STATUS    ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect);

    TEST_STATUS OnConnectDown();
private:
    CWavIn      m_wavIn;
    CWavOut     m_wavOut;

    TEST_INFO   m_ti;

};



#endif