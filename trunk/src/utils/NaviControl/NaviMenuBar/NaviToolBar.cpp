#include "stdafx.h"

#include "NaviToolBar.h"
#include "src/main/common.h"

#if !CHUN_HUA
#define DSM_RCNAVIGATOIN_TEXT_LINE      18
#endif

//
//=============================================================================
// CNaviMenuRecordItem
//=============================================================================
//


CNaviMenuRecordItem::CNaviMenuRecordItem (int nPos)
{
    m_menuItemPos = nPos;
}

//
//-----------------------------------------------------------------------------
// 绘制本item
//-----------------------------------------------------------------------------
//
static void  __DrawText (CDC* pDC, const CString& str, CRect rcItem, ELAlignment align)
{
    ASSERT(pDC);

    if (str.GetLength() > 0)
    {
#if MBT_DRAWTEXTSHADOW  // 绘制阴影
        COLORREF  color = pDC->GetTextColor();
        pDC->SetTextColor(MBT_DRAWTEXTSHADOW_COLOR);
        CRect  rcShadow = rcItem;
#if SU_FEI
        rcShadow.OffsetRect(1, 1);
#else
		rcShadow.OffsetRect(10, 0);
#endif
        
     //   pDC->DrawText(str, rcShadow, align | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
		pDC->DrawText(str, rcShadow,  DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | EL_LEFT);
        pDC->SetTextColor(color);
#endif

      //  pDC->DrawText(str, rcItem, align | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
#if !SU_FEI
		rcItem.OffsetRect(10, 0);
#endif
		pDC->DrawText(str, rcItem,  DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE | EL_LEFT);
    }
}
void  CNaviMenuRecordItem::Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs)
{
    ASSERT(pDrawArgs && pDrawArgs->pDC && pDrawArgs->pControl);
    CNaviToolBar*  pControl = static_cast<CNaviToolBar*>(pDrawArgs->pControl);
    CNaviMenuRecord*  record = static_cast<CNaviMenuRecord*>(pControl->GetRecordRow(pDrawArgs->rowIndex));
    ASSERT(record);
    CNaviMenu*  menu = record->m_menu;
    CBitmap*  drawBitmap = pControl->GetDefaultItemBkImage();
    MM_ItemInfo  menuInfo;

    // 获得菜单信息
    if (menu && m_menuItemPos < menu->GetMenuItemCount())
    {
        menu->GetMenuItemInfo(m_menuItemPos, &menuInfo);

        // 获得需要绘制的背景图片, 根据当前menuItem的状态
        if (menuInfo.grayedImage && menuInfo.isGrayed)
        {
            drawBitmap = menuInfo.grayedImage;
        }
        else if (menuInfo.highlightImage && menuInfo.isHighlight)
        {
            drawBitmap = menuInfo.highlightImage;
        }
        else if (menuInfo.normalImage)
        {
            drawBitmap = menuInfo.normalImage;
        }
    }

    // 绘制背景图片
    if (menuInfo.type & MM_ITEMTYPE_IMAGE && drawBitmap)
    {
        ASSERT_BREAK(drawBitmap->GetSafeHandle());

        CDC  dcMemBmp;
        dcMemBmp.CreateCompatibleDC(pDrawArgs->pDC);
		

        BITMAP  bmpInfo;
        drawBitmap->GetBitmap(&bmpInfo);
        CBitmap*  oldbmp = (CBitmap*)dcMemBmp.SelectObject(drawBitmap);

        {

			pDrawArgs->pDC->BitBlt(pDrawArgs->rcItem.left, pDrawArgs->rcItem.top, 
					   pDrawArgs->rcItem.Width(),
					   pDrawArgs->rcItem.Height(),
					   &dcMemBmp, 0, 0, SRCCOPY);
        }

        dcMemBmp.SelectObject(oldbmp);
    }
#if !SU_FEI
	
	if (menuInfo.type & MM_ITEMTYPE_IMGSTR && drawBitmap)
	{
		ASSERT_BREAK(drawBitmap->GetSafeHandle());

		if (menuInfo.subitem)
		{
			CDC  MemBmp;
			MemBmp.CreateCompatibleDC(pDrawArgs->pDC);
			CString  dispStr = menuInfo.caption;

			BITMAP  bmpInfoStr;
			drawBitmap->GetBitmap(&bmpInfoStr);
			CBitmap*  oldbmpStr = (CBitmap*)MemBmp.SelectObject(drawBitmap);
			{
				pDrawArgs->pDC->BitBlt(pDrawArgs->rcItem.left, pDrawArgs->rcItem.top, 
					pDrawArgs->rcItem.Width(),
					pDrawArgs->rcItem.Height(),
					&MemBmp, 0, 0, SRCCOPY);
			}	
			
			if (menuInfo.isGrayed)
			{
				pDrawArgs->pDC->SetTextColor(MBT_TEXTGRYCOLOR);
			}
			else if (menuInfo.isHighlight)
			{
				pDrawArgs->pDC->SetTextColor(MBT_TEXTHLCOLOR);
			}
			 __DrawText(pDrawArgs->pDC, dispStr, pDrawArgs->rcItem, pDrawArgs->nTextAlign);

			MemBmp.SelectObject(oldbmpStr);
		}

	}


#endif
    // 绘制文字
    if (menuInfo.type & MM_ITEMTYPE_STRING)
    {
        CRect  rectText = pDrawArgs->rcItem;
        CString  dispStr = menuInfo.caption;
        if (menuInfo.subitem)
        {
            //dispStr += L"  〓";
			dispStr += L"";
        }
        if (menuInfo.isChecked)
        {
            dispStr = L"√" + dispStr;
        }
        if (menuInfo.isGrayed)
        {
            pDrawArgs->pDC->SetTextColor(MBT_TEXTGRYCOLOR);
        }
        else if (menuInfo.isHighlight)
        {
            pDrawArgs->pDC->SetTextColor(MBT_TEXTHLCOLOR);
        }
        __DrawText(pDrawArgs->pDC, dispStr, rectText, pDrawArgs->nTextAlign);
    }
}


