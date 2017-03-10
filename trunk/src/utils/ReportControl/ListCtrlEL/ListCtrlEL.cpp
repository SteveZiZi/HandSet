#include "stdafx.h"
#include "ListCtrlEL.h"
#include "../Common/CommonEL.h"


IMPLEMENT_DYNAMIC(CListCtrlEL, ELSUPER)




//
//=============================================================================
// 创建控件
//=============================================================================
//


CListCtrlEL::CListCtrlEL()
{
    m_nRowHeight = 41;
    m_hasHGridline = TRUE;
    m_hasVGridline = TRUE;
    m_gradientHGridline = FALSE;
    m_gradientVGridline = FALSE;
    m_bFreezeSelectionVisible = FALSE;
    m_bRowSelectionVisible = TRUE;
    m_focusedSubItemHighlightColor = RGB(0, 0, 255);
    m_selectionRowHighlightColor = ::GetSysColor(COLOR_HIGHLIGHT);
    m_textHighlightColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    m_selectionRowLowlightColor = ::GetSysColor(COLOR_BTNFACE);
    m_checkBitmap = NULL;
    m_uncheckBitmap = NULL;


    m_nFreezeColumnsCount = 0;
    m_bRowFocusEnable = TRUE;
    m_bSubItemFocusEnable = FALSE;
    m_focusedColIndex = -1;
    m_bAutoRowEditEnable = TRUE;

    m_pInplaceEdit = new CELInplaceEdit();
    m_pInplaceList = new CELInplaceList();
    m_pActiveItem = NULL;
}

CListCtrlEL::~CListCtrlEL()
{
    // 当前正在编辑状态, 不应该析构本对象, 
    // 肯定是某个地方处理流程有问题, 这是异常情况 bug2013.06.19
    ASSERT_BREAK(m_pActiveItem == NULL);

    delete m_pInplaceEdit;
    delete m_pInplaceList;
}



void CListCtrlEL::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    ModifyStyle(0, LVS_OWNERDRAWFIXED);
    ELSUPER::PreSubclassWindow();

    CHeaderCtrl *pHeader = GetHeaderCtrl();
    m_Header.SubclassWindow(pHeader->GetSafeHwnd());


    ELSUPER::SetExtendedStyle(/*LVS_EX_GRIDLINES|*/LVS_EX_FULLROWSELECT);
}

BOOL  CListCtrlEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = ELSUPER::Create(dwStyle|LVS_OWNERDRAWFIXED, rect, pParentWnd, nID);
    PreSubclassWindow();

    HideScrollBars();

    return ret;
}

//
//=============================================================================
// 样式操作
//=============================================================================
//


//
//-----------------------------------------------------------------------------
// 设置表格每行的高度
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowHeight (int nHeight) //高置行高
{
    m_nRowHeight = nHeight;

    CRect  rcWin;
    GetWindowRect(&rcWin);
    WINDOWPOS  wp;
    wp.hwnd = m_hWnd;
    wp.cx = rcWin.Width();
    wp.cy = rcWin.Height();
    wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
    SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}


//
//=============================================================================
// 控件操作
//=============================================================================
//


BOOL  CListCtrlEL::HasFocus()
{
    const CWnd*  pFocusWnd = GetFocus();
    if (!pFocusWnd)
    {
        return FALSE;
    }

    return (pFocusWnd->GetSafeHwnd() == m_hWnd)
        || (pFocusWnd->GetParent()->GetSafeHwnd() == m_hWnd)
        || (pFocusWnd->GetOwner()->GetSafeHwnd() == m_hWnd);
}

//
//-----------------------------------------------------------------------------
// 本控件是否正在处于item编辑状态
//-----------------------------------------------------------------------------
//
BOOL  CListCtrlEL::IsEditMode ()
{
    CELInplaceEdit*  pEdit = GetInplaceEdit();

    BOOL  bEditMode = pEdit && pEdit->GetSafeHwnd() && pEdit->IsWindowVisible();
    bEditMode |= GetInplaceList() && GetInplaceList()->GetSafeHwnd() && GetInplaceList()->IsWindowVisible();

    return bEditMode;
}

//
//-----------------------------------------------------------------------------
// 使得item进入编辑状态
//  pItemArgs 传 NULL 代表取消当前编辑
//  bAutoFocus 代表是否焦点自动定位到目标编辑item
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::EditItem (EL_RECORDITEM_ARGS* pItemArgs, BOOL bAutoFocus)
{
    CELRecordItem*  pItem = pItemArgs ? pItemArgs->pItem : NULL;

    // 取消之前的编辑控件
    if (m_pActiveItem != NULL)
    {
        m_pActiveItem->OnEndEdit(this);
        m_pActiveItem = NULL;
    }


    // 让item进入编辑模式
    if (pItemArgs && pItem && pItem->IsAllowEdit(pItemArgs) && 
        IsValidRowIndex(pItemArgs->rowIndex) && 
        IsValidColIndex(pItemArgs->colIndex))
    {
        // 如果本控件没在焦点, 则设置焦点
        if (!HasFocus())
        {
            SetFocus();
        }
        
        // 自动定位焦点
        if (bAutoFocus)
        {
            SetFocusedSelectionRowIndex(pItemArgs->rowIndex);
            SetFocusedColIndex(pItemArgs->colIndex);
        }

        // 保证正在编辑行可见
        EnsureRowVisibleScrollRow(pItemArgs->rowIndex);
        

        // 因为之前可能有滚动, 重新调整item的rect
        pItemArgs->rcItem = GetRowItemRect(pItemArgs->rowIndex, pItemArgs->colIndex);

        // 进入编辑模式, 并做簿记m_pActiveItem
        m_pActiveItem = pItem;
        pItem->OnBeginEdit(pItemArgs);
        if (!IsEditMode())  // ??  Need to actually cancel the edit - old version (10) did not have this call
        {
            EditItem(NULL);
        }

        // 使得编辑控件中的文本被选中
        if (GetInplaceEdit()->GetSafeHwnd() && 
            GetInplaceEdit()->GetItem() == pItem)
        {
            CELRecordItemEditOptions* pEditOptions = 
                pItem->GetEditOptions(m_Header.GetColumn(pItemArgs->colIndex));
            if (pEditOptions && pEditOptions->IsSelectTextOnEdit())
            {
                GetInplaceEdit()->SetSel(0, -1);
            }
            else
            {
                CString str;
                GetInplaceEdit()->GetWindowText(str);
                GetInplaceEdit()->SetSel(str.GetLength(), str.GetLength());
            }
        }
    }
}


