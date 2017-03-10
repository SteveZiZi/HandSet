#include "stdafx.h"

#include "TreeBoxEL.h"
#include "../Common/CommonEL.h"

//
//=============================================================================
// ���ڵ�ṹ�弰����
//=============================================================================
//

#define LBT_ISROOTITEM(hItem)  (hItem->parent == NULL)

//
//-----------------------------------------------------------------------------
// �������
//-----------------------------------------------------------------------------
//
struct LBT_TreeSubitem
{
    CString                 text;               // �����ı�
    COLORREF                textColor;          // ����������ɫ
    COLORREF                bkColor;            // �������ɫ

public:
    LBT_TreeSubitem ()
    {
        // text = L"";
        textColor = RGB(0, 0, 0);
        bkColor = RGB(255, 255, 255);
    }

    LBT_TreeSubitem (LPCTSTR lpszItem, COLORREF clrText, COLORREF clrBk)
    {
        text = lpszItem;
        textColor = clrText;
        bkColor = clrBk;
    }
};

//
//-----------------------------------------------------------------------------
// ���νڵ��һ����
//-----------------------------------------------------------------------------
//
struct LBT_TreeItem
{
    //
    //=============================================================================
    // ���ṹ
    //=============================================================================
    //
    LBT_HTREEITEM           parent;             // ��item�ڵ�, �����NULL, ������Ѿ�����
    LBT_HTREEITEM           firstChild;         // ��һ������item
    LBT_HTREEITEM           nextSibling;        // ��һ���ֵ�item

    //
    //=============================================================================
    // ���ڵ�����
    //=============================================================================
    //
    CArray<LBT_TreeSubitem> subitems;           // �������� (�б�ͷ�ָ������)
    BOOL                    isChecked;          // �����Ƿ񱻹�ѡ
    BOOL                    hasCheckbox;        // �����Ƿ���checkbox
    DWORD_PTR               itemData;           // �������������
    BOOL                    isExpand;           // �����Ƿ�չ��

    //
    //=============================================================================
    // ��������
    //=============================================================================
    //
    int                     listIndex;          // ����ListBox������, ��Ҫ���ʵ���ʱ�����, -1(LB_ERR)�������ڵ㱻����, ������ʾ����ͼ��

public:
    LBT_TreeItem ()
    {
        parent = NULL;
        firstChild = NULL;
        nextSibling = NULL;
        
        // subitems.RemoveAll();
        isChecked = FALSE;
        hasCheckbox = FALSE;
        itemData = NULL;
        isExpand = TRUE;

        listIndex = LB_ERR;
    }
};

//
//-----------------------------------------------------------------------------
// �������ڵ㺯������
//-----------------------------------------------------------------------------
//
int  LBT_GetItemLevel (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetParentItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetFirstChildItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetLastChildItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetFirstSiblingItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetLastSiblingItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetNextSiblingItem (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_GetPrevSiblingItem (LBT_HTREEITEM hItem);
void  LBT_InsertNode (LBT_HTREEITEM hItem, LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter);
void  LBT_RemoveNode (LBT_HTREEITEM hItem);
LBT_HTREEITEM  LBT_CreateItem (LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter);
void  LBT_DeleteItem (LBT_HTREEITEM hItem);


//
//-----------------------------------------------------------------------------
// ��ýڵ����ڵĲ��
//-----------------------------------------------------------------------------
//
int  LBT_GetItemLevel (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);

    int  level = -1;  // ���ڵ����-1��
    for (LBT_HTREEITEM  parent = LBT_GetParentItem(hItem); parent != NULL; parent = LBT_GetParentItem(parent))
    {
        level++;
    }
    return level;
}
//
//-----------------------------------------------------------------------------
// ���hItem�ĸ���
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetParentItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->parent;
}
//
//-----------------------------------------------------------------------------
// ���hItem�ĵ�һ������
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetFirstChildItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->firstChild;
}
//
//-----------------------------------------------------------------------------
// ���hItem�����һ������
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetLastChildItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    if (hItem->firstChild)
    {
        return LBT_GetLastSiblingItem(hItem->firstChild);
    }
    else
    {
        return NULL;
    }
}
//
//-----------------------------------------------------------------------------
// ���hItem��ͬ�����ĸ��
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetFirstSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    ASSERT_BREAK(hItem->parent);

    return hItem->parent->firstChild;
}
//
//-----------------------------------------------------------------------------
// ���hItem��ͬ����С�ĵܵ�
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetLastSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    for (LBT_HTREEITEM  sibling = hItem; sibling != NULL; sibling = sibling->nextSibling)
    {
        if (sibling->nextSibling == NULL)
        {
            return sibling;
        }
    }

    ASSERT_BREAK(FALSE);
    return NULL;    
}
//
//-----------------------------------------------------------------------------
// ���hItem����һ���ֵܽڵ�(�ܵ�)
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetNextSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->nextSibling;
}
//
//-----------------------------------------------------------------------------
// ���ָ���ڵ����һ���ֵܽڵ�(ͬ���ĸ��),
// ����NULL�����ڵ���Ǹ��׵Ĵ����
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetPrevSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // �Ǹ�Ԫ��

    LBT_HTREEITEM  parent = hItem->parent;
    if (parent->firstChild == hItem)
    {
        // hItem���Ǹ��׵ĵ�һ���ӽڵ�(�����)
        return NULL;
    }
    else
    {
        // hItem�Ǹ��׵�С����, �ҵ�hItem����һ���ֵܽڵ�
        LBT_HTREEITEM  sibling = parent->firstChild;
        for (; sibling != NULL; sibling = sibling->nextSibling)
        {
            if (sibling->nextSibling == hItem)
            {
                return sibling;
            }
        }
    }

    ASSERT_BREAK(FALSE);
    return NULL;
}


