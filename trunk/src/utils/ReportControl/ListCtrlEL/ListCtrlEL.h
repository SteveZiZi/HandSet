#pragma once


//
//=============================================================================
// 包含头文件
//=============================================================================
//
#include "ELDefines.h"

#include "HeaderCtrlEL.h"
#include "ELColumn.h"
#include "ELRecord.h"
#include "ELRecordItem.h"
#include "ELRecordItemText.h"
#include "ELInplaceControls.h"
#include "../Common/CommonEL.h"



//
//-----------------------------------------------------------------------------
// CListCtrlEL
//
//  控件总入口类, 负责整个控件的操作, 显示, 数据维护
//
//  注意: 目前该类是继承CListCtrl, 部分属性还是沿用CListCtrl本身自带的, 其余的是扩展对象中增加的
//  这个可能会导致代码阅读麻烦, 比如col的width, 各种颜色样式, font, 等 是原生的属性
//
//  2013.04.03 -    修改CListCtrlEL与CHeaderCtrlEL为公有继承 CListCtrl..., 但这样有个问题, 
//                  使用者应该只使用CListCtrlEL提供的功能, 而不要轻易使用父类的功能, 不要转换成CListCtrl* 指针使用
//-----------------------------------------------------------------------------
//
class CListCtrlEL : public ELSUPER
{
    DECLARE_DYNAMIC(CListCtrlEL)

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CListCtrlEL ();
    virtual ~CListCtrlEL ();
    virtual void  PreSubclassWindow ();
    BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    LRESULT  SendNotifyMessageToOwner (UINT nMessage, NMHDR* pNMHDR);
    LRESULT  SendMessageToParent (int rowIndex, CELRecordItem* pItem, int colIndex, UINT nMessage);
    void  PostInitTemplateStyle ();

    //
    //=============================================================================
    // header样式
    //=============================================================================
    //
    void  SetHeaderHeight (int Height);
    void  SetHeaderGridlineColor (COLORREF color);
    void  SetHeaderBkColor (COLORREF color);
    void  SetHeaderTextColor (COLORREF color);
    // 注意SetHeaderTextFont, 必须在SetFont后面设置, 否则会被后者覆盖
    void  SetHeaderFont (CFont* pFont, BOOL bRedraw = TRUE);
    CFont*  GetHeaderFont ();


    //
    //=============================================================================
    // 表体样式
    //=============================================================================
    //
    void  SetFreezeSelectionVisible (BOOL bShow);
    void  SetRowSelectionVisible (BOOL bShow);
    void  SetRowHeight (int nHeight);
    void  SetHasGridline (BOOL hasHline, BOOL hasVline);
    void  SetGradientGridline (BOOL hasHline, BOOL hasVline);
    BOOL  SetBkColor (COLORREF cr);
    BOOL  SetTextColor (COLORREF cr);
    void  SetFocusedSubItemHighlightColor (COLORREF cr);
    COLORREF  GetFocusedSubItemHighlightColor ();
    void  SetSelectionRowHighlightColor (COLORREF cr);
    void  SetTextHighlightColor (COLORREF cr);
    void  SetSelectionRowLowlightColor (COLORREF cr);

    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    CFont*  GetFont ();
#if SU_FEI
    void  SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap );
    void  GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap);
#else

	void  SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap ,CBitmap* checkFocusBitmap, CBitmap* uncheckFocusBitmap);
	void  GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap,CBitmap*& checkFocusBitmap, CBitmap*& uncheckFocusBitmap);
#endif

public:
    //
    //=============================================================================
    // 控件控制操作
    //=============================================================================
    //

    CELInplaceEdit*  GetInplaceEdit ();
    CELInplaceList*  GetInplaceList ();
    BOOL  HasFocus();
    BOOL  IsEditMode ();
    void  EditItem (EL_RECORDITEM_ARGS* pItemArgs, BOOL bAutoFocus = TRUE);
    CELRecordItem*  GetCurrentActiveItem ();

    CRect  GetRowItemRect (int rowIndex, int colIndex);
    BOOL  EnsureRowVisibleScrollRow (int rowIndex);
    void  EnsureRowVisibleScrollPage (int rowIndex);
    BOOL  IsRowItemVisible (int rowIndex);
    int  GetTopRowIndex ();
    BOOL  RedrawRowItems (int rowIndexBegin, int rowIndexEnd);
    BOOL  RedrawRowItem (int rowIndex, int nSubItem);

    void  SetFreezeColumnsCount (int count);
    void  SetRowFocusEnable (BOOL bEnable);
    void  SetSubItemFocusEnable (BOOL bEnable);
    void  SetFocusedColIndex (int colIndex);
    int  GetFocusedColIndex ();
    void  SetFocusedSelectionRowIndex (int rowIndex);
    int  GetFocusedRowIndex ();
    int  GetSelectionRowIndex ();
    void  SetAutoRowEditEnable (BOOL bEnable);


    //
    //=============================================================================
    // 导航
    //=============================================================================
    //
    void  KeyBeginEdit ();
    void  KeyMoveDown ();
    void  KeyMoveUp ();
    void  KeyMoveLeft ();
    void  KeyMoveRight ();
    void  ScrollPageDown ();
    void  ScrollPageUp ();
    void  ScrollRowDown ();
    void  ScrollRowUp ();  
    void  AutoSelPageDown ();
    void  AutoSelPageUp ();
    BOOL  IsMoveLeftToPageUp();
    BOOL  IsMoveRightToPageDown();