//
//-----------------------------------------------------------------------------
// 获得指定Item在客户区的矩形大小
//-----------------------------------------------------------------------------
//
CRect  CListCtrlEL::GetRowItemRect (int rowIndex, int colIndex)
{
    CRect  itemRect;
    ELSUPER::GetSubItemRect(rowIndex, colIndex, LVIR_LABEL, itemRect);

    // 修正col0 少2 个像素的问题??
    if (colIndex == 0)
    {
        itemRect.left -= 2;
    }

    return itemRect;
}

//
//-----------------------------------------------------------------------------
// 保证行被显示, 并按页滚动
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::EnsureRowVisibleScrollPage (int rowIndex)
{
    if (!IsRowItemVisible(rowIndex))
    {
        do
        {
            int  topRow = GetTopIndex();
            if (rowIndex > topRow)
            {
                ScrollPageDown();
            }
            else
            {
                ScrollPageUp();
            }
        } while (!IsRowItemVisible(rowIndex));
    }
}

//
//-----------------------------------------------------------------------------
// 判断行是否现在在显示范围
//-----------------------------------------------------------------------------
//
BOOL  CListCtrlEL::IsRowItemVisible (int rowIndex)
{
    int  toprow = ELSUPER::GetTopIndex();
    int  pageCount = ELSUPER::GetCountPerPage();
    if (rowIndex >= toprow && rowIndex < (toprow+pageCount))
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
// 设置被冻结的col, 被冻结的col不能被focus, 不能显示高亮颜色
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetFreezeColumnsCount (int count)
{
    if (count >= 0 && count < m_Header.GetItemCount())
    {
        m_nFreezeColumnsCount = count;
    }
    else
    {
        ASSERT_BREAK(FALSE);
    }
}
//
//-----------------------------------------------------------------------------
// 设置表格中的行是否能被设置焦点, 如果不能, 则相当于只读表格
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowFocusEnable (BOOL bEnable)
{
    m_bRowFocusEnable = bEnable;

    if (!bEnable)
    {
        m_bSubItemFocusEnable = -1;             // 连带关系
    }
}
//
//-----------------------------------------------------------------------------
// 设置表格中的一个item是否能被设置焦点并高亮
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetSubItemFocusEnable (BOOL bEnable)
{
    if (m_bRowFocusEnable)  // 连带关系
    {
        m_bSubItemFocusEnable = bEnable;
    }
    else
    {
        ASSERT_BREAK(FALSE);
    }
}
//
//-----------------------------------------------------------------------------
// 设置获得当前的焦点列坐标
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetFocusedColIndex (int colIndex)
{
    if (m_bSubItemFocusEnable)  // 当不允许选中子item时
    {
        m_focusedColIndex = colIndex;
    }
    else
    {
        ASSERT_BREAK(FALSE);
        m_focusedColIndex = -1;
    }
}
int  CListCtrlEL::GetFocusedColIndex ()
{
    return m_focusedColIndex;
}
//
//-----------------------------------------------------------------------------
// 设置获得当前的焦点行坐标
//  设置rowIndex=-1代表取消选中
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetFocusedSelectionRowIndex (int rowIndex)
{
    if (m_bRowFocusEnable)  //  当表格不允许选中行时
    {
        ASSERT_BREAK(IsValidRowIndex(rowIndex) && GetRecordRow(rowIndex)->IsFocusable());  // 当前行不允许选中
        ELSUPER::SetItemState(rowIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
        ELSUPER::SetSelectionMark(rowIndex);
    }
    else
    {
        ASSERT_BREAK(FALSE);
        ELSUPER::SetItemState(-1, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
        ELSUPER::SetSelectionMark(-1);
    }

	
}
int  CListCtrlEL::GetFocusedRowIndex ()
{
    return ELSUPER::GetNextItem(-1, LVNI_ALL | LVNI_FOCUSED);
}
int  CListCtrlEL::GetSelectionRowIndex ()
{
    return ELSUPER::GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
}
//
//-----------------------------------------------------------------------------
// 是否开启行自动编辑, 当选择焦点在某行, 但列焦点是-1时, 
// 按下OK键可以自动的让该行的第一个可编辑item进入编辑状态
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetAutoRowEditEnable (BOOL bEnable)
{
    m_bAutoRowEditEnable = bEnable;
}

//
//=============================================================================
// 导航
//=============================================================================
//
void  CListCtrlEL::KeyBeginEdit ()
{
    if (!m_bRowFocusEnable)
    {
        // 当前控件不允许编辑
        return;
    }

    //
    // 当行列焦点都是有效焦点时, 让该item进入编辑状态
    //
    if (m_bSubItemFocusEnable)
    {
        //
        // item可以被单独选中, 则让item进入编辑状态
        //
        int  focusRowIndex = GetFocusedRowIndex();
        int  focusColIndex = GetFocusedColIndex();
        if (IsValidRowIndex(focusRowIndex) && 
            IsValidColIndex(focusColIndex) && !IsFreezeColIndex(focusColIndex))
        {
            EL_RECORDITEM_ARGS  itemArgs(this, focusRowIndex, focusColIndex);
            if (itemArgs.pItem && itemArgs.pItem->IsAllowEdit(&itemArgs))
            {
                EnsureRowVisibleScrollRow(focusRowIndex);
                EditItem(&itemArgs);
                return;
            }
        }

    }


    //
    // 当开启行自动编辑时, 自动找到该行的第一个可编辑item, 
    // 让item进入编辑状态, 按优先级: checkbox自动翻转, 否则edit进入编辑, 否则list弹出
    //
    if (m_bAutoRowEditEnable)
    {
        //
        // 如果item没有单独焦点, 则寻找第一个可编辑item
        //
        int  curFocusedRow = GetFocusedRowIndex();
        int  focusColIndex = GetFocusedColIndex();
        if (IsValidRowIndex(curFocusedRow) && !IsValidColIndex(focusColIndex))
        {
            CELRecord*  record = GetRecordRow(curFocusedRow);
            ASSERT_BREAK(record);

            for (int i = 0; i < record->GetItemCount(); i++)
            {
                CELRecordItem*  item = record->GetItem(i);
                if (item->IsEditable())
                {
                    if (item->GetHasCheckbox())
                    {
                        // 如果是checkbox, 则让其翻转
                        item->SetChecked(!item->IsChecked());
                        RedrawRowItem(curFocusedRow, i);

                        // 通知父窗口有一个项被选中
                        SendMessageToParent(curFocusedRow, item, i, ELNM_ITEM_CHECKED);
                        return;
                    }
                    else
                    {
                        // item进入编辑状态
                        EL_RECORDITEM_ARGS  args(this, curFocusedRow, i);
                        EditItem(&args, FALSE);
                        return;
                    }
                }
            }
        }
    }
}
void  CListCtrlEL::KeyMoveDown ()
{
    // 只读表格, 直接滚动显示
    if (!m_bRowFocusEnable)
    {
        ScrollPageDown();
        return;
    }

    // 跳动焦点到下一行
    int  curFocusedRow = GetFocusedRowIndex();
    int  rowCount = GetRecordRowCount();
    for (int  nextFocusedRow = curFocusedRow + 1, loopcnt = 0; nextFocusedRow <= rowCount; 
        nextFocusedRow++, loopcnt++)
    {
        // 防止无限循环
        if (loopcnt >= rowCount)
        {
            EL_DEBUGLOG(L"CListCtrlEL::KeyMoveDown loop back\r\n");
            break;
        }
        // 如果超出列表范围, 自动翻转
        if (nextFocusedRow >= rowCount)
        {
            // fix除0错误, 原来是直接 % GetCount()
            nextFocusedRow = 0;
        }


        CELRecord*  nextRecordRow = GetRecordRow(nextFocusedRow);
        ASSERT_BREAK(nextRecordRow);

        // 跳过不可选中的行
        if (nextRecordRow->IsFocusable())
        {
            // 传递子item选中的坐标
            if (IsValidColIndex(m_focusedColIndex) && 
                !nextRecordRow->GetItem(m_focusedColIndex)->IsFocusable())
            {
                // 目标行的item不能接受focus
                m_focusedColIndex = -1;
            }
            SetFocusedSelectionRowIndex(nextFocusedRow);
            EnsureRowVisibleScrollPage(nextFocusedRow);
            return;
        }
    }

	

}
void  CListCtrlEL::KeyMoveUp ()
{
    // 只读表格, 直接滚动显示
    if (!m_bRowFocusEnable)
    {
        ScrollPageUp();
        return;
    }

    // 跳动焦点到上一行
    int  curFocusedRow = GetFocusedRowIndex();
    int  rowCount = GetRecordRowCount();
    for (int  nextFocusedRow = curFocusedRow - 1, loopcnt = 0; nextFocusedRow >= -1; 
        nextFocusedRow--, loopcnt++)
    {
        // 防止无限循环
        if (loopcnt >= rowCount)
        {
            EL_DEBUGLOG(L"CListCtrlEL::KeyMoveUp loop back\r\n");
            break;
        }
        // 如果超出列表范围, 自动翻转
        if (nextFocusedRow < 0)
        {
            nextFocusedRow = rowCount - 1;
        }

        CELRecord*  nextRecordRow = GetRecordRow(nextFocusedRow);
        ASSERT_BREAK(nextRecordRow);

        // 跳过不可选中的行
        if (nextRecordRow->IsFocusable())
        {
            // 传递子item选中的坐标
            if (IsValidColIndex(m_focusedColIndex) && 
                !nextRecordRow->GetItem(m_focusedColIndex)->IsFocusable())
            {
                // 目标行的item不能接受focus
                m_focusedColIndex = -1;
            }
            SetFocusedSelectionRowIndex(nextFocusedRow);
            EnsureRowVisibleScrollPage(nextFocusedRow);
            return;
        }
    }
}
void  CListCtrlEL::KeyMoveLeft ()
{
    // 不允许选择操作
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return;
    }

    int  curFocusedRow = GetFocusedRowIndex();
    if (IsValidRowIndex(curFocusedRow))
    {
        CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
        ASSERT_BREAK(curRecordRow);

        // 移动本行的列焦点
        int  nextFocusedColIndex = curRecordRow->FindRtoLFocusableItemIndex(
            max(m_focusedColIndex - 1, m_nFreezeColumnsCount));
        if (IsValidColIndex(nextFocusedColIndex) && !IsFreezeColIndex(nextFocusedColIndex))
        {
            m_focusedColIndex = nextFocusedColIndex;
            ELSUPER::RedrawItems(curFocusedRow, curFocusedRow);
        }
    }
}
void  CListCtrlEL::KeyMoveRight ()
{
    // 不允许选择操作
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return;
    }

    int  curFocusedRow = GetFocusedRowIndex();
    if (IsValidRowIndex(curFocusedRow))
    {
        CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
        ASSERT_BREAK(curRecordRow);

        // 移动本行的列焦点
        int  nextFocusedColIndex = curRecordRow->FindLtoRFocusableItemIndex(
            max(m_focusedColIndex + 1, m_nFreezeColumnsCount));
        if (IsValidColIndex(nextFocusedColIndex) && !IsFreezeColIndex(nextFocusedColIndex))
        {
            m_focusedColIndex = nextFocusedColIndex;
            ELSUPER::RedrawItems(curFocusedRow, curFocusedRow);
        }
    }
}
void  CListCtrlEL::ScrollPageDown ()
{
    // 保证EnsureVisible全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    int  tagRow = min(GetRecordRowCount()-1, ELSUPER::GetTopIndex() + (ELSUPER::GetCountPerPage()*2-1));

    // !!本语句可能产生直接拷贝屏幕移动的优化, 会导致控件不能及时重绘问题
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollPageUp ()
{
    // 保证EnsureVisible全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    int  tagRow = max(0, ELSUPER::GetTopIndex() - ELSUPER::GetCountPerPage());
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollRowDown ()
{
    // 保证EnsureVisible全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    int  tagRow = ELSUPER::GetTopIndex() + ELSUPER::GetCountPerPage();
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollRowUp ()
{
    // 保证EnsureVisible全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    int  tagRow = ELSUPER::GetTopIndex() - 1;
    ELSUPER::EnsureVisible(tagRow, FALSE);
}


BOOL  CListCtrlEL::IsMoveLeftToPageUp ()
{
    // 不允许选择操作
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return TRUE;
    }
    else
    {
        int  curFocusedRow = GetFocusedRowIndex();
        if (IsValidRowIndex(curFocusedRow))
        {
            CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
            ASSERT_BREAK(curRecordRow);

            // 移动本行的列焦点
            int  nextFocusedColIndex = curRecordRow->FindRtoLFocusableItemIndex(m_focusedColIndex - 1);
            if (IsValidColIndex(nextFocusedColIndex) && !IsFreezeColIndex(nextFocusedColIndex))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
    return TRUE;
}

BOOL  CListCtrlEL::IsMoveRightToPageDown ()
{
    // 不允许选择操作
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return TRUE;
    }
    else
    {
        int  curFocusedRow = GetFocusedRowIndex();
        if (IsValidRowIndex(curFocusedRow))
        {
            CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
            ASSERT_BREAK(curRecordRow);

            // 移动本行的列焦点
            int  nextFocusedColIndex = curRecordRow->FindLtoRFocusableItemIndex(
                max(m_focusedColIndex + 1, m_nFreezeColumnsCount));

            if (IsValidColIndex(nextFocusedColIndex) && !IsFreezeColIndex(nextFocusedColIndex))
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
    return TRUE;
}

void  CListCtrlEL::AutoSelPageDown ()
{
    Invalidate(FALSE);
    if (GetRecordRowCount() <= ELSUPER::GetCountPerPage())
    {
        return;
    }
    
    //用于恢复页内选中行
    int nFocusIndexInPage  = -1;
    if (m_bRowFocusEnable)
    {
        nFocusIndexInPage =  GetFocusedRowIndex()- ELSUPER::GetTopIndex();
    }

    int  tagRow = min(GetRecordRowCount()-1, ELSUPER::GetTopIndex() + (ELSUPER::GetCountPerPage()*2-1));
    // !!本语句可能产生直接拷贝屏幕移动的优化, 会导致控件不能及时重绘问题
    ELSUPER::EnsureVisible(tagRow, FALSE);
    
    if (m_bRowFocusEnable)
    {
        //恢复页内选中行
        if (nFocusIndexInPage >= 0)
        {
            int tagFocusRow =  ELSUPER::GetTopIndex() + nFocusIndexInPage;
            SetFocusedSelectionRowIndex(tagFocusRow);
        }
        else
        {
            SetFocusedSelectionRowIndex(ELSUPER::GetTopIndex());
        }
    }
}

void  CListCtrlEL::AutoSelPageUp ()
{
    Invalidate(FALSE);

    if (GetRecordRowCount() <= ELSUPER::GetCountPerPage())
    {
        return;
    }

    //用于恢复页内选中行
    int nFocusIndexInPage  = -1;
    if (m_bRowFocusEnable)
    {
        nFocusIndexInPage =  GetFocusedRowIndex()- ELSUPER::GetTopIndex();
    }

    int  tagRow = max(0, ELSUPER::GetTopIndex() - ELSUPER::GetCountPerPage());
    // !!本语句可能产生直接拷贝屏幕移动的优化, 会导致控件不能及时重绘问题
    ELSUPER::EnsureVisible(tagRow, FALSE);
    
    if (m_bRowFocusEnable)
    {
        //恢复页内选中行
        if (nFocusIndexInPage >= 0)
        {
            int tagFocusRow =  ELSUPER::GetTopIndex() + nFocusIndexInPage;
            SetFocusedSelectionRowIndex(tagFocusRow);
        }
        else
        {
            SetFocusedSelectionRowIndex(ELSUPER::GetTopIndex());
        }
    }
}

//
//=============================================================================
// 数据操作
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// id与index的相互映射, 
//  index通常会经常改变, 只是一个顺序号, 比如中间插入数据或删除数据的时候
//  但id是稳定的, 相当于一个对象指针, 唯一标识那一行对象
//  List-view controls internally track items by index. This can present problems because indexes can change during the control's existence.
//  You can use this macro to tag an item with an ID when you create the item. You use this ID to guarantee uniqueness during the existence of the list-view control. 
//-----------------------------------------------------------------------------
//
UINT  CListCtrlEL::MapIDToRowIndex (UINT  id)
{
    return ListView_MapIDToIndex(ELSUPER::GetSafeHwnd(), id);
}
UINT  CListCtrlEL::MapRowIndexToID (UINT  rowIndex)
{
    return ListView_MapIndexToID(ELSUPER::GetSafeHwnd(), rowIndex);
}

//
//-----------------------------------------------------------------------------
// 插入一列, 并指定列的各种样式属性
//  返回插入的真正colindex
//-----------------------------------------------------------------------------
//
int  CListCtrlEL::InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign, int colWidth, 
                   BOOL isEditable, ELAlignment headerAlgin, COLORREF colBkColor, int marginLeft, int marginRight)
{
    CELColumn*  colExAttr = new CELColumn;
    colExAttr->m_bEditable = isEditable;
    colExAttr->m_nAlignment = colAlign;
    colExAttr->m_nHeaderAlignment = headerAlgin;
    colExAttr->m_crBkColor = colBkColor;
    colExAttr->m_nMarginLeft = marginLeft;
    colExAttr->m_nMarginRight = marginRight;

    return ELSUPER::InsertColumn(colIndex, colTitle, LVCFMT_CENTER, colWidth, (int)colExAttr);
}

//
//-----------------------------------------------------------------------------
// 获得指定列的编辑可选项对象
//-----------------------------------------------------------------------------
//
CELColumn*  CListCtrlEL::GetColumnExAttr (int colIndex)
{
    return m_Header.GetColumn(colIndex);
}

//
//-----------------------------------------------------------------------------
// 插入一条记录对象, 返回记录所在的rowindex
//  如果要插入最后, 请在rowindex 填 GetRecordCount()
//  rowindex即是界面上的显示的一行的位置
//-----------------------------------------------------------------------------
//
int  CListCtrlEL::InsertRecordRow (int rowIndex, CELRecord* newRecord)
{
    int  index = ELSUPER::InsertItem(rowIndex, L"");

    ASSERT_BREAK(newRecord);
    ELSUPER::SetItemData(index, (DWORD_PTR)newRecord);

    return index;
}

//
//-----------------------------------------------------------------------------
// 简单接口: 插入一条记录, 并指定第一列的文本, 
//  返回记录所在的rowindex
//  这样插入的一行使用默认的Record对象, 无需用户自己新建或自己继承一个新的record对象
//-----------------------------------------------------------------------------
//
int  CListCtrlEL::InsertRecordRow (int rowIndex, LPCTSTR lpszItem)
{
    CELDefaultRecord*  record = new CELDefaultRecord(m_Header.GetItemCount());
    ASSERT_BREAK(record);

    ASSERT_BREAK(record->GetItemCount() > 0);
    record->GetItem(0)->SetCaption(lpszItem);

    return InsertRecordRow(rowIndex, record);
}

//
//-----------------------------------------------------------------------------
// 获得指定rowindex下的Record对象
//-----------------------------------------------------------------------------
//
CELRecord*  CListCtrlEL::GetRecordRow (int rowIndex)
{
    return (CELRecord*)ELSUPER::GetItemData(rowIndex);
}

//
//-----------------------------------------------------------------------------
// 简单接口: 设置一个Record的自定义数据
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRecordRowData (int rowIndex, DWORD_PTR data)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    record->SetData(data);
}

//
//-----------------------------------------------------------------------------
// 简单接口: 获取一个Record的自定义数据
//-----------------------------------------------------------------------------
//
DWORD_PTR  CListCtrlEL::GetRecordRowData (int rowIndex)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    return record->GetData();
}

//
//-----------------------------------------------------------------------------
// 简单接口: 设置一个record的某个item的显示字符
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowItemText (int rowIndex, int nSubItem, LPCTSTR lpszText)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    CELRecordItem*  item = record->GetItem(nSubItem);
    ASSERT_BREAK(item);

    ASSERT_BREAK(lpszText);
    item->SetCaption(lpszText);

    // 刷新窗口, 用SetItemText的方式,
    // 如果是第一列的格子, 会有左边两个像素不能更新的问题 bug2013.07.03
    ELSUPER::SetItemText(rowIndex, nSubItem, L"");
}

//
//-----------------------------------------------------------------------------
// 简单接口: 获得一个record的某个item的显示字符
//-----------------------------------------------------------------------------
//
CString  CListCtrlEL::GetRowItemText (int rowIndex, int nSubItem)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    CELRecordItem*  item = record->GetItem(nSubItem);
    ASSERT_BREAK(item);

    return item->GetCaption();
}

//
//-----------------------------------------------------------------------------
// 简单接口: 设置一个record的某个item的自定义数据
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowItemData (int rowIndex, int nSubItem, DWORD_PTR data)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    CELRecordItem*  item = record->GetItem(nSubItem);
    ASSERT_BREAK(item);

    item->SetItemData(data);
}

