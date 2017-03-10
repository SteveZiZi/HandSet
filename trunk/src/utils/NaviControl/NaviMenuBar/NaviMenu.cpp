#include "stdafx.h"

#include "NaviToolBar.h"

//
//=============================================================================
// CNaviCmdUI
//=============================================================================
//

CNaviCmdUI::CNaviCmdUI (CNaviMenu* menu, CNaviMenu* submenu)
{
    m_naviMenu = menu;
    m_naviSubMenu = submenu;
}

CNaviCmdUI::~CNaviCmdUI ()
{

}


void  CNaviCmdUI::Enable (BOOL bOn)
{
    m_bEnableChanged = TRUE;                    // 重要, 记录用户已经改变过该属性, 框架不会再插一脚

    m_naviMenu->EnableMenuItem(m_nIndex, bOn);
}

void  CNaviCmdUI::SetCheck (int nCheck)
{
    m_naviMenu->CheckMenuItem(m_nIndex, nCheck);
}

void  CNaviCmdUI::SetRadio (BOOL bOn)
{
    m_naviMenu->CheckMenuItem(m_nIndex, bOn);
}

void  CNaviCmdUI::SetText (LPCTSTR lpszText)
{
    m_naviMenu->SetMenuItemCaption(m_nIndex, lpszText);
}


//
//=============================================================================
// CNaviMenu
//=============================================================================
//

CNaviMenu::CNaviMenu ()
{
    m_ownerToolBar = NULL;
    //m_itemArray;
}

CNaviMenu::~CNaviMenu ()
{
    // 通知m_ownerToolBar 取消关联
    if (m_ownerToolBar)
    {
        m_ownerToolBar->SetNaviMenu(NULL);
    }


    ResetMenu();
}

//
//-----------------------------------------------------------------------------
// 弹出popup菜单
//-----------------------------------------------------------------------------
//
BOOL  CNaviMenu::TrackPopupMenu (const CRect& rcItem, CWnd* pWnd, CNaviPopupBar* popupBar, TrackRetType rt)
{
    
    UINT  selCmdId;
    int  selmmPos;
    int  dlgret = DoModal_NaviPopupBarEx(selCmdId, selmmPos, rcItem, pWnd, this, popupBar);

    if (rt == RET_MMPOS)
    {
        return selmmPos;
    }
    else
    {
        return selCmdId;
    }
}

