/**
 * \file
 *      CtlBottomBar.h
 *
 * \brief
 *      下方状态栏控件头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/6
 */
#pragma once


// CCtlBottomBar

class CCtlBottomBar : public CStatic
{
	DECLARE_DYNAMIC(CCtlBottomBar)

public:
	CCtlBottomBar();
	virtual ~CCtlBottomBar();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL DestroyWindow();
    afx_msg void OnPaint();

public:

    /**
     *      开启定时器
     */
    void _StartTimer()
    {
        if(UINT_MAX == m_nTimer)
        {
            m_nTimer = SetTimer(1, 1000, NULL);
        }
    }

private:

    UINT                m_nTimer;           ///< 刷新时间的定时器ID
    CBitmap             m_panel;            ///< 画布
    SYSTEMTIME          m_systime;          ///< 时间信息结构
    CString             m_csTime;           ///< 时间显示

	CBitmap				m_bmpIcon;			///< 提示图标

    CFont*              m_pFont;            ///< 显示信息的字体对象

    CRect               m_rcSerifTopA;      ///< 顶部线条区域
    CRect               m_rcSerifTopB;      ///< 顶部线条下方线条区域
    CRect               m_rcSerifBottom;    ///< 底部线条区域
};


