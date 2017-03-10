/**
 * \file
 *      ButtonEx.cpp
 *
 * \brief
 *      �Զ��尴ť��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */

// ButtonEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ButtonEx.h"

#include "src/main/common.h"

#define DSM_BUTTONEX_STATU_FOCUS            0x01        ///< ����״̬
#define DSM_BUTTONEX_STATU_PRESS            0x02
#define DSM_BUTTONEX_STATU_NORMAL           0x04        ///< �޽���״̬


// CButtonEx

IMPLEMENT_DYNAMIC(CButtonEx, CButton)

CButtonEx::CButtonEx( const CString& csCaption /*= L"buttonex"*/ )
: m_csCaption(csCaption)
, m_crBkColor(RGB(0xFF, 0x80, 0x80))
, m_crTextColor(RGB(0x73, 0xD3, 0xBD))
, m_crFocusBkColor(RGB(0x42, 0xBE, 0xA5))
, m_crFocusTextColor(RGB(0x00, 0x00, 0x00))
, m_nMode(1)
{

}

CButtonEx::~CButtonEx()
{
}


BEGIN_MESSAGE_MAP(CButtonEx, CButton)
    ON_WM_GETDLGCODE()
END_MESSAGE_MAP()



// CButtonEx ��Ϣ�������



void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    // TODO:  ������Ĵ����Ի���ָ����

    ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

    if(ODS_FOCUS & lpDrawItemStruct->itemState ||
        ODS_SELECTED & lpDrawItemStruct->itemState)
    {
        _drawItem(lpDrawItemStruct->hDC, DSM_BUTTONEX_STATU_PRESS);
    }
    else
    {
        _drawItem(lpDrawItemStruct->hDC, DSM_BUTTONEX_STATU_NORMAL);
    }
}

void CButtonEx::PreSubclassWindow()
{
    // TODO: �ڴ����ר�ô����/����û���
    SetWindowLong(m_hWnd, GWL_STYLE, GetStyle() | BS_OWNERDRAW);

    CButton::PreSubclassWindow();
}


void CButtonEx::_drawItem(HDC hdc, DWORD dwStatu )
{
    CDC dc;
    dc.Attach(hdc);

    CRect rcClient;
    GetClientRect(rcClient);

    if(DSM_BUTTONEX_MODE_BMP == m_nMode)
    {
        CDC dcMemBmp;
        dcMemBmp.CreateCompatibleDC(&dc);

        HGDIOBJ hOriObj = NULL;

        switch(dwStatu)
        {
        case DSM_BUTTONEX_STATU_FOCUS:
            hOriObj = dcMemBmp.SelectObject(m_bitmapFocus);
            dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
            break;

        case DSM_BUTTONEX_STATU_PRESS:
            hOriObj = dcMemBmp.SelectObject(m_bitmapPress);
            dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
            break;

        case DSM_BUTTONEX_STATU_NORMAL:
            hOriObj = dcMemBmp.SelectObject(m_bitmapNormal);
            dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), 
                &dcMemBmp, 0, 0, SRCCOPY);
            break;
        }

        dcMemBmp.SelectObject(hOriObj);
        dcMemBmp.DeleteDC();

        dc.Detach();

        return;
    }

    CFont ftText;
    ftText.CreateFont(16, 0, 0, 0, 
        FW_NORMAL, FALSE, FALSE, 0, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
        CLEARTYPE_QUALITY, FF_ROMAN, 
        DSM_DEFAULT_FONT);

    COLORREF crBkground;
    COLORREF crText;
    if(DSM_BUTTONEX_STATU_FOCUS == dwStatu ||
        DSM_BUTTONEX_STATU_PRESS == dwStatu)
    {
        crText      = m_crFocusTextColor;
        crBkground  = m_crFocusBkColor;
    }
    else
    {
        crText      = m_crTextColor;
        crBkground  = m_crBkColor;
    }


    int nOriDC = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(ftText);
    dc.SetTextColor(crText);
    dc.FillSolidRect(rcClient, crBkground);
    dc.DrawText(m_csCaption, rcClient, DT_CENTER | DT_VCENTER);
    dc.RestoreDC(nOriDC);

    dc.Detach();
}

UINT CButtonEx::OnGetDlgCode()
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    return CButton::OnGetDlgCode() | DLGC_DEFPUSHBUTTON;
}

void CButtonEx::_SetBmp( UINT normal, UINT focus, UINT press )
{
    m_nMode         = DSM_BUTTONEX_MODE_BMP;
    m_bitmapNormal  = dsm_bmpmgr::Get(normal);
    m_bitmapFocus   = dsm_bmpmgr::Get(focus);
    m_bitmapPress   = dsm_bmpmgr::Get(press);
}

