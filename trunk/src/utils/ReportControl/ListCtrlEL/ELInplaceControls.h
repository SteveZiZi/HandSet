#pragma once


#include "ELDefines.h"
#include "ELRecordItem.h"

#include "../Template/ListBoxExt.h"

//
//-----------------------------------------------------------------------------
// CELInplaceControl
//
//  inplace�ؼ�����, ����inplace�ؼ��д�����, 
//  ͨ��inplace�ؼ�����һ����ʵ�ĵ�����, ģ̬�Ĵ���
//-----------------------------------------------------------------------------
//
class CELInplaceControl: public EL_RECORDITEM_ARGS
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELInplaceControl ();
    virtual ~CELInplaceControl ();

public:
    //
    //=============================================================================
    // �ؼ�����
    //=============================================================================
    //
	virtual void  SetItemArgs (EL_RECORDITEM_ARGS* pItemArgs);
};



//
//-----------------------------------------------------------------------------
// CELInplaceEdit
//
//  inplace�ı��༭������, �ÿؼ�ͨ����report���ؼ�һ�����Ͳ���, ֱ��report����
//  �м������Ҫ�ô��ھ��ں��ʵ�λ����ʾ, ����Ҫʱ������
//-----------------------------------------------------------------------------
//
class CELInplaceEdit : public CEdit, public CELInplaceControl
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
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
    // ��Ϣ����
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
    // ����
    //=============================================================================
    //
    void  ApplyInput ();
    void  CancelInput ();


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    CELRecordItemConstraint*  m_pSelectedConstraint;  // Selected constraint.
    COLORREF                m_clrText;          // Text color of in-place edit.
    COLORREF                m_bkColor;          // ������ɫ
    CBrush                  m_bkBrush;          // ����ɫ��ˢ
    COLORREF                m_borderColor;      // �߿���ɫ

    bool                    m_bCancel;          // �û��Ƿ�ȡ���˱༭

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
// ��ñ�inplace�ؼ���������item, ͨ���Ǳ༭����ʱ�������
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
//  inplace�����˵�����, �ÿؼ�ͨ����report���ؼ�һ�����Ͳ���, ֱ��report����
//  �м������Ҫ�ô��ھ��ں��ʵ�λ����ʾ, ����Ҫʱ������
//-----------------------------------------------------------------------------
//
class CELInplaceList : public CListBoxT_FlatBorder, public CELInplaceControl
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
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
    // ��Ϣ����
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
    // ����
    //=============================================================================
    //
    void  Cancel ();
    void  Apply ();


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    BOOL                    m_bApply;


    friend class CELRecordItem;
};


//
//-----------------------------------------------------------------------------
// CELInplaceButton
//
//  ���ⰴť, ����item��, ������ӦOK�������, �������ڽ������Ի�һ����ť
//-----------------------------------------------------------------------------
//
class CELInplaceButton
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CELInplaceButton (ELInplaceButtonType nID);

    ELInplaceButtonType  GetID ();
    CELRecordItem*  GetItem ();
    int  GetWidth ();


public:
    //
    //=============================================================================
    // UI��Ӧ
    //=============================================================================
    //
    virtual void  Draw (EL_RECORDITEM_DRAWARGS* pDrawArgs);


private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    ELInplaceButtonType     m_nID;              // Identifier of the button.
    int                     m_nWidth;           // Width of the button.

    COLORREF                m_crColor;          // �����ɫ


    friend class CELRecordItem;
};

//
//-----------------------------------------------------------------------------
// ��ð�ť�Ķ����ID, ������Ӧ��ť�¼�ʱ�ж�
//-----------------------------------------------------------------------------
//
inline ELInplaceButtonType  CELInplaceButton::GetID ()
{
    return m_nID;
}
//
//-----------------------------------------------------------------------------
// ��ð�ť�Ŀ��
//-----------------------------------------------------------------------------
//
inline int  CELInplaceButton::GetWidth ()
{
    return m_nWidth;
}