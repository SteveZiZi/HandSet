#include "stdafx.h"

#include "ListBoxExt.h"
#include "../Common/CommonEL.h"


//
//=============================================================================
// CListBoxT_3dLine
//=============================================================================
//

CListBoxT_3dLine::CListBoxT_3dLine ()
{
    CListBoxT_3dLine::PostInitTemplateStyle();
}
CListBoxT_3dLine::~CListBoxT_3dLine ()
{

}

//
//-----------------------------------------------------------------------------
// �ӳٳ�ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create, ����������֮��
//-----------------------------------------------------------------------------
//
void  CListBoxT_3dLine::PostInitTemplateStyle ()
{
    // �����հ׻���3d����
    SetMargin(0, 0, LBT_3L_3DBORDER_SIZE, 0);

    // ������ɫ
    SetBkColor(LBT_3L_BKCOLOR);
    SetTextColor(LBT_3L_TEXTCOLOR);
    SetSelStyle(TRUE, LBT_3L_HLCOLOR, LBT_3L_HLTEXTCOLOR, LBT_3L_LLCOLOR);

    // ����Ĭ���и�
    SetItemHeight(-1, LBT_3L_ITEMHEIGHT);
}

//
//=============================================================================
// ��Ϣ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ������߿�
//-----------------------------------------------------------------------------
//
void  CListBoxT_3dLine::PaintNC (CDC* pDC)
{
    CRect  rcClient;
    GetClientRect(rcClient);

    // �����ұߵ�3D����
    CRect  rcRightBorder(rcClient.right-2, rcClient.top, rcClient.right-1, rcClient.bottom);
    pDC->FillSolidRect(rcRightBorder, LBT_3L_3DBORDER1_COLOR);
    rcRightBorder.OffsetRect(1, 0);
    pDC->FillSolidRect(rcRightBorder, LBT_3L_3DBORDER2_COLOR);
}

//
//-----------------------------------------------------------------------------
// �ػ�ÿ��Item
//-----------------------------------------------------------------------------
//
void  CListBoxT_3dLine::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT_BREAK(lpDIS->CtlType == ODT_LISTBOX);

    CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
    HWND  curFocusedWnd = ::GetFocus();
    CRect  rcItem = lpDIS->rcItem; 
    CRect  rcText(lpDIS->rcItem.left + 10, lpDIS->rcItem.top, lpDIS->rcItem.right - 10, lpDIS->rcItem.bottom);
    CString  itemText; GetText(lpDIS->itemID, itemText);
    COLORREF  oldtextcolor = pDC->GetTextColor();

    if (lpDIS->itemID != (UINT)-1)
    {
        // ����ѡ�������
        if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED) && 
            (curFocusedWnd == m_hWnd || m_bShowSelAlways))
        {
            if (curFocusedWnd == m_hWnd)
            {
                // item �������
                pDC->FillSolidRect(rcItem, m_selHighlightColor);

                // �������ҵı�Ǽ�ͷ
                CommonEL::DrawTriangleIcon(pDC, CPoint(rcItem.right-22, rcItem.CenterPoint().y), 5, LBT_3L_SELECTTAG_COLOR, 2, CommonEL::TRI_RIGHT, FALSE, FALSE);

                // ���ָ���
                pDC->SetTextColor(m_textHighlightColor);
            }
            else
            {
                // �ؼ���ʧ����ʱ, ��m_bShowSelAlways
                pDC->FillSolidRect(rcItem, m_selLowlightColor);

                pDC->SetTextColor(m_textColor);
            }

        }
        else
        {
            pDC->SetTextColor(m_textColor);
        }

        // ����item���±�3D��
        CRect  rcBottomLine(rcItem.left, rcItem.bottom-1, rcItem.right, rcItem.bottom);
        pDC->FillSolidRect(rcBottomLine, LBT_3L_3DSEPARATOR1_COLOR);
        rcBottomLine.OffsetRect(0, 1);
        pDC->FillSolidRect(rcBottomLine, LBT_3L_3DSEPARATOR2_COLOR);

        // ��������
        pDC->DrawText(itemText, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
    }

    pDC->SetTextColor(oldtextcolor);
}



