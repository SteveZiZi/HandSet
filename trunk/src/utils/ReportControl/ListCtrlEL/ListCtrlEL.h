#pragma once


//
//=============================================================================
// ����ͷ�ļ�
//=============================================================================
//
#include "ELDefines.h"

#include "HeaderCtrlEL.h"
#include "ELColumn.h"
#include "ELRecord.h"
#include "ELRecordItem.h"
#include "ELRecordItemText.h"
#include "ELInplaceControls.h"
#include "../Common/CommonEL.h"



//
//-----------------------------------------------------------------------------
// CListCtrlEL
//
//  �ؼ��������, ���������ؼ��Ĳ���, ��ʾ, ����ά��
//
//  ע��: Ŀǰ�����Ǽ̳�CListCtrl, �������Ի�������CListCtrl�����Դ���, ���������չ���������ӵ�
//  ������ܻᵼ�´����Ķ��鷳, ����col��width, ������ɫ��ʽ, font, �� ��ԭ��������
//
//  2013.04.03 -    �޸�CListCtrlEL��CHeaderCtrlELΪ���м̳� CListCtrl..., �������и�����, 
//                  ʹ����Ӧ��ֻʹ��CListCtrlEL�ṩ�Ĺ���, ����Ҫ����ʹ�ø���Ĺ���, ��Ҫת����CListCtrl* ָ��ʹ��
//-----------------------------------------------------------------------------
//
class CListCtrlEL : public ELSUPER
{
    DECLARE_DYNAMIC(CListCtrlEL)

public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CListCtrlEL ();
    virtual ~CListCtrlEL ();
    virtual void  PreSubclassWindow ();
    BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    LRESULT  SendNotifyMessageToOwner (UINT nMessage, NMHDR* pNMHDR);
    LRESULT  SendMessageToParent (int rowIndex, CELRecordItem* pItem, int colIndex, UINT nMessage);
    void  PostInitTemplateStyle ();

    //
    //=============================================================================
    // header��ʽ
    //=============================================================================
    //
    void  SetHeaderHeight (int Height);
    void  SetHeaderGridlineColor (COLORREF color);
    void  SetHeaderBkColor (COLORREF color);
    void  SetHeaderTextColor (COLORREF color);
    // ע��SetHeaderTextFont, ������SetFont��������, ����ᱻ���߸���
    void  SetHeaderFont (CFont* pFont, BOOL bRedraw = TRUE);
    CFont*  GetHeaderFont ();


    //
    //=============================================================================
    // ������ʽ
    //=============================================================================
    //
    void  SetFreezeSelectionVisible (BOOL bShow);
    void  SetRowSelectionVisible (BOOL bShow);
    void  SetRowHeight (int nHeight);
    void  SetHasGridline (BOOL hasHline, BOOL hasVline);
    void  SetGradientGridline (BOOL hasHline, BOOL hasVline);
    BOOL  SetBkColor (COLORREF cr);
    BOOL  SetTextColor (COLORREF cr);
    void  SetFocusedSubItemHighlightColor (COLORREF cr);
    COLORREF  GetFocusedSubItemHighlightColor ();
    void  SetSelectionRowHighlightColor (COLORREF cr);
    void  SetTextHighlightColor (COLORREF cr);
    void  SetSelectionRowLowlightColor (COLORREF cr);

    void  SetFont (CFont* pFont, BOOL bRedraw = TRUE);
    CFont*  GetFont ();
#if SU_FEI
    void  SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap );
    void  GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap);
#else

	void  SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap ,CBitmap* checkFocusBitmap, CBitmap* uncheckFocusBitmap);
	void  GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap,CBitmap*& checkFocusBitmap, CBitmap*& uncheckFocusBitmap);
#endif

