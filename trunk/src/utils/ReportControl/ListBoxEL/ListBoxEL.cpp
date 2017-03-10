#include "stdafx.h"

#include "../Common/CommonEL.h"
#include "ListBoxEL.h"


CListBoxEL::CListBoxEL ()
{
    // ע���Ӧ�Ĵ�����
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
// �����ؼ�����
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
// ���õ�ǰʹ�õ�����
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
// ���ñ���ıߵĿհױ߾�, �հ�������Է�ֹ�����Ŀؼ�, �����ͷ, ������
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
// ���ݲ���
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ���item������
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetCount ()
{
    return m_itemArray.GetSize();
}

//
//-----------------------------------------------------------------------------
// ���һ��item, ��ָ����������û��Զ�������
//  ���ø�API��, ���Զ��ص�MeasureItem����item�ĸ߶�, �û��������ظýӿ��Զ�����Ŀ�ĸ߶�
//-----------------------------------------------------------------------------
//
int  CListBoxEL::AddString (LPCTSTR lpszItem, DWORD_PTR data)
{
    LB_ItemInfo*  itemInfo = new LB_ItemInfo;
    itemInfo->caption = lpszItem;
    itemInfo->data = data;
    itemInfo->cyHeight = m_defItemHeight;

    // ��item��������
    int  newIndex = m_itemArray.Add(itemInfo);

    // �ص�������Ŀ�߶�
    CallbackMeasureItem(newIndex, itemInfo);

    UpdateListBox();

    return newIndex;
}

//
//-----------------------------------------------------------------------------
// ��ָ����λ��, ����һ����Ŀ
// ��nIndex=-1ʱ, ��ʾ���һ������Ŀ
//-----------------------------------------------------------------------------
//
int  CListBoxEL::InsertString (int nIndex, LPCTSTR lpszItem, DWORD_PTR data)
{
    if (nIndex >= 0 && nIndex <= GetCount())    // Insert������ں����Index
    {
        LB_ItemInfo*  itemInfo = new LB_ItemInfo;
        itemInfo->caption = lpszItem;
        itemInfo->data = data;
        itemInfo->cyHeight = m_defItemHeight;

        // ��item��������
        m_itemArray.InsertAt(nIndex, itemInfo);

        // �ص�������Ŀ�߶�
        CallbackMeasureItem(nIndex, itemInfo);

        // ����ԭ����Sel����
        if (m_curSelIndex >= nIndex)
        {
            SetCurSel(m_curSelIndex + 1);
        }

        UpdateListBox();
        return nIndex;        
    }
    else if (nIndex == -1)                      // nIndex==-1 �൱�����һ���µ���
    {
        return AddString(lpszItem, data);
    }
    else                                        // nIndex���ں���Χ
    {
        ASSERT_BREAK(FALSE);
        return LB_ERR;
    }
}

//
//-----------------------------------------------------------------------------
// ɾ��ָ������Ŀ
//-----------------------------------------------------------------------------
//
int  CListBoxEL::DeleteString (int nIndex)
{
    LB_ASSERTINDEX(nIndex, 0);

    // ɾ������Ŀ�պ��Ǳ�ѡ�е���Ŀ, �������, ��Ҫsel���ֲ���
    if (nIndex == m_curSelIndex)
    {
        SetCurSel(-1);
    }
    else if (m_curSelIndex > nIndex)
    {
        SetCurSel(m_curSelIndex - 1);
    }

    // ����Top, ͨ�������top���ֲ���, 
    // ����top�����һ��Ԫ�ز��Ҹպ���ɾ����ĳ��Ԫ��, ��֤topIndex�ĺ�����
    if (m_topIndex >= (GetCount() - 1))
    {
        int  newTop = GetCount() - 2;           // ���潫��ɾ��һ��Ԫ��, ���� -2
        if (newTop >= 0)
        {
            SetTopIndex(newTop);
        }
    }



    // ɾ��item
    LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
    ASSERT_BREAK(itemInfo);
    delete itemInfo;
    m_itemArray.RemoveAt(nIndex);


    UpdateListBox();
    return GetCount();
}

//
//-----------------------------------------------------------------------------
// ����б��������
//-----------------------------------------------------------------------------
//
void  CListBoxEL::ResetContent ()
{
    // �ָ���item��ص�����
    m_curSelIndex = LB_ERR;
    m_topIndex = 0;

    // �ͷ�����item
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
// ���ָ��item����ʾ�ı�
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
// ����ָ��item�������û��Զ�������
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
// �ؼ�����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ȷ��item�ɼ�
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
            // ��nIndex�����ҵ�����topIndex, ֻҪ��֤nIndex�ɼ�
            CRect  clientRect;
            GetMarginClientRect(&clientRect);
            if (clientRect.IsRectEmpty())
            {
                // �ոմ�������, ������0,0,0,0
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
                        // ��nIndex�����̫��ʱ(���ڴ��ڴ�С)
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
// �ж�item�Ƿ��ڵ�ǰ���ڿɼ�
//  bPartialOK ָ���Ƿ������ֿɼ��ͷ���TRUE, ���� ����ȫ���ɼ��ŷ���TRUE
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
// ���õ�ǰѡ�����ʾ�ĵ�һ��item, Ĭ����0
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
        // �¾�ֵһ��, û�б�Ҫ, ����ˢ��
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
// ���õ�ǰѡ���item, -1����û��ѡ���κ�item
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
        // �¾�ֵһ��, û�б�Ҫ, ����ˢ��
        return LB_OKAY;
    }


    int  oldSelIndex = m_curSelIndex;
    if (nSelect >= -1 && nSelect < GetCount())
    {
        m_curSelIndex = nSelect;

        if (nSelect >= 0)
        {
            // ֻ��Ŀ��item����Чitemʱ, ��ʹ֮�ɼ�
            EnsureVisible(nSelect, FALSE);

            // ˢ�¸��Ĺ��ĸ���
            UpdateListBox(nSelect);
        }
        
        // ˢ�¸��Ĺ��ĸ���
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
// ���ָ��item����ǰ���ڵ�λ��(��topIndexӰ��)
//-----------------------------------------------------------------------------
//
int  CListBoxEL::GetItemRect (int nIndex, LPRECT lpRect)
{
    LB_ASSERTINDEX(nIndex, 0);
    ASSERT_BREAK(lpRect);

    // ��ô��ڴ�С
    CRect  clientRect;
    GetMarginClientRect(&clientRect);


    // �������ҷ���
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
    
    // ��topindex��ʼѭ������, ֱ��Ŀ��index
    int  targetIndex = GetTopIndex();
    int  targetRectTop = clientRect.top;        // top����
    do 
    {
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(targetIndex);
        ASSERT_BREAK(itemInfo);

        if (targetIndex == nIndex)
        {
            // �ҵ�Ŀ��
            lpRect->left = clientRect.left;
            lpRect->right = clientRect.right;
            lpRect->top = targetRectTop;
            lpRect->bottom = targetRectTop + itemInfo->cyHeight;
            break;
        }
        else
        {
            
            // ���ݲ��ҷ���, ����targetRectTop
            if (direct < 0)
            {
                // ��һ��item
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
// ����ָ��item����ʾ�߶�
//-----------------------------------------------------------------------------
//
int  CListBoxEL::SetItemHeight (int nIndex, UINT cyItemHeight)
{
    if (nIndex >= 0 && nIndex < GetCount())
    {
        // ���nIndex��Ч, ������ָ����ĸ߶�
        LB_ItemInfo*  itemInfo = m_itemArray.GetAt(nIndex);
        ASSERT_BREAK(itemInfo);

        itemInfo->cyHeight = cyItemHeight;

        UpdateListBox();
    }
    else
    {
        // ���nIndex��Ч, ������Ĭ�ϸ߶�
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
// ��Ϣ����
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

    // ��������б�Χ, �Զ���ת
    if (targetSel < 0)
    {
        targetSel = GetCount() - 1;
    }
    else if (targetSel >= GetCount())
    {
        // fix��0����, ԭ����ֱ�� % GetCount()
        targetSel = 0;
    }

    // ѡ��Ŀ����
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
// ��úͶ�ʧ����ʱ, �ػ�ؼ�, ʹ��ѡ��ĸ�������������ʾ
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
// ��������Ʒǿͻ����Ļ���, �ǿͻ�������ͨ��SetMargin����
// SetMargin֮���������Ƿǿͻ���
//-----------------------------------------------------------------------------
//
void  CListBoxEL::PaintNC (CDC* /*pDC*/)
{

}
//
//-----------------------------------------------------------------------------
// ��䱳��λͼ
//-----------------------------------------------------------------------------
//
void  CListBoxEL::FillBkgnd (CDC* pDC)
{
    // �����������ı���
    CRect  clientRect;
    GetMarginClientRect(&clientRect);
    pDC->FillSolidRect(clientRect, m_bkColor);
}
//
//-----------------------------------------------------------------------------
// ���������ؼ�
//-----------------------------------------------------------------------------
//
void  CListBoxEL::Paint (CDC* pDC)
{
    ASSERT_BREAK(pDC);
    CDC&  dc = *pDC;

    // ����������Ʒǿͻ���
    PaintNC(&dc);

    // ���ݱ߾����òü���, ʹ��DC�����ƿհױ߾ಿ��
    CRect  marginClientRect;
    GetMarginClientRect(&marginClientRect);
    dc.IntersectClipRect(marginClientRect);

    // ��䱳��
    FillBkgnd(&dc);

    CFont*  oldfont = dc.SelectObject(GetFont());
    dc.SetBkMode(TRANSPARENT);

    // �������ӵ�item, ����
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
// ����һ��С��, ���Ը��û�����
//  ��������ÿ��OnPaint�ػ�ʱ������
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
        // ����ѡ�������
        if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED) && 
            (curFocusedWnd == m_hWnd || m_bShowSelAlways))
        {
            if (curFocusedWnd == m_hWnd)
            {
                // item �������
                pDC->FillSolidRect(rcItem, m_selHighlightColor);

                // ���ָ���
                pDC->SetTextColor(m_textHighlightColor);
            }
            else
            {
                // �ؼ���ʧ����ʱ, ��m_bShowSelAlways
                pDC->FillSolidRect(rcItem, m_selLowlightColor);

                pDC->SetTextColor(m_textColor);
            }
        }
        else
        {
            pDC->SetTextColor(m_textColor);
        }
        
        // ��������
        pDC->DrawText(itemText, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);

        // ���ƽ����
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
// �����������Ŀ����, �����趨��Ŀ�Ŀ��, ���Ը��û�����
//  ���������������ĿAddStringʱ������
//-----------------------------------------------------------------------------
//
void  CListBoxEL::MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{

}

//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ��ȡ��ȥ�հױ߽���Ŀͻ�������
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

    // ʹ�þ������滯, ��ֹ�߾����
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
// ������ѡ������, ����itemĬ�ϸ߶�
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
// ��ȡ����ߴ�
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
// ˢ�±��ؼ�һ��
//  nIndex == -1�����������ˢ��, ����ֻˢ���ض��ĸ���
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
// �����ص�����item
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

    // ���û��Լ�ȥ����item
    DrawItem(&drawItemStruct);
}

//
//-----------------------------------------------------------------------------
// �����ص�����item���
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

    // ѯ���û�item����ʵ��ȸ߶�
    MeasureItem(&measureItemStruct);

    // ��������item�Ŀ��
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
// �Զ��������ڵ�λ��, ʹ�ô�����������ʾ����Ļ��, ��Χ����surroundRect
// ���ص�����ľ���λ��
// ��������: ListCtrl��һ�����ӵ��������˵�(ListBox), ListBox���������ڸ���surroundRect��Χ, ���ֲ��ܳ�����Ļ
//-----------------------------------------------------------------------------
//
void  CListBoxEL::AdjustWindowRectInsideScreen(const CRect& surroundRect, IN OUT CRect& retRectFix)
{
    // ������Ļ��������С, ��������λ��, ����պ�����Ļ��Եʱ, �ô�����ʾ����
    CRect  rcWork = __GetScreenArea(retRectFix);
	
	//�ѹ��������������С
	rcWork.top += 38;
	rcWork.bottom  -= 37;
	rcWork.left += 110;

	//����֮ǰ����ˮƽ����Ĳ˵��������Դ��ڲ˵����뵯����߶ȵ����⣬���ڸĳ���ֱ����Ĳ˵��������Կ��Ժ��ԶԲ˵����͵�������߶ȵ�����
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
// ע�ᴰ����
//-----------------------------------------------------------------------------
//
BOOL  CListBoxEL::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class�Ѿ�����
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