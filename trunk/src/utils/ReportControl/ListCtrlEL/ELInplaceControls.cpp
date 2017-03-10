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
// 设置本inplace控件所关联的item以及其他信息
//  通常在控件显示/创建的时候, 会关联上当前编辑的item
//  在控件销毁/隐藏的时候, 会取消关联SetItemArgs(NULL);
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
// 设置文字及背景颜色
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
// 重新设置内部变量
//-----------------------------------------------------------------------------
//
void  CELInplaceEdit::SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs)
{
    m_bCancel = false;
    CELInplaceControl::SetItemArgs(pItemArgs);
}

//
//-----------------------------------------------------------------------------
// 创建并显示编辑器控件
//-----------------------------------------------------------------------------
//
void  CELInplaceEdit::Create (EL_RECORDITEM_ARGS* pItemArgs)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);
    SetItemArgs(pItemArgs);  // 重要:挂接

    CRect  rect = pItemArgs->rcItem;

    //
    // 如果窗口还没有创建, 则先创建窗口
    //
    if (!m_hWnd)
    {
        DWORD  dwEditStyle = WS_CHILD | WS_VISIBLE | ES_LEFT | /*WS_BORDER |*/
            ES_AUTOHSCROLL | ES_MULTILINE;
        CEdit::Create(dwEditStyle, rect, pItemArgs->pControl, 0);
    }

    //
    // 显示窗口并调整窗口位置
    //
    this->EnableWindow(TRUE);

    this->SetFocus();
    this->SetFont(pItemArgs->pControl->GetFont());
    this->SetWindowText(pItem->GetCaption());
    this->SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_NOZORDER | SWP_SHOWWINDOW);
    this->SetMargins(0, 0);

    //
    // 使得文本垂直居中, 注意: 必须ES_MULTILINE 样式
    //
    CRect  rcWnd;
    this->GetClientRect(&rcWnd);
    CDC*  pDC = this->GetDC();
    CFont*  oldfont = pDC->SelectObject(GetFont());
    TEXTMETRIC  tm;
    pDC->GetTextMetrics(&tm);
    pDC->SelectObject(oldfont);
    this->ReleaseDC(pDC);                                               // 记得释放

    int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
    int nMargin = (rcWnd.Height() - nFontHeight) / 2;
    rcWnd.DeflateRect(10, nMargin);                                     // 默认设置编辑器左右边距是10像素
    this->SetRectNP(&rcWnd);


    m_bCancel       = false;                                            // 复位标志

    m_strText_prev = pItem->GetCaption();
}

///
/// @brief
///     隐藏Inplace编辑窗口
///
void  CELInplaceEdit::HideWindow ()
{
    if (m_hWnd)
    {
        this->EnableWindow(FALSE);
        this->ShowWindow(SW_HIDE);
    }

    SetItemArgs(0);  // 重要:挂接
}


///
/// @brief
///     确认用户输入修改, 通知Listbox的父窗口
///
void  CELInplaceEdit::ApplyInput ()
{
    //
    // 给item发送消息, 确认修改
    //
    pItem->OnValidateEdit(this);


    // 取消簿记
    pControl->EditItem(NULL);
}

///
/// @brief
///     用户取消修改
///
void  CELInplaceEdit::CancelInput ()
{

    //
    // 给item发送消息, 确认取消
    //
    pItem->OnCancelEdit(this);

    // 取消簿记
    pControl->EditItem(NULL);
}




