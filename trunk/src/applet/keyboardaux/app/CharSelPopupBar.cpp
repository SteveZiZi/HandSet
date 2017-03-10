#include "stdafx.h"
#include "KeyboardAux.h"
#include "CharSelPopupBar.h"

//
//=============================================================================
// ��������ʽ
//=============================================================================
//
CCharSelPopupBar::CCharSelPopupBar()
{
    // ע���Ӧ�Ĵ�����
    RegisterWndClass(NULL, KBPOPUPBAR_CLASSNAME, 0);


    m_font = NULL;

	//��ɫ�޸�
	/*************************************************************************/
    m_textColor = RGB(0, 0, 0);
    m_bkColor = RGB(217, 223, 231);
    m_selHighlightColor = RGB(245, 210, 71);
    m_textHighlightColor = RGB(0, 0, 0);
    m_charSize = CSize(0, 0);

    //m_charsOptional
    m_curSel = -1;
}

CCharSelPopupBar::~CCharSelPopupBar()
{
}

BOOL  CCharSelPopupBar::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::Create(KBPOPUPBAR_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

BOOL  CCharSelPopupBar::CreateEx (DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::CreateEx(dwExStyle, KBPOPUPBAR_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

//
//-----------------------------------------------------------------------------
// ���õ�ǰʹ�õ�����
//-----------------------------------------------------------------------------
//
void  CCharSelPopupBar::SetFont (CFont* pFont, BOOL bRedraw)
{
    m_font = pFont;
    UpdateTextSize(pFont);

    if (bRedraw)
    {
        Invalidate();
    }
}

//
//-----------------------------------------------------------------------------
// ���õ�ǰ������ʾ�ĺ�ѡ����, ���������е�һ��
//-----------------------------------------------------------------------------
//
void  CCharSelPopupBar::SetCharsOptional (LPCTSTR charsOptional, int curSel)
{
    m_charsOptional = charsOptional;
    m_curSel = curSel;

    UpdateWindowSize();
    Invalidate(FALSE);
}


//
//=============================================================================
// ��Ϣ����
//=============================================================================
//
BEGIN_MESSAGE_MAP(CCharSelPopupBar, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void  CCharSelPopupBar::OnPaint ()
{
    CPaintDC  dc(this);

    // �����������ı���
    CRect  clientRect;
    GetClientRect(&clientRect);
	//�޸���ɫ��ʽ
	/******************************************************************/
    dc.FillSolidRect(clientRect, RGB(255,255,255));
	clientRect.left = 1;

    // ��������
    CFont*  oldfont = dc.SelectObject(m_font);

    CRect  charRect = clientRect;
    LPCTSTR  strbuf = (LPCTSTR)m_charsOptional;

	//����һ�����
	/********************************************************************/

	charRect.bottom -=1;

    for (int i = 0; i < m_charsOptional.GetLength(); i++)
    {
        // �����ַ�λ��
		if(i==0)
		{
			charRect.left = 1;
			charRect.right = charRect.left + m_charSize.cx-2;
		}
		else
		{
			charRect.left = m_charSize.cx * i;
			charRect.right = charRect.left + m_charSize.cx-1;
		}


		
        // ����
        if (i == m_curSel)
        {
            dc.SetTextColor(m_textHighlightColor);
            dc.SetBkColor(m_selHighlightColor);
            dc.FillSolidRect(charRect, m_selHighlightColor);
        }
        else
        {
            dc.SetTextColor(m_textColor);
            dc.SetBkColor(m_bkColor);
			dc.FillSolidRect(charRect,m_bkColor);
        }
        // ����
        dc.DrawText(strbuf++, 1, charRect, DT_VCENTER|DT_CENTER|DT_NOPREFIX);
    }


    dc.SelectObject(oldfont);
}

BOOL  CCharSelPopupBar::OnEraseBkgnd (CDC* pDC)
{
    return TRUE;
}


//
//=============================================================================
// ����
//=============================================================================
//


//
//-----------------------------------------------------------------------------
// ������ѡ������, ��¼�����size, ���������ڴ�С
//-----------------------------------------------------------------------------
//
void  CCharSelPopupBar::UpdateTextSize (CFont* font)
{
    ASSERT(font);

    HDC  dc = ::GetDC(GetSafeHwnd());
    HGDIOBJ  oldfont = ::SelectObject(dc, font->GetSafeHandle());
    {
        // ��ò�����ÿ���ַ��Ĵ�С(ռ�����), ʹ����ʾ��ʱ�������ʾ���ֿ�
        TEXTMETRIC  tm;
        ::GetTextMetrics(dc, &tm);
        m_charSize.cx = tm.tmAveCharWidth * 3;
        m_charSize.cy = tm.tmHeight * 1.5;
    }
    ::SelectObject(dc, oldfont);
    ::ReleaseDC(GetSafeHwnd(), dc);

    UpdateWindowSize();
}
void  CCharSelPopupBar::UpdateWindowSize ()
{
    CSize  textExtent;
    textExtent.cx = m_charsOptional.GetLength() * m_charSize.cx;
    textExtent.cy = m_charSize.cy;


    // �����ַ�����ʾ�������ڴ�С
    SetWindowPos(NULL, 0, 0, textExtent.cx, textExtent.cy, 
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

//
//-----------------------------------------------------------------------------
// ע�ᴰ����
//-----------------------------------------------------------------------------
//
BOOL  CCharSelPopupBar::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class�Ѿ�����
        return TRUE;
    }

    WNDCLASS wndcls;
    ZeroMemory(&wndcls, sizeof(wndcls));

    wndcls.style = style;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = hInstance ? hInstance : AfxGetInstanceHandle();
    wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.lpszClassName = lpszClassName;
    wndcls.hIcon = hIcon;
    wndcls.hbrBackground = (HBRUSH)::GetStockObject(bgStockBrush);

    return AfxRegisterClass(&wndcls);
}
