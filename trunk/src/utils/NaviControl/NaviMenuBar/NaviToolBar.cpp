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
// ���Ʊ�item
//-----------------------------------------------------------------------------
//
static void  __DrawText (CDC* pDC, const CString& str, CRect rcItem, ELAlignment align)
{
    ASSERT(pDC);

    if (str.GetLength() > 0)
    {
#if MBT_DRAWTEXTSHADOW  // ������Ӱ
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

    // ��ò˵���Ϣ
    if (menu && m_menuItemPos < menu->GetMenuItemCount())
    {
        menu->GetMenuItemInfo(m_menuItemPos, &menuInfo);

        // �����Ҫ���Ƶı���ͼƬ, ���ݵ�ǰmenuItem��״̬
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

    // ���Ʊ���ͼƬ
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
    // ��������
    if (menuInfo.type & MM_ITEMTYPE_STRING)
    {
        CRect  rectText = pDrawArgs->rcItem;
        CString  dispStr = menuInfo.caption;
        if (menuInfo.subitem)
        {
            //dispStr += L"  ��";
			dispStr += L"";
        }
        if (menuInfo.isChecked)
        {
            dispStr = L"��" + dispStr;
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
        // �½��˵�item, ������������˵����pos
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
// ����һ���������˵�����
//  ָ��itemWidth(���ӵĿ��), itemCount(������Ŀ)
//  ���д��ڵ���������
//  autoClip���Դ���������ӳ������ڿ��, �Ƿ��Զ��ü����ӵĿ��, 
//-----------------------------------------------------------------------------
//

#if CHUN_HUA
BOOL  CNaviToolBar::Create (int itemWidth, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip)
{
    ASSERT(!IsWindow(m_hWnd));

    // ����������
    CRect  wndRect = rect;
    BOOL  ret = MBSUPER::Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SINGLESEL, 
        wndRect, pParentWnd, nID);

    // ���ڲ���ý���
    MBSUPER::ShowWindow(SW_SHOWNOACTIVATE);
    MBSUPER::ModifyStyleEx(0, WS_EX_NOACTIVATE);


    // ���ô�����ʽ
    MBSUPER::SetHeaderHeight(0);
    MBSUPER::SetRowHeight(wndRect.Height());
    MBSUPER::SetRowFocusEnable(FALSE);
    MBSUPER::SetTextColor(MBT_TEXTCOLOR);

    // ��������
    int  totalWidth = 0;
    int  wndWidth = rect.right - rect.left;
    for (int i = 0; i < itemCount; i++)
    {
        // �Զ��ü��������ڴ�С�Ĳ���
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

    // ������ʱ��, ˢ��CmdUI
    SetTimer(1, 1000, NULL);
    
    return ret;
}

#else
BOOL  CNaviToolBar::Create (int itemHeight, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip)
{
    ASSERT(!IsWindow(m_hWnd));

    // ����������
    CRect  wndRect = rect;
    BOOL  ret = MBSUPER::Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SINGLESEL, 
        wndRect, pParentWnd, nID);


    // ���ڲ���ý���
    MBSUPER::ShowWindow(SW_SHOWNOACTIVATE);
    MBSUPER::ModifyStyleEx(0, WS_EX_NOACTIVATE);

 
    // ���ô�����ʽ
    MBSUPER::SetHeaderHeight(0);
    MBSUPER::SetRowHeight(itemHeight);
    MBSUPER::SetRowFocusEnable(TRUE);
    MBSUPER::SetTextColor(MBT_TEXTCOLOR);

    // ��������
//     int  totalHeight = 0;
//     int  wndHeight = rect.top - rect.bottom;
//     for (int i = 0; i < itemCount; i++)
//     {
//         // �Զ��ü��������ڴ�С�Ĳ���
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

    // ������ʱ��, ˢ��CmdUI
    SetTimer(1, 1000, NULL);

    return ret;
}

#endif
//
//-----------------------------------------------------------------------------
// ����Toolbar�����Ĳ˵�
// isAutoUpdateCmdUI -- �����Ƿ�ʱ�Զ�ˢ��CmdUI
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::SetNaviMenu (CNaviMenu* menu, BOOL isAutoUpdateCmdUI)
{
    // bug20130823: �л��˵�ǰ, �ر����еĵ����˵�
    //              ��ֹ�˵�������, ������л��˵����²˵��ڲ�ָ��ִ�д���Ķ���
    CloseNaviPopupBar();

    if (m_naviMenu)
    {
        // ����
        m_naviMenu->m_ownerToolBar = NULL;
    }

    m_naviMenu = menu;
    if (menu)
    {
        // ����
        menu->m_ownerToolBar = this;
    }

    // ˢ�¿ؼ�
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

        UpdateNaviMenu();                       // ˢ�����в˵�item����ʾ
        UpdateCmdUI(menu);                      // ���²˵�ʱ, ����ѯ�ʸ����˵�item��״̬
    }
    
    m_isAutoUpdateCmdUI = isAutoUpdateCmdUI;
}

//
//-----------------------------------------------------------------------------
// ����Toolbar�����Ŀ��COMMAND�����
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::SetCommandTarget (CWnd* target)
{
    m_commandTarget = target;
}



//
//=============================================================================
// ��Ϣ��Ӧ
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
    // ���ͽ���ˢ����ϢCN_UPDATE_COMMAND_UI
    if (nIDEvent == 1 && m_isAutoUpdateCmdUI)
    {
        UpdateCmdUI(m_naviMenu);
    }

    MBSUPER::OnTimer(nIDEvent);
}

void CNaviToolBar::OnSetFocus(CWnd* pOldWnd)
{
    MBSUPER::OnSetFocus(pOldWnd);

    // TODO: �ڴ˴������Ϣ����������
    ASSERT(0);  // �����ڲ��ܻ�ý���
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
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ���ݵ�ǰ���õ�menu����, ������ͼ
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::UpdateNaviMenu ()
{
    // bug2013.07.03: ԭ����ResetNaviMenu�����SetRowItemText��ʽȥ���´���
    // ��һ�����ӵ�����������ػ�ò�������, �ƺ���CListCtrl������, ��֪��Ϊʲô, 
    // ��ListCtrlEL��DrawItem����Ҳ������Ӧ���޲�����, ���������Ϊʹ��Invalidate ���´���
    //ResetNaviMenu();
    Invalidate(FALSE);
}

//
//-----------------------------------------------------------------------------
// ������в˵���
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
// ��Ӧ��������������Ϣ, key����0~N�����, ��Ӧ���ܴ����ҵĸ���
//  ����Ѿ�����Ϣ������, �򷵻�TRUE, ���򷵻�FALSE
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

            // �����ò˵���Ŀ
            m_naviMenu->HighlightMenuItem(recItem->m_menuItemPos, TRUE);
            UpdateNaviMenu();

            // �����Ӳ˵�, ��Ŀ�귢��command����
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

            // �ָ���Ŀ����
            m_naviMenu->HighlightMenuItem(recItem->m_menuItemPos, FALSE);
            UpdateNaviMenu();
        }
        else
        {
            // ��Ŀ�귢��command����
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
// ѯ��Ŀ�괰��, ˢ�²˵�����
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

            // popup item ��ID�޷���Ӧ����Ϣ�Ǳ�, Ŀǰ��֪����δ���
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
// �رյ�ǰ�����Ĳ˵�����
//-----------------------------------------------------------------------------
//
void  CNaviToolBar::CloseNaviPopupBar ()
{
    if (m_popupBar.GetSafeHwnd() && (m_popupBar.m_nFlags & WF_MODALLOOP))
    {
        m_popupBar.EndModalLoop(IDCANCEL);
    }
}