//
//-----------------------------------------------------------------------------
// ��һ�����ڵ�������ĳ�����׵�����, ������ĳ���ֵ�
//-----------------------------------------------------------------------------
//
static void  LBT_InsertNode (LBT_HTREEITEM hItem, LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    ASSERT_BREAK(hParent);  // �Ǹ�Ԫ��

    // �븸�ڵ��ֵܽڵ㷢����ϵ
    hItem->parent = hParent;

    if (hInsertAfter)
    {
        // �������м����һ��Ԫ�� (��Ԫ����Ϊ���׵�С����, ���ֵ�Ԫ�ط�����ϵ)
        ASSERT_BREAK(hParent == hInsertAfter->parent);  // �ֵ�ͬ��
        hItem->nextSibling = hInsertAfter->nextSibling;
        hInsertAfter->nextSibling = hItem;
    }
    else
    {
        // ֪ͨ���ڵ���뱾Ԫ��, ����������ͷ���� (��Ԫ����Ϊ���׵Ĵ����)
        hItem->nextSibling = hParent->firstChild;  // ԭ���Ĵ�������ڱ�ΪС��
        hParent->firstChild = hItem;
    }
}

//
//-----------------------------------------------------------------------------
// ��һ�����ڵ�������޳�
//-----------------------------------------------------------------------------
//
static void  LBT_RemoveNode (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // �Ǹ�Ԫ��

    // �Ͽ��ֵܽڵ�͸��ڵ�
    LBT_HTREEITEM  prevSibling = LBT_GetPrevSiblingItem(hItem);
    if (prevSibling == NULL)
    {
        // hItem���Ǹ��׵ĵ�һ���ӽڵ�(�����), �ø���ָ����һ���ֵ�, �Ͽ����ӹ�ϵ
        hItem->parent->firstChild = hItem->nextSibling;
    }
    else if (prevSibling != NULL)
    {
        // hItem�Ǹ��׵�С����, �Ͽ����ֵܵ���ϵ
        prevSibling->nextSibling = hItem->nextSibling;
    }

    hItem->parent = NULL;
    hItem->nextSibling = NULL;
}

//
//-----------------------------------------------------------------------------
// ����һ�����ڵ����
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_CreateItem (LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    // ����һ�������Ľڵ�
    LBT_HTREEITEM  hItem =  new LBT_TreeItem;
    ASSERT_BREAK(hItem);

    if (hParent)
    {
        // �Ǹ�Ԫ��, ���ڵ��������
        LBT_InsertNode(hItem, hParent, hInsertAfter);
    }
    
    return hItem;
}

//
//-----------------------------------------------------------------------------
// ����һ�����ڵ����
//-----------------------------------------------------------------------------
//
static void  LBT_DeleteItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);

    // �ݹ����item�Ķ���Ԫ�ز�ɾ��
    for (LBT_HTREEITEM  child = LBT_GetFirstChildItem(hItem); child != NULL; )
    {
        LBT_HTREEITEM  nextChild = LBT_GetNextSiblingItem(child);

        LBT_DeleteItem(child);
        child = nextChild;
    }

    // ����Ǹ�Ԫ��, ��ѽڵ�����ϰѽڵ��޳�
    if (!LBT_ISROOTITEM(hItem))
    {
        LBT_RemoveNode(hItem);
    }

    // ɾ���Լ�
    delete hItem;
}





//
//=============================================================================
// CTreeBoxEL
//=============================================================================
//

CTreeBoxEL::CTreeBoxEL ()
{
    // m_treeHeader;
    m_treeRoot = LBT_CreateItem(NULL, NULL);    // �������ڵ�

    m_treeIndent = 20;
    m_expandBitmap = NULL;
    m_collapseBitmap = NULL;
    m_checkBitmap = NULL;
    m_uncheckBitmap = NULL;

    m_hasHGridline = FALSE;
    m_hasVGridline = FALSE;
    m_gradientHGridline = FALSE;
    m_gradientVGridline = FALSE;
}

CTreeBoxEL::~CTreeBoxEL ()
{
    // ɾ�����зǸ�Ԫ��
    DeleteAllTreeItems();

    // �ͷŸ��ڵ�
    LBT_DeleteItem(m_treeRoot);
    m_treeRoot = NULL;
}

//
//-----------------------------------------------------------------------------
// �����ؼ�����
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CListBoxEL::Create(dwStyle, rect, pParentWnd, nID);
    
    // ������ͷ
    CRect  headerRect(0, 0, rect.right - rect.left, m_defItemHeight);
    m_treeHeader.Create(WS_CHILD|WS_VISIBLE, headerRect, this, 0);
    SetMargin(0, headerRect.Height(), 0, 0);
    return ret;
}
BOOL  CTreeBoxEL::CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CListBoxEL::CreateEx(dwExStyle, dwStyle, rect, pParentWnd, nID);

    // ������ͷ
    CRect  headerRect(0, 0, rect.right - rect.left, m_defItemHeight);
    m_treeHeader.Create(WS_CHILD|WS_VISIBLE, headerRect, this, 0);
    SetMargin(0, headerRect.Height(), 0, 0);
    return ret;
}

