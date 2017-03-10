#include "stdafx.h"

#include "../Common/CommonEL.h"
#include "ListBoxEL.h"


CListBoxEL::CListBoxEL ()
{
    // 注册对应的窗口类
    RegisterWndClass(NULL, LISTBOXEL_CLASSNAME, CS_DBLCLKS);


    //m_itemArray
    m_curSelIndex = LB_ERR;
    m_topIndex = 0;
    m_defItemHeight = 20;
    m_font = NULL;
    m_textColor = RGB(0, 0, 0);
    m_bkColor = RGB(255, 255, 255);

    m_bShowSelAlways = TRUE;
    m_selHighlightColor = ::GetSysColor(COLOR_HIGHLIGHT);
    m_textHighlightColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    m_selLowlightColor = RGB(222, 222, 222);

    m_marginLeft    = 0;
    m_marginTop     = 0;
    m_marginRight   = 0;
    m_marginBottom  = 0;
}
CListBoxEL::~CListBoxEL ()
{
    ResetContent();
}

//
//-----------------------------------------------------------------------------
// 创建控件窗口
//-----------------------------------------------------------------------------
//
BOOL  CListBoxEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::Create(LISTBOXEL_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

BOOL  CListBoxEL::CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::CreateEx(dwExStyle, LISTBOXEL_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

//
//-----------------------------------------------------------------------------
// 设置当前使用的字体
//-----------------------------------------------------------------------------
//
void  CListBoxEL::SetFont (CFont* pFont, BOOL bRedraw)
{
    m_font = pFont;
    UpdateDefItemHeight(pFont);

    if (bRedraw)
    {
        UpdateListBox();
    }
}
CFont*  CListBoxEL::GetFont ()
{
    return m_font;
}
//
//-----------------------------------------------------------------------------
// 设置表格四边的空白边距, 空白区域可以防止其他的控件, 比如表头, 滚动条
//-----------------------------------------------------------------------------
//
void  CListBoxEL::SetMargin (int left, int top, int right, int bottom)
{
    m_marginLeft = left;
    m_marginTop = top;
    m_marginRight = right;
    m_marginBottom = bottom;
}

//
//=============================================================================
// 数据操作
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 获得item的数量
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetCount ()
{
    return m_itemArray.GetSize();
}

//
//-----------------------------------------------------------------------------
// 添加一个item, 并指定其关联的用户自定义数据
//  调用该API后, 会自动回调MeasureItem计算item的高度, 用户可以重载该接口自定义项目的高度
//-----------------------------------------------------------------------------
//
int  CListBoxEL::AddString (LPCTSTR lpszItem, DWORD_PTR data)
{
    LB_ItemInfo*  itemInfo = new LB_ItemInfo;
    itemInfo->caption = lpszItem;
    itemInfo->data = data;
    itemInfo->cyHeight = m_defItemHeight;

    // 将item加入容器
    int  newIndex = m_itemArray.Add(itemInfo);

    // 回调计算项目高度
    CallbackMeasureItem(newIndex, itemInfo);

    UpdateListBox();

    return newIndex;
}

//
//-----------------------------------------------------------------------------
// 在指定的位置, 插入一个项目
// 当nIndex=-1时, 表示添加一个新项目
//-----------------------------------------------------------------------------
//
int  CListBoxEL::InsertString (int nIndex, LPCTSTR lpszItem, DWORD_PTR data)
{
    if (nIndex >= 0 && nIndex <= GetCount())    // Insert项必须在合理的Index
    {
        LB_ItemInfo*  itemInfo = new LB_ItemInfo;
        itemInfo->caption = lpszItem;
        itemInfo->data = data;
        itemInfo->cyHeight = m_defItemHeight;

        // 将item加入容器
        m_itemArray.InsertAt(nIndex, itemInfo);

        // 回调计算项目高度
        CallbackMeasureItem(nIndex, itemInfo);

        // 保持原来的Sel不变
        if (m_curSelIndex >= nIndex)
        {
            SetCurSel(m_curSelIndex + 1);
        }

        UpdateListBox();
        return nIndex;        
    }
    else if (nIndex == -1)                      // nIndex==-1 相当于添加一个新的项
    {
        return AddString(lpszItem, data);
    }
    else                                        // nIndex不在合理范围
    {
        ASSERT_BREAK(FALSE);
        return LB_ERR;
    }
}

//
//-----------------------------------------------------------------------------
// 删除指定的项目
//-----------------------------------------------------------------------------
//
int  CListBoxEL::DeleteString (int nIndex)
{
    LB_ASSERTINDEX(nIndex, 0);

    // 删除的项目刚好是被选中的项目, 如果不是, 则要sel保持不变
    if (nIndex == m_curSelIndex)
    {
        SetCurSel(-1);
    }
    else if (m_curSelIndex > nIndex)
    {
        SetCurSel(m_curSelIndex - 1);
    }

    // 调整Top, 通常情况下top保持不变, 
    // 除非top是最后一个元素并且刚好又删除了某个元素, 保证topIndex的合理性
    if (m_topIndex >= (GetCount() - 1))
    {
        int  newTop = GetCount() - 2;           // 后面将会删除一个元素, 所以 -2
        if (newTop >= 0)
        {
            SetTopIndex(newTop);
        }
    }



    // 删除item
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);
    delete itemInfo;
    m_itemArray.RemoveAt(nIndex);


    UpdateListBox();
    return GetCount();
}

//
//-----------------------------------------------------------------------------
// 清空列表的所有项
//-----------------------------------------------------------------------------
//
void  CListBoxEL::ResetContent ()
{
    // 恢复与item相关的属性
    m_curSelIndex = LB_ERR;
    m_topIndex = 0;

    // 释放所有item
    for (int i = 0; i < m_itemArray.GetSize(); i++)
    {
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(i);
        ASSERT_BREAK(itemInfo);

        delete itemInfo;
    }
    m_itemArray.RemoveAll();

    
    UpdateListBox();
}

//
//-----------------------------------------------------------------------------
// 获得指定item的显示文本
//-----------------------------------------------------------------------------
//
void  CListBoxEL::GetText (int nIndex, CString& rString)
{
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);

    rString = itemInfo->caption;
}

