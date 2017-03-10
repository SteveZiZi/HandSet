#include "stdafx.h"

#include "ListCtrlEL.h"
#include "../Common/CommonEL.h"


//
//=============================================================================
// CELInplaceControl
//=============================================================================
//


CELInplaceControl::CELInplaceControl ()
{

}


CELInplaceControl::~CELInplaceControl ()
{
    SetItemArgs(NULL);
}

//
//-----------------------------------------------------------------------------
// ���ñ�inplace�ؼ���������item�Լ�������Ϣ
//  ͨ���ڿؼ���ʾ/������ʱ��, ������ϵ�ǰ�༭��item
//  �ڿؼ�����/���ص�ʱ��, ��ȡ������SetItemArgs(NULL);
//-----------------------------------------------------------------------------
//
void  CELInplaceControl::SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs)
{
    if (pItemArgs)
    {
        pControl = pItemArgs->pControl;
        rowIndex = pItemArgs->rowIndex;
        colIndex = pItemArgs->colIndex;
        pItem = pItemArgs->pItem;
        rcItem = pItemArgs->rcItem;
    }
    else
    {
        pControl = NULL;
        rowIndex = NULL;
        colIndex = NULL;
        pItem = NULL;
        rcItem.SetRectEmpty();
    }
}

//
//=============================================================================
// CELInplaceEdit
//=============================================================================
//

BEGIN_MESSAGE_MAP(CELInplaceEdit, CWnd)
    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
    ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
    ON_WM_KEYDOWN()
    ON_WM_SYSKEYDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_CHAR()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_PAINT()

    ON_COMMAND(EL_COMMAND_INPLACEEDIT_BACKSPACE, &CELInplaceEdit::OnBtnBackspace)
    ON_UPDATE_COMMAND_UI(EL_COMMAND_INPLACEEDIT_BACKSPACE, &CELInplaceEdit::OnUpdateBtnBackspace)
END_MESSAGE_MAP()

CELInplaceEdit::CELInplaceEdit ()
{
    m_pSelectedConstraint = NULL;
    m_clrText = RGB(0, 0, 0);
    m_bkColor = RGB(255, 255, 255);
    m_bkBrush.CreateSolidBrush(RGB(255, 255, 255));
    m_borderColor = RGB(255, 255, 255);

    m_bCancel = false;


    m_strText_prev = L"";
    m_bSetWindowText = FALSE;
}


CELInplaceEdit::~CELInplaceEdit ()
{

}

void  CELInplaceEdit::SetWindowText (LPCTSTR lpszString)
{
    m_bSetWindowText = TRUE;

    CWnd::SetWindowText(lpszString);
    m_strText_prev = lpszString;

    m_bSetWindowText = FALSE;
}


//
//-----------------------------------------------------------------------------
// �������ּ�������ɫ
//-----------------------------------------------------------------------------
//
void  CELInplaceEdit::SetBkColor (COLORREF color)
{
    m_bkColor = color;

    m_bkBrush.DeleteObject();
    m_bkBrush.CreateSolidBrush(color);
}
void  CELInplaceEdit::SetTextColor (COLORREF color)
{
    m_clrText = color;
}
void  CELInplaceEdit::SetBorderColor (COLORREF color)
{
    m_borderColor = color;
}


//
//-----------------------------------------------------------------------------
// ���������ڲ�����
//-----------------------------------------------------------------------------
//
void  CELInplaceEdit::SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs)
{
    m_bCancel = false;
    CELInplaceControl::SetItemArgs(pItemArgs);
}