//
//=============================================================================
// CNaviMenuRecord
//=============================================================================
//




CNaviMenuRecord::CNaviMenuRecord (int itemCount, CNaviMenu* menu)
{
    m_menu = menu;

#if CHUN_HUA
    for (int i = 0; i < itemCount; i++)
    {
        // 新建菜单item, 并赋予其关联菜单项的pos
        CNaviMenuRecordItem*  newitem = new CNaviMenuRecordItem(i);
        AddItem(newitem);
    }
#else
	CNaviMenuRecordItem* newItem = new CNaviMenuRecordItem(itemCount);
	AddItem(newItem);
#endif
}

CNaviMenuRecord::~CNaviMenuRecord ()
{

}



//
//=============================================================================
// CNaviToolBar
//=============================================================================
//

CNaviToolBar::CNaviToolBar ()
{
    m_naviMenu = NULL;
    m_commandTarget = NULL;
    m_defItemBkImage = NULL;
    m_vkmapBegin = VK_F1;
    m_isAutoUpdateCmdUI = FALSE;
#if !SU_FEI

	m_switchBkImage = NULL;
#endif
}

CNaviToolBar::~CNaviToolBar ()
{

    SetNaviMenu(NULL);
}

//
//-----------------------------------------------------------------------------
// 创建一个工具栏菜单窗口
//  指定itemWidth(格子的宽度), itemCount(格子数目)
//  还有窗口的其他属性
//  autoClip属性代表如果格子超过窗口宽度, 是否自动裁剪格子的宽度, 
//-----------------------------------------------------------------------------
//