///
/// @brief
///     当Inplace窗口丢失焦点时, 确认用户已经完成修改
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
///     处理输入修改事件, 用于过滤用户输入的字符
//      允许用户自己处理是否接收当次修改pItem->OnEditChanging
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
///     截获ESC, 回车等键, 确认用户修改完成
///
void  CELInplaceEdit::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_TAB:
        return;
        break;
    case ELVK_ESC:
        m_bCancel = true;                                               // 用户取消修改
        GetParent()->SetFocus();                                        // 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
        return;
        break;
    case ELVK_OK:
        GetParent()->SetFocus();                                        // 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
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
// 截获按键消息, 让父窗口不能拿到ESC Enter按键消息
//-----------------------------------------------------------------------------
//
BOOL  CELInplaceEdit::PreTranslateMessage (MSG* pMsg)
{
    /* bug: wince的IsDialogMessage不像winxp, 其不会调用getdlgcode, keydown等消息
    if (pMsg->message == WM_KEYDOWN && IsDialogMessage(pMsg))
    {
        return TRUE;
    }
    */

    // 直接把关键按键处理掉, 不需要windows消息循环去DispatchMessage 
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
            // 截获方向按键, 防止外层框架接收并处理
            //
            //  很奇怪的是, 在PreTranslateMessage的时候, pThreadState->m_lastSentMsg的参数是全0, 
            //  导致CEdit::OnKeyDown执行时没有任何作用(CEdit::OnKeyDown内部会调用m_lastSentMsg)
            //  所以这里直接调用DefWindowProc就OK了, 这个可能设计深层的mfc机制原因而我还没有搞清楚
            //
            DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
            return TRUE;
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

//
//-----------------------------------------------------------------------------
// 修改控件颜色
//-----------------------------------------------------------------------------
//
HBRUSH  CELInplaceEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  在此更改 DC 的任何属性
    
    pDC->SetTextColor(m_clrText);               // 文本颜色
    pDC->SetBkColor(m_bkColor);                 // 文本背景色
    //pDC->SetBkMode(TRANSPARENT);


    // TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
    return m_bkBrush;                           // 控件整体背景色
}

void  CELInplaceEdit::OnPaint ()
{
    CPaintDC  dc(this);
    CRect  rcClient;
    GetClientRect(&rcClient);

    // 绘制自定义的边框
    CPen  pen(PS_SOLID, 2, m_borderColor);
    CPen*  oldpen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);
    {
        CRect  rcFrame = rcClient;
        rcFrame.DeflateRect(1, 1);
        dc.Rectangle(rcFrame);
    }
    dc.SelectObject(oldpen);

    // 调用默认的画法
    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);
}

//
//-----------------------------------------------------------------------------
// 响应外部菜单命令, 当主菜单点击删除按钮时, 删除一个字符
//  替代键盘上的Backspace
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
// 重新设置内部变量
//-----------------------------------------------------------------------------
//
void  CELInplaceList::SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs)
{
    m_bApply = FALSE;
    CELInplaceControl::SetItemArgs(pItemArgs);
}



//
//-----------------------------------------------------------------------------
// 绘制弹出List的标题头部
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

        // 根据List和Item的位置关系, 调整绘制的矩形大小, 使得边线均匀
        CRect  rcItemInScreen = rcItem;  owner->ClientToScreen(&rcItemInScreen);
        if (rcListInScreen.CenterPoint().y > rcItemInScreen.CenterPoint().y)
        {
            // list在item的下面弹出, 则caption向下扩展一个像素
            rcCaption.InflateRect(0, 0, 0, 2);
        }
        else
        {
            // list在item的上面弹出, 则caption向上扩展一个像素
            rcCaption.InflateRect(0, 2, 0, 0);
        }

        dc.Rectangle(rcCaption);
    }
    dc.SelectObject(oldpen);


    // 绘制下拉样式的三角形箭头
    CSize  captionSize = dc.GetTextExtent(oldCaption);
    if ((rcItem.Width() - captionSize.cx) > 50)  // 不让箭头挡住了文字
    {
        CommonEL::DrawTriangleIcon(&dc, CPoint(rcItem.right-25, rcItem.CenterPoint().y), 5, color, 1, CommonEL::TRI_DOWN, TRUE, TRUE);
    }
}