public:
    //
    //=============================================================================
    // �ؼ����Ʋ���
    //=============================================================================
    //

    CELInplaceEdit*  GetInplaceEdit ();
    CELInplaceList*  GetInplaceList ();
    BOOL  HasFocus();
    BOOL  IsEditMode ();
    void  EditItem (EL_RECORDITEM_ARGS* pItemArgs, BOOL bAutoFocus = TRUE);
    CELRecordItem*  GetCurrentActiveItem ();

    CRect  GetRowItemRect (int rowIndex, int colIndex);
    BOOL  EnsureRowVisibleScrollRow (int rowIndex);
    void  EnsureRowVisibleScrollPage (int rowIndex);
    BOOL  IsRowItemVisible (int rowIndex);
    int  GetTopRowIndex ();
    BOOL  RedrawRowItems (int rowIndexBegin, int rowIndexEnd);
    BOOL  RedrawRowItem (int rowIndex, int nSubItem);

    void  SetFreezeColumnsCount (int count);
    void  SetRowFocusEnable (BOOL bEnable);
    void  SetSubItemFocusEnable (BOOL bEnable);
    void  SetFocusedColIndex (int colIndex);
    int  GetFocusedColIndex ();
    void  SetFocusedSelectionRowIndex (int rowIndex);
    int  GetFocusedRowIndex ();
    int  GetSelectionRowIndex ();
    void  SetAutoRowEditEnable (BOOL bEnable);


    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    void  KeyBeginEdit ();
    void  KeyMoveDown ();
    void  KeyMoveUp ();
    void  KeyMoveLeft ();
    void  KeyMoveRight ();
    void  ScrollPageDown ();
    void  ScrollPageUp ();
    void  ScrollRowDown ();
    void  ScrollRowUp ();  
    void  AutoSelPageDown ();
    void  AutoSelPageUp ();
    BOOL  IsMoveLeftToPageUp();
    BOOL  IsMoveRightToPageDown();


public:
    //
    //=============================================================================
    // ���ݲ���
    //=============================================================================
    //
    UINT  MapIDToRowIndex (UINT  id);
    UINT  MapRowIndexToID (UINT  rowIndex);

    int  InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign = EL_CENTER, int colWidth = -1, 
        BOOL isEditable = FALSE, ELAlignment headerAlgin = EL_CENTER, COLORREF colBkColor = EL_COLOR_DEFAULT,
        int marginLeft = EL_ITEMDRAW_MARGIN, int marginRight = EL_ITEMDRAW_MARGIN);
    CELColumn*  GetColumnExAttr (int colIndex);

    int  InsertRecordRow (int rowIndex, CELRecord* newRecord);
    int  InsertRecordRow (int rowIndex, LPCTSTR lpszItem);              // �򵥽ӿ�
    CELRecord*  GetRecordRow (int rowIndex);
    void  SetRecordRowData (int rowIndex, DWORD_PTR data);              // �򵥽ӿ�
    DWORD_PTR  GetRecordRowData (int rowIndex);                         // �򵥽ӿ�
    void  SetRowItemText (int rowIndex, int nSubItem, LPCTSTR lpszText);// �򵥽ӿ�
    CString  GetRowItemText (int rowIndex, int nSubItem);               // �򵥽ӿ�
    void  SetRowItemData (int rowIndex, int nSubItem, DWORD_PTR data);  // �򵥽ӿ�
    DWORD_PTR  GetRowItemData (int rowIndex, int nSubItem);             // �򵥽ӿ�
    int  GetRecordRowCount ();
    void  DeleteRecordRow (int rowIndex);
    void  DeleteAllRecordRow ();


protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void  OnChar (UINT nChar, UINT nRepCntr, UINT nFlags);
    afx_msg void  OnLButtonDown (UINT nFlags, CPoint point);
    afx_msg void  OnLButtonUp (UINT nFlags, CPoint point);
    afx_msg void  OnLButtonDblClk (UINT nFlags, CPoint ptDblClick);
    afx_msg void  OnMeasureItem (int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void  MeasureItem (LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void  OnDestroy ();

    // ���⺯��, ���Ը����ඨ�����grid��
    virtual void  FillBkgnd (CDC* pDC);
    virtual void  FillGridline (CDC* pDC);
    virtual void  DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual BOOL  PreTranslateMessage(MSG* pMsg);


protected:
    //
    //=============================================================================
    // ��������
    //=============================================================================
    //
    void  ReleaseAllExAttr ();
    void  ReleaseAllRecordRowExAttr ();
    void  ReleaseRecordRowExAttr (int rowIndex);

    BOOL  IsValidRowIndex (int rowIndex);
    BOOL  IsValidColIndex (int colIndex);
    BOOL  IsFreezeColIndex (int colIndex);

    void  HideScrollBars ();
    void  SetListCtrlRgn ();

    void  CreatePaintMemBitmap (CDC* dc, int width, int height);

protected:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���, ��ʽ
    //=============================================================================
    //
    CHeaderCtrlEL           m_Header;
    CRgn                    m_wndRgnNoScroll;                           // ȥ����������windows����, ����SetWindowRgnȥ��������, ά��rgn�������������
    CBitmap                 m_paintMemMap;                              // OnPaint˫����ʱʹ�õ��ڴ�λͼ, �Ż�����ÿ�ζ�����

    int                     m_nRowHeight;                               // �����и�
    BOOL                    m_hasHGridline;                             // �Ƿ���ˮƽ������
    BOOL                    m_hasVGridline;                             // �Ƿ��д�ֱ������
    BOOL                    m_gradientHGridline;                        // �Ƿ񽥱��ˮƽ������
    BOOL                    m_gradientVGridline;                        // �Ƿ񽥱�Ĵ�ֱ������
    // bkcolor, textbkcolor, textcolor, font,  ��ʹ��CListctrl�����ṩ������
    BOOL                    m_bFreezeSelectionVisible;                  // ��ʹ�Ǳ��������, ѡ�������Ȼ��ʾ
    BOOL                    m_bRowSelectionVisible;                     // ѡ���еĸ����Ƿ�ɼ�
    COLORREF                m_focusedSubItemHighlightColor;             // ����item�ĸ�����ɫ
    COLORREF                m_selectionRowHighlightColor;               // ѡ���еĸ�����ɫ
    COLORREF                m_textHighlightColor;                       // ѡ���еĸ����ı���ɫ
    COLORREF                m_selectionRowLowlightColor;                // ѡ���еĵ�����ɫ, �����ڿؼ�ʧȥ����, ����LVS_SHOWSELALWAYS��־ʱ
    CBitmap*                m_checkBitmap;                              // ����ѡ��checkboxͼ��
    CBitmap*                m_uncheckBitmap;                            // δ����ѡ��checkboxͼ��

#if !SU_FEI
	CBitmap*				m_checkFocusBitmap;							// ����ʱѡ��
	CBitmap*				m_uncheckFocusBitmap;						// ����ʱδ��ѡ��
#endif
    //
    //=============================================================================
    // �ڲ�״̬���Ա���, ������ѡ��
    //=============================================================================
    //
    int                     m_nFreezeColumnsCount;  // Count of none-scrolled columns at the left side.
    BOOL                    m_bRowFocusEnable;  // TRUE if selection enabled. �����Ƿ��ܶ�λ������ĳ��, �������, �����°�ťֻ�ܷ�ҳ, �൱��ֻ�����
    BOOL                    m_bSubItemFocusEnable;  // TRUE if sub-items can receive focus. �Ƿ���Խ��㶨λ�����е�ĳ������
    int                     m_focusedColIndex;  // ��ǰ������, ��m_focusedRowIndex��������Ϳ��Ա�ʶһ������, ����m_focusedRowIndex ʹ��clistctrl�ṩ�Ľ���������
    BOOL                    m_bAutoRowEditEnable;  // �Ƿ������Զ��༭, ��ѡ�񽹵���ĳ��, ���н�����-1ʱ, ����OK�������Զ����ø��еĵ�һ���ɱ༭item����༭״̬

    //
    //=============================================================================
    // �ڲ�״̬���Ա���: ����
    //=============================================================================
    //
    CELInplaceEdit*         m_pInplaceEdit;     // In-place edit pointer
    CELInplaceList*         m_pInplaceList;     // In-place list pointer
    CELRecordItem*          m_pActiveItem;      // Pointer to the currently focused CXTPReportRecordItem.
};


//
//=============================================================================
// �򵥵�inline����ʵ��
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// �ӳٵ���ʽ��ʼ��
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::PostInitTemplateStyle ()
{
}
//
//-----------------------------------------------------------------------------
// ���ñ�ͷ�ĸ߶�
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderHeight (int Height) //���ñ�ͷ�߶�
{
    m_Header.m_hdrHeight = Height;
}
//
//-----------------------------------------------------------------------------
// ���ñ�ͷ�ķָ�����ɫ
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderGridlineColor (COLORREF color)
{
    m_Header.m_hdrGridlineColor = color;
}
//
//-----------------------------------------------------------------------------
// ���ñ�ͷ��������ɫ
//-----------------------------------------------------------------------------
//
inline void CListCtrlEL::SetHeaderTextColor(COLORREF color) //����ͷ��������ɫ
{
    m_Header.m_hdrTextColor = color;
}
//
//-----------------------------------------------------------------------------
// ���ñ�ͷ�ı�����ɫ
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderBkColor (COLORREF color)
{
    m_Header.m_hdrBGColor = color;
}
//
//-----------------------------------------------------------------------------
// ���ñ�ͷ������, font������Ҫ�û��Լ�ȥά��, ֱ���������
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHeaderFont (CFont* pFont, BOOL bRedraw)
{
    m_Header.SetFont(pFont, bRedraw);
}
inline CFont*  CListCtrlEL::GetHeaderFont ()
{
    return m_Header.GetFont();
}
//
//-----------------------------------------------------------------------------
// �����Ƿ�ʹ�ڶ������, �е�ѡ�������Ȼ��ʾ
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetFreezeSelectionVisible (BOOL bShow)
{
    m_bFreezeSelectionVisible = bShow;
}
//
//-----------------------------------------------------------------------------
// �����Ƿ���ʾ����е���ѡ�и���
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetRowSelectionVisible (BOOL bShow)
{
    m_bRowSelectionVisible = bShow;
}
//
//-----------------------------------------------------------------------------
// ���ñ����Ƿ���������
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetHasGridline (BOOL hasHline, BOOL hasVline)
{
    m_hasHGridline = hasHline;
    m_hasVGridline = hasVline;
}
inline void  CListCtrlEL::SetGradientGridline (BOOL hasHline, BOOL hasVline)
{
    m_gradientHGridline = hasHline;
    m_gradientVGridline = hasVline;
}
//
//-----------------------------------------------------------------------------
// ���ñ�������屳����ɫ
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::SetBkColor (COLORREF cr)
{
    return ELSUPER::SetBkColor(cr);
}
//
//-----------------------------------------------------------------------------
// ���ñ����������ɫ
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::SetTextColor (COLORREF cr)
{
    return ELSUPER::SetTextColor(cr);
}
inline void  CListCtrlEL::SetFocusedSubItemHighlightColor (COLORREF cr)
{
    m_focusedSubItemHighlightColor = cr;
}
inline COLORREF  CListCtrlEL::GetFocusedSubItemHighlightColor ()
{
    return m_focusedSubItemHighlightColor;
}
inline void  CListCtrlEL::SetSelectionRowHighlightColor (COLORREF cr)
{
    m_selectionRowHighlightColor = cr;
}
inline void  CListCtrlEL::SetTextHighlightColor (COLORREF cr)
{
    m_textHighlightColor = cr;
}
inline void  CListCtrlEL::SetSelectionRowLowlightColor (COLORREF cr)
{
    m_selectionRowLowlightColor = cr;
}
//
//-----------------------------------------------------------------------------
// ���ñ��������
//-----------------------------------------------------------------------------
//
inline void  CListCtrlEL::SetFont (CFont* pFont, BOOL bRedraw)
{
    ELSUPER::SetFont(pFont, bRedraw);
}
inline CFont*  CListCtrlEL::GetFont ()
{
    return ELSUPER::GetFont();
}
//
//-----------------------------------------------------------------------------
// ����ͼƬ
//-----------------------------------------------------------------------------
//

#if SU_FEI
inline void  CListCtrlEL::SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap)
{
	
	m_checkBitmap = checkBitmap;
	m_uncheckBitmap = uncheckBitmap;

}
inline void  CListCtrlEL::GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap)
{
	
	retCheckBitmap = m_checkBitmap;
	retUncheckBitmap = m_uncheckBitmap;
	
}

