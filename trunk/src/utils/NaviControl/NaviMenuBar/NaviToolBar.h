#pragma once


//
//=============================================================================
// 包含头文件
//=============================================================================
//
#include "MBDefines.h"

#include "NaviMenu.h"
#include "NaviPopupBar.h"

#include "src/utils/ReportControl/ListCtrlEL/ListCtrlEL.h"



//
//-----------------------------------------------------------------------------
// CNaviMenuRecordItem
//
//  菜单项的Item
//-----------------------------------------------------------------------------
//
class CNaviMenuRecordItem : public CELRecordItem
{
public:
    CNaviMenuRecordItem (int nPos);

public:
    void  SetMenuItemPos (int nPos);

public:
    virtual void  Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs);


protected:
    int                     m_menuItemPos;      // 本item与菜单项关联的indexPosition


    friend class CNaviToolBar;
};


//
//-----------------------------------------------------------------------------
// CNaviMenuRecord
//
//  菜单行
//-----------------------------------------------------------------------------
//
class CNaviMenuRecord: public CELRecord
{

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CNaviMenuRecord (int itemCount, CNaviMenu* menu = NULL);
    virtual ~CNaviMenuRecord ();

    void  SetNaviMenu (CNaviMenu* menu);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CNaviMenu*              m_menu;             // 本record关联的菜单

    friend class CNaviMenuRecordItem;
    friend class CNaviToolBar;
};
//
//-----------------------------------------------------------------------------
// 设置与record关联的菜单对象
//-----------------------------------------------------------------------------
//
inline void  CNaviMenuRecord::SetNaviMenu (CNaviMenu* menu)
{
    m_menu = menu;
}




//
//-----------------------------------------------------------------------------
// 设置本item与菜单项关联的indexPosition
//-----------------------------------------------------------------------------
//
inline void  CNaviMenuRecordItem::SetMenuItemPos (int nPos)
{
    m_menuItemPos = nPos;
}




//
//-----------------------------------------------------------------------------
// CNaviToolBar
//
//  导航大按钮显示控件窗口, 类似XTPToolBar, 一级菜单
//-----------------------------------------------------------------------------
//
class CNaviToolBar: public MBSUPER
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CNaviToolBar ();
    virtual ~CNaviToolBar ();
#if SU_FEI
	BOOL  Create (int itemWidth, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip = FALSE);
#else
	BOOL  Create (int itemHeight, int itemCount, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autoClip = FALSE);
#endif  
    void  SetHasGridline (BOOL hasHline, BOOL hasVline);
    void  SetGridlineColor (COLORREF color);
    void  SetBkColor (COLORREF cr);
    void  SetTextColor (COLORREF cr);
    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    void  SetDefaultItemBkImage (CBitmap* bkImage);
    CBitmap*  GetDefaultItemBkImage ();
public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    void  UpdateNaviMenu ();

    void  SetNaviMenu (CNaviMenu* menu, BOOL isAutoUpdateCmdUI = FALSE);
    CNaviMenu*  GetNaviMenu ();
    
    void  SetCommandTarget (CWnd* target);
    CWnd*  GetCommandTarget ();


protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()
    afx_msg void  OnTimer (UINT_PTR nIDEvent);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);
    afx_msg void  OnPaint ();
public:
    virtual BOOL  PreTranslateMessage (MSG* pMsg);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    void  ResetNaviMenu ();
    BOOL  OnNaviKeyDown (int key);
    void  UpdateCmdUI (CNaviMenu* menu);
    void  CloseNaviPopupBar ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CNaviMenu*              m_naviMenu;         // 本控件关联的菜单
    CWnd*                   m_commandTarget;    // 菜单项被用户点选时, 发送WM_COMMAND的目标窗口
    CBitmap*                m_defItemBkImage;   // 默认的一个Item的背景图片, 由外层用户去维护对象生命周期
    UINT                    m_vkmapBegin;       // 按键开始映射, 比如赋予VK_F1, 则 VK_F1~VK_F6就对应用户点击Col0~Col5
    BOOL                    m_isAutoUpdateCmdUI;// 是否自动定时发CmdUI命令刷新菜单当前状态
    CNaviPopupBar           m_popupBar;         // 弹出菜单控件对象

#if !SU_FEI
	CBitmap*				m_switchBkImage;	// 显示既有底亮图片又有高亮文字的图片
	
#endif
};


//
//=============================================================================
// 简单的inline函数实现
//=============================================================================
//
inline CNaviMenu*  CNaviToolBar::GetNaviMenu ()
{
    return m_naviMenu;
}
inline CWnd*  CNaviToolBar::GetCommandTarget ()
{
    return m_commandTarget;
}
inline void  CNaviToolBar::SetHasGridline (BOOL hasHline, BOOL hasVline)
{
    MBSUPER::SetHasGridline(hasHline, hasVline);
}
inline void  CNaviToolBar::SetGridlineColor (COLORREF color)
{
    MBSUPER::SetHeaderGridlineColor(color);
}
inline void  CNaviToolBar::SetBkColor (COLORREF cr)
{
    MBSUPER::SetBkColor(cr);
}
inline void  CNaviToolBar::SetTextColor (COLORREF cr)
{
    MBSUPER::SetTextColor(cr);
}
inline void  CNaviToolBar::SetFont (CFont* pFont, BOOL /*bRedraw*/)
{
    MBSUPER::SetFont(pFont);
}
inline void  CNaviToolBar::SetDefaultItemBkImage (CBitmap* bkImage)
{
    m_defItemBkImage = bkImage;
}
inline CBitmap*  CNaviToolBar::GetDefaultItemBkImage ()
{
    return m_defItemBkImage;
}