//
//-----------------------------------------------------------------------------
// ��������ʾ�༭���ؼ�
//-----------------------------------------------------------------------------
//
void  CELInplaceEdit::Create (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);
    SetItemArgs(pItemArgs);  // ��Ҫ:�ҽ�

    CRect  rect = pItemArgs->rcItem;

    //
    // ������ڻ�û�д���, ���ȴ�������
    //
    if (!m_hWnd)
    {
        DWORD  dwEditStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | /*WS_BORDER |*/
            ES_AUTOHSCROLL | ES_MULTILINE;
        CEdit::Create(dwEditStyle, rect, pItemArgs->pControl, 0);
    }

    //
    // ��ʾ���ڲ���������λ��
    //
    this->EnableWindow(TRUE);

    this->SetFocus();
    this->SetFont(pItemArgs->pControl->GetFont());
    this->SetWindowText(pItem->GetCaption());
    this->SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_NOZORDER | SWP_SHOWWINDOW);
    this->SetMargins(0, 0);

    //
    // ʹ���ı���ֱ����, ע��: ����ES_MULTILINE ��ʽ
    //
    CRect  rcWnd;
    this->GetClientRect(&rcWnd);
    CDC*  pDC = this->GetDC();
    CFont*  oldfont = pDC->SelectObject(GetFont());
    TEXTMETRIC  tm;
    pDC->GetTextMetrics(&tm);
    pDC->SelectObject(oldfont);
    this->ReleaseDC(pDC);                                               // �ǵ��ͷ�

    int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
    int nMargin = (rcWnd.Height() - nFontHeight) / 2;
    rcWnd.DeflateRect(10, nMargin);                                     // Ĭ�����ñ༭�����ұ߾���10����
    this->SetRectNP(&rcWnd);


    m_bCancel       = false;                                            // ��λ��־

    m_strText_prev = pItem->GetCaption();
}

///
/// @brief
///     ����Inplace�༭����
///
void  CELInplaceEdit::HideWindow ()
{
    if (m_hWnd)
    {
        this->EnableWindow(FALSE);
        this->ShowWindow(SW_HIDE);
    }

    SetItemArgs(0);  // ��Ҫ:�ҽ�
}


///
/// @brief
///     ȷ���û������޸�, ֪ͨListbox�ĸ�����
///
void  CELInplaceEdit::ApplyInput ()
{
    //
    // ��item������Ϣ, ȷ���޸�
    //
    pItem->OnValidateEdit(this);


    // ȡ������
    pControl->EditItem(NULL);
}

///
/// @brief
///     �û�ȡ���޸�
///
void  CELInplaceEdit::CancelInput ()
{

    //
    // ��item������Ϣ, ȷ��ȡ��
    //
    pItem->OnCancelEdit(this);

    // ȡ������
    pControl->EditItem(NULL);
}




///
/// @brief
///     ��Inplace���ڶ�ʧ����ʱ, ȷ���û��Ѿ�����޸�
///
void  CELInplaceEdit::OnEnKillfocus ()
{
    if (m_bCancel)
    {
        CancelInput();
    }
    else
    {
        ApplyInput();
    }
}

///
/// @brief
///     ���������޸��¼�, ���ڹ����û�������ַ�
//      �����û��Լ������Ƿ���յ����޸�pItem->OnEditChanging
///
void  CELInplaceEdit::OnEnChange ()
{
    if (m_bSetWindowText || !pControl || !pItem)
        return;

    CString strValue, strValNew;
    GetWindowText(strValue);
    strValNew = strValue;

    //recover previous selection by diff between old and new text - compare from the start and from the end
    int kO = strValue.GetLength();
    int kP = m_strText_prev.GetLength();
    int K = min(kO, kP);
    int kOld(0);
    int kOldEnd(kP);
    if (m_strText_prev != strValue)
    {
        for (int j = 0; j < K; j++)
        {
            if (m_strText_prev[j] != strValue[j])
            {
                kOld = j;
                break;
            }
        }
        for (int jB = 1; jB <= K; jB++)
        {
            if (m_strText_prev[kP - jB] != strValue[kO - jB])
            {
                kOldEnd = kP + 1 - jB;
                break;
            }
        }
    }
    BOOL bCommit = pItem->OnEditChanging((EL_RECORDITEM_ARGS*)this, strValNew);

    if (!bCommit || strValNew != strValue)
    {
        int nSelStart = 0, nSelEnd = 0;
        GetSel(nSelStart, nSelEnd);

        int kN = strValNew.GetLength();
        int kk = min(kO, kN);
        if (strValNew != strValue) //it means that bCommit is TRUE
        {
            SetWindowText(strValNew);
            int k(kk - 1);
            for (int j = 0; j < kk; j++)
            {
                if (strValNew[j] != strValue[j])
                {
                    k = j;
                    break;
                }
            }
            SetSel(k, k);
        }
        else //it means that bCommit is FALSE - need rollback
        {
            SetWindowText(m_strText_prev);

            SetSel(kOld, kOldEnd);
            //Cursor position before 1st OnEditChanging - handler - modified symbol
            if (nSelStart == kO && nSelEnd == kO && kP < kO)
                SetSel(nSelStart, nSelEnd);

            if (nSelStart == 1 && nSelEnd == 1 && kP < kO)
                SetSel(0, 0);
        }
    }
    else
    {
        m_strText_prev = strValue;
        //Normal (default) cursor positioning
    }
}