//
//-----------------------------------------------------------------------------
// 设置指定item关联的用户自定义数据
//-----------------------------------------------------------------------------
//
DWORD_PTR  CListBoxEL::GetItemData (int nIndex)
{
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);

    return itemInfo->data;
}
int  CListBoxEL::SetItemData (int nIndex, DWORD_PTR dwItemData)
{
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);

    itemInfo->data = dwItemData;
    return LB_OKAY;
}
void*  CListBoxEL::GetItemDataPtr (int nIndex)
{
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);

    return (void*)itemInfo->data;
}
int  CListBoxEL::SetItemDataPtr (int nIndex, void* pData)
{
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);

    itemInfo->data = (DWORD_PTR)pData;
    return LB_OKAY;
}


//
//=============================================================================
// 控件操作
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 确保item可见
//-----------------------------------------------------------------------------
//
void  CListBoxEL::EnsureVisible (int nIndex, BOOL bPartialOK)
{
    LB_ASSERTINDEX(nIndex, 0);

    if (!IsItemVisible(nIndex, bPartialOK))
    {
        int  curTopIndex = GetTopIndex();
        if (nIndex >= curTopIndex)
        {
            // 从nIndex往上找到最大的topIndex, 只要保证nIndex可见
            CRect  clientRect;
            GetMarginClientRect(&clientRect);
            if (clientRect.IsRectEmpty())
            {
                // 刚刚创建窗口, 矩形是0,0,0,0
                return;
            }

            int  totalHeight = 0;
            int targetTopIndex = nIndex;
            for ( ; targetTopIndex >= 0; targetTopIndex--)
            {
                LB_ItemInfo*  itemInfo = m_itemArray.GetAt(targetTopIndex);
                ASSERT_BREAK(itemInfo);

                totalHeight += itemInfo->cyHeight;
                if (totalHeight > clientRect.Height())
                {
                    if (targetTopIndex < nIndex)
                    {
                        targetTopIndex++;
                    }
                    else
                    {
                        // 当nIndex本身就太高时(高于窗口大小)
                    }
                    break;
                }
            }
            SetTopIndex(targetTopIndex);
        }
        else
        {
            SetTopIndex(nIndex);
        }
    }
}