//
//-----------------------------------------------------------------------------
// 简单接口: 获得一个record的某个item的自定义数据
//-----------------------------------------------------------------------------
//
DWORD_PTR  CListCtrlEL::GetRowItemData (int rowIndex, int nSubItem)
{
    CELRecord*  record = GetRecordRow(rowIndex);
    ASSERT_BREAK(record);

    CELRecordItem*  item = record->GetItem(nSubItem);
    ASSERT_BREAK(item);

    return item->GetItemData();
}

//
//-----------------------------------------------------------------------------
// 获得所有记录的数目
//-----------------------------------------------------------------------------
//
int  CListCtrlEL::GetRecordRowCount ()
{
    return ELSUPER::GetItemCount();
}

//
//-----------------------------------------------------------------------------
// 删除一条记录
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::DeleteRecordRow (int rowIndex)
{
    // 保证全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    // 释放关联的record
    ReleaseRecordRowExAttr(rowIndex);


    ELSUPER::DeleteItem(rowIndex);
}

//
//-----------------------------------------------------------------------------
// 删除所有记录
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::DeleteAllRecordRow ()
{
    // 保证全部重绘, 不会产生直接拷贝屏幕移动的问题, 比如全部Gridline需要重绘
    Invalidate(FALSE);

    // 释放关联的record
    ReleaseAllRecordRowExAttr();


    ELSUPER::DeleteAllItems();
}



