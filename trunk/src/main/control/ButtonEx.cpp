/**
 * \file
 *      ButtonEx.cpp
 *
 * \brief
 *      自定义按钮类实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */

// ButtonEx.cpp : 实现文件
//

#include "stdafx.h"
#include "ButtonEx.h"

#include "src/main/common.h"

#define DSM_BUTTONEX_STATU_FOCUS            0x01        ///< 焦点状态
#define DSM_BUTTONEX_STATU_PRESS            0x02
#define DSM_BUTTONEX_STATU_NORMAL           0x04        ///< 无焦点状态


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



// CButtonEx 消息处理程序



void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    // TODO:  添加您的代码以绘制指定项

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
    // TODO: 在此添加专用代码和/或调用基类
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    return CButton::OnGetDlgCode() | DLGC_DEFPUSHBUTTON;
}

void CButtonEx::_SetBmp( UINT normal, UINT focus, UINT press )
{
    m_nMode         = DSM_BUTTONEX_MODE_BMP;
    m_bitmapNormal  = dsm_bmpmgr::Get(normal);
    m_bitmapFocus   = dsm_bmpmgr::Get(focus);
    m_bitmapPress   = dsm_bmpmgr::Get(press);
}