//
//-----------------------------------------------------------------------------
// 判断item是否在当前窗口可见
//  bPartialOK 指定是否允许部分可见就返回TRUE, 否则 必须全部可见才返回TRUE
//-----------------------------------------------------------------------------
//
BOOL  CListBoxEL::IsItemVisible (int nIndex, BOOL bPartialOK)
{
    CRect  itemRect;
    GetItemRect(nIndex, &itemRect);

    CRect  clientRect;
    GetMarginClientRect(&clientRect);

    CRect  intersectRect;
    if (intersectRect.IntersectRect(clientRect, itemRect))
    {
        if (bPartialOK)
        {
            return TRUE;
        }
        else
        {
            return (itemRect.Height() == intersectRect.Height());
        }
    }
    else
    {
        return FALSE;
    }
}
//
//-----------------------------------------------------------------------------
// 设置当前选择的显示的第一条item, 默认是0
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetTopIndex ()
{
    return m_topIndex;
}
int  CListBoxEL::SetTopIndex (int nIndex)
{
    LB_ASSERTINDEX(nIndex, 0);

    if (nIndex == m_topIndex)
    {
        // 新旧值一样, 没有必要, 无需刷新
        return LB_OKAY;
    }

    if (nIndex >= 0 && nIndex < m_itemArray.GetSize())
    {
        m_topIndex = nIndex;

        UpdateListBox();
        return LB_OKAY;
    }
    else
    {
        return LB_ERR;
    }
}

//
//-----------------------------------------------------------------------------
// 设置当前选择的item, -1代表没有选中任何item
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetCurSel ()
{
    return m_curSelIndex;
}
int  CListBoxEL::SetCurSel (int nSelect)
{
    LB_ASSERTINDEX(nSelect, -1);

    if (nSelect == m_curSelIndex)
    {
        // 新旧值一样, 没有必要, 无需刷新
        return LB_OKAY;
    }


    int  oldSelIndex = m_curSelIndex;
    if (nSelect >= -1 && nSelect < GetCount())
    {
        m_curSelIndex = nSelect;

        if (nSelect >= 0)
        {
            // 只有目标item是有效item时, 才使之可见
            EnsureVisible(nSelect, FALSE);

            // 刷新更改过的格子
            UpdateListBox(nSelect);
        }
        
        // 刷新更改过的格子
        if (oldSelIndex >= 0 && oldSelIndex < GetCount())
        {
            UpdateListBox(oldSelIndex);
        }

        return LB_OKAY;
    }
    else
    {
        return LB_ERR;
    }
}

