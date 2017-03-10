#pragma once
#include "DspArmProtocol.h"
#include "../../src/dspclient/dsp/lock.h"
class CDataManager;
class CDsp;

class CDspTest
{
public:
    CDspTest();
    void RunTest();
    void ExitTest();
    ~CDspTest(void);
private:
    static bool m_fRun;
    CDsp *m_pDsp;
    CDsp *m_pDsp1;
    HANDLE m_hThreadSmv;
    HANDLE m_hThreadMsgTime;
    static DWORD SendSmvThread(LPVOID pArg);
    static DWORD SendSmvThread1(LPVOID pArg);
    static DWORD SendMsgTimeThread(LPVOID pArg);


    static DWORD SendSmvThreadPcap(LPVOID pArg);

    void SendSmv();
    void SetConfig();
    void ResetCache();
    void SendSmvGen();    
    void SmvGenCalc();
    void GetInfo();
    Lock w_lock;
};
