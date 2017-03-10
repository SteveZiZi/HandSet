/**
 * \file
 *      CtlTopBar.h
 *
 * \brief
 *      顶部状态栏信息头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/6
 */

#pragma once

#include "src/include/p_filemapping_def.h"

// CCtlTopBar

class CCtlTopBar : public CStatic
{
	DECLARE_DYNAMIC(CCtlTopBar)

public:
	CCtlTopBar();
	virtual ~CCtlTopBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();


public:

    /**
     *      设置标题
     */
    inline void _SetTitle(const CString& csTitle)
    {
        m_csCaption = csTitle;
        InvalidateRect(m_rcCaption, TRUE);
        UpdateWindow();
    }

    /**
     *      设置状态栏ICON区域无效
     */
    inline void _InvalidateICON()
    {
        InvalidateRect(m_rcICONStatusAll, TRUE);
    }

    // topbar状态信息
private:

    CBitmap             m_panel;                    ///< 背景画布

    CString             m_csCaption;                ///< 标题字符串

    CRect               m_rcCaption;                ///< 标题栏区域
    CRect               m_rcICONStatusAll;          ///< icon区域
	bool                m_bSD;                      ///< SD接入状态
#if !SHENG_ZE
	bool                m_bUCard;                   ///< U盘接入状态
#endif
};


