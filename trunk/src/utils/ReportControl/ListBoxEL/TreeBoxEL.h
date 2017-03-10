#pragma once

#include "../ListCtrlEL/ELDefines.h"
#include "ListBoxEL.h"
#include "HeaderBoxEL.h"


#define LBT_ROOT            NULL                // LBT_ROOT与m_treeRoot意义等同, 作为插入时简写
#define LBT_FIRST           NULL                // 第一个元素, 作为插入时简写
#define LBT_LAST            ((LBT_HTREEITEM)-1) // 最后一个元素, 作为插入时简写

struct LBT_TreeSubitem;
struct LBT_TreeItem;
typedef LBT_TreeItem*       LBT_HTREEITEM;      // 代表树上一个节点对象



//
//-----------------------------------------------------------------------------
// CTreeBoxEL
//
//  自绘的Tree控件
//-----------------------------------------------------------------------------
//
class CTreeBoxEL: public CListBoxEL
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CTreeBoxEL ();
    virtual ~CTreeBoxEL ();
    virtual BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    virtual BOOL  CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    void  SetTreeItemHeight (int height);
    void  SetHasGridline (BOOL hasHline, BOOL hasVline);
    void  SetGradientGridline (BOOL hasHline, BOOL hasVline);

    int  GetTreeIndent ();
    void  SetTreeIndent (int nIndent);

    void  SetTreeExpandImage (CBitmap* expandBitmap, CBitmap* collapseBitmap);
#if SU_FEI
    void  SetTreeCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap);
#else
	void  SetTreeCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap, CBitmap* checkFocusBitmap, CBitmap* uncheckFocusBitmap);
#endif
    //
    //=============================================================================
    // header样式
    //=============================================================================
    //
    void  SetHeaderHeight (int Height);
    void  SetHeaderGridlineColor (COLORREF color);
    void  SetHeaderBkColor (COLORREF color);
    void  SetHeaderTextColor (COLORREF color);
    void  SetHeaderFont (CFont* pFont, BOOL bRedraw = TRUE);
    int  InsertTreeColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign = EL_CENTER, int colWidth = -1, ELAlignment headerAlgin = EL_CENTER,
        int marginLeft = EL_ITEMDRAW_MARGIN, int marginRight = EL_ITEMDRAW_MARGIN);

public:
    //
    //=============================================================================
    // 数据操作
    //=============================================================================
    //
    void  DeleteAllTreeItems ();
    void  DeleteTreeItem (LBT_HTREEITEM hItem);
    LBT_HTREEITEM  InsertTreeItem (LPCTSTR lpszItem, LBT_HTREEITEM hParent = LBT_ROOT, LBT_HTREEITEM hInsertAfter = LBT_LAST);
    void  ExpandTreeItem (LBT_HTREEITEM hItem, BOOL isExpand);
    
    void  TreePageUp();
    void  TreePageDown();

    BOOL  GetTreeCheck (LBT_HTREEITEM hItem);
    void  SetTreeCheck (LBT_HTREEITEM hItem, BOOL fCheck = TRUE);
    void  SetTreeHasCheckbox (LBT_HTREEITEM hItem, BOOL hasCheckbox = TRUE);

    DWORD_PTR  GetTreeItemData (LBT_HTREEITEM hItem);
    void  SetTreeItemData (LBT_HTREEITEM hItem, DWORD_PTR dwData);

    CString  GetTreeItemText (LBT_HTREEITEM hItem, int subItem);
    void  SetTreeItemText (LBT_HTREEITEM hItem, int subItem, LPCTSTR lpszItem);
    void  SetTreeItemColor (LBT_HTREEITEM hItem, int subItem, COLORREF bkColor, COLORREF textColor);


    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    LBT_HTREEITEM  GetTreeRootItem ();
    LBT_HTREEITEM  GetTreeParentItem (LBT_HTREEITEM hItem);

    BOOL  TreeItemHasChildren (LBT_HTREEITEM hItem);
    LBT_HTREEITEM  GetTreeChildItem (LBT_HTREEITEM hItem);
    LBT_HTREEITEM  GetTreeNextSiblingItem (LBT_HTREEITEM hItem);

    LBT_HTREEITEM  GetTreeFirstVisibleItem ();
    LBT_HTREEITEM  GetTreeNextVisibleItem (LBT_HTREEITEM hItem);
    void  SetTreeFirstVisible (LBT_HTREEITEM hItem);

    LBT_HTREEITEM  GetTreeSelectedItem ();
    void  SelectTreeItem (LBT_HTREEITEM hItem);

#if !CHUN_HUA
	void RecordExpandStatus();
	void RestoreExpandStatus();
	void ExpandAllItem(bool isExpand);

