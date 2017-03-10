#include "stdafx.h"

#include "ProgressSlider.h"

#include "src/utils/ReportControl/Common/CommonEL.h"


#define PRO_TRI_MARGIN_MID          10              ///< 离中线10个像素


CProgressSlider::CProgressSlider()
{
    m_min       = 0;
    m_max       = 0;
    m_val       = 0;
    m_minOp     = 0;
    m_maxOp     = 0;
    m_ftText    = NULL;
    m_bkcolor   = RGB(255, 255, 255);
    m_fgcolor   = RGB(0, 255, 0);
    m_textcolor = RGB(0, 0, 255);
    m_csDisplayFmt = L"%d";
}

BEGIN_MESSAGE_MAP(CProgressSlider, CStatic)
    ON_WM_GETDLGCODE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_KEYDOWN()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


void CProgressSlider::_SetProgressColor (COLORREF bkcolor, COLORREF fgcolor, COLORREF textcolor)
{
    if (bkcolor != -1)
    {
        m_bkcolor = bkcolor;
    }
    if (fgcolor != -1)
    {
        m_fgcolor = fgcolor;
    }
    if (textcolor != -1)
    {
        m_textcolor = textcolor;
    }
}


int CProgressSlider::_GoModal(const CRect& rcItem, CWnd* pParentWnd)
{
    if (::IsWindow(m_hWnd)) 
    {
        ASSERT(FALSE);
        return FALSE;
    }

    // 创建窗口
    BOOL bCreate = CStatic::Create(L"", WS_CHILD|WS_VISIBLE, rcItem, pParentWnd);
    ASSERT(bCreate);

    // Enable this window
    EnableWindow(TRUE);
    CWnd* pFocusWnd = SetFocus();
    SetCapture();

    m_nFlags |= WF_CONTINUEMODAL;
    int nResult = m_nModalResult;
    if (ContinueModal())
    {
        // enter modal loop
        DWORD dwFlags = MLF_SHOWONIDLE;
        if (GetStyle() & DS_NOIDLEMSG)
        {
            dwFlags |= MLF_NOIDLEMSG;
        }
        nResult = RunModalLoop(dwFlags);
    }

    ReleaseCapture();
    DestroyWindow();

    if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd()))
    {
        pFocusWnd->SetFocus();
    }

    return (nResult == IDOK);
}

int CProgressSlider::_GetVal()
{
    return m_val;
}

UINT CProgressSlider::OnGetDlgCode()
{
    return DLGC_WANTALLKEYS;
}

BOOL CProgressSlider::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}

void CProgressSlider::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // 填充背景
    CRect clientRect;
    GetClientRect(&clientRect);

    // 填充前景
    CRect fgRect = clientRect;
    CRect bkRect = clientRect;
    int width = (int)(double(m_val)*clientRect.Width() / double(m_max - m_min));
    fgRect.right = fgRect.left + width;
    bkRect.left = bkRect.left + width;
    dc.FillSolidRect(fgRect, m_fgcolor);
    dc.FillSolidRect(bkRect, m_bkcolor);

    // 添加文字
    CString str;
    str.Format(m_csDisplayFmt, m_val);

    int nOriDC = dc.SaveDC();
    dc.SetTextColor(m_textcolor);
    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(m_ftText);
    dc.DrawText(str, clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    dc.RestoreDC(nOriDC);

    int nTriWidth   = PRO_TRI_MARGIN_MID + 5;
    int nTextWidth  = dc.GetOutputTextExtent(str).cx;

    // 计算文字区域
    CRect rcText(clientRect);
    rcText.left     = (clientRect.Width() - nTextWidth) / 2;
    rcText.right    = rcText.left + nTextWidth;

    CPoint ptLeft, ptRight;
    ptLeft.y    = clientRect.top + clientRect.Height() / 2;
    ptRight.y   = ptLeft.y;
    ptLeft.x    = rcText.left - nTriWidth;
    ptRight.x   = rcText.right + nTriWidth;

    bool bleft(false), bright(false);

    if(width >= ptLeft.x)
    {
        bleft = true;
    }

    if(width >= ptRight.x)
    {
        bright = true;
    }

    CommonEL::DrawTriangleIcon(&dc, ptLeft, 5, bleft ? m_bkcolor : m_fgcolor, 1, CommonEL::TRI_LEFT, TRUE, TRUE);
    CommonEL::DrawTriangleIcon(&dc, ptRight, 5, bright ? m_bkcolor : m_fgcolor, 1, CommonEL::TRI_RIGHT, TRUE, TRUE);
}


void CProgressSlider::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_LEFT)
    {
        if (m_val > m_minOp)
        {
            m_val--;

            // 告诉父窗口值被改变
            _sendMessage();

            Invalidate();
        }
    }
    else if (nChar == VK_RIGHT)
    {
        if (m_val < m_maxOp)
        {
            m_val++;

            // 告诉父窗口值被改变
            _sendMessage();

            Invalidate();
        }
    }
    else if (nChar == VK_ESCAPE)
    {
#if SHENG_ZE
        EndModalLoop(IDCANCEL);
#else
		EndModalLoop(IDOK);
#endif
    }
    else if (nChar == VK_RETURN)
    {
        EndModalLoop(IDOK);
    }
    else
    {
        CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

void CProgressSlider::OnCaptureChanged(CWnd *pWnd)
{
    EndModalLoop(IDCANCEL);
    CStatic::OnCaptureChanged(pWnd);
}

BOOL CProgressSlider::PreTranslateMessage( MSG* pMsg )
{
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_ESCAPE:
        case VK_RETURN:
        case VK_TAB:
                OnKeyDown(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
                OnChar(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
                return TRUE;
        default:
            break;
        }
    }

    return CStatic::PreTranslateMessage(pMsg);
}

void CProgressSlider::_SetRange( int nMin, int nMax )
{
    m_min = nMin;
    m_max = nMax;
}

void CProgressSlider::_SetRangeOp( int nMinOp, int nMaxOp )
{
    m_minOp = nMinOp < m_min ? m_min : nMinOp;
    m_maxOp = nMaxOp > m_max ? m_max : nMaxOp;
}

void CProgressSlider::_SetVal( int nVal )
{
    m_val = nVal;
}

void CProgressSlider::_sendMessage()
{
    CWnd *pOwner = GetOwner();

    while(pOwner && IsWindow(pOwner->m_hWnd))
    {
        if(1 == pOwner->SendMessage(WM_COMCTRL_PROGRESSSLIDER_CHANGE, m_val, 0))
        {
            break;
        }

        pOwner = pOwner->GetOwner();
    }
}