//
//=============================================================================
// CListBoxT_FlatBorder
//=============================================================================
//


CListBoxT_FlatBorder::CListBoxT_FlatBorder ()
{
    CListBoxT_FlatBorder::PostInitTemplateStyle();
}
CListBoxT_FlatBorder::~CListBoxT_FlatBorder ()
{

}

//
//-----------------------------------------------------------------------------
// �ӳٳ�ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create, ����������֮��
//-----------------------------------------------------------------------------
//
void  CListBoxT_FlatBorder::PostInitTemplateStyle ()
{
    // �����հ׻���3d����
    SetMargin(LBT_FB_BORDER_SIZE, LBT_FB_BORDER_SIZE, LBT_FB_BORDER_SIZE, LBT_FB_BORDER_SIZE);

    // ������ɫ
    SetBkColor(LBT_FB_BKCOLOR);
    SetTextColor(LBT_FB_TEXTCOLOR);
    SetSelStyle(FALSE, LBT_FB_HLCOLOR, LBT_FB_HLTEXTCOLOR, LBT_FB_LLCOLOR);
}

//
//-----------------------------------------------------------------------------
// ������߿�
//-----------------------------------------------------------------------------
//
void  CListBoxT_FlatBorder::PaintNC (CDC* pDC)
{
    CRect  rcClient;
    GetClientRect(rcClient);

    CPen  pen(PS_SOLID, LBT_FB_BORDER_SIZE, LBT_FB_BORDERCOLOR);
    CPen*  oldpen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    {
        rcClient.DeflateRect(1, 1);
        pDC->Rectangle(rcClient);
    }
    pDC->SelectObject(oldpen);
}

//
//-----------------------------------------------------------------------------
// �ػ�ÿ��Item
//-----------------------------------------------------------------------------
//
void  CListBoxT_FlatBorder::DrawItem (LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT_BREAK(lpDIS->CtlType == ODT_LISTBOX);

    CDC*  pDC = CDC::FromHandle(lpDIS->hDC);
    HWND  curFocusedWnd = ::GetFocus();
    CRect  rcItem = lpDIS->rcItem; 
    CRect  rcText(lpDIS->rcItem.left + 10, lpDIS->rcItem.top, lpDIS->rcItem.right - 10, lpDIS->rcItem.bottom);
    CString  itemText; GetText(lpDIS->itemID, itemText);
    COLORREF  oldtextcolor = pDC->GetTextColor();

    if (lpDIS->itemID != (UINT)-1)
    {
        // ����ѡ�������
        if ((lpDIS->itemAction | ODA_SELECT) && (lpDIS->itemState & ODS_SELECTED) && 
            (curFocusedWnd == m_hWnd || m_bShowSelAlways))
        {
            if (curFocusedWnd == m_hWnd)
            {
                // item �������
                pDC->FillSolidRect(rcItem, m_selHighlightColor);

                // ���ָ���
                pDC->SetTextColor(m_textHighlightColor);
            }
            else
            {
                // �ؼ���ʧ����ʱ, ��m_bShowSelAlways
                pDC->FillSolidRect(rcItem, m_selLowlightColor);

                pDC->SetTextColor(m_textColor);
            }
        }
        else
        {
            pDC->SetTextColor(m_textColor);
        }

        // ��������
        pDC->DrawText(itemText, rcText, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
    }
    pDC->SetTextColor(oldtextcolor);


    // ����ɫ����Χ��һȦ���ɫ��ͬ�ľ���
    CPen  pen(PS_SOLID, 1, m_bkColor);
    CPen*  oldpen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    {
        pDC->Rectangle(CRect(lpDIS->rcItem));
    }
    pDC->SelectObject(oldpen);
}

