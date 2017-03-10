#include "stdafx.h"

#include "NaviToolBar.h"



//
//=============================================================================
// CNaviPopupBar
//=============================================================================
//

BEGIN_MESSAGE_MAP(CNaviPopupBar, CListBoxEL)
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()



CNaviPopupBar::CNaviPopupBar ()
{
    m_submenu = NULL;
    m_selCmdId = 0;
    m_selmmPos = LB_ERR;

    m_bApply = FALSE;
}

CNaviPopupBar::~CNaviPopupBar ()
{

}

int  CNaviPopupBar::AddMenuItem (int pos)
{
    return AddString(L"", pos);
}


//
//-----------------------------------------------------------------------------
// �������ض���DC��Font������, �ַ����Ŀ��
//-----------------------------------------------------------------------------
//
static int  CalcTMStringWidth (LPTEXTMETRIC tm, const CString& str)
{
    int  totalWidth = 0;
    int  strLen = str.GetLength();
    for (int i = 0; i < strLen; i++)
    {
#if SHENG_ZE
        // �ж��Ƿ�������, ��Ӣ�ķ���
        totalWidth += (str.GetAt(i) & 0xFF00)? tm->tmAveCharWidth * 2: tm->tmAveCharWidth;
#else
		// �ж��Ƿ�������, ��Ӣ�ķ���
		if(str.GetAt(i) & 0xFF00)
		{
			totalWidth += (tm->tmAveCharWidth) * 2;
		}
		else
		{
			totalWidth += (tm->tmAveCharWidth) * 1.5;
		}
#endif
    }

    return totalWidth;
}

//
//-----------------------------------------------------------------------------
// ��������ʾ�ؼ�
//-----------------------------------------------------------------------------
//
int  CNaviPopupBar::GoModal (const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* subMenu)
{
    ASSERT(subMenu);
    if (::IsWindow(m_hWnd)) 
    {
        ASSERT(FALSE);
        return FALSE;
    }

    CRect  rect = rcItem;
    m_submenu = subMenu;


    // ��������
    if (!m_hWnd)
    {
        CListBoxEL::CreateEx(WS_EX_TOOLWINDOW, WS_POPUP, 
            CRect(0, 0, 0, 0), pParentWnd, 0);
        SetOwner(pParentWnd);
    }

    // ��������
    SetFont(pParentWnd->GetFont());
    SetItemHeight(-1, GetItemHeight(-1) + 6);  // ������Ŀ�߶�, ������SetFont��

    // �������ߴ�, ���ڼ���ؼ����
    TEXTMETRIC  tm;
    GetTextMetricsHelper(GetFont(), &tm);
    
    //
    // ���ѡ��, �����ó�ʼ������
    //
    ResetContent();
    for (int i = 0; i < subMenu->GetMenuItemCount(); i++)
    {
        MM_ItemInfo  menuInfo;
        subMenu->GetMenuItemInfo(i, &menuInfo);

        int  nIndex = AddString(L"", i);
        if (menuInfo.isHighlight)
        {
            SetCurSel(nIndex);
        }

        // ��������
        int  captionWidth = CalcTMStringWidth(&tm, menuInfo.caption) + 33;
        rect.right = rect.left + max(rect.Width(), captionWidth);
    }

    // �����ǰûѡ���κ���Ŀ, ��Ĭ��ѡ���һ��
    if (GetCount() > 0 && GetCurSel() == LB_ERR)
    {
        SetCurSel(0);
    }


    // ���ڲ���ý���
    ShowWindow(SW_SHOWNOACTIVATE);
    ModifyStyleEx(0, WS_EX_NOACTIVATE);



    // ����ѡ����Ŀ, �����ؼ��߶�, ���12��
    int  nHeight = GetItemHeight(0);

    rect.top = rect.bottom - 68;
    rect.bottom += nHeight * min(12, GetCount()) + 4;//4������߿���2������
	rect.bottom -= 68;
	
    pParentWnd->ClientToScreen(&rect);

    // ������Ļ��������С, ��������λ��, ����պ�����Ļ��Եʱ, �ô�����ʾ����
    AdjustWindowRectInsideScreen(rcItem, rect);


    SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	/*SetWindowPos(&CWnd::wndTopMost, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), 
		SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);*/

    // ʹ�õ�ǰѡ��ɼ�
    if (GetCurSel() != LB_ERR)
    {
        EnsureVisible(GetCurSel(), FALSE);
    }

    // ����׼������ģ̬
    // Enable this window
    EnableWindow(TRUE);
    CWnd* pFocusWnd = SetFocus();
    SetCapture();


    m_nFlags |= WF_CONTINUEMODAL;
    int nResult = m_nModalResult;
    if (ContinueModal())
    {
        // enter modal loop
        DWORD dwFlags = MLF_SHOWONIDLE;
        if (GetStyle() & DS_NOIDLEMSG)
        {
            dwFlags |= MLF_NOIDLEMSG;
        }
        nResult = RunModalLoopEx(dwFlags);
    }

    ReleaseCapture();
    DestroyWindow();

    if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
    {
        pFocusWnd->SetFocus();
    }


    return (nResult == IDOK);
}