#else
inline void  CListCtrlEL::SetCheckImage (CBitmap* checkBitmap, CBitmap* uncheckBitmap, CBitmap* checkfocusBitmap, CBitmap* uncheckfocusBitmap)
{

	m_checkBitmap = checkBitmap;
	m_uncheckBitmap = uncheckBitmap;

	m_checkFocusBitmap = checkfocusBitmap;
	m_uncheckFocusBitmap = uncheckfocusBitmap;

}
inline void  CListCtrlEL::GetCheckImage (CBitmap*& retCheckBitmap, CBitmap*& retUncheckBitmap, CBitmap*& retCheckFocusBitmap, CBitmap*& retUncheckFocusBitmap)
{

	retCheckBitmap = m_checkBitmap;
	retUncheckBitmap = m_uncheckBitmap;

	retCheckFocusBitmap = m_checkFocusBitmap;
	retUncheckFocusBitmap = m_uncheckFocusBitmap;

}

#endif

//
//-----------------------------------------------------------------------------
// ���inplace�ؼ�����, �����Ƿ��ڱ༭״̬
//-----------------------------------------------------------------------------
//
inline CELInplaceEdit*  CListCtrlEL::GetInplaceEdit ()
{
    return m_pInplaceEdit;
}
//
//-----------------------------------------------------------------------------
// ���inplace�ؼ�����, �����Ƿ��ڱ༭״̬
//-----------------------------------------------------------------------------
//
inline CELInplaceList*  CListCtrlEL::GetInplaceList ()
{
    return m_pInplaceList;
}