#if CHUN_HUA
BOOL  CNaviToolBar::Create (int itemWidth, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip)
{
    ASSERT(!IsWindow(m_hWnd));

    // 创建主窗口
    CRect  wndRect = rect;
    BOOL  ret = MBSUPER::Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SINGLESEL, 
        wndRect, pParentWnd, nID);

    // 窗口不获得焦点
    MBSUPER::ShowWindow(SW_SHOWNOACTIVATE);
    MBSUPER::ModifyStyleEx(0, WS_EX_NOACTIVATE);


    // 设置窗口样式
    MBSUPER::SetHeaderHeight(0);
    MBSUPER::SetRowHeight(wndRect.Height());
    MBSUPER::SetRowFocusEnable(FALSE);
    MBSUPER::SetTextColor(MBT_TEXTCOLOR);

    // 创建格子
    int  totalWidth = 0;
    int  wndWidth = rect.right - rect.left;
    for (int i = 0; i < itemCount; i++)
    {
        // 自动裁剪超出窗口大小的部分
        int  itemWidthFix = 0;
        if (autoClip && (totalWidth+itemWidth) > wndWidth)
        {
            itemWidthFix = wndWidth - totalWidth;
        }
        else
        {
            itemWidthFix = itemWidth;
        }
        totalWidth += itemWidthFix;

        MBSUPER::InsertColumn(i, L"", EL_CENTER, itemWidthFix);
    }
    MBSUPER::InsertRecordRow(0, new CNaviMenuRecord(itemCount));

    // 启动定时器, 刷新CmdUI
    SetTimer(1, 1000, NULL);
    
    return ret;
}

#else
BOOL  CNaviToolBar::Create (int itemHeight, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip)
{
    ASSERT(!IsWindow(m_hWnd));

    // 创建主窗口
    CRect  wndRect = rect;
    BOOL  ret = MBSUPER::Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SINGLESEL, 
        wndRect, pParentWnd, nID);


    // 窗口不获得焦点
    MBSUPER::ShowWindow(SW_SHOWNOACTIVATE);
    MBSUPER::ModifyStyleEx(0, WS_EX_NOACTIVATE);

 
    // 设置窗口样式
    MBSUPER::SetHeaderHeight(0);
    MBSUPER::SetRowHeight(itemHeight);
    MBSUPER::SetRowFocusEnable(TRUE);
    MBSUPER::SetTextColor(MBT_TEXTCOLOR);

    // 创建格子
//     int  totalHeight = 0;
//     int  wndHeight = rect.top - rect.bottom;
//     for (int i = 0; i < itemCount; i++)
//     {
//         // 自动裁剪超出窗口大小的部分
//         int  itemHeightFix = 0;
//         if (autoClip && (totalHeight+itemHeight) > wndHeight)
//         {
//             itemHeightFix = wndHeight - totalHeight;
//         }
//         else
//         {
//             itemHeightFix = itemHeight;
//         }
//         totalHeight += itemHeightFix;
// 
//         MBSUPER::InsertColumn(i, L"", EL_CENTER, itemHeightFix);
//     }
    MBSUPER::InsertColumn(0, L"", EL_CENTER, wndRect.Width());
    for (int i = 0; i < itemCount; i++)
	{
        MBSUPER::InsertRecordRow(i, new CNaviMenuRecord(i));
	}

    // 启动定时器, 刷新CmdUI
    SetTimer(1, 1000, NULL);

    return ret;
}

#endif
//
//-----------------------------------------------------------------------------
// 设置Toolbar关联的菜单
// isAutoUpdateCmdUI -- 设置是否定时自动刷新CmdUI
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::SetNaviMenu (CNaviMenu* menu, BOOL isAutoUpdateCmdUI)
{
    // bug20130823: 切换菜单前, 关闭所有的弹出菜单
    //              防止菜单弹出中, 意外的切换菜单导致菜单内部指针执行错误的对象
    CloseNaviPopupBar();

    if (m_naviMenu)
    {
        // 簿记
        m_naviMenu->m_ownerToolBar = NULL;
    }

    m_naviMenu = menu;
    if (menu)
    {
        // 簿记
        menu->m_ownerToolBar = this;
    }

    // 刷新控件
    if (MBSUPER::GetSafeHwnd())
    {
#if CHUN_HUA
        ASSERT(MBSUPER::GetRecordRowCount() == 1);
        static_cast<CNaviMenuRecord*>(MBSUPER::GetRecordRow(0))->SetNaviMenu(menu);
#else
		for (int i = 0; i < 6; i++ )
		{
			static_cast<CNaviMenuRecord*>(MBSUPER::GetRecordRow(i))->SetNaviMenu(menu);
		}
#endif

        UpdateNaviMenu();                       // 刷新所有菜单item的显示
        UpdateCmdUI(menu);                      // 更新菜单时, 立即询问各个菜单item的状态
    }
    
    m_isAutoUpdateCmdUI = isAutoUpdateCmdUI;
}

