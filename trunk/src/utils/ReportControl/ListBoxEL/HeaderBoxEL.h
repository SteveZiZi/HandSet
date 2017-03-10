#pragma once

#include "../ListCtrlEL/ELDefines.h"
#include "ListBoxEL.h"

#define HEADERBOXEL_CLASSNAME  L"HeaderBoxEL"

//
//-----------------------------------------------------------------------------
// 表头一个列的元素属性
//-----------------------------------------------------------------------------
//
struct HeaderBoxELColumn
{
    ELAlignment         itemAlignment;      // 本列的所有元素的对齐方式
    ELAlignment         hdrAlignment;       // 本列表头文本的对齐方式
    CString             hdrText;            // 本列表头文本
    int                 hdrWidth;           // 本列表头的宽度

    int                 marginLeft;         // 绘制Item的内容时, 左边的留白区域
    int                 marginRight;        // 绘制Item的内容时, 右边的留白区域

public:
    HeaderBoxELColumn ()
    {
        itemAlignment = EL_CENTER;
       // hdrAlignment = EL_CENTER;
		hdrAlignment = EL_LEFT;
        // hdrText = L"";
        hdrWidth = 0;

        marginLeft = 0;
        marginRight = 0;
    }
};


//
//-----------------------------------------------------------------------------
// CHeaderBox
//
//  控件表头控件, 负责表头的显示样式, 以及管理col
//-----------------------------------------------------------------------------
//
class CHeaderBoxEL : public CWnd
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CHeaderBoxEL ();
    virtual ~CHeaderBoxEL ();
    virtual BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    void  SetHeaderGridlineColor (COLORREF color);
    COLORREF  GetHeaderGridlineColor ();
    void  SetHeaderBkColor (COLORREF color);
    void  SetHeaderTextColor (COLORREF color);
    void  SetHeaderFont (CFont* pFont, BOOL bRedraw = TRUE);

public:
    //
    //=============================================================================
    // 数据操作
    //=============================================================================
    //
    int  GetColumnCount ();
    CRect  GetItemRect (int nIndex);
    int  InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign = EL_CENTER, int colWidth = -1, ELAlignment headerAlgin = EL_CENTER,
        int marginLeft=0, int marginRight=0);
    HeaderBoxELColumn*  GetColumn (int colIndex);

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
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    COLORREF                m_hdrGridlineColor; // 间隔线的颜色
    COLORREF                m_hdrTextColor;     // header文本颜色
    COLORREF                m_hdrBkColor;       // header背景颜色
    CFont*                  m_hdrFont;          // 当前使用的字体

    CArray<HeaderBoxELColumn*>  m_hdrColumns;   // 所有列信息
};

//
//-----------------------------------------------------------------------------
// 设置表头的字体颜色等
//-----------------------------------------------------------------------------
//
inline void  CHeaderBoxEL::SetHeaderGridlineColor (COLORREF color)
{
    m_hdrGridlineColor = color;
}
inline COLORREF  CHeaderBoxEL::GetHeaderGridlineColor ()
{
    return m_hdrGridlineColor;
}
inline void  CHeaderBoxEL::SetHeaderBkColor (COLORREF color)
{
    m_hdrBkColor = color;
}
inline void  CHeaderBoxEL::SetHeaderTextColor (COLORREF color)
{
    m_hdrTextColor = color;
}
inline void  CHeaderBoxEL::SetHeaderFont (CFont* pFont, BOOL bRedraw)
{
    m_hdrFont = pFont;

    if (bRedraw)
    {
        Invalidate();
    }
}

//
//-----------------------------------------------------------------------------
// 获得表头有多少列
//-----------------------------------------------------------------------------
//
inline int  CHeaderBoxEL::GetColumnCount ()
{
    return m_hdrColumns.GetSize();
}