//
//=============================================================================
// 消息响应
//=============================================================================
//
BEGIN_MESSAGE_MAP(CListCtrlEL, ELSUPER)
    ON_WM_MEASUREITEM()
    ON_WM_MEASUREITEM_REFLECT()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_CHAR()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

//
//-----------------------------------------------------------------------------
// 截获处理一些特殊按键消息, 比如Enter, ESC等
//-----------------------------------------------------------------------------
//
BOOL  CListCtrlEL::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->hwnd == GetSafeHwnd())
    {
        if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP || pMsg->message == WM_CHAR)
        {
            switch (pMsg->wParam)
            {
            case VK_RETURN:
                {
                    if (ELVK_OK == VK_RETURN)
                    {
                        ::TranslateMessage(pMsg); 
                        ::DispatchMessage(pMsg);
                        return TRUE;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    return ELSUPER::PreTranslateMessage(pMsg);
}






void CListCtrlEL::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    ELSUPER::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CListCtrlEL::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    if (m_nRowHeight > 0)
    {
        lpMeasureItemStruct->itemHeight = m_nRowHeight;
    }
}


void  CListCtrlEL::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    EL_DEBUGLOG(L"CListCtrlEL: OnKeyDown 0x%x\n", nChar);

    // 处理焦点及选中的键盘操作功能
    switch (nChar)
    {
    case VK_UP:
        {
            KeyMoveUp();
        }
        break;
    case VK_DOWN:
        {
            KeyMoveDown();
        }
        break;
    case VK_LEFT:
        {   
            if (IsMoveLeftToPageUp())
            {
                AutoSelPageUp();
            }
            else
            {
                KeyMoveLeft();
            }
        }
        break;
    case VK_RIGHT:
        {
            if (IsMoveRightToPageDown())
            {
                AutoSelPageDown();
            }
            else
            {
                KeyMoveRight();
            }
        }
        break;
    case ELVK_OK:
        break;
    default:
        ELSUPER::OnKeyDown(nChar, nRepCnt, nFlags);
        break;
    }
}
void  CListCtrlEL::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    EL_DEBUGLOG(L"CListCtrlEL: OnKeyUp 0x%x\n", nChar);

    ELSUPER::OnKeyUp(nChar, nRepCnt, nFlags);
}
void  CListCtrlEL::OnChar (UINT nChar, UINT nRepCntr, UINT nFlags)
{
    EL_DEBUGLOG(L"CListCtrlEL: OnChar 0x%x\n", nChar);

    // 处理特殊按键
    if (nChar == ELVK_OK)
    {
        // 先交给父窗口, 看父窗口需不需要处理该按键
        if (SendNotifyMessageToOwner(ELNM_OKCLICK, NULL))
        {
            // 父窗口已经处理了该按键
            return;
        }
    }

    // 给子item发送char消息, 让item自己处理按键事件
    if (m_bRowFocusEnable && m_bSubItemFocusEnable)
    {    
        int  focusRowIndex = GetFocusedRowIndex();
        int  focusColIndex = GetFocusedColIndex();
        if (IsValidRowIndex(focusRowIndex) && 
            IsValidColIndex(focusColIndex) && !IsFreezeColIndex(focusColIndex))
        {
            EL_RECORDITEM_ARGS itemArgs(this, focusRowIndex, focusColIndex);
            if (itemArgs.pItem && itemArgs.pItem->OnChar(&itemArgs, nChar))
            {
                return;
            }
        }
    }

    // 都没有处理, 则按我们自己默认处理
    if (nChar == ELVK_OK)
    {
        KeyBeginEdit();
        return;
    }

    ELSUPER::OnChar(nChar, nRepCntr, nFlags);
}
void  CListCtrlEL::OnLButtonDown (UINT nFlags, CPoint point)
{
    LVHITTESTINFO  lvhit;
    lvhit.pt = point;
    ELSUPER::SubItemHitTest(&lvhit);
    if (lvhit.flags & LVHT_ONITEMLABEL)
    {
        // 但仍然能让控件处于焦点
        ELSUPER::SetFocus();

        // 当禁用焦点时, 鼠标点击不能选中item, 
        if (!m_bRowFocusEnable)
        {
            return;
        }

        // 当对应的row是禁用状态时, 鼠标点击不能选中item
        if (IsValidRowIndex(lvhit.iItem) && !GetRecordRow(lvhit.iItem)->IsFocusable())
        {
            return;
        }
    }


    ELSUPER::OnLButtonDown(nFlags, point);
}
void  CListCtrlEL::OnLButtonUp (UINT nFlags, CPoint point)
{
    ELSUPER::OnLButtonUp(nFlags, point);
}
void  CListCtrlEL::OnLButtonDblClk (UINT nFlags, CPoint ptDblClick)
{
    ELSUPER::OnLButtonDblClk(nFlags, ptDblClick);
}


