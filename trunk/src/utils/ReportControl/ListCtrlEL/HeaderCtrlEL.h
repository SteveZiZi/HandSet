#pragma once

#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// CHeaderCtrlEL
//
//  控件表头控件, 负责表头的显示样式, 以及管理col
//
//  2013.04.03 -    修改CListCtrlEL与CHeaderCtrlEL为公有继承 CListCtrl..., 但这样有个问题, 
//                  使用者应该只使用CListCtrlEL提供的功能, 而不要轻易使用父类的功能.
//-----------------------------------------------------------------------------
//
class CHeaderCtrlEL : public ELHDRSUPER
{
    DECLARE_DYNAMIC(CHeaderCtrlEL)


public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CHeaderCtrlEL ();
    virtual ~CHeaderCtrlEL ();

    int  GetHdrHeight ();
    COLORREF  GetHdrGridlineColor ();


protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    LRESULT  OnLayout ( WPARAM wParam, LPARAM lParam );
    afx_msg void  OnDestroy ();


private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    CELColumn*  GetColumn (int colIndex);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    int                     m_hdrHeight;        // Header的高度.
    COLORREF                m_hdrGridlineColor; // 间隔线的颜色
    COLORREF                m_hdrTextColor;     // header文本颜色
    COLORREF                m_hdrBGColor;       // header背景颜色
    //  font, imagelist 都使用CHeaderCtrl本身提供的属性

    friend class CListCtrlEL;
};


inline int  CHeaderCtrlEL::GetHdrHeight ()
{
    return m_hdrHeight;
}
inline COLORREF  CHeaderCtrlEL::GetHdrGridlineColor ()
{
    return m_hdrGridlineColor;
}