//
//-----------------------------------------------------------------------------
// 获得指定item当当前窗口的位置(受topIndex影响)
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetItemRect (int nIndex, LPRECT lpRect)
{
    LB_ASSERTINDEX(nIndex, 0);
    ASSERT_BREAK(lpRect);

    // 获得窗口大小
    CRect  clientRect;
    GetMarginClientRect(&clientRect);


    // 决定查找方向
    int  direct = 0;
    int  topIndex = GetTopIndex();
    if (nIndex >= topIndex)
    {
        direct = 1;
    }
    else
    {
        direct = -1;
    }
    
    // 从topindex开始循环查找, 直到目标index
    int  targetIndex = GetTopIndex();
    int  targetRectTop = clientRect.top;        // top坐标
    do 
    {
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(targetIndex);
        ASSERT_BREAK(itemInfo);

        if (targetIndex == nIndex)
        {
            // 找到目标
            lpRect->left = clientRect.left;
            lpRect->right = clientRect.right;
            lpRect->top = targetRectTop;
            lpRect->bottom = targetRectTop + itemInfo->cyHeight;
            break;
        }
        else
        {
            
            // 根据查找方向, 更新targetRectTop
            if (direct < 0)
            {
                // 上一个item
                LB_ItemInfo*  prevItemInfo = m_itemArray.GetAt(targetIndex -1);
                ASSERT_BREAK(prevItemInfo);

                targetRectTop -= prevItemInfo->cyHeight;
            }
            else
            {
                targetRectTop += itemInfo->cyHeight;
            }

            targetIndex += direct;              // next
        }

    } while(1);


    return LB_OKAY;
}

//
//-----------------------------------------------------------------------------
// 设置指定item的显示高度
//-----------------------------------------------------------------------------
//
int  CListBoxEL::SetItemHeight (int nIndex, UINT cyItemHeight)
{
    if (nIndex >= 0 && nIndex < GetCount())
    {
        // 如果nIndex有效, 则设置指定项的高度
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
        ASSERT_BREAK(itemInfo);

        itemInfo->cyHeight = cyItemHeight;

        UpdateListBox();
    }
    else
    {
        // 如果nIndex无效, 则设置默认高度
        m_defItemHeight = cyItemHeight;
    }

    return LB_OKAY;
}
int  CListBoxEL::GetItemHeight (int nIndex)
{
    if (nIndex >= 0 && nIndex < GetCount())
    {
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
        ASSERT_BREAK(itemInfo);

        return itemInfo->cyHeight;
    }
    else
    {
        return m_defItemHeight;
    }
}


//
//=============================================================================
// 消息处理
//=============================================================================
//
BEGIN_MESSAGE_MAP(CListBoxEL, CWnd)
    ON_WM_GETDLGCODE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_KEYDOWN()
    ON_WM_DESTROY()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()


