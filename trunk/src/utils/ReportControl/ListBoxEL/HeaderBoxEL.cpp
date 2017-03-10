#include "stdafx.h"

#include "HeaderBoxEL.h"
#include "../Common/CommonEL.h"



//
//=============================================================================
// 创建控件
//=============================================================================
//

CHeaderBoxEL::CHeaderBoxEL()
{
    // 注册对应的窗口类
    RegisterWndClass(NULL, HEADERBOXEL_CLASSNAME, 0);

    m_hdrGridlineColor = RGB(127, 127, 127);
    m_hdrTextColor = RGB(255, 255, 255);
    m_hdrBkColor = RGB(0, 0, 0);
    m_hdrFont = NULL;

    // m_hdrColumns.RemoveAll();
}

CHeaderBoxEL::~CHeaderBoxEL()
{
    // 释放列对象
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
// 创建控件窗口
//-----------------------------------------------------------------------------
//
BOOL  CHeaderBoxEL::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    BOOL  ret = CWnd::Create(HEADERBOXEL_CLASSNAME, L"", dwStyle, rect, pParentWnd, nID);
    return ret;
}

//
//-----------------------------------------------------------------------------
// 获得表头一个格子的位置
//-----------------------------------------------------------------------------
//
CRect  CHeaderBoxEL::GetItemRect (int nIndex)
{
    ASSERT_BREAK(::IsWindow(GetSafeHwnd()));
    ASSERT_BREAK(nIndex >= 0 && nIndex < GetColumnCount());
    
    CRect  itemRect;
    GetClientRect(&itemRect);

    // 获得第一个格子的位置
    if (GetColumnCount() > 0)
    {
        HeaderBoxELColumn*  colAttr = GetColumn(0);
        ASSERT_BREAK(colAttr);

        itemRect.left = 0;
        itemRect.right = itemRect.left + colAttr->hdrWidth;
    }

    // 获得后面格子的位置
    for (int i = 1; i <= nIndex && i < GetColumnCount(); i++)
    {
        HeaderBoxELColumn*  colAttr = GetColumn(i);
        ASSERT_BREAK(colAttr);

        // 基于上一个格子的位移
        itemRect.left = itemRect.right;
        itemRect.right = itemRect.left + colAttr->hdrWidth;
    }

    return itemRect;
}

//
//-----------------------------------------------------------------------------
// 插入一列
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
// 获取列的信息
//-----------------------------------------------------------------------------
//
HeaderBoxELColumn*  CHeaderBoxEL::GetColumn (int colIndex)
{
    ASSERT_BREAK(colIndex < m_hdrColumns.GetSize());

    return m_hdrColumns.GetAt(colIndex);
}

//
//=============================================================================
// 消息响应
//=============================================================================
//

BEGIN_MESSAGE_MAP(CHeaderBoxEL, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



//
//-----------------------------------------------------------------------------
// 按照用户设置的样式, 重绘表头
//-----------------------------------------------------------------------------
//
void  CHeaderBoxEL::OnPaint ()
{
    CPaintDC  dc(this);

    // 获得客户区大小
    CRect  clientRect;
    GetClientRect(&clientRect);

    // 设置文本背景透明
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(m_hdrTextColor);

    // 设置字体
    CFont*  oldfont = NULL;
    CFont*  newfont = m_hdrFont;
    if (newfont)
    {
        oldfont = dc.SelectObject(newfont);     // 记得选回
    }

    {
        //
        // 绘制每个col格子
        //
        int  nItem = GetColumnCount();          // 得到有几个单元 
        for (int i = 0; i < nItem; i++) 
        { 
            CRect  tRect;                       // 得到Item的尺寸
            tRect = GetItemRect(i);             // 得到Item的尺寸

            HeaderBoxELColumn*  colAttr = GetColumn(i);
            ASSERT_BREAK(colAttr);

            // 绘制背景
            dc.FillSolidRect(tRect, m_hdrBkColor);

            // 绘制分割线, 格子的右边
            if (i > 0)  // 第一个格子不画
            {
                dc.FillSolidRect(tRect.left, tRect.top, 1, tRect.Height(), m_hdrGridlineColor);
            }


            dc.DrawText(colAttr->hdrText, &tRect, colAttr->hdrAlignment | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
        } 

        {
            //
            // 画头部剩余部分
            //
            if (nItem > 0)  // 判断是否有至少一个格子
            {
                CRect  rtRect;
                rtRect = GetItemRect(nItem - 1);
                dc.FillSolidRect(rtRect.right, rtRect.top, 
                    clientRect.right-rtRect.right, rtRect.Height(), m_hdrBkColor);
                // 绘制分割线, 格子的左边
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    return TRUE;
}



//
//=============================================================================
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 注册窗口类
//-----------------------------------------------------------------------------
//
BOOL  CHeaderBoxEL::RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon, int bgStockBrush)
{
    WNDCLASS  wlsold = {0};
    if (::GetClassInfo(hInstance, lpszClassName, &wlsold))
    {
        // class已经存在
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