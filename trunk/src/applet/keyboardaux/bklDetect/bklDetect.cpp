#include "stdafx.h"
#include "bklDetect.h"

CBKLMonitor::CBKLMonitor()
: m_hEvt(NULL)
{
    _init();
}

CBKLMonitor::~CBKLMonitor()
{
    _deinit();
}

BOOL CBKLMonitor::waitBklOff(HANDLE hUsrCancelEvt/* = NULL*/, DWORD dwTimeOut/* = INFINITE*/)
{
    ASSERT(m_hEvt);
    HANDLE hWait[2] = {m_hEvt, hUsrCancelEvt};
    DWORD dwNum = (hUsrCancelEvt) ? 2 : 1;

    return (WAIT_OBJECT_0 == WaitForMultipleObjects(dwNum, hWait, FALSE, dwTimeOut));
}

void CBKLMonitor::_deinit()
{
    if (m_hEvt) {
        SetEvent(m_hEvt);
        CloseHandle(m_hEvt);
        m_hEvt = NULL;
    }
}

void CBKLMonitor::_init()
{
    if (m_hEvt == NULL) {
        m_hEvt = CreateEvent(NULL, FALSE, FALSE, BKL_SHARE_EVENT);
    }
}