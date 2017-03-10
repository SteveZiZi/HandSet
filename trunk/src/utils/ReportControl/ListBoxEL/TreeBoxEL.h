#pragma once

#include "../ListCtrlEL/ELDefines.h"
#include "ListBoxEL.h"
#include "HeaderBoxEL.h"


#define LBT_ROOT            NULL                // LBT_ROOT��m_treeRoot�����ͬ, ��Ϊ����ʱ��д
#define LBT_FIRST           NULL                // ��һ��Ԫ��, ��Ϊ����ʱ��д
#define LBT_LAST            ((LBT_HTREEITEM)-1) // ���һ��Ԫ��, ��Ϊ����ʱ��д

struct LBT_TreeSubitem;
struct LBT_TreeItem;
typedef LBT_TreeItem*       LBT_HTREEITEM;      // ��������һ���ڵ����



//
//-----------------------------------------------------------------------------
// CTreeBoxEL
//
//  �Ի��Tree�ؼ�
//-----------------------------------------------------------------------------
//
class CTreeBoxEL: public CListBoxEL
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
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
    // header��ʽ
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
    // ���ݲ���
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
    // �ؼ�����
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
    // ��Ϣ����
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
    // ����
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
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CHeaderBoxEL            m_treeHeader;       // ��ͷ�ָ���
    LBT_HTREEITEM           m_treeRoot;         // ����Ԫ�صĸ��ڵ�, ��Ϊһ������ʾ������ڵ�, ���ڹҽ����е��ӽڵ�

    int                     m_treeIndent;       // ÿ��Ԫ����������(����)
    CBitmap*                m_expandBitmap;     // չ���İ�ťͼƬ ( + )
    CBitmap*                m_collapseBitmap;   // �����İ�ťͼƬ ( - )
    CBitmap*                m_checkBitmap;      // ����ѡ��checkboxͼ��
    CBitmap*                m_uncheckBitmap;    // δ����ѡ��checkboxͼ��

	CBitmap*				m_checkFocusBitmap;	/// ����ѡ��checkbox����ͼ��
	CBitmap*				m_uncheckFocusBitmap; ///< δ����ѡ��checkbox����ͼ��

    BOOL                    m_hasHGridline;                             // �Ƿ���ˮƽ������
    BOOL                    m_hasVGridline;                             // �Ƿ��д�ֱ������
    BOOL                    m_gradientHGridline;                        // �Ƿ񽥱��ˮƽ������
    BOOL                    m_gradientVGridline;                        // �Ƿ񽥱�Ĵ�ֱ������
};

//
//-----------------------------------------------------------------------------
// ������ÿ�еĸ߶�
//-----------------------------------------------------------------------------
//
inline void CTreeBoxEL::SetTreeItemHeight (int height)
{
    SetItemHeight(-1, height);
}
//
//-----------------------------------------------------------------------------
// ���ñ����Ƿ���������
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
// ���û��ÿ�����ڵ����������
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
// �������ϵĸ���ͼ��
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