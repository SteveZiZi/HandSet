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
    m_bEnableChanged = TRUE;                    // ��Ҫ, ��¼�û��Ѿ��ı��������, ��ܲ����ٲ�һ��

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
    // ֪ͨm_ownerToolBar ȡ������
    if (m_ownerToolBar)
    {
        m_ownerToolBar->SetNaviMenu(NULL);
    }


    ResetMenu();
}

//
//-----------------------------------------------------------------------------
// ����popup�˵�
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
// ������в˵���, ���ͷ���Դ
//-----------------------------------------------------------------------------
//
void  CNaviMenu::ResetMenu ()
{
    for (int i = 0; i < m_itemArray.GetSize(); i++)
    {
        MM_ItemInfo*  itemInfo = m_itemArray.GetAt(i);
        ASSERT(itemInfo);

        // ɾ���Ӳ˵�
        if (itemInfo->subitem)
        {
            delete itemInfo->subitem;
        }

        // ɾ�����˵���
        delete itemInfo;
    }
    m_itemArray.RemoveAll();
}
//
//-----------------------------------------------------------------------------
// ���һ���˵���
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
// ��ò˵������Ϣ, ����ͨ��flag ����Ҫ��ȡ��Щ��Ϣ(�Ż�Ч��)
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
// ���ָ���˵�������Ķ����˵�
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
// ��cmenu�����������в˵���Ϣ, ������rc��Դ�ӿ�
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
        // ��ò˵�����Ϣ
        MENUITEMINFO info = {0};
        info.cbSize = sizeof (MENUITEMINFO);
        info.fMask = MIIM_SUBMENU|MIIM_TYPE|MIIM_STATE|MIIM_ID;
        info.dwTypeData = capbuf;
        info.cch = MAX_CAPTION_LEN;
        VERIFY(menu->GetMenuItemInfo(i, &info, TRUE));

        // ����
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

        // �Ӳ˵���
        CMenu*  sub = menu->GetSubMenu(i);
        CNaviMenu*  subitem = NULL;
        if (sub)
        {
            subitem = new CNaviMenu;
            subitem->LoadMenu(sub);
        }

        // ���һ��
        int  newpos = AppendMenuItem(type, caption, info.wID, subitem);

        // ������������
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
// ���ò˵��������
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
// ���ò˵������ʾ�ı�
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
// ���ò˵����WM_COMMAND������Id
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
// ���ò˵�����Ӳ˵�(�����˵�)
//-----------------------------------------------------------------------------
//
void  CNaviMenu::SetMenuSubItem (int pos, CNaviMenu* subitem)
{
    MM_ItemInfo*  itemInfo = m_itemArray.GetAt(pos);
    ASSERT(itemInfo);

    // �ͷ��ϵ�
    if (itemInfo->subitem)
    {
        delete itemInfo->subitem;
    }

    // �ӹ��µ�
    itemInfo->subitem = subitem;
}
//
//-----------------------------------------------------------------------------
// ���ò˵������״̬
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
// ���ò˵����Ƿ�����, ��disable���, ���disable����ܱ���ѡ
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
// ���ò˵����Ƿ�һ���ʾ
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
// ���ò˵���ı���λͼ
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
