#pragma once


#include "MBDefines.h"

#include "src/utils/ReportControl/Template/ListBoxExt.h"


int DoModal_NaviPopupBar(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu);
int DoModal_NaviPopupBarEx(UINT& retval, int& retmmPos, const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* menu, CNaviPopupBar* popupBar);


//
//-----------------------------------------------------------------------------
// CNaviPopupBar
//
//  从导航大按钮弹出的二级popup菜单窗口, 类似XTPPopupBar
//-----------------------------------------------------------------------------
//
class CNaviPopupBar: public CListBoxT_FlatBorder
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CNaviPopupBar ();
    virtual ~CNaviPopupBar ();

    int  AddMenuItem (int pos);
    int  GoModal (const CRect& rcItem, CWnd* pParentWnd, CNaviMenu* subMenu);
    int  RunModalLoopEx (DWORD dwFlags = 0);

    UINT  GetSelCmdId ();
    int  GetSelmmPos ();

protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnKillFocus (CWnd* pNewWnd);
    afx_msg void  OnSetFocus (CWnd* pOldWnd);
    afx_msg void  OnCaptureChanged (CWnd *pWnd);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void  MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    void  Cancel ();
    void  Apply ();
    void  Close (INT_PTR result);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CNaviMenu*              m_submenu;          // 本pop窗口所关联的子菜单项
    UINT                    m_selCmdId;         // 用户选择的CmdId
    int                     m_selmmPos;         // 用户选择的菜单索引位置
    BOOL                    m_bApply;

};
//
//-----------------------------------------------------------------------------
// 获得用户选中的命令ID
//-----------------------------------------------------------------------------
//
inline UINT  CNaviPopupBar::GetSelCmdId ()
{
    return m_selCmdId;
}
//
//-----------------------------------------------------------------------------
// 获得用户选中的项在当前菜单的索引位置
//-----------------------------------------------------------------------------
//
inline int  CNaviPopupBar::GetSelmmPos ()
{
    return m_selmmPos;
}
