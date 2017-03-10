#pragma once

#include "ELDefines.h"


//
//-----------------------------------------------------------------------------
// CHeaderCtrlEL
//
//  �ؼ���ͷ�ؼ�, �����ͷ����ʾ��ʽ, �Լ�����col
//
//  2013.04.03 -    �޸�CListCtrlEL��CHeaderCtrlELΪ���м̳� CListCtrl..., �������и�����, 
//                  ʹ����Ӧ��ֻʹ��CListCtrlEL�ṩ�Ĺ���, ����Ҫ����ʹ�ø���Ĺ���.
//-----------------------------------------------------------------------------
//
class CHeaderCtrlEL : public ELHDRSUPER
{
    DECLARE_DYNAMIC(CHeaderCtrlEL)


public:
    //
    //=============================================================================
    // �ؼ���������ʽ
    //=============================================================================
    //
    CHeaderCtrlEL ();
    virtual ~CHeaderCtrlEL ();

    int  GetHdrHeight ();
    COLORREF  GetHdrGridlineColor ();


protected:
    //
    //=============================================================================
    // ��Ϣ����
    //=============================================================================
    //
    DECLARE_MESSAGE_MAP()

    afx_msg void  OnPaint ();
    afx_msg BOOL  OnEraseBkgnd (CDC* pDC);
    LRESULT  OnLayout ( WPARAM wParam, LPARAM lParam );
    afx_msg void  OnDestroy ();


private:
    //
    //=============================================================================
    // ����
    //=============================================================================
    //
    CELColumn*  GetColumn (int colIndex);

private:
    //
    //=============================================================================
    // �ڲ�״̬���Ա���
    //=============================================================================
    //
    int                     m_hdrHeight;        // Header�ĸ߶�.
    COLORREF                m_hdrGridlineColor; // ����ߵ���ɫ
    COLORREF                m_hdrTextColor;     // header�ı���ɫ
    COLORREF                m_hdrBGColor;       // header������ɫ
    //  font, imagelist ��ʹ��CHeaderCtrl�����ṩ������

    friend class CListCtrlEL;
};


inline int  CHeaderCtrlEL::GetHdrHeight ()
{
    return m_hdrHeight;
}
inline COLORREF  CHeaderCtrlEL::GetHdrGridlineColor ()
{
    return m_hdrGridlineColor;
}