void  CListCtrlEL::OnPaint ()
{
#ifdef EL_DOUBLEBUFFER

    CPaintDC                dc(this);           // device context for painting

    CRect                   clientRect;
    CDC                     MenDC;

    GetClientRect(&clientRect);
    MenDC.CreateCompatibleDC(&dc);

    CreatePaintMemBitmap(&dc, clientRect.Width(), clientRect.Height());
    CBitmap*  oldbmp = MenDC.SelectObject(&m_paintMemMap);

    {
        FillBkgnd(&MenDC);
        FillGridline(&MenDC);

        // 这一句是调用默认的OnPaint(),把图形画在内存DC表上
        DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);
        

        //输出
        dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &MenDC, 0, 0, SRCCOPY);

	}

    MenDC.SelectObject(oldbmp);

    MenDC.DeleteDC();

#else  // EL_DOUBLEBUFFER

    CPaintDC dc(this);

    FillBkgnd(&dc);
    FillGridline(&dc);
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
    
    

#endif  // EL_DOUBLEBUFFER

}


BOOL  CListCtrlEL::OnEraseBkgnd (CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    
    return TRUE;
}


void  CListCtrlEL::DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    COLORREF  defTextColor = ELSUPER::GetTextColor();
    COLORREF  defBkColor = ELSUPER::GetBkColor();
    CDC*  pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CELRecord*  record = GetRecordRow(lpDrawItemStruct->itemID);
    HWND  curFocusedWnd = ::GetFocus();
    DWORD  dwStyle = GetStyle();
    ASSERT_BREAK(record);
    
    //
    // 遍历每个col, 绘制
    //
    LV_COLUMN  lvc;
    lvc.mask = LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
    for (int nCol = 0; ELSUPER::GetColumn(nCol, &lvc); nCol++)
    {
        CELColumn*  colExAttr = (CELColumn*)lvc.iSubItem;
        ASSERT_BREAK(colExAttr);

        CELRecordItem*  curItem = record->GetItem(nCol);
        ASSERT_BREAK(curItem);

        CRect rcItem;   
        if (!ELSUPER::GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_LABEL, rcItem))  
        {
            continue;
        }

        if (nCol == 0)
        {
            rcItem.left -= 2;                   // why?
        }

        // 获得文字颜色和背景颜色, 如果item有自己独特的颜色
        COLORREF  textColor = (curItem->m_clrText == EL_COLOR_DEFAULT)? defTextColor: curItem->m_clrText;
        COLORREF  bkColor = (curItem->m_clrBackground == EL_COLOR_DEFAULT)? defBkColor: curItem->m_clrBackground;


        // 绘制选择区高亮
        if (m_bRowFocusEnable && 
            (nCol >= m_nFreezeColumnsCount || m_bFreezeSelectionVisible) && 
            (lpDrawItemStruct->itemState & (ODS_SELECTED|ODS_FOCUS)) && 
            (curFocusedWnd == m_hWnd || dwStyle & LVS_SHOWSELALWAYS))
        {
            CRect  selItemRect = rcItem;

            if (m_bSubItemFocusEnable && m_focusedColIndex == nCol)
            {
                if (curFocusedWnd == m_hWnd)
                {
                    // item 焦点高亮
                    pDC->FillSolidRect(&selItemRect, m_focusedSubItemHighlightColor);
                }
                else
                {
                    // 控件丢失焦点时, 但LVS_SHOWSELALWAYS
                    pDC->FillSolidRect(&selItemRect, m_selectionRowLowlightColor);
                }

                // 高亮字体
                pDC->SetTextColor(m_textHighlightColor);
            }
            else if (m_bRowSelectionVisible)
            {
                if (curFocusedWnd == m_hWnd)
                {
                    // 行 选择高亮
                    pDC->FillSolidRect(&selItemRect, m_selectionRowHighlightColor);
                }
                else
                {
                    // 控件丢失焦点时, 但LVS_SHOWSELALWAYS
                    pDC->FillSolidRect(&selItemRect, m_selectionRowLowlightColor);
                }

                // 高亮字体
                pDC->SetTextColor(m_textHighlightColor);
            }
			
            else
            {
                // 重新渲染item独特的背景色
                if (bkColor != defBkColor)
                {
                    pDC->FillSolidRect(rcItem, bkColor);
                }

                // 普通字体
                pDC->SetTextColor(textColor);
            }
		}
        else
        {
            // 重新渲染item独特的背景色
            if (bkColor != defBkColor)
            {
                pDC->FillSolidRect(rcItem, bkColor);
            }

            // 普通字体
            pDC->SetTextColor(textColor);
        }
		//if(!curItem->m_pRecord->IsFocusable())
		//{
		//	// 重新渲染item独特的背景色
		//	if (bkColor != defBkColor)
		//	{
		//		pDC->FillSolidRect(rcItem, bkColor);
		//	}

		//	// 普通字体
		//	pDC->SetTextColor(m_textHighlightColor);
		//}
        EL_RECORDITEM_DRAWARGS  drawArgs;
        drawArgs.pControl = this;
        drawArgs.rowIndex = lpDrawItemStruct->itemID;
        drawArgs.colIndex = nCol;
        drawArgs.rcItem = rcItem;
        drawArgs.pItem = curItem;
        drawArgs.pDC = pDC;
        drawArgs.nTextAlign = colExAttr->m_nAlignment;
        drawArgs.isFocusedRow = (lpDrawItemStruct->itemState & ODS_FOCUS)? TRUE: FALSE;
        drawArgs.isSelectionRow = (lpDrawItemStruct->itemState & ODS_SELECTED)? TRUE: FALSE;
        drawArgs.isFocusedCol = (m_focusedColIndex == nCol)? TRUE: FALSE;

        drawArgs.pItem->Draw(&drawArgs);
    }

}


