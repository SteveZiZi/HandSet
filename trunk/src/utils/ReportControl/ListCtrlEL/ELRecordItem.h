#pragma once



#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// report消息传递参数
//-----------------------------------------------------------------------------
//
struct EL_RECORDITEM_ARGS
{
    EL_RECORDITEM_ARGS ();
    EL_RECORDITEM_ARGS (CListCtrlEL* pControl, int rowIndex, int colIndex);
    virtual ~EL_RECORDITEM_ARGS ();


    CListCtrlEL*            pControl;           // Pointer to the main Report control.
    int                     rowIndex;           // Pointer to the associated row.
    int                     colIndex;           // Report column at click position, if any, NULL otherwise.
    CELRecordItem*          pItem;              // Pointer to the associated item.
    CRect                   rcItem;             // Item position in control client coordinates.
};


//
//-----------------------------------------------------------------------------
// report的item自绘参数
//-----------------------------------------------------------------------------
//
struct EL_RECORDITEM_DRAWARGS : public EL_RECORDITEM_ARGS
{
    EL_RECORDITEM_DRAWARGS()
    {
        nTextAlign = EL_LEFT;//EL_CENTER;
        pDC = NULL;

        isFocusedRow = FALSE;
        isSelectionRow = FALSE;
        isFocusedCol = FALSE;
    }

    CDC*                    pDC;                // Pointer to control drawing context.
    ELAlignment             nTextAlign;         // Text alignment mode DT_LEFT || DT_RIGHT || DT_CENTER

    BOOL                    isFocusedRow;       // 当前绘制的item是否处于焦点行
    BOOL                    isSelectionRow;     // 当前绘制的item是否处于选择行
    BOOL                    isFocusedCol;       // 当前绘制的item是否处于焦点列
};

//
//-----------------------------------------------------------------------------
// report的item通知消息参数
//-----------------------------------------------------------------------------
//
struct ELNM_RECORDITEM_ARGS
{
    NMHDR                   hdr;                // Standard structure, containing information about a notification message.
    int                     rowIndex;           // Pointer to the row associated with the notification.
    CELRecordItem*          pItem;              // Pointer to the record item associated with the notification.
    int                     colIndex;           // Pointer to the column associated with the notification. 
};

//
//-----------------------------------------------------------------------------
// report的item的下拉项被选中参数
//-----------------------------------------------------------------------------
//
struct ELNM_INPLACELIST_ARGS : public ELNM_RECORDITEM_ARGS
{
    CELInplaceList*         pInplaceList;       // 当前的下拉控件
    CELRecordItemConstraint*  pConstraint;      // 被选中的下拉项
};
//
//-----------------------------------------------------------------------------
// report的item的内置按钮被点击参数
//-----------------------------------------------------------------------------
//
struct ELNM_INPLACEBUTTON_ARGS : public ELNM_RECORDITEM_ARGS
{
    CELInplaceButton*       pButton;            // Pointer to in-place button
};


//
//-----------------------------------------------------------------------------
// CELRecordItemConstraint
//
//  item可以输入或显示的值的约束(下拉菜单元素)
//  该对象相当于item下拉菜单的一个菜单项, 可以限制item的可选值, 
//  当然也可以不限制, 只是提供用户快捷的选项输入
//-----------------------------------------------------------------------------
//
class CELRecordItemConstraint
{

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELRecordItemConstraint ();

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    int  GetIndex ();
    CString  GetConstraintCaption ();
    DWORD_PTR  GetConstraintData ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CString                 m_strConstraint;    // Caption text of constraint.  This is the text displayed for this constraint.
    DWORD_PTR               m_dwData;           // The 32-bit value associated with the item.
    int                     m_nIndex;           // Index of constraint.


    friend class CELRecordItem;
    friend class CELRecordItemEditOptions;
    friend class CELInplaceList;
};
//
//-----------------------------------------------------------------------------
// 获得本选项的绑定的标题, 数据等
//-----------------------------------------------------------------------------
//
inline int  CELRecordItemConstraint::GetIndex ()
{
    return m_nIndex;
}
inline CString  CELRecordItemConstraint::GetConstraintCaption ()
{
    return m_strConstraint;
}
inline DWORD_PTR  CELRecordItemConstraint::GetConstraintData ()
{
    return m_dwData;
}


//
//-----------------------------------------------------------------------------
// CELRecordItemConstraints
//
//  item约束项的集合容器, 相当于一个数组
//-----------------------------------------------------------------------------
//
class CELRecordItemConstraints
{

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELRecordItemConstraints ();
    ~CELRecordItemConstraints ();

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    int  GetCount ();
    void  RemoveAll ();
    CELRecordItemConstraint*  GetAt (int nIndex);

private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    CArray<CELRecordItemConstraint*, CELRecordItemConstraint*>  m_arrConstraints;  // Collection of constraints

