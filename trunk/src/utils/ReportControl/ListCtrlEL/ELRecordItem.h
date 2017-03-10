#pragma once



#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// report��Ϣ���ݲ���
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
// report��item�Ի����
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

    BOOL                    isFocusedRow;       // ��ǰ���Ƶ�item�Ƿ��ڽ�����
    BOOL                    isSelectionRow;     // ��ǰ���Ƶ�item�Ƿ���ѡ����
    BOOL                    isFocusedCol;       // ��ǰ���Ƶ�item�Ƿ��ڽ�����
};

//
//-----------------------------------------------------------------------------
// report��item֪ͨ��Ϣ����
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
// report��item�������ѡ�в���
//-----------------------------------------------------------------------------
//
struct ELNM_INPLACELIST_ARGS : public ELNM_RECORDITEM_ARGS
{
    CELInplaceList*         pInplaceList;       // ��ǰ�������ؼ�
    CELRecordItemConstraint*  pConstraint;      // ��ѡ�е�������
};
//
//-----------------------------------------------------------------------------
// report��item�����ð�ť���������
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
//  item�����������ʾ��ֵ��Լ��(�����˵�Ԫ��)
//  �ö����൱��item�����˵���һ���˵���, ��������item�Ŀ�ѡֵ, 
//  ��ȻҲ���Բ�����, ֻ���ṩ�û���ݵ�ѡ������
//-----------------------------------------------------------------------------
//
class CELRecordItemConstraint
{

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELRecordItemConstraint ();

public:
    //
    //=============================================================================
    // �ؼ�����
    //=============================================================================
    //
    int  GetIndex ();
    CString  GetConstraintCaption ();
    DWORD_PTR  GetConstraintData ();


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
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
// ��ñ�ѡ��İ󶨵ı���, ���ݵ�
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
//  itemԼ����ļ�������, �൱��һ������
//-----------------------------------------------------------------------------
//
class CELRecordItemConstraints
{

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELRecordItemConstraints ();
    ~CELRecordItemConstraints ();

public:
    //
    //=============================================================================
    // �ؼ�����
    //=============================================================================
    //
    int  GetCount ();
    void  RemoveAll ();
    CELRecordItemConstraint*  GetAt (int nIndex);

private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
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
//  item�ϵİ�ť����, һ��item���Է����ɵİ�ť, ����Combobox��ť, �Լ���ͨ�İ�ť
//-----------------------------------------------------------------------------
//
class CELInplaceButtons : public CArray<CELInplaceButton*, CELInplaceButton*>
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //

};


//
//-----------------------------------------------------------------------------
// CELRecordItemEditOptions
//
//  item��ѡ�������, ���ڹ���item�����˵�������ʾ����Ŀ, �Ƿ��ܵ�������Ŀ��Լ��
//  �Ƿ���������ť, �Ƿ��������Զ��尴ť��,
//  ÿ��col���������CELRecordItemEditOptions����, �����е�����item��ѭ���Լ��
//  ÿ��itemҲ�������Լ���CELRecordItemEditOptions, ����item����ѭ���е�Լ��
//-----------------------------------------------------------------------------
//
class CELRecordItemEditOptions
{

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
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
    // �ؼ�����
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
    // �ڲ�״̬���Ա���
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
//  report�ؼ��е���С��Ԫitem, Ҳ����һ��С��, 
//  �������С�����ʾ����, ��ɫ, ����, �Ƿ�ɱ༭, �Ƿ���checkbox, �Ƿ��п�ѡ��
//  һ��record���������ɵ�item����
//-----------------------------------------------------------------------------
//
class CELRecordItem
{


public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELRecordItem ();
    virtual ~CELRecordItem ();

    void  SetFocusable (BOOL bFocusable);
    BOOL  IsFocusable ();


public:
    //
    //=============================================================================
    // �ؼ�����
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
    // UI��Ӧ
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
    // ����
    //=============================================================================
    //
    virtual CRect  CalcCaptionRect (const CRect& rcItem);

protected:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
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
    CELRecordItemEditOptions*  m_pEditOptions;  // Edit options of the item., ���ö����ǿ�ʱ, ��ʹ�õ���col��EditOptions

    friend class CELRecord;
    friend class CListCtrlEL;
};


//
//-----------------------------------------------------------------------------
// ���ü����item���ı�, ��ʾ�����Ǿ�����ʽ����GetDispCaption
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
// ��ø�ʽ������ı�, ����������ʾ
//-----------------------------------------------------------------------------
//
inline CString  CELRecordItem::GetDispCaption ()
{
    return GetCaption();
}
//
//-----------------------------------------------------------------------------
// ���ø�ʽ���ı�, ���� "%s", "xxx %d"
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
// ���û����item�������û��Զ�������
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
// ���ø�item�����Ƿ�ɵ���ѡ��
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
// ���ø�item�����Ƿ�ɱ༭
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
// ���ø�item�����Ƿ��и�ѡ��
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
// ���ø�item�����Ƿ�ѡ��
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
// ����item��ɫ
//-----------------------------------------------------------------------------
//
inline void  CELRecordItem::SetColor (COLORREF textColor, COLORREF bkColor)
{
    m_clrText = textColor;
    m_clrBackground = bkColor;
}