public:
    //
    //=============================================================================
    // 数据操作
    //=============================================================================
    //
    UINT  MapIDToRowIndex (UINT  id);
    UINT  MapRowIndexToID (UINT  rowIndex);

    int  InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign = EL_CENTER, int colWidth = -1, 
        BOOL isEditable = FALSE, ELAlignment headerAlgin = EL_CENTER, COLORREF colBkColor = EL_COLOR_DEFAULT,
        int marginLeft = EL_ITEMDRAW_MARGIN, int marginRight = EL_ITEMDRAW_MARGIN);
    CELColumn*  GetColumnExAttr (int colIndex);

    int  InsertRecordRow (int rowIndex, CELRecord* newRecord);
    int  InsertRecordRow (int rowIndex, LPCTSTR lpszItem);              // 简单接口
    CELRecord*  GetRecordRow (int rowIndex);
    void  SetRecordRowData (int rowIndex, DWORD_PTR data);              // 简单接口
    DWORD_PTR  GetRecordRowData (int rowIndex);                         // 简单接口
    void  SetRowItemText (int rowIndex, int nSubItem, LPCTSTR lpszText);// 简单接口
    CString  GetRowItemText (int rowIndex, int nSubItem);               // 简单接口
    void  SetRowItemData (int rowIndex, int nSubItem, DWORD_PTR data);  // 简单接口
    DWORD_PTR  GetRowItemData (int rowIndex, int nSubItem);             // 简单接口
    int  GetRecordRowCount ();
    void  DeleteRecordRow (int rowIndex);
    void  DeleteAllRecordRow ();


protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnChar (UINT nChar, UINT nRepCntr, UINT nFlags);
    afx_msg void  OnLButtonDown (UINT nFlags, CPoint point);
    afx_msg void  OnLButtonUp (UINT nFlags, CPoint point);
    afx_msg void  OnLButtonDblClk (UINT nFlags, CPoint ptDblClick);
    afx_msg void  OnMeasureItem (int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void  MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void  OnDestroy ();

    // 虚拟函数, 可以给子类定制外观grid线
    virtual void  FillBkgnd (CDC* pDC);
    virtual void  FillGridline (CDC* pDC);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual BOOL  PreTranslateMessage(MSG* pMsg);


protected:
    //
    //=============================================================================
    // 辅助操作
    //=============================================================================
    //
    void  ReleaseAllExAttr ();
    void  ReleaseAllRecordRowExAttr ();
    void  ReleaseRecordRowExAttr (int rowIndex);

    BOOL  IsValidRowIndex (int rowIndex);
    BOOL  IsValidColIndex (int colIndex);
    BOOL  IsFreezeColIndex (int colIndex);

    void  HideScrollBars ();
    void  SetListCtrlRgn ();

    void  CreatePaintMemBitmap (CDC* dc, int width, int height);

protected:
    //
    //=============================================================================
    // 内部状态属性变量, 样式
    //=============================================================================
    //
    CHeaderCtrlEL           m_Header;
    CRgn                    m_wndRgnNoScroll;                           // 去除滚动条的windows区域, 用于SetWindowRgn去除滚动条, 维护rgn对象的生命周期
    CBitmap                 m_paintMemMap;                              // OnPaint双缓冲时使用的内存位图, 优化不用每次都创建

    int                     m_nRowHeight;                               // 内容行高
    BOOL                    m_hasHGridline;                             // 是否有水平网格线
    BOOL                    m_hasVGridline;                             // 是否有垂直网格线
    BOOL                    m_gradientHGridline;                        // 是否渐变的水平网格线
    BOOL                    m_gradientVGridline;                        // 是否渐变的垂直网格线
    // bkcolor, textbkcolor, textcolor, font,  都使用CListctrl本身提供的属性
    BOOL                    m_bFreezeSelectionVisible;                  // 即使是被冻结的列, 选择高亮仍然显示
    BOOL                    m_bRowSelectionVisible;                     // 选择行的高亮是否可见
    COLORREF                m_focusedSubItemHighlightColor;             // 焦点item的高亮颜色
    COLORREF                m_selectionRowHighlightColor;               // 选择行的高亮颜色
    COLORREF                m_textHighlightColor;                       // 选择行的高亮文本颜色
    COLORREF                m_selectionRowLowlightColor;                // 选择行的低亮颜色, 比如在控件失去焦点, 但有LVS_SHOWSELALWAYS标志时
    CBitmap*                m_checkBitmap;                              // 被勾选的checkbox图标
    CBitmap*                m_uncheckBitmap;                            // 未被勾选的checkbox图标

#if !SU_FEI
	CBitmap*				m_checkFocusBitmap;							// 高亮时选中
	CBitmap*				m_uncheckFocusBitmap;						// 高亮时未被选中
#endif
    //
    //=============================================================================
    // 内部状态属性变量, 焦点与选择
    //=============================================================================
    //
    int                     m_nFreezeColumnsCount;  // Count of none-scrolled columns at the left side.
    BOOL                    m_bRowFocusEnable;  // TRUE if selection enabled. 焦点是否能定位到表格的某行, 如果不能, 则上下按钮只能翻页, 相当于只读表格
    BOOL                    m_bSubItemFocusEnable;  // TRUE if sub-items can receive focus. 是否可以焦点定位到行中的某个格子
    int                     m_focusedColIndex;  // 当前焦点列, 与m_focusedRowIndex配合起来就可以标识一个格子, 其中m_focusedRowIndex 使用clistctrl提供的焦点行属性
    BOOL                    m_bAutoRowEditEnable;  // 是否开启行自动编辑, 当选择焦点在某行, 但列焦点是-1时, 按下OK键可以自动的让该行的第一个可编辑item进入编辑状态

    //
    //=============================================================================
    // 内部状态属性变量: 辅助
    //=============================================================================
    //
    CELInplaceEdit*         m_pInplaceEdit;     // In-place edit pointer
    CELInplaceList*         m_pInplaceList;     // In-place list pointer
    CELRecordItem*          m_pActiveItem;      // Pointer to the currently focused CXTPReportRecordItem.
};


