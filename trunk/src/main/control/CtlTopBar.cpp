/**
 * \file
 *      CtlTopBar.cpp
 *
 * \brief
 *      顶部状态栏信息实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/6
 */

// CtlTopBar.cpp : 实现文件
//

#include "stdafx.h"
#include "CtlTopBar.h"

#include "src/main/common.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"


#define DSM_CRTOPBAR_FONT               RGB(0xFF, 0xFF, 0xFF)       ///< topbar字体颜色
#define DSM_CRTOPBAR_BK                 RGB(39, 40, 41)            ///< topbar背景颜色


// CCtlTopBar

IMPLEMENT_DYNAMIC(CCtlTopBar, CStatic)

CCtlTopBar::CCtlTopBar()
: m_rcCaption(DSM_RCTOP_CAPTION)
, m_rcICONStatusAll(DSM_RCTOP_ICONRECT)
, m_bSD(CStatusMgr::GetSD() == DSM_STATU_CONNECT_SD ? true : false)
#if !SHENG_ZE
, m_bUCard(CStatusMgr::GetUSBDisk() == DSM_STATU_CONNECT_UCARD ? true : false)
#endif
{

}

CCtlTopBar::~CCtlTopBar()
{
    m_panel.DeleteObject();
}


BEGIN_MESSAGE_MAP(CCtlTopBar, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CCtlTopBar 消息处理程序


void CCtlTopBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CStatic::OnPaint()

    CRect rcClip;
    dc.GetClipBox(rcClip);

    CRect rcClient, rcIntersect;
    GetClientRect(rcClient);

    CRect rcSerifTopA   = rcClient; 
    rcSerifTopA.DeflateRect(0, rcClient.Height() - 1, 0, 0);
	

    if(!m_panel.GetSafeHandle())
    {
        m_panel.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
    }

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);
    int nPre = dcMem.SaveDC();
	
    dcMem.SelectObject(m_panel);
    dcMem.FillSolidRect(rcClient, DSM_CRTOPBAR_BK);
    dcMem.FillSolidRect(rcSerifTopA, DSM_LINE_COLOR);

    CDC dcMemBmp;
    dcMemBmp.CreateCompatibleDC(&dcMem);


    // 是否绘制标题
    if(rcIntersect.IntersectRect(rcClip, m_rcCaption))
    {
        //dcMem.BitBlt(rcClient.left, rcClient.top, m_rcCaption.Width() + m_rcCaption.left, m_rcCaption.Height(), &dcMemBmp, rcClient.left, rcClient.top, SRCCOPY);

        dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(DSM_FTSIZE_TOPBAR));
        dcMem.SetBkMode(TRANSPARENT);
        dcMem.SetTextColor(DSM_CRTOPBAR_FONT);
        dcMem.DrawText(m_csCaption, m_rcCaption, DT_VCENTER | DT_LEFT | DT_WORD_ELLIPSIS | DT_NOCLIP);
    }

    // 绘制电池状态
    if(rcIntersect.IntersectRect(rcClip, m_rcICONStatusAll))
    {
        CRect rcICON(DSM_RCTOP_BATTERY);

        // 画背景
        dcMem.BitBlt(m_rcICONStatusAll.left, m_rcICONStatusAll.top, m_rcICONStatusAll.Width(), m_rcICONStatusAll.Height(), &dcMemBmp, m_rcICONStatusAll.left, m_rcICONStatusAll.top, SRCCOPY);

        // 电量图标一直绘制
        dcMemBmp.SelectObject(CStatusMgr::GetBatteryBitmap());
        dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);


        // SD插入时才绘制
        if(DSM_STATU_CONNECT_SD == CStatusMgr::GetSD())
        {
            dcMemBmp.SelectObject(CStatusMgr::GetSDBitmap());
            rcICON.OffsetRect(-rcICON.Width(), 0);
            dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);

            if(!m_bSD)
            {
                m_bSD = true;
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_INSERTED));
            }
        }
        else
        {
            if(m_bSD)
            {
                m_bSD = false;
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_EJECTED));
            }
        }
#if !SHENG_ZE
		// U盘插入时才绘制
		if(DSM_STATU_CONNECT_UCARD == CStatusMgr::GetUSBDisk())
		{
			dcMemBmp.SelectObject(CStatusMgr::GetUSBDiskBitmap());
			rcICON.OffsetRect(-rcICON.Width(), 0);
			dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);
			if(!m_bUCard)
			{
				m_bUCard = true;
				CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_UCARD_INSERTED));
			}
		}
		else
		{
			if(m_bUCard)
			{
				m_bUCard = false;
				CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_UCARD_EJECTED));
			}
		}
#endif
        // USB接入时才绘制
        if(DSM_STATU_CONNECT_USB == CStatusMgr::GetUSB())
        {
            dcMemBmp.SelectObject(CStatusMgr::GetUSBBitmap());
            rcICON.OffsetRect(-rcICON.Width(), 0);
            dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);
        }

        // 锁屏时才绘制图标
        if(CStatusMgr::GetScreenLock())
        {
            dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_LOCK));
            rcICON.OffsetRect(-rcICON.Width(), 0);
            dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);
        }

        // 输入模式图标 只有数字与字符模式才绘制
        if(CStatusMgr::GetShowInputMode())
        {
            APP_KBInputMode mode = CStatusMgr::GetDSM800InputMode();

            if(APP_KB_MODE_LET == mode || 
                APP_KB_MODE_NUM == mode)
            {
                switch(mode)
                {
                    // 数字模式
                case APP_KB_MODE_NUM:
                    dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_NUM));
                    break;

                    // 字符模式
                case APP_KB_MODE_LET:
                    dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_LET));
                    break;

                default:
                    break;
                }

                rcICON.OffsetRect(-rcICON.Width(), 0);
                dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);
            }
        }

        bool bSync =  true;

        // 时间同步图标
        static statusmgr::SyncMode s_preSyncMode = statusmgr::Mode_UNSYNC;
        statusmgr::SyncMode curSyncMode = CStatusMgr::GetSyncMode();
        switch(curSyncMode)
        {
        case statusmgr::Mode_1588:
            dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_1588));
            break;

        case statusmgr::Mode_IRIGB:
            dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_IRIGB));
            break;

        case statusmgr::Mode_IRIGBN:
            dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_TOPBAR_IRIGBN));
            break;

        default:
            s_preSyncMode = statusmgr::Mode_UNSYNC;
            bSync = false;
            break;
        }

        if(bSync)
        {
            rcICON.OffsetRect(-rcICON.Width(), 0);
            dcMem.BitBlt(rcICON.left, rcICON.top + 10, rcICON.Width(), rcICON.Height(), &dcMemBmp, 0, 0, SRCCOPY);
            if (s_preSyncMode != curSyncMode) {
                ::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_FPGA_TO_LOCAL, 0);
                s_preSyncMode = curSyncMode;
            }
        }
    }

    dcMemBmp.DeleteDC();

    dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.RestoreDC(nPre);
}