UINT  CListBoxEL::OnGetDlgCode ()
{
    return DLGC_WANTARROWS;
}
void  CListBoxEL::OnPaint ()
{
    CPaintDC  dc(this);
    Paint(&dc);
}
BOOL  CListBoxEL::OnEraseBkgnd (CDC* pDC)
{
    return TRUE;
}
void  CListBoxEL::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int direct = 0;
    if (nChar == VK_UP || nChar == VK_LEFT)
    {
        direct = -1;
    }
    else if (nChar == VK_DOWN || nChar == VK_RIGHT)
    {
        direct = 1;
    }
    int  targetSel = GetCurSel() + direct;

    // 如果超出列表范围, 自动翻转
    if (targetSel < 0)
    {
        targetSel = GetCount() - 1;
    }
    else if (targetSel >= GetCount())
    {
        // fix除0错误, 原来是直接 % GetCount()
        targetSel = 0;
    }

    // 选择目标行
    if (targetSel >= 0 && targetSel <GetCount())
    {
        SetCurSel(targetSel);
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void  CListBoxEL::OnDestroy ()
{
    ResetContent();
}


//
//-----------------------------------------------------------------------------
// 获得和丢失焦点时, 重绘控件, 使得选项的高亮低亮可以显示
//-----------------------------------------------------------------------------
//
void  CListBoxEL::OnKillFocus (CWnd* pNewWnd)
{
    UpdateListBox();

    CWnd::OnKillFocus(pNewWnd);
}
void  CListBoxEL::OnSetFocus (CWnd* pOldWnd)
{
    UpdateListBox();

    CWnd::OnSetFocus(pOldWnd);
}



BOOL  CListBoxEL::PreTranslateMessage (MSG* pMsg)
{
    if (pMsg->hwnd == GetSafeHwnd())
    {

    }

    return CWnd::PreTranslateMessage(pMsg);
}

//
//-----------------------------------------------------------------------------
// 给子类绘制非客户区的机会, 非客户区可以通过SetMargin设置
// SetMargin之外的区域就是非客户区
//-----------------------------------------------------------------------------
//
void  CListBoxEL::PaintNC (CDC* /*pDC*/)
{

}
//
//-----------------------------------------------------------------------------
// 填充背景位图
//-----------------------------------------------------------------------------
//
void  CListBoxEL::FillBkgnd (CDC* pDC)
{
    // 绘制整个表格的背景
    CRect  clientRect;
    GetMarginClientRect(&clientRect);
    pDC->FillSolidRect(clientRect, m_bkColor);
}
//
//-----------------------------------------------------------------------------
// 绘制整个控件
//-----------------------------------------------------------------------------
//
void  CListBoxEL::Paint (CDC* pDC)
{
    ASSERT_BREAK(pDC);
    CDC&  dc = *pDC;

    // 给派生类绘制非客户区
    PaintNC(&dc);

    // 根据边距设置裁剪区, 使得DC不绘制空白边距部分
    CRect  marginClientRect;
    GetMarginClientRect(&marginClientRect);
    dc.IntersectClipRect(marginClientRect);

    // 填充背景
    FillBkgnd(&dc);

    CFont*  oldfont = dc.SelectObject(GetFont());
    dc.SetBkMode(TRANSPARENT);

    // 遍历可视的item, 绘制
    for (int i = GetTopIndex(); i < GetCount() && IsItemVisible(i, TRUE); i++)
    {
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(i);
        ASSERT_BREAK(itemInfo);

        UINT  itemAction = 0;
        UINT  itemState = 0;
        if (m_curSelIndex == i)
        {
            itemAction |= ODA_SELECT | ODA_FOCUS;
            itemState |= ODS_SELECTED | ODS_FOCUS;
        }

        CRect  itemRect;
        GetItemRect(i, &itemRect);
        CallbackDrawItem(i, itemInfo, itemAction, itemState, dc.GetSafeHdc(), itemRect);
    }

    dc.SelectObject(oldfont);
}
//
//-----------------------------------------------------------------------------
// 绘制一个小项, 可以给用户重载
//  本函数在每次OnPaint重绘时被调用
//-----------------------------------------------------------------------------
//
void  CListBoxEL::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT_BREAK(lpDIS->CtlType == ODT_LISTBOX);

    CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
    HWND  curFocusedWnd = ::GetFocus();
    CRect  rcItem = lpDIS->rcItem;
    CRect  rcText(lpDIS->rcItem.left + 10, lpDIS->rcItem.top, lpDIS->rcItem.right - 10, lpDIS->rcItem.bottom);
    CString  itemText; GetText(lpDIS->itemID, itemText);
    COLORREF  oldtextcolor = pDC->GetTextColor();

    if (lpDIS->itemID != (UINT)-1)
    {
        // 绘制选择高亮条
        if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED) && 
            (curFocusedWnd == m_hWnd || m_bShowSelAlways))
        {
            if (curFocusedWnd == m_hWnd)
            {
                // item 焦点高亮
                pDC->FillSolidRect(rcItem, m_selHighlightColor);

                // 文字高亮
                pDC->SetTextColor(m_textHighlightColor);
            }
            else
            {
                // 控件丢失焦点时, 但m_bShowSelAlways
                pDC->FillSolidRect(rcItem, m_selLowlightColor);

                pDC->SetTextColor(m_textColor);
            }
        }
        else
        {
            pDC->SetTextColor(m_textColor);
        }
        
        // 绘制文字
        pDC->DrawText(itemText, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);

        // 绘制焦点框
        if ((lpDIS->itemAction | ODA_FOCUS) && (lpDIS->itemState & ODS_FOCUS) &&
            curFocusedWnd == m_hWnd)  
        {
            pDC->DrawFocusRect(rcItem);
        }
    }

    pDC->SetTextColor(oldtextcolor);
}