//
//=============================================================================
// header��ʽ
//=============================================================================
//
void  CTreeBoxEL::SetHeaderHeight (int Height)
{
    ASSERT_BREAK(::IsWindow(m_treeHeader.GetSafeHwnd()));

    CRect  headerRect;
    m_treeHeader.GetWindowRect(&headerRect);
    m_treeHeader.SetWindowPos(NULL, 0, 0, headerRect.Width(), Height, 
        SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

    SetMargin(0, Height, 0, 0);
    UpdateListBox();
}
void  CTreeBoxEL::SetHeaderGridlineColor (COLORREF color)
{
    m_treeHeader.SetHeaderGridlineColor(color);
}
void  CTreeBoxEL::SetHeaderBkColor (COLORREF color)
{
    m_treeHeader.SetHeaderBkColor(color);
}
void  CTreeBoxEL::SetHeaderTextColor (COLORREF color)
{
    m_treeHeader.SetHeaderTextColor(color);
}
void  CTreeBoxEL::SetHeaderFont (CFont* pFont, BOOL bRedraw)
{
    m_treeHeader.SetHeaderFont(pFont, bRedraw);
}
int  CTreeBoxEL::InsertTreeColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign, int colWidth, ELAlignment headerAlgin, int marginLeft, int marginRight)
{
    return m_treeHeader.InsertColumn(colIndex, colTitle, colAlign, colWidth, headerAlgin, marginLeft, marginRight);
}



//
//-----------------------------------------------------------------------------
// ɾ�����еķǸ��ڵ�
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DeleteAllTreeItems ()
{
    // ��ͼ����
    ResetContent();

    // �ݹ����root�Ķ���Ԫ�ز�ɾ��
    for (LBT_HTREEITEM  child = LBT_GetFirstChildItem(m_treeRoot); child != NULL; )
    {
        LBT_HTREEITEM  nextChild = LBT_GetNextSiblingItem(child);

        LBT_DeleteItem(child);
        child = nextChild;
    }
}

//
//-----------------------------------------------------------------------------
// ɾ��һ�����ؼ��Ľڵ�
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DeleteTreeItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // �Ǹ�Ԫ��

    // ɾ��List��ͼ�ж�Ӧ��
    if (IsTreeItemInList(hItem))
    {
        // ���hItem��������������ͼ�еķ�Χ
        int  lastIndex = CalcTreeItemRangeInList(hItem);
        for (int i = lastIndex; i >= hItem->listIndex; i--)
        {
            DeleteString(i);
        }

        RefreshListIndex();
    }

    // ɾ��ʵ������
    LBT_DeleteItem(hItem);
}

//
//-----------------------------------------------------------------------------
// ����һ�����ؼ��Ľڵ�
//-----------------------------------------------------------------------------
//
LBT_HTREEITEM  CTreeBoxEL::InsertTreeItem (LPCTSTR lpszItem, LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    // ��������, ����λ��
    if (hParent == LBT_ROOT)
    {
        hParent = m_treeRoot;
    }
    if (hInsertAfter == LBT_FIRST)
    {
        hInsertAfter = LBT_GetFirstChildItem(hParent);
    }
    else if (hInsertAfter == LBT_LAST)
    {
        hInsertAfter = LBT_GetLastChildItem(hParent);
    }
    ASSERT_BREAK(hParent);

    // �����µĽڵ�, ���ҽӵ�����
    LBT_HTREEITEM  hItem = LBT_CreateItem(hParent, hInsertAfter);
    ASSERT_BREAK(hItem);

    // ��ʼ��item����
    AutoResizeTreeSubitems(hItem);
    ASSERT_BREAK(hItem->subitems.GetSize() > 0);
    hItem->subitems.SetAt(0, LBT_TreeSubitem(lpszItem, m_textColor, m_bkColor));
    

    // ���item��list, �����ʵ���λ�ò���һ��list��, ��ˢ�·�������
    if (IsTreeItemInList(hItem))
    {
        int  listIndex = LB_ERR;
        LBT_HTREEITEM  prevSibling = LBT_GetPrevSiblingItem(hItem);
        if (prevSibling)
        {
            listIndex = CalcTreeItemRangeInList(prevSibling) + 1;
        }
        else
        {
            listIndex = hParent->listIndex + 1;
        }
        InsertString(listIndex, L"", (DWORD_PTR)hItem);

        // ˢ�·�������
        RefreshListIndex();
    }

    return hItem;
}

//
//-----------------------------------------------------------------------------
// չ�����������ڵ�
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::ExpandTreeItem (LBT_HTREEITEM hItem, BOOL isExpand)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // �Ǹ�Ԫ��

    // �ж��µ�״̬�Ƿ�����״̬��ͬ, �����ͬ��ֱ���˳�
    if ((hItem->isExpand ^ isExpand) == 0)
    {
        return;
    }

    hItem->isExpand = isExpand;

    // ��List������в���, �����չ��, ���ڶ�Ӧ�ĵط�����List��
    // ���������, ��Ѷ�Ӧ��List��ɾ��
    if (IsTreeItemInList(hItem))
    {
        if (isExpand)
        {
            // ����hItem������, ��չ���������list
            InsertTreeItemKidsToList(hItem);
        }
        else
        {
            // ���hItem��������������ͼ�еķ�Χ, ɾ��list�������Ԫ��
            int  lastIndex = CalcTreeItemRangeInList(hItem);
            for (int i = lastIndex; i > hItem->listIndex; i--)
            {
                // ����listIndex
                LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(i);
                ASSERT_BREAK(hItem);
                hItem->listIndex = LB_ERR;

                // ɾ�������ϵ�Ԫ��
                DeleteString(i);
            }
        }

        RefreshListIndex();
    }
}