    friend class CELRecordItem;
    friend class CELRecordItemEditOptions;
};


//
//-----------------------------------------------------------------------------
// CELInplaceButtons
//
//  item上的按钮容器, 一个item可以放若干的按钮, 包括Combobox按钮, 以及普通的按钮
//-----------------------------------------------------------------------------
//
class CELInplaceButtons : public CArray<CELInplaceButton*, CELInplaceButton*>
{
public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //

};


//
//-----------------------------------------------------------------------------
// CELRecordItemEditOptions
//
//  item可选项管理器, 用于管理item下拉菜单可以显示的项目, 是否受到下拉项目的约束
//  是否有下拉按钮, 是否有其他自定义按钮等,
//  每个col都附有这个CELRecordItemEditOptions对象, 代表本列的所有item遵循这个约束
//  每个item也可以有自己的CELRecordItemEditOptions, 代表本item不遵循本列的约束
//-----------------------------------------------------------------------------
//
class CELRecordItemEditOptions
{

public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELRecordItemEditOptions ();
    ~CELRecordItemEditOptions ();

    void  SetAllowEdit (BOOL bAllow);
    void  SetConstraintEdit (BOOL bConstraint);
    BOOL  IsSelectTextOnEdit ();

public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    CELRecordItemConstraint*  AddConstraint (LPCTSTR lpszConstraint, DWORD_PTR dwData = 0);
    CELRecordItemConstraint*  FindConstraint (DWORD_PTR dwData);
    CELRecordItemConstraint*  FindConstraint (LPCTSTR lpszConstraint);
    CELRecordItemConstraints*  GetConstraints ();
    CELInplaceButton*  AddComboButton ();
    CELInplaceButton*  AddExpandButton ();
    CELInplaceButton*  GetButton (int nIndex);
    void  RemoveButtons ();


private:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    BOOL                    m_bAllowEdit;       // TRUE to add an edit box to this item.
    BOOL                    m_bConstraintEdit;  // If TRUE, then you can only choose from the list of constraints added, If FALSE, then you can type a custom response not listed in the list of constraints.
    CELRecordItemConstraints*  m_pConstraints;  // Constraint list.
    BOOL                    m_bSelectTextOnEdit;// Select all text on edit

    CELInplaceButtons       arrInplaceButtons;  // Array of in-place buttons.


    friend class CELRecordItem;
};

inline void  CELRecordItemEditOptions::SetAllowEdit (BOOL bAllow)
{
    m_bAllowEdit = bAllow;
}
inline void  CELRecordItemEditOptions::SetConstraintEdit (BOOL bConstraint)
{
    m_bConstraintEdit = bConstraint;
}
inline BOOL  CELRecordItemEditOptions::IsSelectTextOnEdit ()
{
    return m_bSelectTextOnEdit;
}


//
//-----------------------------------------------------------------------------
// CELRecordItem
//
//  report控件中的最小单元item, 也就是一个小格, 
//  本类管理小格的显示内容, 颜色, 字体, 是否可编辑, 是否有checkbox, 是否有可选项
//  一个record就是由若干的item构成
//-----------------------------------------------------------------------------
//
class CELRecordItem
{


public:
    //
    //=============================================================================
    // 控件创建及样式
    //=============================================================================
    //
    CELRecordItem ();
    virtual ~CELRecordItem ();

    void  SetFocusable (BOOL bFocusable);
    BOOL  IsFocusable ();


public:
    //
    //=============================================================================
    // 控件操作
    //=============================================================================
    //
    virtual void  SetCaption (LPCTSTR strCaption);
    virtual CString  GetCaption ();
    virtual CString  GetDispCaption ();

    virtual void  SetFormatString (LPCTSTR strFormat);
    virtual CString  GetFormatString ();
    
    virtual void  SetItemData (DWORD_PTR data);
    virtual DWORD_PTR  GetItemData ();

    virtual void  SetEditable(BOOL bEditable = TRUE);
    virtual BOOL  IsEditable ();
    virtual void  HasCheckbox (BOOL bHasCheckbox);
    virtual BOOL  GetHasCheckbox (); 
    virtual void  SetChecked (BOOL bChecked);
    virtual BOOL  IsChecked ();

    virtual BOOL  IsAllowEdit (EL_RECORDITEM_ARGS* pItemArgs);
    CELRecordItemEditOptions*  GetEditOptions (CELColumn* pColumn);