//
//-----------------------------------------------------------------------------
// ģ̬ѭ��, �ػ������Ϣ����������
//-----------------------------------------------------------------------------
//
int  CNaviPopupBar::RunModalLoopEx (DWORD dwFlags)
{
    ASSERT(::IsWindow(m_hWnd)); // window must be created
    ASSERT(!(m_nFlags & WF_MODALLOOP)); // window must not already be in modal state

    m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL);
    MSG *pMsg = AfxGetCurrentMessage();

    // ����Ϣѭ��:
    while (GetMessage(pMsg, NULL, 0, 0))
    {
        if (!ContinueModal())
        {
            goto ExitModal;
        }

        // �ػ񰴼��������Ϣ, ת����������, ������Ϣ����������������
        if ((pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST) || 
            (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) || 
            pMsg->message == WM_SYSKEYUP)
        {
            pMsg->hwnd = GetSafeHwnd();
        }

        TranslateMessage(pMsg);
        DispatchMessage(pMsg);
    }

ExitModal:
    m_nFlags &= ~(WF_MODALLOOP|WF_CONTINUEMODAL);
    return m_nModalResult;
}


//
//-----------------------------------------------------------------------------
// ȡ���޸�
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::Cancel ()
{
    m_bApply = FALSE;

    Close(IDCANCEL);
}

//
//-----------------------------------------------------------------------------
// ȷ���޸�
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::Apply ()
{
    ASSERT(m_submenu);

    int  nIndex = GetCurSel();
    if (nIndex != LB_ERR)
    {
        m_bApply = TRUE;

        // ��ò˵�����Ϣ
        MM_ItemInfo  menuInfo;
        int  mmpos = GetItemData(nIndex);
        m_submenu->GetMenuItemInfo(mmpos, &menuInfo);
        m_selCmdId = menuInfo.cmdId;            // ��䷵��cmdid
        m_selmmPos = nIndex;                    // ����û�ѡ��Ĳ˵�����

        Close(IDOK);
    }
    else
    {
        Close(IDCANCEL);
    }
}

//
//-----------------------------------------------------------------------------
// �رնԻ���
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::Close (INT_PTR result)
{
    if (ContinueModal())
    {
        EndModalLoop(result);
    }
    else
    {
        DestroyWindow();
    }
}


UINT  CNaviPopupBar::OnGetDlgCode ()
{
    // ԭ�������, ���ڸĳ�DLGC_WANTARROWS, û���ʲô����
    // return DLGC_WANTALLKEYS;

    return DLGC_WANTARROWS;
}
void  CNaviPopupBar::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == MBVK_OK)
    {
        Apply();
    }
    else if (nChar == VK_UP || nChar == VK_LEFT || nChar == VK_DOWN || nChar == VK_RIGHT)
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


        if (m_submenu && direct)
        {
            int targetSel = GetCurSel();
            int itemCount = GetCount();
            do  // ѭ����������ѡ����Ŀ, ����ָ���
            {
                targetSel = targetSel + direct;
                if (targetSel >= 0 && targetSel < itemCount)
                {
                    int  mmpos = GetItemData(targetSel);
                    MM_ItemInfo  menuInfo;
                    m_submenu->GetMenuItemInfo(mmpos, &menuInfo, MM_GET_TYPE);
                    if (menuInfo.type & MM_ITEMTYPE_SEPARATOR)
                    {
                        // �����ָ���
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
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
                    break;
                }

            } while(1);

            if (targetSel >= 0 && targetSel <GetCount())
            {
                SetCurSel(targetSel);
            }
        }
    }
    else  // ESC������������
    {
        Cancel();
    }
}