void  CTreeBoxEL::TreePageUp()
{
    CRect rcClient(CRect(0,0,0,0));
    GetMarginClientRect(&rcClient);
    
    //
    //�Ӻ���ǰ�ۻ�һҳ�߶�
    //
    int nBackTopIndex = GetTopIndex();
    if (nBackTopIndex > 0)
    {
        //
        //ֻ��ǰ�滹���в�������Ĵ���
        //

        int nBackHeightCount = 0;
        while(nBackTopIndex > 0  && 
            nBackHeightCount + GetItemHeight(nBackTopIndex - 1) <= rcClient.Height() )
        {
            --nBackTopIndex;
            nBackHeightCount += GetItemHeight(nBackTopIndex);
        }

        //
        //�������ñ�ҳ���
        //
        SetTopIndex(nBackTopIndex);
        SetCurSel(nBackTopIndex);
    }

}

void  CTreeBoxEL::TreePageDown()
{

    CRect rcClient(CRect(0,0,0,0));
    GetMarginClientRect(&rcClient);

    //
    //��ǰ�����ۻ�һҳ�߶�
    //
    int nFrontTopIndex = GetTopIndex();
    int nFrontHeightCount = 0;
    while(nFrontTopIndex < GetCount() &&
        nFrontHeightCount + GetItemHeight(nFrontTopIndex) <= rcClient.Height())
    {
        nFrontHeightCount += GetItemHeight(nFrontTopIndex);
        ++nFrontTopIndex;
    }

    //
    //�Ӻ���ǰ�ۻ�һҳ�߶�
    //

    int nBackTopIndex = GetCount();
    int nBackHeightCount = 0;
    while(nBackTopIndex - 1 >= 0  && 
        nBackHeightCount + GetItemHeight(nBackTopIndex - 1) <= rcClient.Height() )
    {
        --nBackTopIndex;
        nBackHeightCount += GetItemHeight(nBackTopIndex);
    }

    //
    //�������ñ�ҳ���
    //

    if (nFrontTopIndex <= nBackTopIndex)
    {
        if (nFrontTopIndex != GetTopIndex())
        {
            SetTopIndex(nFrontTopIndex);
            SetCurSel(nFrontTopIndex);
        }
    }
    else
    {
        if (nBackTopIndex != GetTopIndex())
        {
            SetTopIndex(nBackTopIndex);
            SetCurSel(nBackTopIndex);
        }
    }
}

//
//-----------------------------------------------------------------------------
// ��û��������ڵ��check����
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::GetTreeCheck (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->isChecked;
}
void  CTreeBoxEL::SetTreeCheck (LBT_HTREEITEM hItem, BOOL fCheck)
{
    ASSERT_BREAK(hItem);
    hItem->isChecked = fCheck;

    UpdateTreeBox(hItem);
}
void  CTreeBoxEL::SetTreeHasCheckbox (LBT_HTREEITEM hItem, BOOL hasCheckbox)
{
    ASSERT_BREAK(hItem);
    hItem->hasCheckbox = hasCheckbox;

    UpdateTreeBox(hItem);
}
//
//-----------------------------------------------------------------------------
// ��û��������ڵ�ĸ���Data
//-----------------------------------------------------------------------------
//
DWORD_PTR  CTreeBoxEL::GetTreeItemData (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->itemData;
}
void  CTreeBoxEL::SetTreeItemData (LBT_HTREEITEM hItem, DWORD_PTR dwData)
{
    ASSERT_BREAK(hItem);
    hItem->itemData = dwData;
}
//
//-----------------------------------------------------------------------------
// ��û��������ڵ���ı���ɫ��
//-----------------------------------------------------------------------------
//
CString  CTreeBoxEL::GetTreeItemText (LBT_HTREEITEM hItem, int subItem)
{
    ASSERT_BREAK(hItem && subItem < m_treeHeader.GetColumnCount());
    AutoResizeTreeSubitems(hItem);

    return hItem->subitems.GetAt(subItem).text;    
}
void  CTreeBoxEL::SetTreeItemText (LBT_HTREEITEM hItem, int subItem, LPCTSTR lpszItem)
{
    ASSERT_BREAK(hItem && subItem < m_treeHeader.GetColumnCount());
    AutoResizeTreeSubitems(hItem);
    
    LBT_TreeSubitem  subAttr = hItem->subitems.GetAt(subItem);
    subAttr.text = lpszItem;
    hItem->subitems.SetAt(subItem, subAttr);
}
void  CTreeBoxEL::SetTreeItemColor (LBT_HTREEITEM hItem, int subItem, COLORREF bkColor, COLORREF textColor)
{
    ASSERT_BREAK(hItem && subItem < m_treeHeader.GetColumnCount());
    AutoResizeTreeSubitems(hItem);

    LBT_TreeSubitem  subAttr = hItem->subitems.GetAt(subItem);
    subAttr.bkColor = bkColor;
    subAttr.textColor = textColor;
    hItem->subitems.SetAt(subItem, subAttr);
}


