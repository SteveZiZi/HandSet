#pragma once

#define KBPOPUPBAR_CLASSNAME  L"kbpopbar"


//
//-----------------------------------------------------------------------------
// CCharSelPopupBar
//
//  字符选择条, 用户在输入法的时候, 显示候选字符的窗口
//-----------------------------------------------------------------------------
//
class CCharSelPopupBar : public CWnd
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
	CCharSelPopupBar();
	virtual ~CCharSelPopupBar();

    virtual BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    virtual BOOL  CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    void  SetTextColor (COLORREF color);
    void  SetBkColor (COLORREF color);
    void  SetSelStyle (COLORREF selHighlightColor, COLORREF textHighlightColor);

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    void  SetCharsOptional (LPCTSTR charsOptional, int curSel);

protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
	DECLARE_MESSAGE_MAP()
    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);

private:
    void  UpdateTextSize (CFont* font);
    void  UpdateWindowSize ();
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

private:
    CFont*                  m_font;             // 当前使用的字体
    COLORREF                m_textColor;        // 文字颜色
    COLORREF                m_bkColor;          // 背景颜色
    COLORREF                m_selHighlightColor;    // 选择行的高亮颜色
    COLORREF                m_textHighlightColor;   // 选择行的高亮文本颜色
    CSize                   m_charSize;         // 当前字体下的要显示文字的大小

    CString                 m_charsOptional;    // 可选字符
    int                     m_curSel;           // 当前选中的字符
};

//
//=============================================================================
// 设置颜色属性
//=============================================================================
//
inline void  CCharSelPopupBar::SetTextColor (COLORREF color)
{
    m_textColor = color;
}
inline void  CCharSelPopupBar::SetBkColor (COLORREF color)
{
    m_bkColor = color;
}
inline void  CCharSelPopupBar::SetSelStyle (COLORREF selHighlightColor, COLORREF textHighlightColor)
{
    m_selHighlightColor = selHighlightColor;
    m_textHighlightColor = textHighlightColor;
}

