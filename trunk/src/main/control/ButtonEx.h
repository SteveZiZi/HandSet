/**
 * \file
 *      ButtonEx.h
 *
 * \brief
 *      自定义按钮类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */

#pragma once


// CButtonEx

#define DSM_BUTTONEX_MODE_COLOR             0x01        ///< 纯色模式
#define DSM_BUTTONEX_MODE_BMP               0x02        ///< 图片模式

class CButtonEx : public CButton
{
	DECLARE_DYNAMIC(CButtonEx)

public:
	CButtonEx(const CString& csCaption = L"buttonex");
	virtual ~CButtonEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    afx_msg UINT OnGetDlgCode();

protected:
    virtual void PreSubclassWindow();

public:
    /**
     *      设置按钮标题
     *
     *  \note
     *      这和GetWindowText不同
     */
    void _SetCaption(const CString& rcsCaption)
    {
        m_csCaption = rcsCaption;
    }

    /**
     *      
     */
    void _SetColorMode(COLORREF bk, COLORREF text, COLORREF focusbk, COLORREF focustext)
    {
        m_nMode             = DSM_BUTTONEX_MODE_COLOR;
        m_crBkColor         = bk;
        m_crTextColor       = text;
        m_crFocusBkColor    = focusbk;
        m_crFocusTextColor  = focustext;
    }

    void _SetBmp(UINT normal, UINT focus, UINT push);

private:
    /**
     *      绘制button
     *
     *  \param[in]
     *      dwStatu         DSM_BUTTONEX_STATU_FOCUS:   焦点状态
     *                      DSM_BUTTONEX_STATU_UNFOCUS: 无焦点状态
     */
    void _drawItem(HDC hdc, DWORD dwStatu);

private:
    CString                 m_csCaption;            ///< 对话框显示标题

    // 纯色模式：   DSM_BUTTONEX_MODE_COLOR   0x01
    // 图片模式：   DSM_BUTTONEX_MODE_BMP     0x02
    int                     m_nMode;                ///< 纯色或图片模式

    // 纯色
    COLORREF                m_crBkColor;            ///< 背景色
    COLORREF                m_crTextColor;          ///< 文字颜色
    COLORREF                m_crFocusBkColor;       ///< 高亮背景色
    COLORREF                m_crFocusTextColor;     ///< 高亮文字颜色

    // 图片
    HBITMAP                 m_bitmapNormal;         ///< 普通模式
    HBITMAP                 m_bitmapFocus;          ///< 焦点模式
    HBITMAP                 m_bitmapPress;          ///< 按下模式

};