//
//=============================================================================
// �ؼ�����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ���������Ԫ��
//-----------------------------------------------------------------------------
//
LBT_HTREEITEM  CTreeBoxEL::GetTreeRootItem ()
{
    return m_treeRoot;
}
LBT_HTREEITEM  CTreeBoxEL::GetTreeParentItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->parent;
}
//
//-----------------------------------------------------------------------------
// ���ڵ�Ķ����ֵܱ���
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::TreeItemHasChildren (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return (hItem->firstChild != NULL);
}
LBT_HTREEITEM  CTreeBoxEL::GetTreeChildItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->firstChild;
}
LBT_HTREEITEM  CTreeBoxEL::GetTreeNextSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->nextSibling;
}
//
//-----------------------------------------------------------------------------
// ���ڵ��Ƿ�����ͼ�ڵı���
//-----------------------------------------------------------------------------
//
LBT_HTREEITEM  CTreeBoxEL::GetTreeFirstVisibleItem ()
{
    int  topIndex = GetTopIndex();
    if (topIndex != LB_ERR)
    {
        return (LBT_HTREEITEM)GetItemDataPtr(topIndex);
    }
    else
    {
        return NULL;
    }
}
LBT_HTREEITEM  CTreeBoxEL::GetTreeNextVisibleItem (LBT_HTREEITEM hItem)
{
    if (hItem->listIndex != LB_ERR && hItem->listIndex < (GetCount() - 1))
    {
        // ��һ��List��
        int  nextIndex = hItem->listIndex + 1;
        if (IsItemVisible(nextIndex, TRUE))
        {
            return (LBT_HTREEITEM)GetItemDataPtr(hItem->listIndex + 1);
        }
    }
    
    return NULL;
}
void  CTreeBoxEL::SetTreeFirstVisible (LBT_HTREEITEM hItem)
{
    if (hItem->listIndex != LB_ERR)
    {
        SetTopIndex(hItem->listIndex);
    }
}

//
//-----------------------------------------------------------------------------
// ��û����ý����ϱ�ѡ�еĽڵ�
//-----------------------------------------------------------------------------
//
LBT_HTREEITEM  CTreeBoxEL::GetTreeSelectedItem ()
{
    int  sel = GetCurSel();
    if (sel != LB_ERR)
    {
        LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(sel);
        ASSERT_BREAK(hItem);

        return hItem;
    }
    else
    {
        return NULL;
    }
}
void  CTreeBoxEL::SelectTreeItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem && IsTreeItemInList(hItem));

    SetCurSel(hItem->listIndex);
}


