#include "stdafx.h"

#include "ListCtrlEL.h"


IMPLEMENT_DYNAMIC(CHeaderCtrlEL, ELHDRSUPER)



//
//=============================================================================
// 创建控件
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
// 消息响应
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
// 按照用户设置的样式, 重绘表头
//-----------------------------------------------------------------------------
//
void  CHeaderCtrlEL::OnPaint ()
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
    CFont*  newfont = ELHDRSUPER::GetFont();
    if (newfont)
    {
        oldfont = dc.SelectObject(newfont);  // 记得选回
    }

    {
        //
        // 绘制每个col格子
        //
        int  nItem = ELHDRSUPER::GetItemCount();// 得到有几个单元 
        for (int i = 0; i < nItem; i++) 
        { 
            CRect  tRect;
            GetItemRect(i, &tRect);             // 得到Item的尺寸

            // 获得需要绘制的文本
            HDITEM  hdrinfo;
#define COLTEXT_MAX         16
            wchar_t  colText[COLTEXT_MAX];
            hdrinfo.mask = HDI_TEXT | HDI_FORMAT | HDI_LPARAM;
            hdrinfo.pszText = colText;
            hdrinfo.cchTextMax = COLTEXT_MAX;
            ELHDRSUPER::GetItem(i, &hdrinfo);

            // 获得列扩展属性
            CELColumn*  colExAttr = (CELColumn*)hdrinfo.lParam;
            ASSERT_BREAK(colExAttr);


            // 绘制背景
            dc.FillSolidRect(tRect, m_hdrBGColor);
#if SU_FEI
            // 绘制分割线, 格子的右边
            if (i > 0)  // 第一个格子不画
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
            // 画头部剩余部分
            //
            if (nItem > 0)  // 判断是否有至少一个格子
            {
                CRect  rtRect;
                GetItemRect(nItem - 1, &rtRect);
                dc.FillSolidRect(rtRect.right, rtRect.top, 
                    clientRect.right-rtRect.right, rtRect.Height(), m_hdrBGColor);
                // 绘制分割线, 格子的左边
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    return TRUE;
}



LRESULT  CHeaderCtrlEL::OnLayout ( WPARAM wParam, LPARAM lParam )
{

    LRESULT  lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
    HD_LAYOUT&  hdl = *( HD_LAYOUT * ) lParam; 
    RECT*  prc = hdl.prc; 
    WINDOWPOS*  pwpos = hdl.pwpos; 

    // 设置表头高度
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
// 辅助
//=============================================================================
//

//
//-----------------------------------------------------------------------------
// 获得指定列的列信息对象
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