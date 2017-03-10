/**
 * \file
 *      CountDownWnd.cpp
 *
 * \brief
 *      倒计时信息显示框实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/12/10
 */

// CountDownWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "CountDownWnd.h"


// CCountDownWnd

IMPLEMENT_DYNAMIC(CCountDownWnd, CStatic)

CCountDownWnd::CCountDownWnd()
: m_nLeftSeconds(0)
, m_nTotalSeconds(0)
, m_nTimer(0)
, m_csTips(L"")
, m_crBackground(RGB(255, 0, 0))
{

}

CCountDownWnd::~CCountDownWnd()
{
    m_panel.DeleteObject();
}


BEGIN_MESSAGE_MAP(CCountDownWnd, CStatic)
    ON_WM_TIMER()
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CCountDownWnd 消息处理程序



void CCountDownWnd::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    m_nLeftSeconds--;

    if(m_nLeftSeconds == 0)
    {
        KillTimer(m_nTimer);
        m_nTimer = 0;
    }

    Invalidate(TRUE);

    CStatic::OnTimer(nIDEvent);
}

void CCountDownWnd::_Init()
{
    m_nTimer = SetTimer(1, 1000, NULL);
}

void CCountDownWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CStatic::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    if(!m_panel.GetSafeHandle())
    {
        m_panel.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    }

    if(m_nLeftSeconds > 0)
    {
        int nMin    = m_nLeftSeconds / 60;
        int nSec    = m_nLeftSeconds % 60;

        if(m_nTotalSeconds / 60 > 0)
        {
            m_csTips.Format(m_csTipsFmt, nMin, nSec);
        }
        else
        {
            m_csTips.Format(m_csTipsFmt, nSec);
        }
    }
    else
    {
        m_csTips = L"";
    }

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);
    int nPre = dcMem.SaveDC();
    dcMem.SelectObject(m_panel);
    dcMem.FillSolidRect(rcClient, m_crBackground);
    dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(-13));
    dcMem.SetBkMode(TRANSPARENT);
    dcMem.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
    dcMem.DrawText(m_csTips, rcClient, DT_VCENTER | DT_LEFT | DT_WORD_ELLIPSIS | DT_NOCLIP);
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.RestoreDC(nPre);
}

BOOL CCountDownWnd::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    if(m_nTimer != 0)
    {
        KillTimer(m_nTimer);
        m_nTimer = 0;
    }

    return CStatic::DestroyWindow();
}