//
//-----------------------------------------------------------------------------
// 设置Toolbar管理的目标COMMAND命令窗口
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::SetCommandTarget (CWnd* target)
{
    m_commandTarget = target;
}



//
//=============================================================================
// 消息响应
//=============================================================================
//
BEGIN_MESSAGE_MAP(CNaviToolBar, MBSUPER)
    ON_WM_TIMER()
    ON_WM_SETFOCUS()
END_MESSAGE_MAP()


BOOL  CNaviToolBar::PreTranslateMessage (MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
#if CHUN_HUA
        if (m_naviMenu && pMsg->wParam >= m_vkmapBegin && 
            pMsg->wParam < (m_vkmapBegin + GetHeaderCtrl()->GetItemCount()))
#else
		if (m_naviMenu && pMsg->wParam >= m_vkmapBegin && 
			pMsg->wParam < (m_vkmapBegin + GetRecordRowCount()))
#endif
        {
            return OnNaviKeyDown(pMsg->wParam - m_vkmapBegin);
        }
    }

    return MBSUPER::PreTranslateMessage(pMsg);
}

void  CNaviToolBar::OnTimer (UINT_PTR nIDEvent)
{
    // 发送界面刷新消息CN_UPDATE_COMMAND_UI
    if (nIDEvent == 1 && m_isAutoUpdateCmdUI)
    {
        UpdateCmdUI(m_naviMenu);
    }

    MBSUPER::OnTimer(nIDEvent);
}

void CNaviToolBar::OnSetFocus(CWnd* pOldWnd)
{
    MBSUPER::OnSetFocus(pOldWnd);

    // TODO: 在此处添加消息处理程序代码
    ASSERT(0);  // 本窗口不能获得焦点
}

void CNaviToolBar::OnPaint()
{
    MBSUPER::OnPaint();

    CPaintDC                dc(this);           // device context for painting

    CRect clientRect;
    GetClientRect(&clientRect);
    clientRect.DeflateRect(clientRect.Width()-1, 0, 0, 0);
    dc.FillSolidRect(clientRect, DSM_LINE_COLOR);

}




//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 根据当前设置的menu对象, 更新视图
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::UpdateNaviMenu ()
{
    // bug2013.07.03: 原来用ResetNaviMenu里面的SetRowItemText方式去更新窗口
    // 第一个格子的左边两个像素会得不到更新, 似乎是CListCtrl的特性, 不知道为什么, 
    // 在ListCtrlEL的DrawItem里面也做了相应的修补代码, 所以这里改为使用Invalidate 更新窗口
    //ResetNaviMenu();
    Invalidate(FALSE);
}

//
//-----------------------------------------------------------------------------
// 清空所有菜单项
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::ResetNaviMenu ()
{
#if CHUN_HUA
    ASSERT(MBSUPER::GetRecordRowCount() == 1);

    int  itemCount = MBSUPER::GetHeaderCtrl()->GetItemCount();
    for (int i = 0; i < itemCount; i++)
    {
        MBSUPER::SetRowItemText(0, i, L"");
    }
#else
	int itemCount = MBSUPER::GetRecordRowCount();
	for (int i = 0; i < itemCount; i++)
	{
		MBSUPER::SetRowItemText(i, 0, L"");
	}
#endif
}