#endif

protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void  FillBkgnd (CDC* pDC);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    BOOL  IsTreeItemInList (LBT_HTREEITEM hItem);
    int  CalcTreeItemRangeInList (LBT_HTREEITEM hItem);
    int  InsertTreeItemKidsToList (LBT_HTREEITEM hItem);
    void  RefreshListIndex ();
    void  UpdateTreeBox (LBT_HTREEITEM hItem = NULL);
    void  AutoResizeTreeSubitems (LBT_HTREEITEM hItem);
    LRESULT  SendNotifyMessageToOwner (UINT nMessage, NMHDR* pNMHDR);

    void  DrawSelection (CDC* pDC, const CRect& rcItem, BOOL isHighlight);
    CRect  DrawExpandButton (CDC* pDC, const CRect& rcText, BOOL isExpand);
#if SU_FEI
    CRect  DrawCheckbox (CDC* pDC, const CRect& rcText, BOOL isChecked);
#else
	CRect  DrawCheckbox (CDC* pDC, const CRect& rcText, BOOL isChecked, CBitmap* checkFocusBitmap, CBitmap* uncheckFocusBitmap);
#endif
    void  DrawSubItem (CDC* pDC, const CRect& rcSubitem, LBT_HTREEITEM hItem, int colIndex, BOOL isItemHighlight);
    void  DrawFocus (CDC* pDC, const CRect& rcItem);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CHeaderBoxEL            m_treeHeader;       // 表头分隔栏
    LBT_HTREEITEM           m_treeRoot;         // 所有元素的父节点, 作为一个不显示的虚拟节点, 用于挂接所有的子节点

    int                     m_treeIndent;       // 每层元素缩进距离(像素)
    CBitmap*                m_expandBitmap;     // 展开的按钮图片 ( + )
    CBitmap*                m_collapseBitmap;   // 收缩的按钮图片 ( - )
    CBitmap*                m_checkBitmap;      // 被勾选的checkbox图标
    CBitmap*                m_uncheckBitmap;    // 未被勾选的checkbox图标

	CBitmap*				m_checkFocusBitmap;	/// 被勾选的checkbox高亮图标
	CBitmap*				m_uncheckFocusBitmap; ///< 未被勾选的checkbox高亮图标

    BOOL                    m_hasHGridline;                             // 是否有水平网格线
    BOOL                    m_hasVGridline;                             // 是否有垂直网格线
    BOOL                    m_gradientHGridline;                        // 是否渐变的水平网格线
    BOOL                    m_gradientVGridline;                        // 是否渐变的垂直网格线
};

//
//-----------------------------------------------------------------------------
// 设置树每行的高度
//-----------------------------------------------------------------------------
//
inline void CTreeBoxEL::SetTreeItemHeight (int height)
{
    SetItemHeight(-1, height);
}
//
//-----------------------------------------------------------------------------
// 设置表体是否有网格线
//-----------------------------------------------------------------------------
//
inline void  CTreeBoxEL::SetHasGridline (BOOL hasHline, BOOL hasVline)
{
    m_hasHGridline = hasHline;
    m_hasVGridline = hasVline;
}
inline void  CTreeBoxEL::SetGradientGridline (BOOL hasHline, BOOL hasVline)
{
    m_gradientHGridline = hasHline;
    m_gradientVGridline = hasVline;
}
//
//-----------------------------------------------------------------------------
// 设置获得每层树节点的缩进像素
//-----------------------------------------------------------------------------
//
inline int  CTreeBoxEL::GetTreeIndent ()
{
    return m_treeIndent;
}
inline void  CTreeBoxEL::SetTreeIndent (int nIndent)
{
    m_treeIndent = nIndent;
}
//
//-----------------------------------------------------------------------------
// 设置树上的各种图标
//-----------------------------------------------------------------------------
//
inline void  CTreeBoxEL::SetTreeExpandImage (CBitmap* expandBitmap, CBitmap* collapseBitmap)
{
    m_expandBitmap = expandBitmap;
    m_collapseBitmap = collapseBitmap;
}

#if SU_FEI
inline void  CTreeBoxEL::SetTreeCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap)
{
    m_checkBitmap = checkBitmap;
    m_uncheckBitmap = uncheckBitmap;
}
#else
inline void  CTreeBoxEL::SetTreeCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap, CBitmap* checkFocusBitmap, CBitmap* uncheckFocusBitmap)
{
	m_checkBitmap = checkBitmap;
	m_uncheckBitmap = uncheckBitmap;

	m_checkFocusBitmap = checkFocusBitmap;
	m_uncheckFocusBitmap = uncheckFocusBitmap;
}
#endif