UINT  CELInplaceEdit::OnGetDlgCode ()
{
    return DLGC_WANTALLKEYS;
}

///
/// @brief
///     �ػ�ESC, �س��ȼ�, ȷ���û��޸����
///
void  CELInplaceEdit::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_TAB:
        return;
        break;
    case ELVK_ESC:
        m_bCancel = true;                                               // �û�ȡ���޸�
        GetParent()->SetFocus();                                        // ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
        return;
        break;
    case ELVK_OK:
        GetParent()->SetFocus();                                        // ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
        return;
        break;
    default:
        break;
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void  CELInplaceEdit::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void  CELInplaceEdit::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CEdit::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

//
//-----------------------------------------------------------------------------
// �ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ
//-----------------------------------------------------------------------------
//
BOOL  CELInplaceEdit::PreTranslateMessage (MSG* pMsg)
{
    /* bug: wince��IsDialogMessage����winxp, �䲻�����getdlgcode, keydown����Ϣ
    if (pMsg->message == WM_KEYDOWN && IsDialogMessage(pMsg))
    {
        return TRUE;
    }
    */

    // ֱ�Ӱѹؼ����������, ����Ҫwindows��Ϣѭ��ȥDispatchMessage 
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB)
        {
            OnKeyDown(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
            OnChar(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
            return TRUE;
        }
        else if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || 
            pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN)
        {
            //
            // �ػ��򰴼�, ��ֹ����ܽ��ղ�����
            //
            //  ����ֵ���, ��PreTranslateMessage��ʱ��, pThreadState->m_lastSentMsg�Ĳ�����ȫ0, 
            //  ����CEdit::OnKeyDownִ��ʱû���κ�����(CEdit::OnKeyDown�ڲ������m_lastSentMsg)
            //  ��������ֱ�ӵ���DefWindowProc��OK��, ��������������mfc����ԭ����һ�û�и����
            //
            DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
            return TRUE;
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

//
//-----------------------------------------------------------------------------
// �޸Ŀؼ���ɫ
//-----------------------------------------------------------------------------
//
HBRUSH  CELInplaceEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  �ڴ˸��� DC ���κ�����
    
    pDC->SetTextColor(m_clrText);               // �ı���ɫ
    pDC->SetBkColor(m_bkColor);                 // �ı�����ɫ
    //pDC->SetBkMode(TRANSPARENT);


    // TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
    return m_bkBrush;                           // �ؼ����屳��ɫ
}

void  CELInplaceEdit::OnPaint ()
{
    CPaintDC  dc(this);
    CRect  rcClient;
    GetClientRect(&rcClient);

    // �����Զ���ı߿�
    CPen  pen(PS_SOLID, 2, m_borderColor);
    CPen*  oldpen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    {
        CRect  rcFrame = rcClient;
        rcFrame.DeflateRect(1, 1);
        dc.Rectangle(rcFrame);
    }
    dc.SelectObject(oldpen);

    // ����Ĭ�ϵĻ���
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
}

//
//-----------------------------------------------------------------------------
// ��Ӧ�ⲿ�˵�����, �����˵����ɾ����ťʱ, ɾ��һ���ַ�
//  ��������ϵ�Backspace
//-----------------------------------------------------------------------------
//
void CELInplaceEdit::OnBtnBackspace()
{
    PostMessage(WM_CHAR, VK_BACK, 0);
}
void CELInplaceEdit::OnUpdateBtnBackspace(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(1);
}



//
//=============================================================================
// CELInplaceList
//=============================================================================
//

BEGIN_MESSAGE_MAP(CELInplaceList, CListBoxEL)
    ON_WM_GETDLGCODE()
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


CELInplaceList::CELInplaceList ()
{

    m_bApply = FALSE;
}


//
//-----------------------------------------------------------------------------
// ���������ڲ�����
//-----------------------------------------------------------------------------
//
void  CELInplaceList::SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs)
{
    m_bApply = FALSE;
    CELInplaceControl::SetItemArgs(pItemArgs);
}



