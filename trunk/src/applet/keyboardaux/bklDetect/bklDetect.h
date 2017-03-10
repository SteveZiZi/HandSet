#pragma once

#define BKL_SHARE_EVENT     _T(".bkl.off.evt")

class CBKLMonitor 
{
public:
    CBKLMonitor();
    ~CBKLMonitor();

    BOOL waitBklOff(HANDLE hUsrCancelEvt = NULL, DWORD dwTimeOut = INFINITE);

private:
    void _init(void);
    void _deinit(void);
private:
    HANDLE m_hEvt;
};