//
//=============================================================================
// ��Ϣ����
//=============================================================================
//
BEGIN_MESSAGE_MAP(CTreeBoxEL, CListBoxEL)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void  CTreeBoxEL::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_LEFT || nChar == VK_RIGHT)
    {
#if SU_FEI
		// չ��������ǰѡ�нڵ�
        int  sel = GetCurSel();
        if (sel != LB_ERR)
        {
            LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(sel);
            ASSERT_BREAK(hItem);
            if (hItem->firstChild)
            {
                if (!hItem->isExpand && (nChar == VK_LEFT))
                {
                    //ԭ���������ٰ���������Ϸ�ҳ
                    TreePageUp();
                }
                else if (hItem->isExpand && (nChar == VK_RIGHT))
                {
                    //ԭ��չ�����ٰ��Ҽ������·�ҳ
                    TreePageDown();
                }
                else 
                {
                    //���ӽڵ�
                    ExpandTreeItem(hItem, (nChar == VK_RIGHT));
                }
            }
            else
            {

                if (nChar == VK_LEFT)
                {
                    TreePageUp();
                }
                else
                {
                    TreePageDown();
                }
            }
        }
#else
		if (nChar == VK_LEFT)
		{
			TreePageUp();
		}
		else
		{
			TreePageDown();
		}


#endif
    }
    else
    {
        CListBoxEL::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

BOOL  CTreeBoxEL::PreTranslateMessage (MSG* pMsg)
{
    if (pMsg->hwnd == GetSafeHwnd())
    {
        // �ػ�س�������Ϣ���͸�������, �ø����ڿ��Դ������س�ȷ��, ���߻س�checkbox���¼�
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
        {
#if SU_FEI
			if (SendNotifyMessageToOwner(NM_RETURN, NULL))
            {
                return TRUE;
            }
#else
			int sel = GetCurSel();

			if (sel != LB_ERR)
			{
				LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(sel);
				ASSERT_BREAK(hItem);
				if (hItem->firstChild)
				{
					if (!hItem->isExpand)
					{
						//ԭ������
						ExpandTreeItem(hItem, TRUE);
					}
					else if (hItem->isExpand )
					{
						//ԭ��չ��
						ExpandTreeItem(hItem, FALSE);
					}
					
				}
				else if (SendNotifyMessageToOwner(NM_RETURN, NULL))
				{
					return TRUE;
				}
			}

#endif
        }
    }

    return CListBoxEL::PreTranslateMessage(pMsg);
}


//
//-----------------------------------------------------------------------------
// ����item��ѡ��ı���ɫ��
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawSelection (CDC* pDC, const CRect& rcItem, BOOL isHighlight)
{
    if (isHighlight)
    {
        // item �������
        pDC->FillSolidRect(rcItem, m_selHighlightColor);
    }
    else
    {
        // �ؼ���ʧ����ʱ
        pDC->FillSolidRect(rcItem, m_selLowlightColor);
    }
}
//
//-----------------------------------------------------------------------------
// ���Ƽ򵥵�չ����ťͼ�� + - 
//-----------------------------------------------------------------------------
//
static void  xxxDrawExpandButton (CDC* pDC, const CRect& rcDraw, BOOL isExpand, int iImage, COLORREF color)
{
    switch (iImage)
    {
    case 1:  // ���������η���
        CommonEL::DrawExpandIcon(pDC, rcDraw.CenterPoint(), 5, color, isExpand, CommonEL::EXP_TRIANGLE);
        break;
    default:  // ������ͨ�� + - ��
        CommonEL::DrawExpandIcon(pDC, rcDraw.CenterPoint(), 5, color, isExpand, CommonEL::EXP_ADDSUB);
        break;
    }
}
//
//-----------------------------------------------------------------------------
// ����չ����ť, ���ر����Ƶľ���
//-----------------------------------------------------------------------------
//
CRect  CTreeBoxEL::DrawExpandButton (CDC* pDC, const CRect& rcIcon, BOOL isExpand)
{
    CBitmap*  expandBitmap = (isExpand)? m_expandBitmap: m_collapseBitmap;

    CRect  rcDraw = rcIcon;
    rcDraw.DeflateRect(4, 4);

    if (expandBitmap)
    {
        // ����λͼ
        CDC  dcMemBmp;
        dcMemBmp.CreateCompatibleDC(pDC);
        CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(expandBitmap);
        {
            pDC->BitBlt(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
        }
        dcMemBmp.SelectObject(oldbmp);
    }
    else
    {
        xxxDrawExpandButton(pDC, rcDraw, isExpand, 1, m_textColor);
    }

    return rcIcon;
}
//
//-----------------------------------------------------------------------------
// ����CheckBox, ���ر����Ƶľ���
//-----------------------------------------------------------------------------
//
#if SU_FEI
CRect  CTreeBoxEL::DrawCheckbox(CDC* pDC, const CRect& rcIcon, BOOL isChecked)
{
	if ()
	{
	}

    CBitmap*  checkboxBitmap = (isChecked)? m_checkBitmap: m_uncheckBitmap;

    CRect  rcDraw = rcIcon;
    rcDraw.DeflateRect(4, 4);

    if (checkboxBitmap)
    {
        // ����λͼ
        CDC  dcMemBmp;
        dcMemBmp.CreateCompatibleDC(pDC);
        CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(checkboxBitmap);
        {
            pDC->BitBlt(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
        }
        dcMemBmp.SelectObject(oldbmp);
    }
    else
    {
        rcDraw.right = rcDraw.left + rcDraw.Height();

        UINT  nState = DFCS_BUTTONCHECK;
        if (isChecked)
        {
            nState |= DFCS_CHECKED;
        }
        pDC->DrawFrameControl(rcDraw, DFC_BUTTON, nState);
    }

    return rcIcon;
}
#else
CRect  CTreeBoxEL::DrawCheckbox(CDC* pDC, const CRect& rcIcon, BOOL isChecked, CBitmap* checkBitmap, CBitmap* uncheckFBitmap)
{
	
	CBitmap*  checkboxBitmap = (isChecked)? checkBitmap: uncheckFBitmap;


	CRect  rcDraw = rcIcon;
	rcDraw.DeflateRect(4, 8);

	if (checkboxBitmap)
	{
		// ����λͼ
		CDC  dcMemBmp;
		dcMemBmp.CreateCompatibleDC(pDC);
		CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(checkboxBitmap);
		{
			pDC->BitBlt(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), 
				&dcMemBmp, 0, 0, SRCCOPY);
		}
		dcMemBmp.SelectObject(oldbmp);
	}
	else
	{
		rcDraw.right = rcDraw.left + rcDraw.Height();

		UINT  nState = DFCS_BUTTONCHECK;
		if (isChecked)
		{
			nState |= DFCS_CHECKED;
		}
		pDC->DrawFrameControl(rcDraw, DFC_BUTTON, nState);
	}

	return rcIcon;
}


#endif
//
//-----------------------------------------------------------------------------
// ������item���ı�������Ԫ��, ��checkbox, expandͼ��
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawSubItem (CDC* pDC, const CRect& rcSubitem, LBT_HTREEITEM hItem, int colIndex, BOOL isItemHighlight)
{
    HeaderBoxELColumn*  colAttr = m_treeHeader.GetColumn(colIndex);
    LBT_TreeSubitem  subAttr = hItem->subitems.GetAt(colIndex);
    ASSERT_BREAK(colAttr);

    CRect  rcText = rcSubitem;
    rcText.left += colAttr->marginLeft;
    rcText.right -= colAttr->marginRight;


    // ��䱳��ɫ, ������������ɫ
    if (isItemHighlight)
    {
        pDC->SetTextColor(m_textHighlightColor);
    }
    else
    {
        pDC->FillSolidRect(rcSubitem, subAttr.bkColor);
        pDC->SetTextColor(subAttr.textColor);
    }

    // ���Ƶ�0�е�Ԫ��: չ����ť, checkbox, ��������
    if (colIndex == 0)
    {
        // ��������, ���ݲ�ε���������ʾλ��
        int  treelevel = LBT_GetItemLevel(hItem);
        rcText.left +=  treelevel * m_treeIndent;

        // ����չ��������ť
        if (TreeItemHasChildren(hItem))
        {
            CRect  rcIcon(rcText.left, rcText.top, rcText.left+rcText.Height(), rcText.bottom);
            DrawExpandButton(pDC, rcIcon, !hItem->isExpand);
        }
        rcText.left += rcText.Height();

        // ����չ����ť
        if (hItem->hasCheckbox)
        {
            CRect  rcIcon(rcText.left, rcText.top, rcText.left+rcText.Height(), rcText.bottom);
			
			if (isItemHighlight)
			{
				DrawCheckbox(pDC, rcIcon, hItem->isChecked, m_checkFocusBitmap, m_uncheckFocusBitmap);
			}
			else
			{
				DrawCheckbox(pDC, rcIcon, hItem->isChecked, m_checkBitmap, m_uncheckBitmap);
			}

            //DrawCheckbox(pDC, rcIcon, hItem->isChecked);
            rcText.left += rcText.Height();
        }

        // ����rcText
        if (rcText.left > rcText.right)
        {
            rcText.left = rcText.right;
        }
    }

    // ��������
    pDC->DrawText(subAttr.text, rcText, colAttr->itemAlignment|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
}
//
//-----------------------------------------------------------------------------
// ���ƽ������
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawFocus (CDC* pDC, const CRect& rcItem)
{
    pDC->DrawFocusRect(rcItem);
}
//
//-----------------------------------------------------------------------------
// ��������item������������
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT_BREAK(lpDIS->CtlType == ODT_LISTBOX);

    CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
    HWND  curFocusedWnd = ::GetFocus();
    BOOL  isWndFocus = (curFocusedWnd == m_hWnd);
    CRect  rcItem = lpDIS->rcItem;
    BOOL  isItemCurSel = (lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED);
    BOOL  isItemHighlight = FALSE;
    

    if (lpDIS->itemID != LB_ERR)
    {
        // ��õ�ǰItem�Ƿ���Ҫ����ѡ��ɫ��
        if (isItemCurSel && (isWndFocus || m_bShowSelAlways))
        {
            isItemHighlight = TRUE;
            DrawSelection(pDC, rcItem, isWndFocus);
        }


        LBT_HTREEITEM  hItem = (LBT_HTREEITEM)lpDIS->itemData;
        ASSERT_BREAK(hItem);

        // �����������е�subitem
        for (int colIndex = 0; 
            colIndex < m_treeHeader.GetColumnCount() && colIndex < hItem->subitems.GetSize(); colIndex++)
        {
            CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);

            // ���һ����item�Ļ�������, +1 -1 ��Ϊ�˱ܿ�����Grid��
            CRect  rcSubitem(rcColumn.left+1, rcItem.top, rcColumn.right, rcItem.bottom-1);
            DrawSubItem(pDC, rcSubitem, hItem, colIndex, isItemHighlight);
        }

        // ���ƽ����
        if (isItemCurSel && isWndFocus)  
        {
            // �����ƽ������߿�
            //DrawFocus(pDC, rcItem);
        }

    }

}
//
//-----------------------------------------------------------------------------
// ��������������������
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::FillBkgnd (CDC* pDC)
{
    // �����������ı���
    CRect  clientRect;
    GetMarginClientRect(&clientRect);
    pDC->FillSolidRect(clientRect, m_bkColor);


    COLORREF  gridColor = m_treeHeader.GetHeaderGridlineColor();
    int  centerX = (clientRect.left + clientRect.right) / 2;  // �����е�

    // ���ƴ�ֱ������
    if (m_hasVGridline)
    {
        // �Ƿ���ƽ����������
        if (m_gradientVGridline)
        {
            // ��������������
            for (int colIndex = 1; colIndex < m_treeHeader.GetColumnCount(); colIndex++)
            {
                CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);
                CRect  gridRect(rcColumn.left, clientRect.top, rcColumn.left+1, clientRect.bottom);
                CommonEL::DrawGradient(pDC, gridRect, gridColor, GetBkColor(), GRADIENT_FILL_RECT_V);
            }

            // ����ˮƽ�����, �����λ���
            CRect  gridRectLeft(clientRect.left, clientRect.bottom-1, centerX, clientRect.bottom);
            CRect  gridRectRight(centerX, clientRect.bottom-1, clientRect.right, clientRect.bottom);
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
        }
        else
        {
            // ����ʵ��
            for (int colIndex = 1; colIndex < m_treeHeader.GetColumnCount(); colIndex++)
            {
                CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);
                pDC->FillSolidRect(rcColumn.left, clientRect.top, 1, clientRect.Height(), gridColor);
            }

            // ����ˮƽ�����
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);
        }
    }
}