//
//-----------------------------------------------------------------------------
// ���Ƶ���List�ı���ͷ��
//-----------------------------------------------------------------------------
//
static void  xxxDrawListCaption (CWnd* owner, const CRect& rcItem, const CRect& rcListInScreen, COLORREF color, const CString& oldCaption)
{
    CClientDC  dc(owner);


    CPen  pen(PS_SOLID, 2, color);
    CPen*  oldpen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    {
        CRect  rcCaption = rcItem;
        rcCaption.DeflateRect(1, 1);

        // ����List��Item��λ�ù�ϵ, �������Ƶľ��δ�С, ʹ�ñ��߾���
        CRect  rcItemInScreen = rcItem;  owner->ClientToScreen(&rcItemInScreen);
        if (rcListInScreen.CenterPoint().y > rcItemInScreen.CenterPoint().y)
        {
            // list��item�����浯��, ��caption������չһ������
            rcCaption.InflateRect(0, 0, 0, 2);
        }
        else
        {
            // list��item�����浯��, ��caption������չһ������
            rcCaption.InflateRect(0, 2, 0, 0);
        }

        dc.Rectangle(rcCaption);
    }
    dc.SelectObject(oldpen);


    // ����������ʽ�������μ�ͷ
    CSize  captionSize = dc.GetTextExtent(oldCaption);
    if ((rcItem.Width() - captionSize.cx) > 50)  // ���ü�ͷ��ס������
    {
        CommonEL::DrawTriangleIcon(&dc, CPoint(rcItem.right-25, rcItem.CenterPoint().y), 5, color, 1, CommonEL::TRI_DOWN, TRUE, TRUE);
    }
}

//
//-----------------------------------------------------------------------------
// ��������ʾ�ؼ�
//-----------------------------------------------------------------------------
//
void  CELInplaceList::Create (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraints* pConstaints)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);
    SetItemArgs(pItemArgs);  // ��Ҫ:�ҽ�

    CRect  rect = pItemArgs->rcItem;

    if (!m_hWnd)
    {
        CListBoxEL::CreateEx(WS_EX_TOOLWINDOW, WS_POPUP, 
            CRect(0, 0, 0, 0), pControl, 0);
        SetOwner(pControl);
    }

    SetFont(pControl->GetFont());
    SetItemHeight(-1, GetItemHeight(-1) + 8);  // ������Ŀ�߶�, ������SetFont��
    
    
    
    ResetContent();
    

    //
    // ���ѡ��, �����ó�ʼ������
    //
    CString  strCaption = pItem->GetCaption();
    DWORD  dwData = pItem->GetSelectedConstraintData(pItemArgs);
    for (int i = 0; i < pConstaints->GetCount(); i++)
    {
        CELRecordItemConstraint*  pConstaint = pConstaints->GetAt(i);
        CString  str = pConstaint->m_strConstraint;
        int  nIndex = AddString(str);
        SetItemDataPtr(nIndex, pConstaint);

        if ((dwData == (DWORD)-1 && strCaption == str) || (dwData == pConstaint->m_dwData))
        {
            SetCurSel(nIndex);
        }
    }

    // ����ѡ����Ŀ, �����ؼ��߶�
    int  nHeight = GetItemHeight(0);
    rect.top = rect.bottom;
    rect.bottom += nHeight * min(10, GetCount()) + 4;
    pControl->ClientToScreen(&rect);

    // ������Ļ��������С, ��������λ��, ����պ�����Ļ��Եʱ, �ô�����ʾ����
    AdjustWindowRectInsideScreen(rcItem, rect);


    ShowWindow(SW_SHOW);  // fixbug: wince��û����仰, ������������, ��xp�ϻ����, ��ΪSetFocus�ᵼ�½����ظ���ʧ, ��֪��Ϊ��
    SetFocus();
    if (!m_hWnd) // ?? Can be destroyed after focus set
    {
        return;
    }

    SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    // ʹ�õ�ǰѡ��ɼ�
    if (GetCurSel() != LB_ERR)
    {
        EnsureVisible(GetCurSel(), FALSE);
    }

    // ����װ��ͷ, �������Ǹ�Item������
    xxxDrawListCaption(pControl, pItemArgs->rcItem, rect, LBT_FB_BORDERCOLOR, strCaption);
}