void  CNaviPopupBar::OnKillFocus (CWnd* pNewWnd)
{
    if (m_bApply)
    {

    }
    else
    {

    }


    CListBoxEL::OnKillFocus(pNewWnd);
}
void  CNaviPopupBar::OnSetFocus (CWnd* pOldWnd)
{
    CListBoxEL::OnSetFocus(pOldWnd);

    // TODO: �ڴ˴������Ϣ����������
    ASSERT(0);  // �����ڲ��ܻ�ý���
}
void  CNaviPopupBar::OnCaptureChanged (CWnd *pWnd)    
{   
    Close(IDCANCEL);
    CListBoxEL::OnCaptureChanged(pWnd);   
}   



//
//-----------------------------------------------------------------------------
// �ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ, ʵ������ģ̬��Ч��
//-----------------------------------------------------------------------------
//
BOOL  CNaviPopupBar::PreTranslateMessage (MSG* pMsg)
{
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
//-----------------------------------------------------------------------------
// ������Ŀ
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT(lpDIS->CtlType == ODT_LISTBOX);

    int  mmpos = (int)lpDIS->itemData;
    CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
    CRect  rcItem = lpDIS->rcItem;
    CRect  rcText(lpDIS->rcItem.left + 10, lpDIS->rcItem.top, lpDIS->rcItem.right - 10, lpDIS->rcItem.bottom);
    COLORREF  textColor = GetTextColor();
    COLORREF  crOldTextColor = pDC->GetTextColor();
    MM_ItemInfo  menuInfo;

    if (m_submenu)
    {
        m_submenu->GetMenuItemInfo(mmpos, &menuInfo);
    }

    if (lpDIS->itemID != (UINT)-1)
    {
        if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED))  
        {
            pDC->FillSolidRect(rcItem, m_selHighlightColor);
            textColor = m_textHighlightColor;
        }
        else
        {
            textColor = m_textColor;
        }

        if (menuInfo.isGrayed)
        {
            textColor = MBT_TEXTGRYCOLOR;
        }

        if (menuInfo.type & MM_ITEMTYPE_SEPARATOR)
        {
            pDC->Draw3dRect(rcText, MBT_TEXTGRYCOLOR, MBT_TEXTGRYCOLOR);
        }
        else
        {
            pDC->SetTextColor(textColor);
            pDC->DrawText(menuInfo.caption, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
        }
    }
    pDC->SetTextColor(crOldTextColor);  

    // ����ɫ����Χ��һȦ���ɫ��ͬ�ľ���
    CPen  pen(PS_SOLID, 1, m_bkColor);
    CPen*  oldpen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    {
        pDC->Rectangle(CRect(lpDIS->rcItem));
    }
    pDC->SelectObject(oldpen);
}



//
//-----------------------------------------------------------------------------
// ������ĸ߶�
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // �������
    int  mmpos = (int)lpMeasureItemStruct->itemData;
    MM_ItemInfo  menuInfo;
    
    if (m_submenu)
    {
        m_submenu->GetMenuItemInfo(mmpos, &menuInfo, MM_GET_TYPE);
    }

    // �ָ����߶���1
    if (menuInfo.type & MM_ITEMTYPE_SEPARATOR)
    {
        lpMeasureItemStruct->itemHeight = 1;
    }
}
//
//-----------------------------------------------------------------------------
// �����˵�����
//
// bug20130823: �������Դ���һ��������CNaviPopupBar����, ����ʹ�þֲ���CNaviPopupBar����
//              �����ⲿ���Բ����������, �����ڵ���ģ̬��PopupBar�ڲ�, ������ģ̬����, ���ٴ���
//-----------------------------------------------------------------------------
//
int DoModal_NaviPopupBarEx(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu, CNaviPopupBar* popupBar)
{
    ASSERT(popupBar);

    int dlgret = popupBar->GoModal(rcItem, pParentWnd, menu);
    if (dlgret == IDOK)
    {
        // ����û�ѡ�����Ŀֵ
        retval = popupBar->GetSelCmdId();
        retmmPos = popupBar->GetSelmmPos();
    }
    else
    {
        // ������Ч�ķ���ֵ
        retval = 0;
        retmmPos = LB_ERR;
    }

    return dlgret;
}
int DoModal_NaviPopupBar(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu)
{
    CNaviPopupBar  wndPS;
    return DoModal_NaviPopupBarEx(retval, retmmPos, rcItem, pParentWnd, menu, &wndPS);
}