//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// �ж����ڵ��Ƿ���ʾ��List����, Ҳ����˵�ڵ�ĸ����Ƿ񶼱�չ��, ����ڵ������ص�
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::IsTreeItemInList (LBT_HTREEITEM hItem)
{
    LBT_HTREEITEM  hParent = LBT_GetParentItem(hItem);
    ASSERT_BREAK(hParent);

    // �жϽڵ��Ƿ�ɼ�(��0��ڵ����ǿɼ�)
    if (LBT_GetItemLevel(hItem) == 0 || (hParent->isExpand && hParent->listIndex != LB_ERR))
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
// ����item������������ڵ���ʾ���б��е�Index��Χ, �������һ��Ԫ�ص�Index
//
//          NODE            LISTINDEX
//  - ROOT
//      |- P1                   0
//          |-C1                1        
//          |-C2                2
//          |   |-CC1           3
//          |   |   |-CCC1      4
//          |   |-CC2           5
//          |   |-CC3           6
//          |       |-CCC2      7
//          |-C3                8
//
// ���CalcTreeItemRangeInList (C2), ��Ӧ�÷��� 7 (CCC2), CCC2��C2�����һ������
// ��������������ɾ��TreeNode��Ӧ��List�еĶ�ӦԪ��, ����ɾ��C2 ��Ӧ�ð������������ﶼɾ��
//
//-----------------------------------------------------------------------------
//
int  CTreeBoxEL::CalcTreeItemRangeInList (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(IsTreeItemInList(hItem));

    int  lastIndex = hItem->listIndex;
    for (LBT_HTREEITEM  child = LBT_GetLastChildItem(hItem); 
        child != NULL && child->listIndex != LB_ERR; child = LBT_GetLastChildItem(child))
    {
        lastIndex = child->listIndex;
    }

    return lastIndex;
}
//
//-----------------------------------------------------------------------------
// ��List����hItem����, ֻҪ��չ����, �������һ�β����Index
//  ͨ���ú�������չ���ڵ�, ����չ��ĳ���ڵ�, ����Ҫ��������ýڵ����������(��չ����)
//
//          ԭ״̬          LISTINDEX
//  - ROOT
//      |- P1                   0
//          |-C1                1        
//          |+C2                2
//          |-C3                3
//
//
// InsertTreeItemKidsToList (C2)
//
//          չ��C2          LISTINDEX
//  - ROOT
//      |- P1                   0
//          |-C1                1        
//          |-C2                2
//          |   |-CC1           3
//          |   |   |-CCC1      4
//          |   |-CC2           5
//          |   |-CC3           6
//          |       |-CCC2      7
//          |-C3                8
//
//
//-----------------------------------------------------------------------------
//
int  CTreeBoxEL::InsertTreeItemKidsToList (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(IsTreeItemInList(hItem) && hItem->isExpand);
    
    int  lastIndex = hItem->listIndex;
    for (LBT_HTREEITEM  child = LBT_GetFirstChildItem(hItem); child != NULL; child = LBT_GetNextSiblingItem(child))
    {
        lastIndex++;
        int  newIndex = InsertString(lastIndex, L"", (DWORD_PTR)child);
        child->listIndex = newIndex;
        
        // �ݹ�����ӽڵ�
        if (child->isExpand)
        {
            lastIndex = InsertTreeItemKidsToList(child);
        }
    }

    return lastIndex;
}
//
//-----------------------------------------------------------------------------
// ˢ�·�������, ʹ�����е����ڵ�ָ����ȷ������List��λ��
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::RefreshListIndex ()
{
    int  listCount = GetCount();
    for (int i = 0; i < listCount; i++)
    {
        LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(i);
        ASSERT_BREAK(hItem);

        hItem->listIndex = i;
    }
}
//
//-----------------------------------------------------------------------------
// ˢ����ʾ���ڵ�, ���hItem=NULL��ˢ�������ؼ�
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::UpdateTreeBox (LBT_HTREEITEM hItem)
{
    if (hItem)
    {
        UpdateListBox(hItem->listIndex);
    }
    else
    {
        UpdateListBox();
    }
}
//
//-----------------------------------------------------------------------------
// ���ݵ�ǰ�ı�ͷ����, �Զ�����hItem����item�����С, ������С��1��Ԫ��
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::AutoResizeTreeSubitems (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);

    int  colCount = max(m_treeHeader.GetColumnCount(), 1);
    int  oldSize = hItem->subitems.GetSize();
    if (oldSize != colCount)
    {
        hItem->subitems.SetSize(colCount);

        // ���ݵ�ǰ��ǰ������ɫ, �����¼����subitem����ɫ
        for (int sub = oldSize; sub < colCount; sub++)
        {
            SetTreeItemColor(hItem, sub, m_bkColor, m_textColor);
        }
    }
}