//
//-----------------------------------------------------------------------------
// ���ش���
//-----------------------------------------------------------------------------
//
void  CELInplaceList::HideWindow ()
{
    if (m_hWnd)
    {
        this->EnableWindow(FALSE);
        this->ShowWindow(SW_HIDE);
    }

    SetItemArgs(0);  // ��Ҫ:�ҽ�
}

//
//-----------------------------------------------------------------------------
// ȡ���޸�
//-----------------------------------------------------------------------------
//
void  CELInplaceList::Cancel ()
{
    m_bApply = FALSE;

    GetOwner()->SetFocus();
}

//
//-----------------------------------------------------------------------------
// ȷ���޸�
//-----------------------------------------------------------------------------
//
void  CELInplaceList::Apply ()
{
    if (!pControl)
    {
        return;
    }

    CListCtrlEL*  pReportControl = pControl;

    int  nIndex = GetCurSel();
    if (nIndex != LB_ERR)
    {
        m_bApply = TRUE;

        // ��item������Ϣ, ȷ���޸�
        CELRecordItemConstraint*  pConstraint = (CELRecordItemConstraint*)GetItemDataPtr(nIndex);
        EL_RECORDITEM_ARGS  itemArgs = *((EL_RECORDITEM_ARGS*)this);
        pItem->OnConstraintChanged(&itemArgs, pConstraint);
        pReportControl->RedrawRowItem(itemArgs.rowIndex, itemArgs.colIndex);

        // ���߸�����ֵ���ı�
        pReportControl->SendMessageToParent(itemArgs.rowIndex, itemArgs.pItem, itemArgs.colIndex, ELNM_ITEM_VALUECHANGED);
    }

    GetOwner()->SetFocus();
}

UINT  CELInplaceList::OnGetDlgCode ()
{
    return DLGC_WANTALLKEYS;
}

void  CELInplaceList::OnKillFocus (CWnd* pNewWnd)
{
    ASSERT_BREAK(pItem);

    if (m_bApply)
    {

    }
    else
    {
        // ��item������Ϣ, ȡ���޸�
        pItem->OnCancelEdit(this);
    }

    // ȡ������
    pControl->EditItem(NULL);

    CListBoxEL::OnKillFocus(pNewWnd);
}


void  CELInplaceList::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{

    if (nChar == ELVK_OK)
    {
        Apply();
    }
    else if (nChar == VK_UP || nChar == VK_LEFT || nChar == VK_DOWN || nChar == VK_RIGHT)
    {

    }
    else  // ESC����������, �����ȡ���༭
    {
        Cancel();
    }

    CListBoxEL::OnKeyDown(nChar, nRepCnt, nFlags);
}