//
//-----------------------------------------------------------------------------
// 窗口退出, 释放所有对象, 特别是我们自己创建的外挂对象
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::OnDestroy ()
{
    // 当前正在编辑状态, 不应该析构本对象, 
    // 肯定是某个地方处理流程有问题, 这是异常情况, bug2013.06.19
    ASSERT_BREAK(m_pActiveItem == NULL);

    // 释放所有外挂对象, 根据测试, 父窗口OnDestroy先执行才到子窗口, 
    // 所以可以在父窗口去统一销毁
    ReleaseAllExAttr();
}


//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 释放所有的外挂属性对象, 包括每个col, 每行record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseAllExAttr ()
{
    //
    // 释放所有col的外挂属性, 
    //
    int  colCount = m_Header.GetItemCount();
    for (int i = 0; i < colCount; i++)
    {
        // 获得外挂对象
        HDITEM  hdrinfo;
        hdrinfo.mask = HDI_LPARAM;
        m_Header.GetItem(i, &hdrinfo);

        CELColumn*  column = (CELColumn*)hdrinfo.lParam;
        ASSERT_BREAK(column);
        delete column;

        // 重设外挂对象
        hdrinfo.lParam = 0;
        m_Header.SetItem(i, &hdrinfo);
    }


    ReleaseAllRecordRowExAttr();
}

