#include "stdafx.h"

#include "ListCtrlEL.h"


IMPLEMENT_DYNAMIC(CHeaderCtrlEL, ELHDRSUPER)



//
//=============================================================================
// �����ؼ�
//=============================================================================
//

CHeaderCtrlEL::CHeaderCtrlEL()
{
    m_hdrHeight = 40;
    m_hdrGridlineColor = RGB(127, 127, 127);
    m_hdrTextColor = RGB(255, 255, 255);
    m_hdrBGColor = RGB(0, 0, 0);
}

CHeaderCtrlEL::~CHeaderCtrlEL()
{

}




//
//=============================================================================
// ��Ϣ��Ӧ
//=============================================================================
//

BEGIN_MESSAGE_MAP(CHeaderCtrlEL, ELHDRSUPER)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(HDM_LAYOUT, OnLayout)
    ON_WM_DESTROY()
END_MESSAGE_MAP()



//
//-----------------------------------------------------------------------------
// �����û����õ���ʽ, �ػ��ͷ
//-----------------------------------------------------------------------------
//
void  CHeaderCtrlEL::OnPaint ()
{
    CPaintDC  dc(this);

    // ��ÿͻ�����С
    CRect  clientRect;
    GetClientRect(&clientRect);

    // �����ı�����͸��
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(m_hdrTextColor);

    // ��������
    CFont*  oldfont = NULL;
    CFont*  newfont = ELHDRSUPER::GetFont();
    if (newfont)
    {
        oldfont = dc.SelectObject(newfont);  // �ǵ�ѡ��
    }

    {
        //
        // ����ÿ��col����
        //
        int  nItem = ELHDRSUPER::GetItemCount();// �õ��м�����Ԫ 
        for (int i = 0; i < nItem; i++) 
        { 
            CRect  tRect;
            GetItemRect(i, &tRect);             // �õ�Item�ĳߴ�

            // �����Ҫ���Ƶ��ı�
            HDITEM  hdrinfo;
#define COLTEXT_MAX         16
            wchar_t  colText[COLTEXT_MAX];
            hdrinfo.mask = HDI_TEXT | HDI_FORMAT | HDI_LPARAM;
            hdrinfo.pszText = colText;
            hdrinfo.cchTextMax = COLTEXT_MAX;
            ELHDRSUPER::GetItem(i, &hdrinfo);

            // �������չ����
            CELColumn*  colExAttr = (CELColumn*)hdrinfo.lParam;
            ASSERT_BREAK(colExAttr);


            // ���Ʊ���
            dc.FillSolidRect(tRect, m_hdrBGColor);
#if SU_FEI
            // ���Ʒָ���, ���ӵ��ұ�
            if (i > 0)  // ��һ�����Ӳ���
            {
                dc.FillSolidRect(tRect.left, tRect.top, 1, tRect.Height(), m_hdrGridlineColor);
            }
#endif
            CRect  rectText = tRect;
            rectText.DeflateRect(colExAttr->m_nMarginLeft, 0, colExAttr->m_nMarginRight, 0);
            //dc.DrawText(colText, rectText, colExAttr->m_nHeaderAlignment | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
			dc.DrawText(colText, rectText, EL_LEFT | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
        } 

        {
            //
            // ��ͷ��ʣ�ಿ��
            //
            if (nItem > 0)  // �ж��Ƿ�������һ������
            {
                CRect  rtRect;
                GetItemRect(nItem - 1, &rtRect);
                dc.FillSolidRect(rtRect.right, rtRect.top, 
                    clientRect.right-rtRect.right, rtRect.Height(), m_hdrBGColor);
                // ���Ʒָ���, ���ӵ����
                dc.FillSolidRect(rtRect.right, rtRect.top, 1, rtRect.Height(), m_hdrGridlineColor);
            }
            else
            {
                dc.FillSolidRect(clientRect, m_hdrBGColor);
            }
        }
    }


    if (oldfont)
    {
        dc.SelectObject(oldfont);
    }
}


BOOL  CHeaderCtrlEL::OnEraseBkgnd (CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return TRUE;
}



LRESULT  CHeaderCtrlEL::OnLayout ( WPARAM wParam, LPARAM lParam )
{

    LRESULT  lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
    HD_LAYOUT&  hdl = *( HD_LAYOUT * ) lParam; 
    RECT*  prc = hdl.prc; 
    WINDOWPOS*  pwpos = hdl.pwpos; 

    // ���ñ�ͷ�߶�
    pwpos->cy = m_hdrHeight; 
    prc->top = m_hdrHeight; 
    return lResult; 
}


void  CHeaderCtrlEL::OnDestroy ()
{
    ELHDRSUPER::OnDestroy();
}


//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ���ָ���е�����Ϣ����
//-----------------------------------------------------------------------------
//
CELColumn*  CHeaderCtrlEL::GetColumn (int colIndex)
{
    HDITEM  hdrinfo;
    hdrinfo.mask = HDI_LPARAM;
    if ( ELHDRSUPER::GetItem(colIndex, &hdrinfo) )
    {
        return (CELColumn*)hdrinfo.lParam;
    }
    else
    {
        ASSERT_BREAK(FALSE);
        return NULL;
    }
}