#pragma once


#include "MBDefines.h"


//
//-----------------------------------------------------------------------------
// CNaviCmdUI
//
//  ��ӦCN_UPDATE_COMMAND_UIʱʹ�õĶ���, ����ʵʱ�޸Ĳ˵��������
//-----------------------------------------------------------------------------
//
class CNaviCmdUI: public CCmdUI
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CNaviCmdUI (CNaviMenu* menu, CNaviMenu* submenu);
    virtual ~CNaviCmdUI ();

public:
    //
    //=============================================================================
    // �������
    //=============================================================================
    //
    virtual void  Enable (BOOL bOn = TRUE);
    virtual void  SetCheck (int nCheck = 1);
    virtual void  SetRadio (BOOL bOn = TRUE);
    virtual void  SetText (LPCTSTR lpszText);


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CNaviMenu*              m_naviMenu;         // ������Menu
    CNaviMenu*              m_naviSubMenu;      // m_naviMenu����
};



//
//-----------------------------------------------------------------------------
// �˵�item������
//-----------------------------------------------------------------------------
//
enum  MM_ItemType
{
    MM_ITEMTYPE_SEPARATOR   = 1,                // �ָ���
    MM_ITEMTYPE_STRING      = 2,                // ���ַ���
    MM_ITEMTYPE_IMAGE       = 4,                // ��ͼƬ
    MM_ITEMTYPE_IMGSTR      = MM_ITEMTYPE_STRING|MM_ITEMTYPE_IMAGE,
    MM_ITEMTYPE_UNDEF       = -1
};
//
//-----------------------------------------------------------------------------
// ��ȡiteminfo��ѡ��ö��, ��GetMenuItemInfoֻ��Ҫ��ȡһ��������ʱ, ����ָ�����±�־
//-----------------------------------------------------------------------------
//
enum MM_GetItemInfoFlag
{
    MM_GET_TYPE             = 0x1,
    MM_GET_CAPTION          = 0x2,
    MM_GET_CMDID            = 0x4,
    MM_GET_SUBITEM          = 0x8,
    MM_GET_STATE            = 0x10,
    MM_GET_ALL              = 0xFFFFFFFF
};
//
//-----------------------------------------------------------------------------
// �˵�һ��Item������
//-----------------------------------------------------------------------------
//
struct MM_ItemInfo
{
    MM_ItemType             type;               // �˵�item������
    CString                 caption;            // �˵�item�ı����ַ���
    UINT                    cmdId;              // �˵�item������Command Id
    CNaviMenu*              subitem;            // �˵�item���Ӳ˵� ����ǿ��� popup, �ɱ������������
    BOOL                    isHighlight;        // �˵�item�Ƿ������ʾ
    BOOL                    isEnable;           // �˵�item�Ƿ�����, �����������Ӧ�����̵��ѡ�е���Ϣ
    BOOL                    isGrayed;           // �˵�item�Ƿ�һ���ʾ, ͨ��disable��ʱ���һ���ʾ
    BOOL                    isChecked;          // �˵�item�Ƿ�check, ��
    CBitmap*                normalImage;        // �˵�item�ı���ͼƬ, ���ʹ���߹�������
    CBitmap*                highlightImage;     // �˵�item�ĸ�������ͼƬ, ���ʹ���߹�������
    CBitmap*                grayedImage;        // �˵�item�Ļһ�����ͼƬ, ���ʹ���߹�������

public:
    MM_ItemInfo ()
    {
        type = MM_ITEMTYPE_STRING;
        // caption = L"";
        cmdId = 0;
        subitem = NULL;
        isHighlight = FALSE;
        isEnable = TRUE;
        isGrayed = FALSE;
        isChecked = FALSE;
        normalImage = NULL;
        highlightImage = NULL;
        grayedImage = NULL;
    }
};
typedef CArray<MM_ItemInfo*, MM_ItemInfo*>  MM_ItemInfoArray;


//
//-----------------------------------------------------------------------------
// CNaviMenu
//
//  �˵�������, ����������в˵�������, ��������ʾ���淽��Ĺ���
//-----------------------------------------------------------------------------
//
class CNaviMenu
{
public:
    enum TrackRetType
    {
        RET_CMDID,
        RET_MMPOS
    };

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CNaviMenu ();
    virtual ~CNaviMenu ();

    BOOL  TrackPopupMenu (const CRect& rcItem, CWnd* pWnd, CNaviPopupBar* popupBar, TrackRetType rt=RET_CMDID);


public:
    //
    //=============================================================================
    // ���ݲ���
    //=============================================================================
    //
    void  ResetMenu ();
    int  AppendMenuItem (MM_ItemType type, const CString& caption = L"", UINT cmdId = 0, CNaviMenu* subitem = NULL);
    void  GetMenuItemInfo (int pos, MM_ItemInfo* retInfo, MM_GetItemInfoFlag flag = MM_GET_ALL);
    CNaviMenu*  GetSubMenu (int pos);
    int  GetMenuItemCount ();
    void  LoadMenu (CMenu* menu);
    

    void  SetMenuItemType (int pos, MM_ItemType type);
    void  SetMenuItemCaption (int pos, const CString& caption);
    void  SetMenuItemId (int pos, UINT cmdId);
    void  SetMenuSubItem (int pos, CNaviMenu* subitem);

    void  HighlightMenuItem (int pos, BOOL bHighlight);
    void  EnableMenuItem (int pos, BOOL bEnable);
    void  CheckMenuItem (int pos, BOOL bCheck);
    void  SetMenuItemBitmaps (int pos, CBitmap* normalBitmap, 
        CBitmap* highlightBitmap = NULL, CBitmap* grayBitmap = NULL);

private:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //



private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CNaviToolBar*           m_ownerToolBar;     // ����, ������ToolBar, ��ToolBarSetMenu��ʱ��������ջ�����
    MM_ItemInfoArray        m_itemArray;        // �˵��������


    friend class CNaviToolBar;
};

inline int  CNaviMenu::GetMenuItemCount ()
{
    return m_itemArray.GetSize();
}
