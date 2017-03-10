#include "stdafx.h"

#include "TreeBoxEL.h"
#include "../Common/CommonEL.h"

//
//=============================================================================
// 树节点结构体及操作
//=============================================================================
//

#define LBT_ISROOTITEM(hItem)  (hItem->parent == NULL)

//
//-----------------------------------------------------------------------------
// 子项对象
//-----------------------------------------------------------------------------
//
struct LBT_TreeSubitem
{
    CString                 text;               // 子项文本
    COLORREF                textColor;          // 子项文字颜色
    COLORREF                bkColor;            // 子项背景颜色

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
// 树形节点的一个项
//-----------------------------------------------------------------------------
//
struct LBT_TreeItem
{
    //
    //=============================================================================
    // 树结构
    //=============================================================================
    //
    LBT_HTREEITEM           parent;             // 父item节点, 如果是NULL, 则代表已经到顶
    LBT_HTREEITEM           firstChild;         // 第一个儿子item
    LBT_HTREEITEM           nextSibling;        // 下一个兄弟item

    //
    //=============================================================================
    // 树节点内容
    //=============================================================================
    //
    CArray<LBT_TreeSubitem> subitems;           // 子项内容 (有表头分割的子项)
    BOOL                    isChecked;          // 该项是否被勾选
    BOOL                    hasCheckbox;        // 该项是否有checkbox
    DWORD_PTR               itemData;           // 该项关联的数据
    BOOL                    isExpand;           // 该项是否被展开

    //
    //=============================================================================
    // 反向索引
    //=============================================================================
    //
    int                     listIndex;          // 反向到ListBox的索引, 需要在适当的时候更新, -1(LB_ERR)代表树节点被收缩, 不会显示在视图上

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
// 操作树节点函数声明
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
// 获得节点所在的层次
//-----------------------------------------------------------------------------
//
int  LBT_GetItemLevel (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);

    int  level = -1;  // 根节点代表-1层
    for (LBT_HTREEITEM  parent = LBT_GetParentItem(hItem); parent != NULL; parent = LBT_GetParentItem(parent))
    {
        level++;
    }
    return level;
}
//
//-----------------------------------------------------------------------------
// 获得hItem的父亲
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetParentItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->parent;
}
//
//-----------------------------------------------------------------------------
// 获得hItem的第一个儿子
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetFirstChildItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->firstChild;
}
//
//-----------------------------------------------------------------------------
// 获得hItem的最后一个儿子
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
// 获得hItem的同父最大的哥哥
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
// 获得hItem的同父最小的弟弟
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
// 获得hItem的下一个兄弟节点(弟弟)
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetNextSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    return hItem->nextSibling;
}
//
//-----------------------------------------------------------------------------
// 获得指定节点的上一个兄弟节点(同父的哥哥),
// 返回NULL代表本节点就是父亲的大儿子
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_GetPrevSiblingItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // 非根元素

    LBT_HTREEITEM  parent = hItem->parent;
    if (parent->firstChild == hItem)
    {
        // hItem就是父亲的第一个子节点(大儿子)
        return NULL;
    }
    else
    {
        // hItem是父亲的小儿子, 找到hItem的上一个兄弟节点
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
// 将一个树节点对象插入某个父亲的名下, 并跟随某个兄弟
//-----------------------------------------------------------------------------
//
static void  LBT_InsertNode (LBT_HTREEITEM hItem, LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    ASSERT_BREAK(hParent);  // 非根元素

    // 与父节点兄弟节点发生联系
    hItem->parent = hParent;

    if (hInsertAfter)
    {
        // 在链表中间插入一个元素 (本元素作为父亲的小儿子, 与兄弟元素发生联系)
        ASSERT_BREAK(hParent == hInsertAfter->parent);  // 兄弟同根
        hItem->nextSibling = hInsertAfter->nextSibling;
        hInsertAfter->nextSibling = hItem;
    }
    else
    {
        // 通知父节点插入本元素, 并且在链表头插入 (本元素作为父亲的大儿子)
        hItem->nextSibling = hParent->firstChild;  // 原来的大儿子现在变为小弟
        hParent->firstChild = hItem;
    }
}

//
//-----------------------------------------------------------------------------
// 将一个树节点从树上剔除
//-----------------------------------------------------------------------------
//
static void  LBT_RemoveNode (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // 非根元素

    // 断开兄弟节点和父节点
    LBT_HTREEITEM  prevSibling = LBT_GetPrevSiblingItem(hItem);
    if (prevSibling == NULL)
    {
        // hItem就是父亲的第一个子节点(大儿子), 让父亲指向下一个兄弟, 断开父子关系
        hItem->parent->firstChild = hItem->nextSibling;
    }
    else if (prevSibling != NULL)
    {
        // hItem是父亲的小儿子, 断开与兄弟的联系
        prevSibling->nextSibling = hItem->nextSibling;
    }

    hItem->parent = NULL;
    hItem->nextSibling = NULL;
}

//
//-----------------------------------------------------------------------------
// 创建一个树节点对象
//-----------------------------------------------------------------------------
//
static LBT_HTREEITEM  LBT_CreateItem (LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    // 创建一个独立的节点
    LBT_HTREEITEM  hItem =  new LBT_TreeItem;
    ASSERT_BREAK(hItem);

    if (hParent)
    {
        // 非根元素, 将节点插入树中
        LBT_InsertNode(hItem, hParent, hInsertAfter);
    }
    
    return hItem;
}

//
//-----------------------------------------------------------------------------
// 销毁一个树节点对象
//-----------------------------------------------------------------------------
//
static void  LBT_DeleteItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(hItem);

    // 递归遍历item的儿子元素并删除
    for (LBT_HTREEITEM  child = LBT_GetFirstChildItem(hItem); child != NULL; )
    {
        LBT_HTREEITEM  nextChild = LBT_GetNextSiblingItem(child);

        LBT_DeleteItem(child);
        child = nextChild;
    }

    // 如果非根元素, 则把节点从树上把节点剔除
    if (!LBT_ISROOTITEM(hItem))
    {
        LBT_RemoveNode(hItem);
    }

    // 删除自己
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
    m_treeRoot = LBT_CreateItem(NULL, NULL);    // 创建根节点

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
    // 删除所有非根元素
    DeleteAllTreeItems();

    // 释放根节点
    LBT_DeleteItem(m_treeRoot);
    m_treeRoot = NULL;
}