//
//-----------------------------------------------------------------------------
// 创建并显示控件
//-----------------------------------------------------------------------------
//
void  CELInplaceList::Create (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraints* pConstaints)
{
    ASSERT_BREAK(pItemArgs && pItemArgs->pControl);
    SetItemArgs(pItemArgs);  // 重要:挂接

    CRect  rect = pItemArgs->rcItem;

    if (!m_hWnd)
    {
        CListBoxEL::CreateEx(WS_EX_TOOLWINDOW, WS_POPUP, 
            CRect(0, 0, 0, 0), pControl, 0);
        SetOwner(pControl);
    }

    SetFont(pControl->GetFont());
    SetItemHeight(-1, GetItemHeight(-1) + 8);  // 调整项目高度, 必须在SetFont后
    
    
    
    ResetContent();
    

    //
    // 添加选项, 并设置初始焦点项
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

    // 根据选项数目, 调整控件高度
    int  nHeight = GetItemHeight(0);
    rect.top = rect.bottom;
    rect.bottom += nHeight * min(10, GetCount()) + 4;
    pControl->ClientToScreen(&rect);

    // 根据屏幕工作区大小, 调整窗口位置, 比如刚好在屏幕边缘时, 让窗口显示完整
    AdjustWindowRectInsideScreen(rcItem, rect);


    ShowWindow(SW_SHOW);  // fixbug: wince上没有这句话, 可以正常运行, 但xp上会断言, 因为SetFocus会导致焦点重复丢失, 不知道为何
    SetFocus();
    if (!m_hWnd) // ?? Can be destroyed after focus set
    {
        return;
    }

    SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    // 使得当前选择可见
    if (GetCurSel() != LB_ERR)
    {
        EnsureVisible(GetCurSel(), FALSE);
    }

    // 绘制装饰头, 标明是那个Item弹出的
    xxxDrawListCaption(pControl, pItemArgs->rcItem, rect, LBT_FB_BORDERCOLOR, strCaption);
}

//
//-----------------------------------------------------------------------------
// 隐藏窗口
//-----------------------------------------------------------------------------
//
void  CELInplaceList::HideWindow ()
{
    if (m_hWnd)
    {
        this->EnableWindow(FALSE);
        this->ShowWindow(SW_HIDE);
    }

    SetItemArgs(0);  // 重要:挂接
}

//
//-----------------------------------------------------------------------------
// 取消修改
//-----------------------------------------------------------------------------
//
void  CELInplaceList::Cancel ()
{
    m_bApply = FALSE;

    GetOwner()->SetFocus();
}

//
//-----------------------------------------------------------------------------
// 确认修改
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

        // 给item发送消息, 确认修改
        CELRecordItemConstraint*  pConstraint = (CELRecordItemConstraint*)GetItemDataPtr(nIndex);
        EL_RECORDITEM_ARGS  itemArgs = *((EL_RECORDITEM_ARGS*)this);
        pItem->OnConstraintChanged(&itemArgs, pConstraint);
        pReportControl->RedrawRowItem(itemArgs.rowIndex, itemArgs.colIndex);

        // 告诉父窗口值被改变
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
        // 给item发送消息, 取消修改
        pItem->OnCancelEdit(this);
    }

    // 取消簿记
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
    else  // ESC或其他按键, 则代表取消编辑
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
// 截获按键消息, 让父窗口不能拿到ESC Enter按键消息, 实现类似模态的效果
//-----------------------------------------------------------------------------
//
BOOL  CELInplaceList::PreTranslateMessage (MSG* pMsg)
{
    /* bug: wince的IsDialogMessage不像winxp, 其不会调用getdlgcode, keydown等消息
    if (pMsg->message == WM_KEYDOWN && IsDialogMessage(pMsg))
    {
        return TRUE;
    }
    */

    // 直接把关键按键处理掉, 不需要windows消息循环去DispatchMessage 
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
// 绘制按钮
//-----------------------------------------------------------------------------
//
void  CELInplaceButton::Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs)
{
    ASSERT_BREAK(pDrawArgs && pDrawArgs->pDC);

    CRect  buttonRect = pDrawArgs->rcItem;
    buttonRect.left = buttonRect.right - pDrawArgs->rcItem.Height();

    // 按照按钮类型, 画出按钮形状, 在item的最右边画
    if (GetID() == EL_BTNTYPE_COMBOBUTTON)
    {
#if 0  // 画出来的很难看, 暂时不绘制

        // 绘制下拉按钮, 倒三角形
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
#if 0  // 画出来的很难看, 暂时不绘制

        // 绘制普通按钮, 三个点

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