    void  SetColor (COLORREF textColor, COLORREF bkColor);

public:
    //
    //=============================================================================
    // UI响应
    //=============================================================================
    //
    virtual void  Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs);
    virtual BOOL  OnChar (EL_RECORDITEM_ARGS* pItemArgs, UINT nChar);
    virtual void  OnInplaceButtonDown (EL_RECORDITEM_ARGS* pItemArgs, CELInplaceButton* pButton);
    virtual void  OnBeginEdit (EL_RECORDITEM_ARGS* pItemArgs);
    virtual void  OnEndEdit (CListCtrlEL* pControl);
    virtual void  OnValidateEdit (EL_RECORDITEM_ARGS* pItemArgs);
    virtual void  OnCancelEdit (EL_RECORDITEM_ARGS* pItemArgs);
    virtual void  OnConstraintChanged (EL_RECORDITEM_ARGS* pItemArgs, CELRecordItemConstraint* pConstraint);
    virtual DWORD  GetSelectedConstraintData (EL_RECORDITEM_ARGS* pItemArgs);
    virtual void  OnEditChanged (EL_RECORDITEM_ARGS* pItemArgs, LPCTSTR szText);
    virtual BOOL  OnEditChanging (EL_RECORDITEM_ARGS* pItemArgs, CString& rstrNewText);

private:
    //
    //=============================================================================
    // 辅助
    //=============================================================================
    //
    virtual CRect  CalcCaptionRect (const CRect& rcItem);

protected:
    //
    //=============================================================================
    // 内部状态属性变量
    //=============================================================================
    //
    COLORREF                m_clrText;          // Storage for item text color.
    COLORREF                m_clrBackground;    // Storage for item background color.
    BOOL                    m_bBoldText;        // Is bold text.
    BOOL                    m_bEditable;        // Stores editable flag.
    CELRecord*              m_pRecord;          // Pointer to the associated record item, if any, or NULL otherwise.
    CString                 m_strCaption;       // Caption of the item
    CString                 m_strFormatString;  // Stores string for extra formatting of item caption.
    BOOL                    m_bFocusable;       // TRUE if item accept focus.
    BOOL                    m_bChecked;         // TRUE if item checked.
    BOOL                    m_bHasCheckbox;     // TRUE if item has check box.
    DWORD_PTR               m_dwData;           // The 32-bit value associated with the item.
    CELRecordItemEditOptions*  m_pEditOptions;  // Edit options of the item., 当该对象是空时, 就使用当列col的EditOptions

    friend class CELRecord;
    friend class CListCtrlEL;
};


//
//-----------------------------------------------------------------------------
// 设置及获得item的文本, 显示可能是经过格式化的GetDispCaption
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetCaption (LPCTSTR strCaption)
{
    m_strCaption = strCaption;
}
inline CString  CELRecordItem::GetCaption ()
{
    return m_strCaption;
}
//
//-----------------------------------------------------------------------------
// 获得格式化后的文本, 纯粹用于显示
//-----------------------------------------------------------------------------
//
inline CString  CELRecordItem::GetDispCaption ()
{
    return GetCaption();
}
//
//-----------------------------------------------------------------------------
// 设置格式化文本, 比如 "%s", "xxx %d"
//-----------------------------------------------------------------------------
//
inline CString  CELRecordItem::GetFormatString ()
{
    return m_strFormatString;
}
inline void  CELRecordItem::SetFormatString (LPCTSTR strFormat)
{
    m_strFormatString = strFormat;
}
//
//-----------------------------------------------------------------------------
// 设置获得与item关联的用户自定义数据
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetItemData (DWORD_PTR data)
{
    m_dwData = data;
}
inline DWORD_PTR  CELRecordItem::GetItemData ()
{
    return m_dwData;
}


//
//-----------------------------------------------------------------------------
// 设置该item对象是否可单独选中
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetFocusable (BOOL bFocusable)
{
    m_bFocusable = bFocusable;
}
inline BOOL  CELRecordItem::IsFocusable ()
{
    return m_bFocusable;
}

//
//-----------------------------------------------------------------------------
// 设置该item对象是否可编辑
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetEditable (BOOL bEditable)
{
    m_bEditable = bEditable;
}
inline BOOL  CELRecordItem::IsEditable ()
{
    return m_bEditable;
}
//
//-----------------------------------------------------------------------------
// 设置该item对象是否有复选框
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::HasCheckbox (BOOL bHasCheckbox)
{
    m_bHasCheckbox = bHasCheckbox;
}
inline BOOL  CELRecordItem::GetHasCheckbox ()
{
    return m_bHasCheckbox;
}
//
//-----------------------------------------------------------------------------
// 设置该item对象是否被选中
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetChecked (BOOL bChecked)
{
    m_bChecked = bChecked;
}
inline BOOL  CELRecordItem::IsChecked ()
{
    return m_bChecked;
}
//
//-----------------------------------------------------------------------------
// 设置item颜色
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetColor (COLORREF textColor, COLORREF bkColor)
{
    m_clrText = textColor;
    m_clrBackground = bkColor;
}