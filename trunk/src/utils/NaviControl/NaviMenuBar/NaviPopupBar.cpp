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
// 计算在特定的DC和Font环境内, 字符串的宽度
//-----------------------------------------------------------------------------
//
static int  CalcTMStringWidth (LPTEXTMETRIC tm, const CString& str)
{
    int  totalWidth = 0;
    int  strLen = str.GetLength();
    for (int i = 0; i < strLen; i++)
    {
#if SHENG_ZE
        // 判断是否是中文, 或英文符号
        totalWidth += (str.GetAt(i) & 0xFF00)? tm->tmAveCharWidth * 2: tm->tmAveCharWidth;
#else
		// 判断是否是中文, 或英文符号
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
// 创建并显示控件
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


    // 创建窗口
    if (!m_hWnd)
    {
        CListBoxEL::CreateEx(WS_EX_TOOLWINDOW, WS_POPUP, 
            CRect(0, 0, 0, 0), pParentWnd, 0);
        SetOwner(pParentWnd);
    }

    // 设置字体
    SetFont(pParentWnd->GetFont());
    SetItemHeight(-1, GetItemHeight(-1) + 6);  // 调整项目高度, 必须在SetFont后

    // 获得字体尺寸, 用于计算控件宽度
    TEXTMETRIC  tm;
    GetTextMetricsHelper(GetFont(), &tm);
    
    //
    // 添加选项, 并设置初始焦点项
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

        // 获得最大宽度
        int  captionWidth = CalcTMStringWidth(&tm, menuInfo.caption) + 33;
        rect.right = rect.left + max(rect.Width(), captionWidth);
    }

    // 如果当前没选中任何项目, 则默认选择第一项
    if (GetCount() > 0 && GetCurSel() == LB_ERR)
    {
        SetCurSel(0);
    }


    // 窗口不获得焦点
    ShowWindow(SW_SHOWNOACTIVATE);
    ModifyStyleEx(0, WS_EX_NOACTIVATE);



    // 根据选项数目, 调整控件高度, 最多12项
    int  nHeight = GetItemHeight(0);

    rect.top = rect.bottom - 68;
    rect.bottom += nHeight * min(12, GetCount()) + 4;//4代表外边框有2个像素
	rect.bottom -= 68;
	
    pParentWnd->ClientToScreen(&rect);

    // 根据屏幕工作区大小, 调整窗口位置, 比如刚好在屏幕边缘时, 让窗口显示完整
    AdjustWindowRectInsideScreen(rcItem, rect);


    SetWindowPos(&CWnd::wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 
        SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

	/*SetWindowPos(&CWnd::wndTopMost, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), 
		SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);*/

    // 使得当前选择可见
    if (GetCurSel() != LB_ERR)
    {
        EnsureVisible(GetCurSel(), FALSE);
    }

    // 下面准备进入模态
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
// 模态循环, 截获键盘消息发给本窗口
//-----------------------------------------------------------------------------
//
int  CNaviPopupBar::RunModalLoopEx (DWORD dwFlags)
{
    ASSERT(::IsWindow(m_hWnd)); // window must be created
    ASSERT(!(m_nFlags & WF_MODALLOOP)); // window must not already be in modal state

    m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL);
    MSG *pMsg = AfxGetCurrentMessage();

    // 主消息循环:
    while (GetMessage(pMsg, NULL, 0, 0))
    {
        if (!ContinueModal())
        {
            goto ExitModal;
        }

        // 截获按键和鼠标消息, 转发给本窗口, 其他消息照样发给其他窗口
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
// 取消修改
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::Cancel ()
{
    m_bApply = FALSE;

    Close(IDCANCEL);
}

//
//-----------------------------------------------------------------------------
// 确认修改
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::Apply ()
{
    ASSERT(m_submenu);

    int  nIndex = GetCurSel();
    if (nIndex != LB_ERR)
    {
        m_bApply = TRUE;

        // 获得菜单项信息
        MM_ItemInfo  menuInfo;
        int  mmpos = GetItemData(nIndex);
        m_submenu->GetMenuItemInfo(mmpos, &menuInfo);
        m_selCmdId = menuInfo.cmdId;            // 填充返回cmdid
        m_selmmPos = nIndex;                    // 填充用户选择的菜单索引

        Close(IDOK);
    }
    else
    {
        Close(IDCANCEL);
    }
}

//
//-----------------------------------------------------------------------------
// 关闭对话框
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
    // 原来是这个, 现在改成DLGC_WANTARROWS, 没测出什么问题
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
            do  // 循环跳过不可选中项目, 比如分割条
            {
                targetSel = targetSel + direct;
                if (targetSel >= 0 && targetSel < itemCount)
                {
                    int  mmpos = GetItemData(targetSel);
                    MM_ItemInfo  menuInfo;
                    m_submenu->GetMenuItemInfo(mmpos, &menuInfo, MM_GET_TYPE);
                    if (menuInfo.type & MM_ITEMTYPE_SEPARATOR)
                    {
                        // 跳过分割条
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
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
                    break;
                }

            } while(1);

            if (targetSel >= 0 && targetSel <GetCount())
            {
                SetCurSel(targetSel);
            }
        }
    }
    else  // ESC键或其他按键
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

    // TODO: 在此处添加消息处理程序代码
    ASSERT(0);  // 本窗口不能获得焦点
}
void  CNaviPopupBar::OnCaptureChanged (CWnd *pWnd)    
{   
    Close(IDCANCEL);
    CListBoxEL::OnCaptureChanged(pWnd);   
}   



//
//-----------------------------------------------------------------------------
// 截获按键消息, 让父窗口不能拿到ESC Enter按键消息, 实现类似模态的效果
//-----------------------------------------------------------------------------
//
BOOL  CNaviPopupBar::PreTranslateMessage (MSG* pMsg)
{
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
//-----------------------------------------------------------------------------
// 绘制项目
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

    // 高亮色块周围画一圈与底色相同的矩形
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
// 设置项的高度
//-----------------------------------------------------------------------------
//
void  CNaviPopupBar::MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // 计算项高
    int  mmpos = (int)lpMeasureItemStruct->itemData;
    MM_ItemInfo  menuInfo;
    
    if (m_submenu)
    {
        m_submenu->GetMenuItemInfo(mmpos, &menuInfo, MM_GET_TYPE);
    }

    // 分隔栏高度是1
    if (menuInfo.type & MM_ITEMTYPE_SEPARATOR)
    {
        lpMeasureItemStruct->itemHeight = 1;
    }
}
//
//-----------------------------------------------------------------------------
// 弹出菜单窗口
//
// bug20130823: 修正可以传递一个外来的CNaviPopupBar对象, 无需使用局部的CNaviPopupBar对象
//              这样外部可以簿记这个对象, 便于在弹出模态的PopupBar内部, 结束其模态过程, 销毁窗口
//-----------------------------------------------------------------------------
//
int DoModal_NaviPopupBarEx(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu, CNaviPopupBar* popupBar)
{
    ASSERT(popupBar);

    int dlgret = popupBar->GoModal(rcItem, pParentWnd, menu);
    if (dlgret == IDOK)
    {
        // 获得用户选择的项目值
        retval = popupBar->GetSelCmdId();
        retmmPos = popupBar->GetSelmmPos();
    }
    else
    {
        // 设置无效的返回值
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