//
//-----------------------------------------------------------------------------
// 释放所有的外挂属性对象: 每行record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseAllRecordRowExAttr ()
{
    //
    // 释放所有record的外挂属性
    //
    int  recordCount = ELSUPER::GetItemCount();
    for (int i = 0; i < recordCount; i++)
    {
        ReleaseRecordRowExAttr(i);
    }
}

//
//-----------------------------------------------------------------------------
// 释放所有的外挂属性对象: 一行record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseRecordRowExAttr (int rowIndex)
{
    CELRecord*  record = (CELRecord*)ELSUPER::GetItemData(rowIndex);
    ASSERT_BREAK(record);

    // 保证当前正在编辑的item不能被删除 bug2013.06.19
    if (m_pActiveItem)
    {
        ASSERT_BREAK(m_pActiveItem->m_pRecord && m_pActiveItem->m_pRecord != record);
    }

    delete record;

    ELSUPER::SetItemData(rowIndex, 0);
}

//
//-----------------------------------------------------------------------------
// 填充背景位图
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::FillBkgnd (CDC* pDC)
{
    COLORREF  bgColor = GetBkColor();

    // 绘制整个表格的背景
    CRect  clientRect;
    GetClientRect(&clientRect);

    // 填充每列的特殊背景色
    int  colCount = m_Header.GetItemCount();
    CRect  colRect = clientRect;
    for (int i = 0; i < colCount; i++)
    {
        // 计算列的位置
        int  colWidth = ELSUPER::GetColumnWidth(i);
        colRect.right = colRect.left + colWidth;

        // 按需要填充
        CELColumn*  colExAttr = GetColumnExAttr(i);
        ASSERT_BREAK(colExAttr);
        if (colExAttr->m_crBkColor == EL_COLOR_DEFAULT)
        {
            pDC->FillSolidRect(colRect, bgColor);
        }
        else
        {
            pDC->FillSolidRect(colRect, colExAttr->m_crBkColor);
        }

        // next
        colRect.left = colRect.right;
    }

    // 填充最后的剩余空间
    if (clientRect.right > colRect.left)
    {
        colRect.right = clientRect.right;
        pDC->FillSolidRect(colRect, bgColor);
    }
}