//
//-----------------------------------------------------------------------------
// 创建控件窗口
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CListBoxEL::Create(dwStyle, rect, pParentWnd, nID);
    
    // 创建表头
    CRect  headerRect(0, 0, rect.right - rect.left, m_defItemHeight);
    m_treeHeader.Create(WS_CHILD|WS_VISIBLE, headerRect, this, 0);
    SetMargin(0, headerRect.Height(), 0, 0);
    return ret;
}
BOOL  CTreeBoxEL::CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CListBoxEL::CreateEx(dwExStyle, dwStyle, rect, pParentWnd, nID);

    // 创建表头
    CRect  headerRect(0, 0, rect.right - rect.left, m_defItemHeight);
    m_treeHeader.Create(WS_CHILD|WS_VISIBLE, headerRect, this, 0);
    SetMargin(0, headerRect.Height(), 0, 0);
    return ret;
}

//
//=============================================================================
// header样式
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
// 删除所有的非根节点
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DeleteAllTreeItems ()
{
    // 视图操作
    ResetContent();

    // 递归遍历root的儿子元素并删除
    for (LBT_HTREEITEM  child = LBT_GetFirstChildItem(m_treeRoot); child != NULL; )
    {
        LBT_HTREEITEM  nextChild = LBT_GetNextSiblingItem(child);

        LBT_DeleteItem(child);
        child = nextChild;
    }
}

//
//-----------------------------------------------------------------------------
// 删除一个树控件的节点
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DeleteTreeItem (LBT_HTREEITEM hItem)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // 非根元素

    // 删除List视图中对应项
    if (IsTreeItemInList(hItem))
    {
        // 获得hItem包括其子孙在视图中的范围
        int  lastIndex = CalcTreeItemRangeInList(hItem);
        for (int i = lastIndex; i >= hItem->listIndex; i--)
        {
            DeleteString(i);
        }

        RefreshListIndex();
    }

    // 删除实际数据
    LBT_DeleteItem(hItem);
}