//
//=============================================================================
// 简单的inline函数实现
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 延迟的样式初始化
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::PostInitTemplateStyle ()
{
}
//
//-----------------------------------------------------------------------------
// 设置表头的高度
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderHeight (int Height) //设置表头高度
{
    m_Header.m_hdrHeight = Height;
}
//
//-----------------------------------------------------------------------------
// 设置表头的分割线颜色
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderGridlineColor (COLORREF color)
{
    m_Header.m_hdrGridlineColor = color;
}
//
//-----------------------------------------------------------------------------
// 设置表头的文字颜色
//-----------------------------------------------------------------------------
//
inline void CListCtrlEL::SetHeaderTextColor(COLORREF color) //设置头部字体颜色
{
    m_Header.m_hdrTextColor = color;
}
//
//-----------------------------------------------------------------------------
// 设置表头的背景颜色
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderBkColor (COLORREF color)
{
    m_Header.m_hdrBGColor = color;
}
//
//-----------------------------------------------------------------------------
// 设置表头的字体, font对象需要用户自己去维护, 直到表格销毁
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderFont (CFont* pFont, BOOL bRedraw)
{
    m_Header.SetFont(pFont, bRedraw);
}
inline CFont*  CListCtrlEL::GetHeaderFont ()
{
    return m_Header.GetFont();
}
//
//-----------------------------------------------------------------------------
// 设置是否即使在冻结的列, 行的选择高亮仍然显示
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetFreezeSelectionVisible (BOOL bShow)
{
    m_bFreezeSelectionVisible = bShow;
}
//
//-----------------------------------------------------------------------------
// 设置是否显示表格中的行选中高亮
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetRowSelectionVisible (BOOL bShow)
{
    m_bRowSelectionVisible = bShow;
}
//
//-----------------------------------------------------------------------------
// 设置表体是否有网格线
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHasGridline (BOOL hasHline, BOOL hasVline)
{
    m_hasHGridline = hasHline;
    m_hasVGridline = hasVline;
}
inline void  CListCtrlEL::SetGradientGridline (BOOL hasHline, BOOL hasVline)
{
    m_gradientHGridline = hasHline;
    m_gradientVGridline = hasVline;
}
//
//-----------------------------------------------------------------------------
// 设置表体的整体背景颜色
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::SetBkColor (COLORREF cr)
{
    return ELSUPER::SetBkColor(cr);
}
//
//-----------------------------------------------------------------------------
// 设置表体的文字颜色
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::SetTextColor (COLORREF cr)
{
    return ELSUPER::SetTextColor(cr);
}
inline void  CListCtrlEL::SetFocusedSubItemHighlightColor (COLORREF cr)
{
    m_focusedSubItemHighlightColor = cr;
}
inline COLORREF  CListCtrlEL::GetFocusedSubItemHighlightColor ()
{
    return m_focusedSubItemHighlightColor;
}
inline void  CListCtrlEL::SetSelectionRowHighlightColor (COLORREF cr)
{
    m_selectionRowHighlightColor = cr;
}
inline void  CListCtrlEL::SetTextHighlightColor (COLORREF cr)
{
    m_textHighlightColor = cr;
}
inline void  CListCtrlEL::SetSelectionRowLowlightColor (COLORREF cr)
{
    m_selectionRowLowlightColor = cr;
}
//
//-----------------------------------------------------------------------------
// 设置表体的字体
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetFont (CFont* pFont, BOOL bRedraw)
{
    ELSUPER::SetFont(pFont, bRedraw);
}
inline CFont*  CListCtrlEL::GetFont ()
{
    return ELSUPER::GetFont();
}
//
//-----------------------------------------------------------------------------
// 设置图片
//-----------------------------------------------------------------------------
//

