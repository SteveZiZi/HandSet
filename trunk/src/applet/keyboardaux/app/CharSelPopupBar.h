#pragma once

#define KBPOPUPBAR_CLASSNAME  L"kbpopbar"


//
//-----------------------------------------------------------------------------
// CCharSelPopupBar
//
//  �ַ�ѡ����, �û������뷨��ʱ��, ��ʾ��ѡ�ַ��Ĵ���
//-----------------------------------------------------------------------------
//
class CCharSelPopupBar : public CWnd
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
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
    // �ؼ�����
    //=============================================================================
    //
    void  SetCharsOptional (LPCTSTR charsOptional, int curSel);

protected:
    //
    //=============================================================================
    // ��Ϣ����
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
    CFont*                  m_font;             // ��ǰʹ�õ�����
    COLORREF                m_textColor;        // ������ɫ
    COLORREF                m_bkColor;          // ������ɫ
    COLORREF                m_selHighlightColor;    // ѡ���еĸ�����ɫ
    COLORREF                m_textHighlightColor;   // ѡ���еĸ����ı���ɫ
    CSize                   m_charSize;         // ��ǰ�����µ�Ҫ��ʾ���ֵĴ�С

    CString                 m_charsOptional;    // ��ѡ�ַ�
    int                     m_curSel;           // ��ǰѡ�е��ַ�
};

//
//=============================================================================
// ������ɫ����
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