//
//-----------------------------------------------------------------------------
// 根据输入的项目参数, 重新设定项目的宽高, 可以给用户重载
//  本函数在新添加项目AddString时被调用
//-----------------------------------------------------------------------------
//
void  CListBoxEL::MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{

}

//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 获取除去空白边界外的客户区矩形
//-----------------------------------------------------------------------------
//
void  CListBoxEL::GetMarginClientRect (LPRECT lpRect)
{
    ASSERT_BREAK(lpRect);
    GetClientRect(lpRect);

    if (::IsRectEmpty(lpRect))
    {
        return;
    }
    
    lpRect->left += m_marginLeft;
    lpRect->top += m_marginTop;
    lpRect->right -= m_marginRight;
    lpRect->bottom -= m_marginBottom;

    // 使得矩形正规化, 防止边距过度
    if (lpRect->left > lpRect->right)
    {
        ASSERT_BREAK(FALSE);
        lpRect->right = lpRect->left;
    }
    if (lpRect->top > lpRect->bottom)
    {
        ASSERT_BREAK(FALSE);
        lpRect->bottom = lpRect->top;
    }
}

//
//-----------------------------------------------------------------------------
// 根据所选的字体, 更新item默认高度
//-----------------------------------------------------------------------------
//
void  CListBoxEL::UpdateDefItemHeight (CFont* font)
{
    TEXTMETRIC  tm;
    GetTextMetricsHelper(font, &tm);
    m_defItemHeight = tm.tmHeight;
}
//
//-----------------------------------------------------------------------------
// 获取字体尺寸
//-----------------------------------------------------------------------------
//
void  CListBoxEL::GetTextMetricsHelper (CFont* font, LPTEXTMETRIC lptm)
{
    ASSERT_BREAK(font && lptm);

    HDC  dc = ::GetDC(GetSafeHwnd());
    HGDIOBJ  oldfont = ::SelectObject(dc, font->GetSafeHandle());
    {
        ::GetTextMetrics(dc, lptm);
    }
    ::SelectObject(dc, oldfont);
    ::ReleaseDC(GetSafeHwnd(), dc);
}
//
//-----------------------------------------------------------------------------
// 刷新本控件一次
//  nIndex == -1代表整个表格刷新, 否则只刷新特定的格子
//-----------------------------------------------------------------------------
//
void  CListBoxEL::UpdateListBox (int nIndex)
{
    if (GetSafeHwnd())
    {
        if (nIndex < 0)
        {
            Invalidate();
        }
        else
        {
            CRect  itemRect;
            GetItemRect(nIndex, &itemRect);
            InvalidateRect(itemRect);
        }
    }
}

//
//-----------------------------------------------------------------------------
// 辅助回调绘制item
//-----------------------------------------------------------------------------
//
void  CListBoxEL::CallbackDrawItem (int nIndex, LB_ItemInfo* item, 
                                    UINT itemAction, UINT itemState, 
                                    HDC hDC, RECT rcItem)
{
    DRAWITEMSTRUCT  drawItemStruct;
    drawItemStruct.CtlType = ODT_LISTBOX;
    drawItemStruct.CtlID = GetDlgCtrlID();
    drawItemStruct.itemID = nIndex;
    drawItemStruct.itemAction = itemAction;
    drawItemStruct.itemState = itemState;
    drawItemStruct.hwndItem = GetSafeHwnd();
    drawItemStruct.hDC = hDC;
    drawItemStruct.rcItem = rcItem;
    drawItemStruct.itemData = item->data;

    // 让用户自己去绘制item
    DrawItem(&drawItemStruct);
}

