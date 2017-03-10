#pragma once

#include "../ListCtrlEL/ELDefines.h"
#include "ListBoxEL.h"

#define HEADERBOXEL_CLASSNAME  L"HeaderBoxEL"

//
//-----------------------------------------------------------------------------
// ��ͷһ���е�Ԫ������
//-----------------------------------------------------------------------------
//
struct HeaderBoxELColumn
{
    ELAlignment         itemAlignment;      // ���е�����Ԫ�صĶ��뷽ʽ
    ELAlignment         hdrAlignment;       // ���б�ͷ�ı��Ķ��뷽ʽ
    CString             hdrText;            // ���б�ͷ�ı�
    int                 hdrWidth;           // ���б�ͷ�Ŀ��

    int                 marginLeft;         // ����Item������ʱ, ��ߵ���������
    int                 marginRight;        // ����Item������ʱ, �ұߵ���������

public:
    HeaderBoxELColumn ()
    {
        itemAlignment = EL_CENTER;
       // hdrAlignment = EL_CENTER;
		hdrAlignment = EL_LEFT;
        // hdrText = L"";
        hdrWidth = 0;

        marginLeft = 0;
        marginRight = 0;
    }
};


//
//-----------------------------------------------------------------------------
// CHeaderBox
//
//  �ؼ���ͷ�ؼ�, �����ͷ����ʾ��ʽ, �Լ�����col
//-----------------------------------------------------------------------------
//
class CHeaderBoxEL : public CWnd
{
public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CHeaderBoxEL ();
    virtual ~CHeaderBoxEL ();
    virtual BOOL  Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    void  SetHeaderGridlineColor (COLORREF color);
    COLORREF  GetHeaderGridlineColor ();
    void  SetHeaderBkColor (COLORREF color);
    void  SetHeaderTextColor (COLORREF color);
    void  SetHeaderFont (CFont* pFont, BOOL bRedraw = TRUE);

public:
    //
    //=============================================================================
    // ���ݲ���
    //=============================================================================
    //
    int  GetColumnCount ();
    CRect  GetItemRect (int nIndex);
    int  InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign = EL_CENTER, int colWidth = -1, ELAlignment headerAlgin = EL_CENTER,
        int marginLeft=0, int marginRight=0);
    HeaderBoxELColumn*  GetColumn (int colIndex);

protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);


private:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    COLORREF                m_hdrGridlineColor; // ����ߵ���ɫ
    COLORREF                m_hdrTextColor;     // header�ı���ɫ
    COLORREF                m_hdrBkColor;       // header������ɫ
    CFont*                  m_hdrFont;          // ��ǰʹ�õ�����

    CArray<HeaderBoxELColumn*>  m_hdrColumns;   // ��������Ϣ
};

//
//-----------------------------------------------------------------------------
// ���ñ�ͷ��������ɫ��
//-----------------------------------------------------------------------------
//
inline void  CHeaderBoxEL::SetHeaderGridlineColor (COLORREF color)
{
    m_hdrGridlineColor = color;
}
inline COLORREF  CHeaderBoxEL::GetHeaderGridlineColor ()
{
    return m_hdrGridlineColor;
}
inline void  CHeaderBoxEL::SetHeaderBkColor (COLORREF color)
{
    m_hdrBkColor = color;
}
inline void  CHeaderBoxEL::SetHeaderTextColor (COLORREF color)
{
    m_hdrTextColor = color;
}
inline void  CHeaderBoxEL::SetHeaderFont (CFont* pFont, BOOL bRedraw)
{
    m_hdrFont = pFont;

    if (bRedraw)
    {
        Invalidate();
    }
}

//
//-----------------------------------------------------------------------------
// ��ñ�ͷ�ж�����
//-----------------------------------------------------------------------------
//
inline int  CHeaderBoxEL::GetColumnCount ()
{
    return m_hdrColumns.GetSize();
}