//
//-----------------------------------------------------------------------------
// 插入一个树控件的节点
//-----------------------------------------------------------------------------
//
LBT_HTREEITEM  CTreeBoxEL::InsertTreeItem (LPCTSTR lpszItem, LBT_HTREEITEM hParent, LBT_HTREEITEM hInsertAfter)
{
    // 调整参数, 修正位置
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

    // 创建新的节点, 并挂接到树上
    LBT_HTREEITEM  hItem = LBT_CreateItem(hParent, hInsertAfter);
    ASSERT_BREAK(hItem);

    // 初始化item数据
    AutoResizeTreeSubitems(hItem);
    ASSERT_BREAK(hItem->subitems.GetSize() > 0);
    hItem->subitems.SetAt(0, LBT_TreeSubitem(lpszItem, m_textColor, m_bkColor));
    

    // 如果item在list, 则在适当的位置插入一条list项, 并刷新反向索引
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

        // 刷新反向索引
        RefreshListIndex();
    }

    return hItem;
}

//
//-----------------------------------------------------------------------------
// 展开或收缩树节点
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::ExpandTreeItem (LBT_HTREEITEM hItem, BOOL isExpand)
{
    ASSERT_BREAK(!LBT_ISROOTITEM(hItem));  // 非根元素

    // 判断新的状态是否与老状态相同, 如果相同则直接退出
    if ((hItem->isExpand ^ isExpand) == 0)
    {
        return;
    }

    hItem->isExpand = isExpand;

    // 对List的项进行操作, 如果是展开, 则在对应的地方插入List项
    // 如果是收缩, 则把对应的List项删除
    if (IsTreeItemInList(hItem))
    {
        if (isExpand)
        {
            // 遍历hItem的子孙, 把展开的项插入list
            InsertTreeItemKidsToList(hItem);
        }
        else
        {
            // 获得hItem包括其子孙在视图中的范围, 删除list里的子孙元素
            int  lastIndex = CalcTreeItemRangeInList(hItem);
            for (int i = lastIndex; i > hItem->listIndex; i--)
            {
                // 簿记listIndex
                LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(i);
                ASSERT_BREAK(hItem);
                hItem->listIndex = LB_ERR;

                // 删除界面上的元素
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
    //从后往前累积一页高度
    //
    int nBackTopIndex = GetTopIndex();
    if (nBackTopIndex > 0)
    {
        //
        //只有前面还有行才做下面的处理
        //

        int nBackHeightCount = 0;
        while(nBackTopIndex > 0  && 
            nBackHeightCount + GetItemHeight(nBackTopIndex - 1) <= rcClient.Height() )
        {
            --nBackTopIndex;
            nBackHeightCount += GetItemHeight(nBackTopIndex);
        }

        //
        //重新设置本页最顶行
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
    //从前往后累积一页高度
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
    //从后往前累积一页高度
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
    //重新设置本页最顶行
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
// 获得或设置树节点的check属性
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
// 获得或设置树节点的附加Data
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
// 获得或设置树节点的文本颜色等
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
// 控件操作
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 获得树根或父元素
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
// 树节点的儿子兄弟遍历
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
// 树节点是否在视图内的遍历
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
        // 下一个List项
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
// 获得或设置界面上被选中的节点
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
// 消息处理
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
		// 展开收缩当前选中节点
        int  sel = GetCurSel();
        if (sel != LB_ERR)
        {
            LBT_HTREEITEM  hItem = (LBT_HTREEITEM)GetItemDataPtr(sel);
            ASSERT_BREAK(hItem);
            if (hItem->firstChild)
            {
                if (!hItem->isExpand && (nChar == VK_LEFT))
                {
                    //原来收缩，再按左键则向上翻页
                    TreePageUp();
                }
                else if (hItem->isExpand && (nChar == VK_RIGHT))
                {
                    //原来展开，再按右键则向下翻页
                    TreePageDown();
                }
                else 
                {
                    //有子节点
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
        // 截获回车按键消息发送给父窗口, 让父窗口可以处理比如回车确认, 或者回车checkbox的事件
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
						//原来收缩
						ExpandTreeItem(hItem, TRUE);
					}
					else if (hItem->isExpand )
					{
						//原来展开
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
// 绘制item被选择的背景色块
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawSelection (CDC* pDC, const CRect& rcItem, BOOL isHighlight)
{
    if (isHighlight)
    {
        // item 焦点高亮
        pDC->FillSolidRect(rcItem, m_selHighlightColor);
    }
    else
    {
        // 控件丢失焦点时
        pDC->FillSolidRect(rcItem, m_selLowlightColor);
    }
}
//
//-----------------------------------------------------------------------------
// 绘制简单的展开按钮图形 + - 
//-----------------------------------------------------------------------------
//
static void  xxxDrawExpandButton (CDC* pDC, const CRect& rcDraw, BOOL isExpand, int iImage, COLORREF color)
{
    switch (iImage)
    {
    case 1:  // 绘制三角形符号
        CommonEL::DrawExpandIcon(pDC, rcDraw.CenterPoint(), 5, color, isExpand, CommonEL::EXP_TRIANGLE);
        break;
    default:  // 绘制普通的 + - 号
        CommonEL::DrawExpandIcon(pDC, rcDraw.CenterPoint(), 5, color, isExpand, CommonEL::EXP_ADDSUB);
        break;
    }
}
//
//-----------------------------------------------------------------------------
// 绘制展开按钮, 返回被绘制的矩形
//-----------------------------------------------------------------------------
//
CRect  CTreeBoxEL::DrawExpandButton (CDC* pDC, const CRect& rcIcon, BOOL isExpand)
{
    CBitmap*  expandBitmap = (isExpand)? m_expandBitmap: m_collapseBitmap;

    CRect  rcDraw = rcIcon;
    rcDraw.DeflateRect(4, 4);

    if (expandBitmap)
    {
        // 绘制位图
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
// 绘制CheckBox, 返回被绘制的矩形
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
        // 绘制位图
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
		// 绘制位图
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
// 绘制子item的文本及附属元件, 如checkbox, expand图标
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


    // 填充背景色, 及设置文字颜色
    if (isItemHighlight)
    {
        pDC->SetTextColor(m_textHighlightColor);
    }
    else
    {
        pDC->FillSolidRect(rcSubitem, subAttr.bkColor);
        pDC->SetTextColor(subAttr.textColor);
    }

    // 绘制第0列的元素: 展开按钮, checkbox, 树形缩进
    if (colIndex == 0)
    {
        // 加入缩进, 根据层次调整文字显示位置
        int  treelevel = LBT_GetItemLevel(hItem);
        rcText.left +=  treelevel * m_treeIndent;

        // 绘制展开收缩按钮
        if (TreeItemHasChildren(hItem))
        {
            CRect  rcIcon(rcText.left, rcText.top, rcText.left+rcText.Height(), rcText.bottom);
            DrawExpandButton(pDC, rcIcon, !hItem->isExpand);
        }
        rcText.left += rcText.Height();

        // 绘制展开按钮
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

        // 修正rcText
        if (rcText.left > rcText.right)
        {
            rcText.left = rcText.right;
        }
    }

    // 绘制文字
    pDC->DrawText(subAttr.text, rcText, colAttr->itemAlignment|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
}
//
//-----------------------------------------------------------------------------
// 绘制焦点虚框
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::DrawFocus (CDC* pDC, const CRect& rcItem)
{
    pDC->DrawFocusRect(rcItem);
}
//
//-----------------------------------------------------------------------------
// 绘制整个item的总驱动函数
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
        // 获得当前Item是否需要绘制选择色块
        if (isItemCurSel && (isWndFocus || m_bShowSelAlways))
        {
            isItemHighlight = TRUE;
            DrawSelection(pDC, rcItem, isWndFocus);
        }


        LBT_HTREEITEM  hItem = (LBT_HTREEITEM)lpDIS->itemData;
        ASSERT_BREAK(hItem);

        // 遍历绘制所有的subitem
        for (int colIndex = 0; 
            colIndex < m_treeHeader.GetColumnCount() && colIndex < hItem->subitems.GetSize(); colIndex++)
        {
            CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);

            // 获得一个子item的绘制区域, +1 -1 是为了避开表格的Grid线
            CRect  rcSubitem(rcColumn.left+1, rcItem.top, rcColumn.right, rcItem.bottom-1);
            DrawSubItem(pDC, rcSubitem, hItem, colIndex, isItemHighlight);
        }

        // 绘制焦点框
        if (isItemCurSel && isWndFocus)  
        {
            // 不绘制焦点虚线框
            //DrawFocus(pDC, rcItem);
        }

    }

}
//
//-----------------------------------------------------------------------------
// 绘制整个背景和网格线
//-----------------------------------------------------------------------------
//
void  CTreeBoxEL::FillBkgnd (CDC* pDC)
{
    // 绘制整个表格的背景
    CRect  clientRect;
    GetMarginClientRect(&clientRect);
    pDC->FillSolidRect(clientRect, m_bkColor);


    COLORREF  gridColor = m_treeHeader.GetHeaderGridlineColor();
    int  centerX = (clientRect.left + clientRect.right) / 2;  // 计算中点

    // 绘制垂直网格线
    if (m_hasVGridline)
    {
        // 是否绘制渐变的网格线
        if (m_gradientVGridline)
        {
            // 遍历绘制网格线
            for (int colIndex = 1; colIndex < m_treeHeader.GetColumnCount(); colIndex++)
            {
                CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);
                CRect  gridRect(rcColumn.left, clientRect.top, rcColumn.left+1, clientRect.bottom);
                CommonEL::DrawGradient(pDC, gridRect, gridColor, GetBkColor(), GRADIENT_FILL_RECT_V);
            }

            // 绘制水平封底线, 分两段绘制
            CRect  gridRectLeft(clientRect.left, clientRect.bottom-1, centerX, clientRect.bottom);
            CRect  gridRectRight(centerX, clientRect.bottom-1, clientRect.right, clientRect.bottom);
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
        }
        else
        {
            // 绘制实线
            for (int colIndex = 1; colIndex < m_treeHeader.GetColumnCount(); colIndex++)
            {
                CRect  rcColumn = m_treeHeader.GetItemRect(colIndex);
                pDC->FillSolidRect(rcColumn.left, clientRect.top, 1, clientRect.Height(), gridColor);
            }

            // 绘制水平封底线
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);
        }
    }
}

//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 判断树节点是否显示在List里面, 也就是说节点的父辈是否都被展开, 否则节点是隐藏的
//-----------------------------------------------------------------------------
//
BOOL  CTreeBoxEL::IsTreeItemInList (LBT_HTREEITEM hItem)
{
    LBT_HTREEITEM  hParent = LBT_GetParentItem(hItem);
    ASSERT_BREAK(hParent);

    // 判断节点是否可见(第0层节点总是可见)
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
// 计算item及其所有子孙节点显示在列表中的Index范围, 返回最后一个元素的Index
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
// 如果CalcTreeItemRangeInList (C2), 则应该返回 7 (CCC2), CCC2是C2的最后一个子孙
// 本函数可以用于删除TreeNode对应的List中的对应元素, 比如删除C2 就应该把他的所有子孙都删除
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
// 向List插入hItem子孙, 只要是展开的, 返回最后一次插入的Index
//  通常该函数用于展开节点, 比如展开某个节点, 可能要批量插入该节点的所有子孙(被展开的)
//
//          原状态          LISTINDEX
//  - ROOT
//      |- P1                   0
//          |-C1                1        
//          |+C2                2
//          |-C3                3
//
//
// InsertTreeItemKidsToList (C2)
//
//          展开C2          LISTINDEX
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
        
        // 递归插入子节点
        if (child->isExpand)
        {
            lastIndex = InsertTreeItemKidsToList(child);
        }
    }

    return lastIndex;
}
//
//-----------------------------------------------------------------------------
// 刷新反向索引, 使得所有的树节点指向正确的所在List的位置
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
// 刷新显示树节点, 如果hItem=NULL则刷新整个控件
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
// 根据当前的表头列数, 自动调整hItem的子item数组大小, 数组最小是1个元素
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

        // 根据当前的前景背景色, 调整新加入的subitem的颜色
        for (int sub = oldSize; sub < colCount; sub++)
        {
            SetTreeItemColor(hItem, sub, m_bkColor, m_textColor);
        }
    }
}

//
//-----------------------------------------------------------------------------
// 给父窗口发送notify消息
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

//记录展开/收缩项
void CTreeBoxEL::RecordExpandStatus()
{

	for (int i =0; i<GetCount();i++)
	{
		LBT_HTREEITEM hItem = (LBT_HTREEITEM) GetItemDataPtr(i);
		IsExpand[i] = hItem->isExpand;
	}
	nSelectIndex = GetTreeSelectedItem()->listIndex;

}
//恢复展开/收缩
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
