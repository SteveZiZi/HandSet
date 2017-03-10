#include "stdafx.h"

#include "HeaderBoxEL.h"
#include "../Common/CommonEL.h"



//
//=============================================================================
// �����ؼ�
//=============================================================================
//

CHeaderBoxEL::CHeaderBoxEL()
{
    // ע���Ӧ�Ĵ�����
    RegisterWndClass(NULL, HEADERBOXEL_CLASSNAME, 0);

    m_hdrGridlineColor = RGB(127, 127, 127);
    m_hdrTextColor = RGB(255, 255, 255);
    m_hdrBkColor = RGB(0, 0, 0);
    m_hdrFont = NULL;

    // m_hdrColumns.RemoveAll();
}

CHeaderBoxEL::~CHeaderBoxEL()
{
    // �ͷ��ж���
    for (int i = 0; i < m_hdrColumns.GetSize(); i++)
    {
        HeaderBoxELColumn*  colInfo = m_hdrColumns.GetAt(i);
        ASSERT_BREAK(colInfo);

        delete colInfo;
    }
    m_hdrColumns.RemoveAll();
}

//
//-----------------------------------------------------------------------------
// �����ؼ�����
//-----------------------------------------------------------------------------
//
BOOL  CHeaderBoxEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::Create(HEADERBOXEL_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

//
//-----------------------------------------------------------------------------
// ��ñ�ͷһ�����ӵ�λ��
//-----------------------------------------------------------------------------
//
CRect  CHeaderBoxEL::GetItemRect (int nIndex)
{
    ASSERT_BREAK(::IsWindow(GetSafeHwnd()));
    ASSERT_BREAK(nIndex >= 0 && nIndex < GetColumnCount());
    
    CRect  itemRect;
    GetClientRect(&itemRect);

    // ��õ�һ�����ӵ�λ��
    if (GetColumnCount() > 0)
    {
        HeaderBoxELColumn*  colAttr = GetColumn(0);
        ASSERT_BREAK(colAttr);

        itemRect.left = 0;
        itemRect.right = itemRect.left + colAttr->hdrWidth;
    }

    // ��ú�����ӵ�λ��
    for (int i = 1; i <= nIndex && i < GetColumnCount(); i++)
    {
        HeaderBoxELColumn*  colAttr = GetColumn(i);
        ASSERT_BREAK(colAttr);

        // ������һ�����ӵ�λ��
        itemRect.left = itemRect.right;
        itemRect.right = itemRect.left + colAttr->hdrWidth;
    }

    return itemRect;
}

//
//-----------------------------------------------------------------------------
// ����һ��
//-----------------------------------------------------------------------------
//
int  CHeaderBoxEL::InsertColumn (int colIndex, LPCTSTR colTitle, ELAlignment colAlign, int colWidth, ELAlignment headerAlgin, int marginLeft, int marginRight)
{
    ASSERT_BREAK(colIndex <= m_hdrColumns.GetSize());

    HeaderBoxELColumn*  colInfo = new HeaderBoxELColumn;
    ASSERT_BREAK(colInfo);

    colInfo->hdrText = colTitle;
    colInfo->itemAlignment = colAlign;
    colInfo->hdrAlignment = headerAlgin;
    colInfo->marginLeft = marginLeft;
    colInfo->marginRight = marginRight;
    if (colWidth == -1)
    {
        CRect  wndRect;
        GetClientRect(&wndRect);
        colInfo->hdrWidth = min(wndRect.Width(), 100);
    }
    else
    {
        colInfo->hdrWidth = colWidth;
    }

    m_hdrColumns.InsertAt(colIndex, colInfo);
    ASSERT_BREAK(m_hdrColumns.GetAt(colIndex) == colInfo);
    return colIndex;
}
//
//-----------------------------------------------------------------------------
// ��ȡ�е���Ϣ
//-----------------------------------------------------------------------------
//
HeaderBoxELColumn*  CHeaderBoxEL::GetColumn (int colIndex)
{
    ASSERT_BREAK(colIndex < m_hdrColumns.GetSize());

    return m_hdrColumns.GetAt(colIndex);
}

//
//=============================================================================
// ��Ϣ��Ӧ
//=============================================================================
//

BEGIN_MESSAGE_MAP(CHeaderBoxEL, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



//
//-----------------------------------------------------------------------------
// �����û����õ���ʽ, �ػ��ͷ
//-----------------------------------------------------------------------------
//
void  CHeaderBoxEL::OnPaint ()
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
    CFont*  newfont = m_hdrFont;
    if (newfont)
    {
        oldfont = dc.SelectObject(newfont);     // �ǵ�ѡ��
    }

    {
        //
        // ����ÿ��col����
        //
        int  nItem = GetColumnCount();          // �õ��м�����Ԫ 
        for (int i = 0; i < nItem; i++) 
        { 
            CRect  tRect;                       // �õ�Item�ĳߴ�
            tRect = GetItemRect(i);             // �õ�Item�ĳߴ�

            HeaderBoxELColumn*  colAttr = GetColumn(i);
            ASSERT_BREAK(colAttr);

            // ���Ʊ���
            dc.FillSolidRect(tRect, m_hdrBkColor);

            // ���Ʒָ���, ���ӵ��ұ�
            if (i > 0)  // ��һ�����Ӳ���
            {
                dc.FillSolidRect(tRect.left, tRect.top, 1, tRect.Height(), m_hdrGridlineColor);
            }


            dc.DrawText(colAttr->hdrText, &tRect, colAttr->hdrAlignment | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
        } 

        {
            //
            // ��ͷ��ʣ�ಿ��
            //
            if (nItem > 0)  // �ж��Ƿ�������һ������
            {
                CRect  rtRect;
                rtRect = GetItemRect(nItem - 1);
                dc.FillSolidRect(rtRect.right, rtRect.top, 
                    clientRect.right-rtRect.right, rtRect.Height(), m_hdrBkColor);
                // ���Ʒָ���, ���ӵ����
                dc.FillSolidRect(rtRect.right, rtRect.top, 1, rtRect.Height(), m_hdrGridlineColor);
            }
            else
            {
                dc.FillSolidRect(clientRect, m_hdrBkColor);
            }
        }

        if (oldfont)
        {
            dc.SelectObject(oldfont);
        }
    }
}


BOOL  CHeaderBoxEL::OnEraseBkgnd (CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return TRUE;
}



//
//=============================================================================
// ����
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// ע�ᴰ����
//-----------------------------------------------------------------------------
//
BOOL  CHeaderBoxEL::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class�Ѿ�����
        return TRUE;
    }

    LB_DEBUGLOG(L"CHeaderBoxEL::RegisterWndClass\n");

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