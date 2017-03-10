#include "stdafx.h"
#include "ListCtrlEL.h"
#include "../Common/CommonEL.h"


IMPLEMENT_DYNAMIC(CListCtrlEL, ELSUPER)




//
//=============================================================================
// �����ؼ�
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
    // ��ǰ���ڱ༭״̬, ��Ӧ������������, 
    // �϶���ĳ���ط���������������, �����쳣��� bug2013.06.19
    ASSERT_BREAK(m_pActiveItem == NULL);

    delete m_pInplaceEdit;
    delete m_pInplaceList;
}



void CListCtrlEL::PreSubclassWindow()
{
    // TODO: �ڴ����ר�ô����/����û���
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
// ��ʽ����
//=============================================================================
//


//
//-----------------------------------------------------------------------------
// ���ñ��ÿ�еĸ߶�
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowHeight (int nHeight) //�����и�
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
// �ؼ�����
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
// ���ؼ��Ƿ����ڴ���item�༭״̬
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
// ʹ��item����༭״̬
//  pItemArgs �� NULL ����ȡ����ǰ�༭
//  bAutoFocus �����Ƿ񽹵��Զ���λ��Ŀ��༭item
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::EditItem (EL_RECORDITEM_ARGS* pItemArgs, BOOL bAutoFocus)
{
    CELRecordItem*  pItem = pItemArgs ? pItemArgs->pItem : NULL;

    // ȡ��֮ǰ�ı༭�ؼ�
    if (m_pActiveItem != NULL)
    {
        m_pActiveItem->OnEndEdit(this);
        m_pActiveItem = NULL;
    }


    // ��item����༭ģʽ
    if (pItemArgs && pItem && pItem->IsAllowEdit(pItemArgs) && 
        IsValidRowIndex(pItemArgs->rowIndex) && 
        IsValidColIndex(pItemArgs->colIndex))
    {
        // ������ؼ�û�ڽ���, �����ý���
        if (!HasFocus())
        {
            SetFocus();
        }
        
        // �Զ���λ����
        if (bAutoFocus)
        {
            SetFocusedSelectionRowIndex(pItemArgs->rowIndex);
            SetFocusedColIndex(pItemArgs->colIndex);
        }

        // ��֤���ڱ༭�пɼ�
        EnsureRowVisibleScrollRow(pItemArgs->rowIndex);
        

        // ��Ϊ֮ǰ�����й���, ���µ���item��rect
        pItemArgs->rcItem = GetRowItemRect(pItemArgs->rowIndex, pItemArgs->colIndex);

        // ����༭ģʽ, ��������m_pActiveItem
        m_pActiveItem = pItem;
        pItem->OnBeginEdit(pItemArgs);
        if (!IsEditMode())  // ??  Need to actually cancel the edit - old version (10) did not have this call
        {
            EditItem(NULL);
        }

        // ʹ�ñ༭�ؼ��е��ı���ѡ��
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
// ���ָ��Item�ڿͻ����ľ��δ�С
//-----------------------------------------------------------------------------
//
CRect  CListCtrlEL::GetRowItemRect (int rowIndex, int colIndex)
{
    CRect  itemRect;
    ELSUPER::GetSubItemRect(rowIndex, colIndex, LVIR_LABEL, itemRect);

    // ����col0 ��2 �����ص�����??
    if (colIndex == 0)
    {
        itemRect.left -= 2;
    }

    return itemRect;
}

//
//-----------------------------------------------------------------------------
// ��֤�б���ʾ, ����ҳ����
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
// �ж����Ƿ���������ʾ��Χ
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
// ���ñ������col, �������col���ܱ�focus, ������ʾ������ɫ
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
// ���ñ���е����Ƿ��ܱ����ý���, �������, ���൱��ֻ�����
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetRowFocusEnable (BOOL bEnable)
{
    m_bRowFocusEnable = bEnable;

    if (!bEnable)
    {
        m_bSubItemFocusEnable = -1;             // ������ϵ
    }
}
//
//-----------------------------------------------------------------------------
// ���ñ���е�һ��item�Ƿ��ܱ����ý��㲢����
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetSubItemFocusEnable (BOOL bEnable)
{
    if (m_bRowFocusEnable)  // ������ϵ
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
// ���û�õ�ǰ�Ľ���������
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetFocusedColIndex (int colIndex)
{
    if (m_bSubItemFocusEnable)  // ��������ѡ����itemʱ
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
// ���û�õ�ǰ�Ľ���������
//  ����rowIndex=-1����ȡ��ѡ��
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetFocusedSelectionRowIndex (int rowIndex)
{
    if (m_bRowFocusEnable)  //  ���������ѡ����ʱ
    {
        ASSERT_BREAK(IsValidRowIndex(rowIndex) && GetRecordRow(rowIndex)->IsFocusable());  // ��ǰ�в�����ѡ��
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
// �Ƿ������Զ��༭, ��ѡ�񽹵���ĳ��, ���н�����-1ʱ, 
// ����OK�������Զ����ø��еĵ�һ���ɱ༭item����༭״̬
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetAutoRowEditEnable (BOOL bEnable)
{
    m_bAutoRowEditEnable = bEnable;
}

//
//=============================================================================
// ����
//=============================================================================
//
void  CListCtrlEL::KeyBeginEdit ()
{
    if (!m_bRowFocusEnable)
    {
        // ��ǰ�ؼ�������༭
        return;
    }

    //
    // �����н��㶼����Ч����ʱ, �ø�item����༭״̬
    //
    if (m_bSubItemFocusEnable)
    {
        //
        // item���Ա�����ѡ��, ����item����༭״̬
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
    // ���������Զ��༭ʱ, �Զ��ҵ����еĵ�һ���ɱ༭item, 
    // ��item����༭״̬, �����ȼ�: checkbox�Զ���ת, ����edit����༭, ����list����
    //
    if (m_bAutoRowEditEnable)
    {
        //
        // ���itemû�е�������, ��Ѱ�ҵ�һ���ɱ༭item
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
                        // �����checkbox, �����䷭ת
                        item->SetChecked(!item->IsChecked());
                        RedrawRowItem(curFocusedRow, i);

                        // ֪ͨ��������һ���ѡ��
                        SendMessageToParent(curFocusedRow, item, i, ELNM_ITEM_CHECKED);
                        return;
                    }
                    else
                    {
                        // item����༭״̬
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
    // ֻ�����, ֱ�ӹ�����ʾ
    if (!m_bRowFocusEnable)
    {
        ScrollPageDown();
        return;
    }

    // �������㵽��һ��
    int  curFocusedRow = GetFocusedRowIndex();
    int  rowCount = GetRecordRowCount();
    for (int  nextFocusedRow = curFocusedRow + 1, loopcnt = 0; nextFocusedRow <= rowCount; 
        nextFocusedRow++, loopcnt++)
    {
        // ��ֹ����ѭ��
        if (loopcnt >= rowCount)
        {
            EL_DEBUGLOG(L"CListCtrlEL::KeyMoveDown loop back\r\n");
            break;
        }
        // ��������б�Χ, �Զ���ת
        if (nextFocusedRow >= rowCount)
        {
            // fix��0����, ԭ����ֱ�� % GetCount()
            nextFocusedRow = 0;
        }


        CELRecord*  nextRecordRow = GetRecordRow(nextFocusedRow);
        ASSERT_BREAK(nextRecordRow);

        // ��������ѡ�е���
        if (nextRecordRow->IsFocusable())
        {
            // ������itemѡ�е�����
            if (IsValidColIndex(m_focusedColIndex) && 
                !nextRecordRow->GetItem(m_focusedColIndex)->IsFocusable())
            {
                // Ŀ���е�item���ܽ���focus
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
    // ֻ�����, ֱ�ӹ�����ʾ
    if (!m_bRowFocusEnable)
    {
        ScrollPageUp();
        return;
    }

    // �������㵽��һ��
    int  curFocusedRow = GetFocusedRowIndex();
    int  rowCount = GetRecordRowCount();
    for (int  nextFocusedRow = curFocusedRow - 1, loopcnt = 0; nextFocusedRow >= -1; 
        nextFocusedRow--, loopcnt++)
    {
        // ��ֹ����ѭ��
        if (loopcnt >= rowCount)
        {
            EL_DEBUGLOG(L"CListCtrlEL::KeyMoveUp loop back\r\n");
            break;
        }
        // ��������б�Χ, �Զ���ת
        if (nextFocusedRow < 0)
        {
            nextFocusedRow = rowCount - 1;
        }

        CELRecord*  nextRecordRow = GetRecordRow(nextFocusedRow);
        ASSERT_BREAK(nextRecordRow);

        // ��������ѡ�е���
        if (nextRecordRow->IsFocusable())
        {
            // ������itemѡ�е�����
            if (IsValidColIndex(m_focusedColIndex) && 
                !nextRecordRow->GetItem(m_focusedColIndex)->IsFocusable())
            {
                // Ŀ���е�item���ܽ���focus
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
    // ������ѡ�����
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return;
    }

    int  curFocusedRow = GetFocusedRowIndex();
    if (IsValidRowIndex(curFocusedRow))
    {
        CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
        ASSERT_BREAK(curRecordRow);

        // �ƶ����е��н���
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
    // ������ѡ�����
    if (!m_bRowFocusEnable || !m_bSubItemFocusEnable)
    {
        return;
    }

    int  curFocusedRow = GetFocusedRowIndex();
    if (IsValidRowIndex(curFocusedRow))
    {
        CELRecord*  curRecordRow = GetRecordRow(curFocusedRow);
        ASSERT_BREAK(curRecordRow);

        // �ƶ����е��н���
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
    // ��֤EnsureVisibleȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    int  tagRow = min(GetRecordRowCount()-1, ELSUPER::GetTopIndex() + (ELSUPER::GetCountPerPage()*2-1));

    // !!�������ܲ���ֱ�ӿ�����Ļ�ƶ����Ż�, �ᵼ�¿ؼ����ܼ�ʱ�ػ�����
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollPageUp ()
{
    // ��֤EnsureVisibleȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    int  tagRow = max(0, ELSUPER::GetTopIndex() - ELSUPER::GetCountPerPage());
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollRowDown ()
{
    // ��֤EnsureVisibleȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    int  tagRow = ELSUPER::GetTopIndex() + ELSUPER::GetCountPerPage();
    ELSUPER::EnsureVisible(tagRow, FALSE);
}
void  CListCtrlEL::ScrollRowUp ()
{
    // ��֤EnsureVisibleȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    int  tagRow = ELSUPER::GetTopIndex() - 1;
    ELSUPER::EnsureVisible(tagRow, FALSE);
}


BOOL  CListCtrlEL::IsMoveLeftToPageUp ()
{
    // ������ѡ�����
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

            // �ƶ����е��н���
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
    // ������ѡ�����
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

            // �ƶ����е��н���
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
    
    //���ڻָ�ҳ��ѡ����
    int nFocusIndexInPage  = -1;
    if (m_bRowFocusEnable)
    {
        nFocusIndexInPage =  GetFocusedRowIndex()- ELSUPER::GetTopIndex();
    }

    int  tagRow = min(GetRecordRowCount()-1, ELSUPER::GetTopIndex() + (ELSUPER::GetCountPerPage()*2-1));
    // !!�������ܲ���ֱ�ӿ�����Ļ�ƶ����Ż�, �ᵼ�¿ؼ����ܼ�ʱ�ػ�����
    ELSUPER::EnsureVisible(tagRow, FALSE);
    
    if (m_bRowFocusEnable)
    {
        //�ָ�ҳ��ѡ����
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

    //���ڻָ�ҳ��ѡ����
    int nFocusIndexInPage  = -1;
    if (m_bRowFocusEnable)
    {
        nFocusIndexInPage =  GetFocusedRowIndex()- ELSUPER::GetTopIndex();
    }

    int  tagRow = max(0, ELSUPER::GetTopIndex() - ELSUPER::GetCountPerPage());
    // !!�������ܲ���ֱ�ӿ�����Ļ�ƶ����Ż�, �ᵼ�¿ؼ����ܼ�ʱ�ػ�����
    ELSUPER::EnsureVisible(tagRow, FALSE);
    
    if (m_bRowFocusEnable)
    {
        //�ָ�ҳ��ѡ����
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
// ���ݲ���
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// id��index���໥ӳ��, 
//  indexͨ���ᾭ���ı�, ֻ��һ��˳���, �����м�������ݻ�ɾ�����ݵ�ʱ��
//  ��id���ȶ���, �൱��һ������ָ��, Ψһ��ʶ��һ�ж���
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
// ����һ��, ��ָ���еĸ�����ʽ����
//  ���ز��������colindex
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
// ���ָ���еı༭��ѡ�����
//-----------------------------------------------------------------------------
//
CELColumn*  CListCtrlEL::GetColumnExAttr (int colIndex)
{
    return m_Header.GetColumn(colIndex);
}

//
//-----------------------------------------------------------------------------
// ����һ����¼����, ���ؼ�¼���ڵ�rowindex
//  ���Ҫ�������, ����rowindex �� GetRecordCount()
//  rowindex���ǽ����ϵ���ʾ��һ�е�λ��
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
// �򵥽ӿ�: ����һ����¼, ��ָ����һ�е��ı�, 
//  ���ؼ�¼���ڵ�rowindex
//  ���������һ��ʹ��Ĭ�ϵ�Record����, �����û��Լ��½����Լ��̳�һ���µ�record����
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
// ���ָ��rowindex�µ�Record����
//-----------------------------------------------------------------------------
//
CELRecord*  CListCtrlEL::GetRecordRow (int rowIndex)
{
    return (CELRecord*)ELSUPER::GetItemData(rowIndex);
}

//
//-----------------------------------------------------------------------------
// �򵥽ӿ�: ����һ��Record���Զ�������
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
// �򵥽ӿ�: ��ȡһ��Record���Զ�������
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
// �򵥽ӿ�: ����һ��record��ĳ��item����ʾ�ַ�
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

    // ˢ�´���, ��SetItemText�ķ�ʽ,
    // ����ǵ�һ�еĸ���, ��������������ز��ܸ��µ����� bug2013.07.03
    ELSUPER::SetItemText(rowIndex, nSubItem, L"");
}

//
//-----------------------------------------------------------------------------
// �򵥽ӿ�: ���һ��record��ĳ��item����ʾ�ַ�
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
// �򵥽ӿ�: ����һ��record��ĳ��item���Զ�������
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
// �򵥽ӿ�: ���һ��record��ĳ��item���Զ�������
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
// ������м�¼����Ŀ
//-----------------------------------------------------------------------------
//
int  CListCtrlEL::GetRecordRowCount ()
{
    return ELSUPER::GetItemCount();
}

//
//-----------------------------------------------------------------------------
// ɾ��һ����¼
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::DeleteRecordRow (int rowIndex)
{
    // ��֤ȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    // �ͷŹ�����record
    ReleaseRecordRowExAttr(rowIndex);


    ELSUPER::DeleteItem(rowIndex);
}

//
//-----------------------------------------------------------------------------
// ɾ�����м�¼
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::DeleteAllRecordRow ()
{
    // ��֤ȫ���ػ�, �������ֱ�ӿ�����Ļ�ƶ�������, ����ȫ��Gridline��Ҫ�ػ�
    Invalidate(FALSE);

    // �ͷŹ�����record
    ReleaseAllRecordRowExAttr();


    ELSUPER::DeleteAllItems();
}



//
//=============================================================================
// ��Ϣ��Ӧ
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
// �ػ���һЩ���ⰴ����Ϣ, ����Enter, ESC��
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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

    // �����㼰ѡ�еļ��̲�������
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

    // �������ⰴ��
    if (nChar == ELVK_OK)
    {
        // �Ƚ���������, ���������費��Ҫ����ð���
        if (SendNotifyMessageToOwner(ELNM_OKCLICK, NULL))
        {
            // �������Ѿ������˸ð���
            return;
        }
    }

    // ����item����char��Ϣ, ��item�Լ��������¼�
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

    // ��û�д���, �������Լ�Ĭ�ϴ���
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
        // ����Ȼ���ÿؼ����ڽ���
        ELSUPER::SetFocus();

        // �����ý���ʱ, ���������ѡ��item, 
        if (!m_bRowFocusEnable)
        {
            return;
        }

        // ����Ӧ��row�ǽ���״̬ʱ, ���������ѡ��item
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

        // ��һ���ǵ���Ĭ�ϵ�OnPaint(),��ͼ�λ����ڴ�DC����
        DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);
        

        //���
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    
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
    // ����ÿ��col, ����
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

        // ���������ɫ�ͱ�����ɫ, ���item���Լ����ص���ɫ
        COLORREF  textColor = (curItem->m_clrText == EL_COLOR_DEFAULT)? defTextColor: curItem->m_clrText;
        COLORREF  bkColor = (curItem->m_clrBackground == EL_COLOR_DEFAULT)? defBkColor: curItem->m_clrBackground;


        // ����ѡ��������
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
                    // item �������
                    pDC->FillSolidRect(&selItemRect, m_focusedSubItemHighlightColor);
                }
                else
                {
                    // �ؼ���ʧ����ʱ, ��LVS_SHOWSELALWAYS
                    pDC->FillSolidRect(&selItemRect, m_selectionRowLowlightColor);
                }

                // ��������
                pDC->SetTextColor(m_textHighlightColor);
            }
            else if (m_bRowSelectionVisible)
            {
                if (curFocusedWnd == m_hWnd)
                {
                    // �� ѡ�����
                    pDC->FillSolidRect(&selItemRect, m_selectionRowHighlightColor);
                }
                else
                {
                    // �ؼ���ʧ����ʱ, ��LVS_SHOWSELALWAYS
                    pDC->FillSolidRect(&selItemRect, m_selectionRowLowlightColor);
                }

                // ��������
                pDC->SetTextColor(m_textHighlightColor);
            }
			
            else
            {
                // ������Ⱦitem���صı���ɫ
                if (bkColor != defBkColor)
                {
                    pDC->FillSolidRect(rcItem, bkColor);
                }

                // ��ͨ����
                pDC->SetTextColor(textColor);
            }
		}
        else
        {
            // ������Ⱦitem���صı���ɫ
            if (bkColor != defBkColor)
            {
                pDC->FillSolidRect(rcItem, bkColor);
            }

            // ��ͨ����
            pDC->SetTextColor(textColor);
        }
		//if(!curItem->m_pRecord->IsFocusable())
		//{
		//	// ������Ⱦitem���صı���ɫ
		//	if (bkColor != defBkColor)
		//	{
		//		pDC->FillSolidRect(rcItem, bkColor);
		//	}

		//	// ��ͨ����
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
// �����˳�, �ͷ����ж���, �ر��������Լ���������Ҷ���
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::OnDestroy ()
{
    // ��ǰ���ڱ༭״̬, ��Ӧ������������, 
    // �϶���ĳ���ط���������������, �����쳣���, bug2013.06.19
    ASSERT_BREAK(m_pActiveItem == NULL);

    // �ͷ�������Ҷ���, ���ݲ���, ������OnDestroy��ִ�вŵ��Ӵ���, 
    // ���Կ����ڸ�����ȥͳһ����
    ReleaseAllExAttr();
}


//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// �ͷ����е�������Զ���, ����ÿ��col, ÿ��record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseAllExAttr ()
{
    //
    // �ͷ�����col���������, 
    //
    int  colCount = m_Header.GetItemCount();
    for (int i = 0; i < colCount; i++)
    {
        // �����Ҷ���
        HDITEM  hdrinfo;
        hdrinfo.mask = HDI_LPARAM;
        m_Header.GetItem(i, &hdrinfo);

        CELColumn*  column = (CELColumn*)hdrinfo.lParam;
        ASSERT_BREAK(column);
        delete column;

        // ������Ҷ���
        hdrinfo.lParam = 0;
        m_Header.SetItem(i, &hdrinfo);
    }


    ReleaseAllRecordRowExAttr();
}

//
//-----------------------------------------------------------------------------
// �ͷ����е�������Զ���: ÿ��record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseAllRecordRowExAttr ()
{
    //
    // �ͷ�����record���������
    //
    int  recordCount = ELSUPER::GetItemCount();
    for (int i = 0; i < recordCount; i++)
    {
        ReleaseRecordRowExAttr(i);
    }
}

//
//-----------------------------------------------------------------------------
// �ͷ����е�������Զ���: һ��record
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::ReleaseRecordRowExAttr (int rowIndex)
{
    CELRecord*  record = (CELRecord*)ELSUPER::GetItemData(rowIndex);
    ASSERT_BREAK(record);

    // ��֤��ǰ���ڱ༭��item���ܱ�ɾ�� bug2013.06.19
    if (m_pActiveItem)
    {
        ASSERT_BREAK(m_pActiveItem->m_pRecord && m_pActiveItem->m_pRecord != record);
    }

    delete record;

    ELSUPER::SetItemData(rowIndex, 0);
}

//
//-----------------------------------------------------------------------------
// ��䱳��λͼ
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::FillBkgnd (CDC* pDC)
{
    COLORREF  bgColor = GetBkColor();

    // �����������ı���
    CRect  clientRect;
    GetClientRect(&clientRect);

    // ���ÿ�е����ⱳ��ɫ
    int  colCount = m_Header.GetItemCount();
    CRect  colRect = clientRect;
    for (int i = 0; i < colCount; i++)
    {
        // �����е�λ��
        int  colWidth = ELSUPER::GetColumnWidth(i);
        colRect.right = colRect.left + colWidth;

        // ����Ҫ���
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

    // �������ʣ��ռ�
    if (clientRect.right > colRect.left)
    {
        colRect.right = clientRect.right;
        pDC->FillSolidRect(colRect, bgColor);
    }
}

//
//-----------------------------------------------------------------------------
// �����ֱ���, ������
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::FillGridline (CDC* pDC)
{
    COLORREF  gridColor = m_Header.m_hdrGridlineColor;
    CRect  clientRect;
    GetClientRect(&clientRect);

    int  centerX = (clientRect.left + clientRect.right) / 2;  // �����е�

    // ���ƴ�ֱ������
    if (m_hasVGridline)
    {
        int  colCount = m_Header.GetItemCount();
        int  curColPos = 0;

        // �Ƿ���ƽ����������
        if (m_gradientVGridline)
        {
            // ���ƽ�����
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);
                curColPos += colWidth;

                CRect  gridRect(curColPos, clientRect.top+m_Header.m_hdrHeight, curColPos+1, clientRect.bottom);
                CommonEL::DrawGradient(pDC, gridRect, gridColor, GetBkColor(), GRADIENT_FILL_RECT_V);
            }

            // ����ˮƽ�����, �����λ���
            CRect  gridRectLeft(clientRect.left, clientRect.bottom-1, centerX, clientRect.bottom);
            CRect  gridRectRight(centerX, clientRect.bottom-1, clientRect.right, clientRect.bottom);
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());

            // ����ˮƽ�ⶥ��, ����������
            gridRectLeft.OffsetRect(0, -(clientRect.Height() - m_Header.m_hdrHeight-1));
            gridRectRight.OffsetRect(0, -(clientRect.Height() - m_Header.m_hdrHeight-1));
            CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
            CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());

        }
        else
        {
            // ����ʵ��
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);

                curColPos += colWidth;
                pDC->FillSolidRect(curColPos, clientRect.top, 1, clientRect.Height(), gridColor);
            }

            // ����ˮƽ�����
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);

            // ����ˮƽ�ⶥ��
            pDC->FillSolidRect(clientRect.left, clientRect.top+m_Header.m_hdrHeight, clientRect.Width(), 1, gridColor);
        }
    }

    // ����ˮƽ������
    if (m_hasHGridline)
    {
#if !SU_FEI	
		pDC->FillSolidRect(clientRect.left, clientRect.top + m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
#endif
        // ÿҳ������
        int  toprow = ELSUPER::GetTopIndex();
        int  chunk_height = ELSUPER::GetCountPerPage();
        int  itemCount = GetItemCount();
        int  hlineCount = min(chunk_height, itemCount);

        // �Ƿ���ƽ����������
        if (m_gradientHGridline)
        {
            // ���ƽ�����
            for (int i = 1; i <= hlineCount; i++)
            {
                POINT  rowPoint;
                ELSUPER::GetItemPosition(toprow+i, &rowPoint);

                // �����λ���
                CRect  gridRectLeft(clientRect.left, rowPoint.y, centerX, rowPoint.y+1);
                CRect  gridRectRight(centerX, rowPoint.y, clientRect.right, rowPoint.y+1);
                CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
                CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
            }
        }
        else
        {
            // ����ʵ��
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
// ��owner���ڷ���֪ͨ��Ϣ, ����֪ͨ�����ڰ���������, ��ʼ�༭״̬, itemֵ���ı��
//  ����ֵ��0���������Ƿ��Ѿ������˸���Ϣ
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
// ����ָ���Ĺ�����
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::HideScrollBars ()
{
    RECT ierect;
    int cxvs;
    // 		GetClientRect(&ierect); //Get client width and height
    // 		RECT windowrect;
    //�޸�Ϊʹ��WindowRect,ԭ�汾����ʹ�õ���ClientRect,���ǹ�������Ȼ�����ˣ����ǹ�����λ�û���ռ��λ�á�ֻ�Ǳ�Ϊ��ɫ��
    GetWindowRect(&ierect);
    ScreenToClient(&ierect);

    cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT

    //Here we set the position of the window to the clientrect + the size of the scrollbars
    SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom, SWP_NOMOVE | SWP_NOZORDER);

    SetListCtrlRgn();
}

//
//-----------------------------------------------------------------------------
// ����windows��ʾ����������, ����ϵͳ������
//-----------------------------------------------------------------------------
//
void  CListCtrlEL::SetListCtrlRgn ()
{
    RECT ierect;
    GetWindowRect(&ierect);
    ScreenToClient(&ierect);

    int  cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
    ierect.right -= cxvs;

    // ����window����, ȥ��������
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
// �����ڴ�λͼ,
//  ��һ�����Ż�: ���λͼ�Ѿ�����, �����ظ�����
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
            // ��λͼ�Ѿ�����, ���ҿ�������һ��, �����ظ�����
            return;
        }
        else
        {
            // ��߲�һ��, ɾ��ԭ����λͼ, �������´���
            m_paintMemMap.DeleteObject();
        }
    }

    EL_DEBUGLOG(L"CListCtrlEL::CreatePaintMemBitmap\n");
    m_paintMemMap.CreateCompatibleBitmap(dc, width, height);
}
