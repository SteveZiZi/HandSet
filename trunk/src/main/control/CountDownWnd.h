/**
 * \file
 *      CountDownWnd.h
 *
 * \brief
 *      倒计时信息显示框头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/12/10
 */


#pragma once


// CCountDownWnd

class CCountDownWnd : public CStatic
{
	DECLARE_DYNAMIC(CCountDownWnd)

public:
	CCountDownWnd();
	virtual ~CCountDownWnd();

public:
    afx_msg void OnPaint();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      设置倒计时秒数
     */
    inline void _SetSeconds(unsigned int nSeconds)
    {
        m_nLeftSeconds  = nSeconds;
        m_nTotalSeconds = nSeconds;
    }

    /**
     *      设置提示信息格式字符串
     */
    inline void _SetTipsFmt(const CString csTips)
    {
        m_csTipsFmt = csTips;
    }

    /**
     *      设置背景色
     */
    inline void _SetBKColor(COLORREF color)
    {
        m_crBackground = color;
    }

    /**
     *      初始化控件
     */
    void _Init();


protected:
	DECLARE_MESSAGE_MAP()

private:
    COLORREF            m_crBackground;         ///< 背景色
    CBitmap             m_panel;                ///< 画布
    UINT                m_nTimer;               ///< 定时器ID
    unsigned int        m_nLeftSeconds;         ///< 剩余时间秒数
    unsigned int        m_nTotalSeconds;        ///< 总共时长
    CString             m_csTipsFmt;            ///< 提示信息格式
    CString             m_csTips;               ///< 提示信息
};


