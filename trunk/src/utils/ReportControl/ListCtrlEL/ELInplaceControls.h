#pragma once


#include "ELDefines.h"
#include "ELRecordItem.h"

#include "../Template/ListBoxExt.h"

//
//-----------------------------------------------------------------------------
// CELInplaceControl
//
//  inplace控件父类, 所有inplace控件有此派生, 
//  通常inplace控件都是一个真实的弹出的, 模态的窗口
//-----------------------------------------------------------------------------
//
class CELInplaceControl: public EL_RECORDITEM_ARGS
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELInplaceControl ();
    virtual ~CELInplaceControl ();

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
	virtual void  SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs);
};



//
//-----------------------------------------------------------------------------
// CELInplaceEdit
//
//  inplace文本编辑器窗口, 该控件通常在report主控件一创建就产生, 直到report销毁
//  中间如果需要该窗口就在合适的位置显示, 不需要时就隐藏
//-----------------------------------------------------------------------------
//
class CELInplaceEdit : public CEdit, public CELInplaceControl
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELInplaceEdit ();
    ~CELInplaceEdit ();
    void  SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs);

    void  Create (EL_RECORDITEM_ARGS* pItemArgs);
    void  HideWindow ();
    CELRecordItem*  GetItem ();

    void  SetBkColor (COLORREF color);
    void  SetTextColor (COLORREF color);
    void  SetBorderColor (COLORREF color);

    void  SetWindowText (LPCTSTR lpszString);

protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()
    afx_msg void  OnEnKillfocus ();
    afx_msg void  OnEnChange ();
    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg HBRUSH  CtlColor(CDC* pDC, UINT /*nCtlColor*/);
    afx_msg void  OnPaint ();

    afx_msg void  OnBtnBackspace ();
    afx_msg void  OnUpdateBtnBackspace (CCmdUI *pCmdUI);

    virtual BOOL  PreTranslateMessage (MSG* pMsg);


private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    void  ApplyInput ();
    void  CancelInput ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CELRecordItemConstraint*  m_pSelectedConstraint;  // Selected constraint.
    COLORREF                m_clrText;          // Text color of in-place edit.
    COLORREF                m_bkColor;          // 背景颜色
    CBrush                  m_bkBrush;          // 背景色画刷
    COLORREF                m_borderColor;      // 边框颜色

    bool                    m_bCancel;          // 用户是否取消了编辑

    CString                 m_strText_prev;     // The text value from previous change.
    BOOL                    m_bSetWindowText;   // Indicates that text is being set with SetWindowText function.

    friend class CELRecordItem;
};

//
//=============================================================================
// inline
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 获得本inplace控件所关联的item, 通常是编辑激活时会关联上
//-----------------------------------------------------------------------------
//
inline CELRecordItem*  CELInplaceEdit::GetItem ()
{
    return pItem;
}


//
//-----------------------------------------------------------------------------
// CELInplaceList
//
//  inplace下拉菜单窗口, 该控件通常在report主控件一创建就产生, 直到report销毁
//  中间如果需要该窗口就在合适的位置显示, 不需要时就隐藏
//-----------------------------------------------------------------------------
//
class CELInplaceList : public CListBoxT_FlatBorder, public CELInplaceControl
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELInplaceList ();
    void  SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs);

    void  Create (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraints* pConstaints);
    void  HideWindow ();
    CELRecordItem*  GetItem ();


protected:
    //
    //=============================================================================
    // 消息处理
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg UINT  OnGetDlgCode ();
    afx_msg void  OnKillFocus (CWnd* pNewWnd);
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnLButtonUp (UINT nFlags, CPoint point);
    
    virtual BOOL  PreTranslateMessage (MSG* pMsg);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    void  Cancel ();
    void  Apply ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    BOOL                    m_bApply;


    friend class CELRecordItem;
};


//
//-----------------------------------------------------------------------------
// CELInplaceButton
//
//  虚拟按钮, 放在item上, 可以响应OK按键点击, 并可以在界面上自绘一个按钮
//-----------------------------------------------------------------------------
//
class CELInplaceButton
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELInplaceButton (ELInplaceButtonType nID);

    ELInplaceButtonType  GetID ();
    CELRecordItem*  GetItem ();
    int  GetWidth ();


public:
    //
    //=============================================================================
    // UI响应
    //=============================================================================
    //
    virtual void  Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs);


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    ELInplaceButtonType     m_nID;              // Identifier of the button.
    int                     m_nWidth;           // Width of the button.

    COLORREF                m_crColor;          // 填充颜色


    friend class CELRecordItem;
};

//
//-----------------------------------------------------------------------------
// 获得按钮的定义的ID, 用于响应按钮事件时判断
//-----------------------------------------------------------------------------
//
inline ELInplaceButtonType  CELInplaceButton::GetID ()
{
    return m_nID;
}
//
//-----------------------------------------------------------------------------
// 获得按钮的宽度
//-----------------------------------------------------------------------------
//
inline int  CELInplaceButton::GetWidth ()
{
    return m_nWidth;
}