#if SU_FEI
inline void  CListCtrlEL::SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap)
{
	
	m_checkBitmap = checkBitmap;
	m_uncheckBitmap = uncheckBitmap;

}
inline void  CListCtrlEL::GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap)
{
	
	retCheckBitmap = m_checkBitmap;
	retUncheckBitmap = m_uncheckBitmap;
	
}

#else
inline void  CListCtrlEL::SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap, CBitmap* checkfocusBitmap, CBitmap* uncheckfocusBitmap)
{

	m_checkBitmap = checkBitmap;
	m_uncheckBitmap = uncheckBitmap;

	m_checkFocusBitmap = checkfocusBitmap;
	m_uncheckFocusBitmap = uncheckfocusBitmap;

}
inline void  CListCtrlEL::GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap, CBitmap*& retCheckFocusBitmap, CBitmap*& retUncheckFocusBitmap)
{

	retCheckBitmap = m_checkBitmap;
	retUncheckBitmap = m_uncheckBitmap;

	retCheckFocusBitmap = m_checkFocusBitmap;
	retUncheckFocusBitmap = m_uncheckFocusBitmap;

}

#endif

//
//-----------------------------------------------------------------------------
// 获得inplace控件对象, 无论是否在编辑状态
//-----------------------------------------------------------------------------
//
inline CELInplaceEdit*  CListCtrlEL::GetInplaceEdit ()
{
    return m_pInplaceEdit;
}
//
//-----------------------------------------------------------------------------
// 获得inplace控件对象, 无论是否在编辑状态
//-----------------------------------------------------------------------------
//
inline CELInplaceList*  CListCtrlEL::GetInplaceList ()
{
    return m_pInplaceList;
}


//
//-----------------------------------------------------------------------------
// 滚动视图保证指定行被显示
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::EnsureRowVisibleScrollRow (int rowIndex)
{
    return ELSUPER::EnsureVisible(rowIndex, FALSE);
}
//
//-----------------------------------------------------------------------------
// 获得当前视图顶部的行rowindex
//-----------------------------------------------------------------------------
//
inline int  CListCtrlEL::GetTopRowIndex ()
{
    return ELSUPER::GetTopIndex();
}
//
//-----------------------------------------------------------------------------
// 更新重绘视图行或item格子
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::RedrawRowItems (int rowIndexBegin, int rowIndexEnd)
{
    return ELSUPER::RedrawItems(rowIndexBegin, rowIndexEnd);

}
inline BOOL  CListCtrlEL::RedrawRowItem (int rowIndex, int nSubItem)
{
    return ELSUPER::SetItemText(rowIndex, nSubItem, L"");
}

//
//-----------------------------------------------------------------------------
// 判断rowindex, colindex参数有效性
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::IsValidRowIndex (int rowIndex)
{
    if (rowIndex >= 0 && rowIndex < GetRecordRowCount())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
inline BOOL  CListCtrlEL::IsValidColIndex (int colIndex)
{
    if (colIndex >= 0 && colIndex < m_Header.GetItemCount())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
inline BOOL  CListCtrlEL::IsFreezeColIndex (int colIndex)
{
    if (colIndex >= 0 && colIndex < m_nFreezeColumnsCount)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//
//-----------------------------------------------------------------------------
// 获得当前活动的(正在inplace编辑的item), 如果非空, 则代表当前正在编辑状态
//-----------------------------------------------------------------------------
//
inline CELRecordItem*  CListCtrlEL::GetCurrentActiveItem ()
{
    return m_pActiveItem;
}