//
//-----------------------------------------------------------------------------
// �������ڷ���notify��Ϣ
//-----------------------------------------------------------------------------
//
LRESULT  CTreeBoxEL::SendNotifyMessageToOwner (UINT nMessage, NMHDR* pNMHDR)
{
    if (!IsWindow(m_hWnd))
        return 0;

    NMHDR nmhdr;
    if (pNMHDR == NULL)
        pNMHDR = &nmhdr;

    pNMHDR->hwndFrom = GetSafeHwnd();
    pNMHDR->idFrom = GetDlgCtrlID();
    pNMHDR->code = nMessage;

    CWnd *pOwner = GetOwner();
    if (pOwner && IsWindow(pOwner->m_hWnd))
        return pOwner->SendMessage(WM_NOTIFY, pNMHDR->idFrom, (LPARAM)pNMHDR);
    else
        return 0;
}


#if !CHUN_HUA
bool IsExpand[100]={TRUE,};
int nSelectIndex = 0;

//��¼չ��/������
void CTreeBoxEL::RecordExpandStatus()
{

	for (int i =0; i<GetCount();i++)
	{
		LBT_HTREEITEM hItem = (LBT_HTREEITEM) GetItemDataPtr(i);
		IsExpand[i] = hItem->isExpand;
	}
	nSelectIndex = GetTreeSelectedItem()->listIndex;

}
//�ָ�չ��/����
void CTreeBoxEL::RestoreExpandStatus()
{
	for (int i =0; i<GetCount();i++)
	{
		LBT_HTREEITEM hItem = (LBT_HTREEITEM) GetItemDataPtr(i);
		
		ExpandTreeItem(hItem,IsExpand[i]);			
	}
	LBT_HTREEITEM selectItem = (LBT_HTREEITEM) GetItemDataPtr(nSelectIndex);
	SelectTreeItem(selectItem);
}

void CTreeBoxEL::ExpandAllItem( bool isExpand )
{
	for (int i =0; i<GetCount();i++)
	{
		LBT_HTREEITEM hItem = (LBT_HTREEITEM) GetItemDataPtr(i);

		ExpandTreeItem(hItem,isExpand);			
	}
}



#endif
