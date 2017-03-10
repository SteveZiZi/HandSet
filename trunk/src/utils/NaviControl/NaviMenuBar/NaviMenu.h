#pragma once


#include "MBDefines.h"


//
//-----------------------------------------------------------------------------
// CNaviCmdUI
//
//  响应CN_UPDATE_COMMAND_UI时使用的对象, 用于实时修改菜单项的属性
//-----------------------------------------------------------------------------
//
class CNaviCmdUI: public CCmdUI
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CNaviCmdUI (CNaviMenu* menu, CNaviMenu* submenu);
    virtual ~CNaviCmdUI ();

public:
    //
    //=============================================================================
    // 对象操作
    //=============================================================================
    //
    virtual void  Enable (BOOL bOn = TRUE);
    virtual void  SetCheck (int nCheck = 1);
    virtual void  SetRadio (BOOL bOn = TRUE);
    virtual void  SetText (LPCTSTR lpszText);


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CNaviMenu*              m_naviMenu;         // 关联的Menu
    CNaviMenu*              m_naviSubMenu;      // m_naviMenu的子
};



//
//-----------------------------------------------------------------------------
// 菜单item的类型
//-----------------------------------------------------------------------------
//
enum  MM_ItemType
{
    MM_ITEMTYPE_SEPARATOR   = 1,                // 分割线
    MM_ITEMTYPE_STRING      = 2,                // 纯字符串
    MM_ITEMTYPE_IMAGE       = 4,                // 纯图片
    MM_ITEMTYPE_IMGSTR      = MM_ITEMTYPE_STRING|MM_ITEMTYPE_IMAGE,
    MM_ITEMTYPE_UNDEF       = -1
};
//
//-----------------------------------------------------------------------------
// 获取iteminfo的选择枚举, 当GetMenuItemInfo只需要获取一部分属性时, 可以指定以下标志
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
// 菜单一个Item的属性
//-----------------------------------------------------------------------------
//
struct MM_ItemInfo
{
    MM_ItemType             type;               // 菜单item的类型
    CString                 caption;            // 菜单item的标题字符串
    UINT                    cmdId;              // 菜单item关联的Command Id
    CNaviMenu*              subitem;            // 菜单item的子菜单 如果非空则 popup, 由本对象管理生命
    BOOL                    isHighlight;        // 菜单item是否高亮显示
    BOOL                    isEnable;           // 菜单item是否启用, 如果启用则响应鼠标键盘点击选中的消息
    BOOL                    isGrayed;           // 菜单item是否灰化显示, 通常disable的时候会灰化显示
    BOOL                    isChecked;          // 菜单item是否被check, 打钩
    CBitmap*                normalImage;        // 菜单item的背景图片, 外层使用者管理生命
    CBitmap*                highlightImage;     // 菜单item的高亮背景图片, 外层使用者管理生命
    CBitmap*                grayedImage;        // 菜单item的灰化背景图片, 外层使用者管理生命

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
//  菜单项容器, 负责管理所有菜单项属性, 不负责显示界面方面的工作
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
    // 控件创建及样式
    //=============================================================================
    //
    CNaviMenu ();
    virtual ~CNaviMenu ();

    BOOL  TrackPopupMenu (const CRect& rcItem, CWnd* pWnd, CNaviPopupBar* popupBar, TrackRetType rt=RET_CMDID);


public:
    //
    //=============================================================================
    // 数据操作
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
    // 辅助
    //=============================================================================
    //



private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CNaviToolBar*           m_ownerToolBar;     // 反向, 所属的ToolBar, 在ToolBarSetMenu的时候会进行清空或设置
    MM_ItemInfoArray        m_itemArray;        // 菜单项的容器


    friend class CNaviToolBar;
};

inline int  CNaviMenu::GetMenuItemCount ()
{
    return m_itemArray.GetSize();
}