//
//-----------------------------------------------------------------------------
// ������ͼ��ָ֤���б���ʾ
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::EnsureRowVisibleScrollRow (int rowIndex)
{
    return ELSUPER::EnsureVisible(rowIndex, FALSE);
}
//
//-----------------------------------------------------------------------------
// ��õ�ǰ��ͼ��������rowindex
//-----------------------------------------------------------------------------
//
inline int  CListCtrlEL::GetTopRowIndex ()
{
    return ELSUPER::GetTopIndex();
}
//
//-----------------------------------------------------------------------------
// �����ػ���ͼ�л�item����
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::RedrawRowItems (int rowIndexBegin, int rowIndexEnd)
{
    return ELSUPER::RedrawItems(rowIndexBegin, rowIndexEnd);

}
inline BOOL  CListCtrlEL::RedrawRowItem (int rowIndex, int nSubItem)
{
    return ELSUPER::SetItemText(rowIndex, nSubItem, L"");
}

//
//-----------------------------------------------------------------------------
// �ж�rowindex, colindex������Ч��
//-----------------------------------------------------------------------------
//
inline BOOL  CListCtrlEL::IsValidRowIndex (int rowIndex)
{
    if (rowIndex >= 0 && rowIndex < GetRecordRowCount())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
inline BOOL  CListCtrlEL::IsValidColIndex (int colIndex)
{
    if (colIndex >= 0 && colIndex < m_Header.GetItemCount())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
inline BOOL  CListCtrlEL::IsFreezeColIndex (int colIndex)
{
    if (colIndex >= 0 && colIndex < m_nFreezeColumnsCount)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
//
//-----------------------------------------------------------------------------
// ��õ�ǰ���(����inplace�༭��item), ����ǿ�, �����ǰ���ڱ༭״̬
//-----------------------------------------------------------------------------
//
inline CELRecordItem*  CListCtrlEL::GetCurrentActiveItem ()
{
    return m_pActiveItem;
}