//
//-----------------------------------------------------------------------------
// 清空所有菜单项, 并释放资源
//-----------------------------------------------------------------------------
//
void  CNaviMenu::ResetMenu ()
{
    for (int i = 0; i < m_itemArray.GetSize(); i++)
    {
        MM_ItemInfo*  itemInfo = m_itemArray.GetAt(i);
        ASSERT(itemInfo);

        // 删除子菜单
        if (itemInfo->subitem)
        {
            delete itemInfo->subitem;
        }

        // 删除本菜单项
        delete itemInfo;
    }
    m_itemArray.RemoveAll();
}
//
//-----------------------------------------------------------------------------
// 添加一个菜单项
//-----------------------------------------------------------------------------
//
int  CNaviMenu::AppendMenuItem (MM_ItemType type, const CString& caption, UINT cmdId, CNaviMenu* subitem)
{
    MM_ItemInfo*  itemInfo = new MM_ItemInfo;
    ASSERT(itemInfo);

    itemInfo->type = type;
    itemInfo->caption = caption;
    itemInfo->cmdId = cmdId;
    itemInfo->subitem = subitem;

    return m_itemArray.Add(itemInfo);
}
//
//-----------------------------------------------------------------------------
// 获得菜单项的信息, 可以通过flag 设置要获取哪些信息(优化效率)
//-----------------------------------------------------------------------------
//
void  CNaviMenu::GetMenuItemInfo (int pos, MM_ItemInfo* retInfo, MM_GetItemInfoFlag flag)
{
    ASSERT(retInfo);
    
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    if (flag & MM_GET_TYPE)
    {
        retInfo->type = itemInfo->type;
    }
    if (flag & MM_GET_CAPTION)
    {
        retInfo->caption = itemInfo->caption;
    }
    if (flag & MM_GET_CMDID)
    {
        retInfo->cmdId = itemInfo->cmdId;
    }
    if (flag & MM_GET_SUBITEM)
    {
        retInfo->subitem = itemInfo->subitem;
    }
    if (flag & MM_GET_STATE)
    {
        retInfo->isHighlight = itemInfo->isHighlight;
        retInfo->isEnable = itemInfo->isEnable;
        retInfo->isGrayed = itemInfo->isGrayed;
        retInfo->isChecked = itemInfo->isChecked;
        retInfo->normalImage = itemInfo->normalImage;
        retInfo->highlightImage = itemInfo->highlightImage;
        retInfo->grayedImage = itemInfo->grayedImage;
    }
}
//
//-----------------------------------------------------------------------------
// 获得指定菜单项关联的二级菜单
//-----------------------------------------------------------------------------
//
CNaviMenu*  CNaviMenu::GetSubMenu (int pos)
{
    MM_ItemInfo  menuInfo;
    GetMenuItemInfo(pos, &menuInfo, MM_GET_SUBITEM);
    return menuInfo.subitem;
}
//
//-----------------------------------------------------------------------------
// 从cmenu对象载入所有菜单信息, 可以与rc资源接口
//-----------------------------------------------------------------------------
//
void  CNaviMenu::LoadMenu (CMenu* menu)
{
    ASSERT(menu && menu->GetSafeHmenu() && ::IsMenu(menu->GetSafeHmenu()));
    ResetMenu();

#define MAX_CAPTION_LEN     255
    wchar_t                 capbuf[MAX_CAPTION_LEN];
    for (int i = 0; i < menu->GetMenuItemCount(); i++)
    {
        // 获得菜单项信息
        MENUITEMINFO info = {0};
        info.cbSize = sizeof (MENUITEMINFO);
        info.fMask = MIIM_SUBMENU|MIIM_TYPE|MIIM_STATE|MIIM_ID;
        info.dwTypeData = capbuf;
        info.cch = MAX_CAPTION_LEN;
        VERIFY(menu->GetMenuItemInfo(i, &info, TRUE));

        // 类型
        MM_ItemType  type = MM_ITEMTYPE_UNDEF;
        CString  caption;
        if (info.fType == MFT_SEPARATOR)
        {
            type = MM_ITEMTYPE_SEPARATOR;
        }
        else
        {
            type = MM_ITEMTYPE_IMGSTR;

            caption = capbuf;
        }

        // 子菜单项
        CMenu*  sub = menu->GetSubMenu(i);
        CNaviMenu*  subitem = NULL;
        if (sub)
        {
            subitem = new CNaviMenu;
            subitem->LoadMenu(sub);
        }

        // 添加一项
        int  newpos = AppendMenuItem(type, caption, info.wID, subitem);

        // 设置其他属性
        if (info.fState & MF_HILITE)
        {
            HighlightMenuItem(newpos, TRUE);
        }
        if (info.fState & MF_GRAYED)
        {
            EnableMenuItem(newpos, FALSE);
        }
        if (info.fState & MF_CHECKED)
        {
            CheckMenuItem(newpos, TRUE);
        }
    }
}
//
//-----------------------------------------------------------------------------
// 设置菜单项的类型
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuItemType (int pos, MM_ItemType type)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->type = type;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项的显示文本
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuItemCaption (int pos, const CString& caption)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->caption = caption;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项的WM_COMMAND发出的Id
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuItemId (int pos, UINT cmdId)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->cmdId = cmdId;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项的子菜单(二级菜单)
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuSubItem (int pos, CNaviMenu* subitem)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    // 释放老的
    if (itemInfo->subitem)
    {
        delete itemInfo->subitem;
    }

    // 接管新的
    itemInfo->subitem = subitem;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项高亮状态
//-----------------------------------------------------------------------------
//
void  CNaviMenu::HighlightMenuItem (int pos, BOOL bHighlight)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->isHighlight = bHighlight;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项是否被启用, 与disable相对, 如果disable该项不能被点选
//-----------------------------------------------------------------------------
//
void  CNaviMenu::EnableMenuItem (int pos, BOOL bEnable)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->isEnable = bEnable;
    
    if (bEnable)
    {
        itemInfo->isGrayed = FALSE;
    }
    else
    {
        itemInfo->isGrayed = TRUE;
    }
}
//
//-----------------------------------------------------------------------------
// 设置菜单项是否灰化显示
//-----------------------------------------------------------------------------
//
void  CNaviMenu::CheckMenuItem (int pos, BOOL bCheck)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->isChecked = bCheck;
}
//
//-----------------------------------------------------------------------------
// 设置菜单项的背景位图
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuItemBitmaps (int pos, CBitmap* normalBitmap, 
                          CBitmap* highlightBitmap, CBitmap* grayBitmap)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    itemInfo->normalImage = normalBitmap;
    itemInfo->highlightImage = highlightBitmap;
    itemInfo->grayedImage = grayBitmap;
}