void  CELInplaceList::OnLButtonUp (UINT nFlags, CPoint point)
{
    CListBoxEL::OnLButtonUp(nFlags, point);

    Apply();
}


//
//-----------------------------------------------------------------------------
// �ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ, ʵ������ģ̬��Ч��
//-----------------------------------------------------------------------------
//
BOOL  CELInplaceList::PreTranslateMessage (MSG* pMsg)
{
    /* bug: wince��IsDialogMessage����winxp, �䲻�����getdlgcode, keydown����Ϣ
    if (pMsg->message == WM_KEYDOWN && IsDialogMessage(pMsg))
    {
        return TRUE;
    }
    */

    // ֱ�Ӱѹؼ����������, ����Ҫwindows��Ϣѭ��ȥDispatchMessage 
    if (pMsg->message == WM_KEYDOWN && 
        (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB))
    {
        OnKeyDown(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
        OnChar(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
        return TRUE;
    }

    return CListBoxEL::PreTranslateMessage(pMsg);
}

//
//=============================================================================
// CELInplaceButton
//=============================================================================
//

CELInplaceButton::CELInplaceButton (ELInplaceButtonType nID)
{
    m_nID = nID;
    m_nWidth = 17;
    m_crColor = RGB(127, 127, 127);
}

//
//-----------------------------------------------------------------------------
// ���ư�ť
//-----------------------------------------------------------------------------
//
void  CELInplaceButton::Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs)
{
    ASSERT_BREAK(pDrawArgs && pDrawArgs->pDC);

    CRect  buttonRect = pDrawArgs->rcItem;
    buttonRect.left = buttonRect.right - pDrawArgs->rcItem.Height();

    // ���հ�ť����, ������ť��״, ��item�����ұ߻�
    if (GetID() == EL_BTNTYPE_COMBOBUTTON)
    {
#if 0  // �������ĺ��ѿ�, ��ʱ������

        // ����������ť, ��������
        CRect  triangleRect = buttonRect;
        triangleRect.DeflateRect(10, 10);

#define TRIANGLE_POINT_NUM  3
        CPoint  trianglePoints[TRIANGLE_POINT_NUM] = {
            CPoint(triangleRect.left, triangleRect.top),
            CPoint(triangleRect.right, triangleRect.top),
            CPoint(triangleRect.CenterPoint().x, triangleRect.bottom)
        };

        CBrush  bkBrush(m_crColor);
        CPen  borderPen(PS_SOLID, 1, m_crColor);

        CBrush*  oldbrush = pDrawArgs->pDC->SelectObject(&bkBrush);
        CPen*  oldpen = pDrawArgs->pDC->SelectObject(&borderPen);
        {
            pDrawArgs->pDC->Polygon(trianglePoints, TRIANGLE_POINT_NUM);
        }
        pDrawArgs->pDC->SelectObject(oldbrush);
        pDrawArgs->pDC->SelectObject(oldpen);
        
#endif
    }
    else
    {
#if 0  // �������ĺ��ѿ�, ��ʱ������

        // ������ͨ��ť, ������

#define  EXTPOINT_SIZE  4
        CRect  extPointRect = buttonRect;
        extPointRect.DeflateRect(5, 5);
        extPointRect.top = extPointRect.bottom - EXTPOINT_SIZE;
        
        CRect  extPointRect1 = extPointRect;
        extPointRect1.right = extPointRect1.left + EXTPOINT_SIZE;

        CRect  extPointRect2 = extPointRect;
        extPointRect2.left = extPointRect.CenterPoint().x - EXTPOINT_SIZE/2;
        extPointRect2.right = extPointRect2.left + EXTPOINT_SIZE;

        CRect  extPointRect3 = extPointRect;
        extPointRect3.left = extPointRect3.right - EXTPOINT_SIZE;

        pDrawArgs->pDC->FillSolidRect(extPointRect1, m_crColor);
        pDrawArgs->pDC->FillSolidRect(extPointRect2, m_crColor);
        pDrawArgs->pDC->FillSolidRect(extPointRect3, m_crColor);
        
#endif
    }
}