//
//-----------------------------------------------------------------------------
// 画各种边线, 网格线
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::FillGridline (CDC* pDC)
{
    COLORREF  gridColor = m_Header.m_hdrGridlineColor;
    CRect  clientRect;
    GetClientRect(&clientRect);

    int  centerX = (clientRect.left + clientRect.right) / 2;  // 计算中点

    // 绘制垂直网格线
    if (m_hasVGridline)
    {
        int  colCount = m_Header.GetItemCount();
        int  curColPos = 0;

        // 是否绘制渐变的网格线
        if (m_gradientVGridline)
        {
            // 绘制渐变线
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);
                curColPos += colWidth;

                CRect  gridRect(curColPos, clientRect.top+m_Header.m_hdrHeight, curColPos+1, clientRect.bottom);
                CommonEL::DrawGradient(pDC, gridRect, gridColor, GetBkColor(), GRADIENT_FILL_RECT_V);
            }

            // 绘制水平封底线, 分两段绘制
            CRect  gridRectLeft(clientRect.left, clientRect.bottom-1, centerX, clientRect.bottom);
            CRect  gridRectRight(centerX, clientRect.bottom-1, clientRect.right, clientRect.bottom);
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());

            // 绘制水平封顶线, 将底线上移
            gridRectLeft.OffsetRect(0, -(clientRect.Height() - m_Header.m_hdrHeight-1));
            gridRectRight.OffsetRect(0, -(clientRect.Height() - m_Header.m_hdrHeight-1));
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());

        }
        else
        {
            // 绘制实线
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);

                curColPos += colWidth;
                pDC->FillSolidRect(curColPos, clientRect.top, 1, clientRect.Height(), gridColor);
            }

            // 绘制水平封底线
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);

            // 绘制水平封顶线
            pDC->FillSolidRect(clientRect.left, clientRect.top+m_Header.m_hdrHeight, clientRect.Width(), 1, gridColor);
        }
    }

    // 绘制水平网格线
    if (m_hasHGridline)
    {
#if !SU_FEI	
		pDC->FillSolidRect(clientRect.left, clientRect.top + m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
#endif
        // 每页的总数
        int  toprow = ELSUPER::GetTopIndex();
        int  chunk_height = ELSUPER::GetCountPerPage();
        int  itemCount = GetItemCount();
        int  hlineCount = min(chunk_height, itemCount);

        // 是否绘制渐变的网格线
        if (m_gradientHGridline)
        {
            // 绘制渐变线
            for (int i = 1; i <= hlineCount; i++)
            {
                POINT  rowPoint;
                ELSUPER::GetItemPosition(toprow+i, &rowPoint);

                // 分两段绘制
                CRect  gridRectLeft(clientRect.left, rowPoint.y, centerX, rowPoint.y+1);
                CRect  gridRectRight(centerX, rowPoint.y, clientRect.right, rowPoint.y+1);
                CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
                CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
            }
        }
        else
        {
            // 绘制实线
            for (int i = 1; i <= hlineCount; i++)
            {
                POINT  rowPoint;
                ELSUPER::GetItemPosition(toprow+i, &rowPoint);

                pDC->FillSolidRect(clientRect.left, rowPoint.y, clientRect.Width(), 1, gridColor);
            }
        }
    }
}

//
//-----------------------------------------------------------------------------
// 向owner窗口发送通知消息, 比如通知父窗口按键被按下, 开始编辑状态, item值被改变等
//  返回值非0代表父窗口是否已经处理了该消息
//-----------------------------------------------------------------------------
//
LRESULT  CListCtrlEL::SendNotifyMessageToOwner (UINT nMessage, NMHDR* pNMHDR)
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
LRESULT  CListCtrlEL::SendMessageToParent (int rowIndex, CELRecordItem* pItem, int colIndex, UINT nMessage)
{
    ELNM_RECORDITEM_ARGS  args;
    args.rowIndex = rowIndex;
    args.colIndex = colIndex;
    args.pItem = pItem;
    return SendNotifyMessageToOwner(nMessage, (NMHDR*)&args);
}

//
//-----------------------------------------------------------------------------
// 隐藏指定的滚动条
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::HideScrollBars ()
{
    RECT ierect;
    int cxvs;
    // 		GetClientRect(&ierect); //Get client width and height
    // 		RECT windowrect;
    //修改为使用WindowRect,原版本作者使用的是ClientRect,但是滚动条虽然隐藏了，但是滚动条位置还是占据位置。只是变为白色了
    GetWindowRect(&ierect);
    ScreenToClient(&ierect);

    cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT

    //Here we set the position of the window to the clientrect + the size of the scrollbars
    SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom, SWP_NOMOVE | SWP_NOZORDER);

    SetListCtrlRgn();
}

//
//-----------------------------------------------------------------------------
// 设置windows显示操作的区域, 隐藏系统滚动条
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetListCtrlRgn ()
{
    RECT ierect;
    GetWindowRect(&ierect);
    ScreenToClient(&ierect);

    int  cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
    ierect.right -= cxvs;

    // 创建window区域, 去除滚动条
    if (m_wndRgnNoScroll.GetSafeHandle())
    {
        m_wndRgnNoScroll.DeleteObject();
    }
    m_wndRgnNoScroll.CreateRectRgn(ierect.left, ierect.top, ierect.right, ierect.bottom);
    

    //After the range has been made we add it...
    //     m_SkinVerticleScrollbar.UpdateThumbPosition();
    //     m_SkinHorizontalScrollbar.UpdateThumbPosition();
    SetWindowRgn (m_wndRgnNoScroll, TRUE);
}

//
//-----------------------------------------------------------------------------
// 创建内存位图,
//  有一定的优化: 如果位图已经存在, 则不用重复创建
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::CreatePaintMemBitmap (CDC* dc, int width, int height)
{
    ASSERT_BREAK(dc);

    if (m_paintMemMap.GetSafeHandle())
    {
        BITMAP  bmpInfo;
        m_paintMemMap.GetBitmap(&bmpInfo);
        if (bmpInfo.bmWidth == width && bmpInfo.bmHeight == height)
        {
            // 当位图已经存在, 并且宽高与参数一致, 则不用重复创建
            return;
        }
        else
        {
            // 宽高不一致, 删除原来的位图, 后面重新创建
            m_paintMemMap.DeleteObject();
        }
    }

    EL_DEBUGLOG(L"CListCtrlEL::CreatePaintMemBitmap\n");
    m_paintMemMap.CreateCompatibleBitmap(dc, width, height);
}