//
//-----------------------------------------------------------------------------
// 响应导航按键按下消息, key代表0~N的序号, 对应功能从左到右的格子
//  如果已经对消息处理了, 则返回TRUE, 否则返回FALSE
//-----------------------------------------------------------------------------
//
BOOL  CNaviToolBar::OnNaviKeyDown (int key)
{
#if CHUN_HUA
    ASSERT(key >= 0 && key < MBSUPER::GetHeaderCtrl()->GetItemCount());
    ASSERT(MBSUPER::GetRecordRowCount() == 1);

    CNaviMenuRecordItem*  recItem = 
        static_cast<CNaviMenuRecordItem*>(GetRecordRow(0)->GetItem(key));
    ASSERT(recItem);

#else
	ASSERT(key >= 0 && key <= MBSUPER::GetItemCount());

	CNaviMenuRecordItem*  recItem = static_cast<CNaviMenuRecordItem*>(GetRecordRow(key)->GetItem(0));
	ASSERT(recItem);
	
#endif
    if (recItem->m_menuItemPos < m_naviMenu->GetMenuItemCount())
    {
        MM_ItemInfo  menuInfo;
        m_naviMenu->GetMenuItemInfo(recItem->m_menuItemPos, &menuInfo, 
            (MM_GetItemInfoFlag)(MM_GET_CMDID|MM_GET_SUBITEM|MM_GET_STATE));

        if (menuInfo.subitem)
        {
            UpdateCmdUI(menuInfo.subitem);

            // 高亮该菜单项目
            m_naviMenu->HighlightMenuItem(recItem->m_menuItemPos, TRUE);
            UpdateNaviMenu();

            // 弹出子菜单, 向目标发送command命令
#if CHUN_HUA
            CRect  rcItem = GetRowItemRect(0, key);

#else
			CRect rcItem = GetRowItemRect(key, 0);
#endif
            UINT  cmdId = menuInfo.subitem->TrackPopupMenu(rcItem, this, &m_popupBar, CNaviMenu::RET_CMDID);
            if (cmdId && m_commandTarget)
            {
                m_commandTarget->SendMessage(WM_COMMAND, cmdId);
            }

            // 恢复项目低亮
            m_naviMenu->HighlightMenuItem(recItem->m_menuItemPos, FALSE);
            UpdateNaviMenu();
        }
        else
        {
            // 向目标发送command命令
            if (m_commandTarget)
            {
                m_commandTarget->SendMessage(WM_COMMAND, menuInfo.cmdId);
            }
        }
        return TRUE;
    }

    return FALSE;
}

//
//-----------------------------------------------------------------------------
// 询问目标窗口, 刷新菜单格子
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::UpdateCmdUI (CNaviMenu* menu)
{
    if (menu && m_commandTarget)
    {
        int  count = menu->GetMenuItemCount();
        for (int i = 0; i < count; i++)
        {
            MM_ItemInfo  menuInfo;
            menu->GetMenuItemInfo(i, &menuInfo, (MM_GetItemInfoFlag)(MM_GET_CMDID|MM_GET_SUBITEM|MM_GET_STATE));

            // popup item 的ID无法对应到消息那边, 目前不知道如何处理
            if (menuInfo.subitem)
            {
                continue;
            }

            CNaviCmdUI  cmdui(menu, menuInfo.subitem);
            cmdui.m_nID = menuInfo.cmdId;
            cmdui.m_pOther = this;
            cmdui.m_nIndex = i;
            cmdui.m_nIndexMax = count - 1;
            cmdui.m_pMenu = NULL;
            cmdui.m_pSubMenu = NULL;
            cmdui.m_bEnableChanged = FALSE;
            cmdui.DoUpdate(m_commandTarget, TRUE);
        }

        UpdateNaviMenu();
    }
}

//
//-----------------------------------------------------------------------------
// 关闭当前弹出的菜单窗口
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::CloseNaviPopupBar ()
{
    if (m_popupBar.GetSafeHwnd() && (m_popupBar.m_nFlags & WF_MODALLOOP))
    {
        m_popupBar.EndModalLoop(IDCANCEL);
    }
}
