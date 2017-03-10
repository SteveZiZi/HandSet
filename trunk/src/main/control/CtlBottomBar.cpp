/**
 * \file
 *      CtlBottomBar.cpp
 *
 * \brief
 *      下方状态栏控件实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/6
 */

// CtlBottomBar.cpp : 实现文件
//

#include "stdafx.h"
#include "CtlBottomBar.h"

#include "src/main/common.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/resourcebmp.h"
#include "src/utils/utility/dsm_utility.h"

// CCtlBottomBar

#define DSM_CRBOTTOMBAR_FONT            RGB(0xFF, 0xFF, 0xFF)       ///< bottombar字体颜色
#define DSM_CRBOTTOMBAR_BK              RGB(39, 40, 41)             ///< bottombar背景颜色


IMPLEMENT_DYNAMIC(CCtlBottomBar, CStatic)

CCtlBottomBar::CCtlBottomBar()
: m_nTimer(UINT_MAX)
, m_pFont(dsm_fontmgr::GetInstance()->GetFont(DSM_FTSIZE_BOTTOMBAR))
{
	m_bmpIcon.Attach(dsm_bmpmgr::Get(DSM_BMP_WARNNING_ICO));
}

CCtlBottomBar::~CCtlBottomBar()
{
    m_panel.DeleteObject();
}


BEGIN_MESSAGE_MAP(CCtlBottomBar, CStatic)
    ON_WM_TIMER()
    ON_WM_SHOWWINDOW()
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CCtlBottomBar 消息处理程序


void CCtlBottomBar::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    Invalidate(TRUE);

    CStatic::OnTimer(nIDEvent);
}


BOOL CCtlBottomBar::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    if(UINT_MAX != m_nTimer)
    {
        KillTimer(m_nTimer);
    }
	m_bmpIcon.Detach();

    return CStatic::DestroyWindow();
}

void CCtlBottomBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CStatic::OnPaint()

    CRect rcClient, rcTime(DSM_RCBOTTOM_TIME_RC), rcTips(DSM_RCBOTTOM_TIPS_RC);
	CRect rcIcon(DSM_RCBOTTOM_ICO_RC);
    GetClientRect(rcClient);

	m_rcSerifTopA   = rcClient; 
	m_rcSerifTopA.DeflateRect(0, 0, 0, rcClient.Height() - 1);

    if(!m_panel.GetSafeHandle())
    {

        
#if SU_FEI        
		m_rcSerifTopB   = rcClient;
        m_rcSerifBottom = rcClient;

       
        m_rcSerifTopB.DeflateRect(0, 1, 0, rcClient.Height() - 2);
        m_rcSerifBottom.DeflateRect(0, rcClient.Height() - 1, 0, 0);
#endif
        m_panel.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    }

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);

    int nSaveDC = dcMem.SaveDC();
    dcMem.SelectObject(m_panel);
    dcMem.SetBkMode(TRANSPARENT);
    dcMem.FillSolidRect(rcClient, DSM_CRBOTTOMBAR_BK);

    dcMem.FillSolidRect(m_rcSerifTopA, DSM_LINE_COLOR);
 #if SU_FEI   
	dcMem.FillSolidRect(m_rcSerifTopB, DSM_LINE_MARGIN_COLOR);
    dcMem.FillSolidRect(m_rcSerifBottom, DSM_LINE_COLOR);
#endif
    dcMem.SelectObject(m_pFont);
    dcMem.SetTextColor(DSM_CRBOTTOMBAR_FONT);

    // 更新时间

    GetLocalTime(&m_systime);

    m_csTime.Format(L"%04d-%02d-%02d %02d:%02d:%02d", m_systime.wYear, m_systime.wMonth, m_systime.wDay, m_systime.wHour, m_systime.wMinute, m_systime.wSecond);
    dcMem.DrawText(m_csTime, rcTime, DT_RIGHT | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOCLIP);

	DWORD dwStyle = CStatusMgr::ShowTipsStyle();

	if(dwStyle & 0x01)
	{
		if (CStatusMgr::GetTips().Left(1) == "1")
		{
			dcMem.SetTextColor(CStatusMgr::GetTipsColor());
			dcMem.DrawText(CStatusMgr::GetTips().Right(CStatusMgr::GetTips().GetLength() - 1), rcTips, DT_LEFT | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOCLIP);
		}
		else
		{
			rcTips.left -= 30;
			dcMem.SetTextColor(CStatusMgr::GetTipsColor());
			dcMem.DrawText(CStatusMgr::GetTips().Right(CStatusMgr::GetTips().GetLength() - 1), rcTips, DT_LEFT | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOCLIP);
		}

	}
    // 内存DC绘到设备DC上
    dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);;
#if !SU_FEI
	// 显示tips信息
	
    if(dwStyle & 0x01)
    {
		if (!CStatusMgr::GetTips().IsEmpty())
		{
			if (CStatusMgr::GetTips().Left(1) == "1")
			{
				dcMem.SelectObject(m_bmpIcon);
				dc.BitBlt(rcIcon.left, rcIcon.top,rcIcon.Width(), rcIcon.Height(), &dcMem, 0, 0, SRCCOPY);
			}
		   	
		}
	}
#endif
    dcMem.RestoreDC(nSaveDC);
    dcMem.DeleteDC();
}