//
//-----------------------------------------------------------------------------
// 辅助回调计算item宽高
//-----------------------------------------------------------------------------
//
void  CListBoxEL::CallbackMeasureItem (int nIndex, LB_ItemInfo* item)
{
    CRect  clientRect;
    GetMarginClientRect(&clientRect);

    MEASUREITEMSTRUCT  measureItemStruct;
    measureItemStruct.CtlType = ODT_LISTBOX;
    measureItemStruct.CtlID = GetDlgCtrlID();
    measureItemStruct.itemID = nIndex;
    measureItemStruct.itemWidth = clientRect.Width();
    measureItemStruct.itemHeight = item->cyHeight;
    measureItemStruct.itemData = item->data;

    // 询问用户item的真实宽度高度
    MeasureItem(&measureItemStruct);

    // 重新设置item的宽高
    item->cyHeight = measureItemStruct.itemHeight;
}

static CRect  __GetScreenArea (HMONITOR hMonitor)
{
    MONITORINFO  info;

    if (::GetMonitorInfo(hMonitor, &info))
    {
        return info.rcMonitor;
    }

    return CRect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
}
static CRect  __GetScreenArea (LPCRECT lprcScreenCoords)
{
    return __GetScreenArea(MonitorFromRect(lprcScreenCoords, 0));
}
//
//-----------------------------------------------------------------------------
// 自动调整窗口的位置, 使得窗口完整的显示在屏幕上, 并围绕着surroundRect
// 返回调整后的矩形位置
// 比如用在: ListCtrl的一个格子弹出下拉菜单(ListBox), ListBox必须依附在格子surroundRect周围, 但又不能超出屏幕
//-----------------------------------------------------------------------------
//
void  CListBoxEL::AdjustWindowRectInsideScreen(const CRect& surroundRect, IN OUT CRect& retRectFix)
{
    // 根据屏幕工作区大小, 调整窗口位置, 比如刚好在屏幕边缘时, 让窗口显示完整
    CRect  rcWork = __GetScreenArea(retRectFix);
	
	//把工作区域调整，大小
	rcWork.top += 38;
	rcWork.bottom  -= 37;
	rcWork.left += 110;

	//由于之前是在水平方向的菜单栏，所以存在菜单栏与弹出框高度的问题，现在改成竖直方向的菜单栏，所以可以忽略对菜单栏和弹出框本身高度的问题
    //if (retRectFix.bottom > rcWork.bottom && retRectFix.top > rcWork.CenterPoint().y)
    //{
    //    retRectFix.OffsetRect(0, - retRectFix.Height() - surroundRect.Height());
    //}
    if (retRectFix.top < rcWork.top)
    {
        retRectFix.OffsetRect(0, rcWork.top - retRectFix.top);
    }
    if (retRectFix.bottom > rcWork.bottom)
    {
        retRectFix.OffsetRect(0, rcWork.bottom - retRectFix.bottom);
    }
    if (retRectFix.left < rcWork.left)
    {
        retRectFix.OffsetRect(rcWork.left - retRectFix.left, 0);
    }
    if (retRectFix.right > rcWork.right)
    {
        retRectFix.OffsetRect(rcWork.right - retRectFix.right, 0);
    }
}

//
//-----------------------------------------------------------------------------
// 注册窗口类
//-----------------------------------------------------------------------------
//
BOOL  CListBoxEL::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class已经存在
        return TRUE;
    }

    LB_DEBUGLOG(L"CListBoxEL::RegisterWndClass\n");

    WNDCLASS wndcls;
    ZeroMemory(&wndcls, sizeof(wndcls));

    wndcls.style = style;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = hInstance ? hInstance : AfxGetInstanceHandle();
    wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.lpszClassName = lpszClassName;
    wndcls.hIcon = hIcon;
    wndcls.hbrBackground = (HBRUSH)::GetStockObject(bgStockBrush);

    return AfxRegisterClass(&wndcls);
}