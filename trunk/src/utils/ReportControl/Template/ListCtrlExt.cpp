#include "stdafx.h"

#include "ListCtrlExt.h"
#include "../Common/CommonEL.h"


//
//=============================================================================
// CListCtrlT_VLine 
//=============================================================================
//


CListCtrlT_VLine::CListCtrlT_VLine ()
{


}
CListCtrlT_VLine::~CListCtrlT_VLine ()
{

}

//
//-----------------------------------------------------------------------------
// 初始化本模板的样式, 
// 调用本函数需要在窗口Create之后并设置字体之后
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLine::PostInitTemplateStyle ()
{
    // 设置表头样式
    SetHeaderHeight(ELT_T1_HDRHEIGHT);                                  // 设置头部高度
    SetHeaderGridlineColor(ELT_T1_HDRGRIDCOLOR);
    SetHeaderBkColor(ELT_T1_HDRBKCOLOR);                                // 设置头部背景色
    SetHeaderTextColor(ELT_T1_HDRTEXTCOLOR);                            // 设置头部字体颜色

    // 设置表体样式
    SetRowHeight(ELT_T1_ROWHEIGHT);
#if SU_FEI
    SetHasGridline(FALSE, TRUE);
#else
	SetHasGridline(TRUE, FALSE);
#endif
    SetGradientGridline(FALSE, FALSE);                                  // 设置渐变的网格线
    SetBkColor(ELT_T1_BKCOLOR);                                         // 设置背景色
    SetTextColor(ELT_T1_TEXTCOLOR);                                     // 设置文本颜色
    SetSelectionRowHighlightColor(ELT_T1_HLCOLOR);                      // 设置选择高亮
    SetTextHighlightColor(ELT_T1_HLTEXTCOLOR);
    SetSelectionRowLowlightColor(ELT_T1_LLCOLOR);                       // 设置丢焦点时选择行低亮颜色
    SetFocusedSubItemHighlightColor(ELT_T1_SUBHLCOLOR);                 // 设置item焦点颜色

    // 设置inplace控件颜色
    GetInplaceEdit()->SetBkColor(ELT_T1_EDITBKCOLOR);
    GetInplaceEdit()->SetTextColor(ELT_T1_EDITTEXTCOLOR);
    GetInplaceEdit()->SetBorderColor(ELT_T1_EDITBORDERCOLOR);
    GetInplaceList()->SetBkColor(ELT_T1_LISTBKCOLOR);
    GetInplaceList()->SetTextColor(ELT_T1_LISTTEXTCOLOR);
    GetInplaceList()->SetSelStyle(FALSE, ELT_T1_LISTHLCOLOR, ELT_T1_LISTHLTEXTCOLOR, ELT_T1_LISTHLCOLOR);
}





//
//=============================================================================
// CListCtrlT_HLine 
//=============================================================================
//


CListCtrlT_HLine::CListCtrlT_HLine ()
{

}
CListCtrlT_HLine::~CListCtrlT_HLine ()
{

}


//
//-----------------------------------------------------------------------------
// 初始化本模板的样式, 
// 调用本函数需要在窗口Create之后并设置字体之后
//-----------------------------------------------------------------------------
//
void  CListCtrlT_HLine::PostInitTemplateStyle ()
{
    // 设置表头样式
    SetHeaderHeight(0);                                  // 设置头部高度
    SetHeaderGridlineColor(ELT_T1_HDRGRIDCOLOR);
    SetHeaderBkColor(ELT_T1_HDRBKCOLOR);                                // 设置头部背景色
    SetHeaderTextColor(ELT_T1_HDRTEXTCOLOR);                            // 设置头部字体颜色

    // 设置表体样式
    SetRowHeight(ELT_T1_ROWHEIGHT);
    SetHasGridline(TRUE, FALSE);
    SetGradientGridline(TRUE, TRUE);                                    // 设置渐变的网格线
    SetBkColor(ELT_T1_BKCOLOR);                                         // 设置背景色
    SetTextColor(ELT_T1_TEXTCOLOR);                                     // 设置文本颜色
    SetSelectionRowHighlightColor(ELT_T1_HLCOLOR);                      // 设置选择高亮
    SetTextHighlightColor(ELT_T1_HLTEXTCOLOR);
    SetSelectionRowLowlightColor(ELT_T1_LLCOLOR);                       // 设置丢焦点时选择行低亮颜色
    SetFocusedSubItemHighlightColor(ELT_T1_SUBHLCOLOR);                 // 设置item焦点颜色

    // 设置inplace控件颜色
    GetInplaceEdit()->SetBkColor(ELT_T1_EDITBKCOLOR);
    GetInplaceEdit()->SetTextColor(ELT_T1_EDITTEXTCOLOR);
    GetInplaceEdit()->SetBorderColor(ELT_T1_EDITBORDERCOLOR);
    GetInplaceList()->SetBkColor(ELT_T1_LISTBKCOLOR);
    GetInplaceList()->SetTextColor(ELT_T1_LISTTEXTCOLOR);
    GetInplaceList()->SetSelStyle(FALSE, ELT_T1_LISTHLCOLOR, ELT_T1_LISTHLTEXTCOLOR, ELT_T1_LISTHLCOLOR);
}


//
//=============================================================================
// CListCtrlT_VLineMidH
//=============================================================================
//

CListCtrlT_VLineMidH::CListCtrlT_VLineMidH ()
{

}
CListCtrlT_VLineMidH::~CListCtrlT_VLineMidH ()
{

}

//
//-----------------------------------------------------------------------------
// 延迟初始化本模板的样式, 
// 调用本函数需要在窗口Create, 并设置字体之后
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLineMidH::PostInitTemplateStyle ()
{
    CListCtrlT_VLine::PostInitTemplateStyle();
}

//
//-----------------------------------------------------------------------------
// 重新绘制边线, 使得除了绘制垂直线, 还在表格的中间画两条水平分割线
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLineMidH::FillGridline (CDC* pDC)
{
    COLORREF  gridColor = m_Header.GetHdrGridlineColor();
    CRect  clientRect;
    GetClientRect(&clientRect);

    // 绘制垂直网格线
    if (m_hasVGridline)
    {
        int  colCount = m_Header.GetItemCount();
        int  curColPos = 0;

        // 是否绘制渐变的网格线
        if (m_gradientVGridline)
        {
            // 本类不支持渐变模式
            ASSERT_BREAK(FALSE);
        }
        else
        {
            // 绘制中间的水平分割线, 找到中间的一行表格, 然后在这行的上下各画一条水平线
            int  itemCount = GetItemCount();
            int  midItemIndex = itemCount / 2;
            CRect  midItemSub0Rect = GetRowItemRect(midItemIndex, 0);
            pDC->FillSolidRect(clientRect.left, midItemSub0Rect.top, clientRect.Width(), 1, gridColor);
            pDC->FillSolidRect(clientRect.left, midItemSub0Rect.bottom, clientRect.Width(), 1, gridColor);


            // 绘制垂直实线, 但根据中心水平线分隔开
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);
                curColPos += colWidth;

                // 判断中心线绘制是否合法, 通常本类的使用所添加的表格行数不会超过一屏幕
                ASSERT_BREAK(midItemSub0Rect.top-clientRect.top > 0);
                ASSERT_BREAK(clientRect.bottom-midItemSub0Rect.bottom > 0);
                pDC->FillSolidRect(curColPos, clientRect.top, 1, midItemSub0Rect.top-clientRect.top, gridColor);
                pDC->FillSolidRect(curColPos, midItemSub0Rect.bottom, 1, clientRect.bottom-midItemSub0Rect.bottom, gridColor);
            }

            // 绘制水平封底线
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);

            // 绘制水平封顶线
            pDC->FillSolidRect(clientRect.left, clientRect.top+m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
        }
    }

	// 绘制水平网格线
	if (m_hasHGridline)
	{
#if !SU_FEI	
		pDC->FillSolidRect(clientRect.left, clientRect.top + m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
#endif
		// 每页的总数
		int  toprow = ELSUPER::GetTopIndex();
		int  chunk_height = ELSUPER::GetCountPerPage();
		int  itemCount = GetItemCount();
		int  hlineCount = min(chunk_height, itemCount);

		// 是否绘制渐变的网格线
		if (m_gradientVGridline)
		{
			// 本类不支持渐变模式
			ASSERT_BREAK(FALSE);
		}
		else
		{
			// 绘制实线
			for (int i = 1; i <= hlineCount; i++)
			{
				POINT  rowPoint;
				ELSUPER::GetItemPosition(toprow+i, &rowPoint);

				pDC->FillSolidRect(clientRect.left, rowPoint.y, clientRect.Width(), 1, gridColor);
			}
		}
	}
}

//
//=============================================================================
// CTreeBoxT_VLine    垂直分割线的美化TreeBox
//=============================================================================
//

CTreeBoxT_VLine::CTreeBoxT_VLine ()
{

}
CTreeBoxT_VLine::~CTreeBoxT_VLine ()
{

}

//
//-----------------------------------------------------------------------------
// 延迟初始化本模板的样式, 
// 调用本函数需要在窗口Create, 并设置字体之后
//-----------------------------------------------------------------------------
//
void  CTreeBoxT_VLine::PostInitTemplateStyle ()
{
    SetHeaderHeight(ELT_T1_HDRHEIGHT);
    SetHeaderGridlineColor(ELT_T1_HDRGRIDCOLOR);
    SetHeaderBkColor(ELT_T1_HDRBKCOLOR);
    SetHeaderTextColor(ELT_T1_HDRTEXTCOLOR);

    SetTreeItemHeight(ELT_T1_ROWHEIGHT);
    SetHasGridline(FALSE, TRUE);
    SetGradientGridline(FALSE, FALSE);
    SetBkColor(ELT_T1_BKCOLOR);
    SetTextColor(ELT_T1_TEXTCOLOR);
    SetSelStyle(FALSE, ELT_T1_HLCOLOR, ELT_T1_HLTEXTCOLOR, ELT_T1_LLCOLOR);
}


//
//=============================================================================
// CListCtrlT_AboutCtrl
//=============================================================================
//

CListCtrlT_AboutCtrl::CListCtrlT_AboutCtrl ()
{

}
CListCtrlT_AboutCtrl::~CListCtrlT_AboutCtrl ()
{

}

void  CListCtrlT_AboutCtrl::PostInitTemplateStyle ()
{
    CListCtrlT_HLine::PostInitTemplateStyle();
    SetRowFocusEnable(FALSE);


    CRect  rcWnd;
    GetClientRect(&rcWnd);

    // 插入列元素
    InsertColumn(COL_TITLE,_T(""), EL_LEFT, ELT_ABT_COLTITLE_WIDTH, FALSE, EL_LEFT, EL_COLOR_DEFAULT, ELT_ABT_COLTITLE_MARGIN);
    InsertColumn(COL_SUBTITLE,_T(""), EL_LEFT, ELT_ABT_COLSUBTITLE_WIDTH);
    InsertColumn(COL_DESC,_T(""), EL_LEFT, rcWnd.Width()-ELT_ABT_COLTITLE_WIDTH-ELT_ABT_COLSUBTITLE_WIDTH);

    // ROW_TOP_BLANK
    InsertRecordRow(ROW_TOP_BLANK, L"");

    // ROW_PRODUCT_NAME
    InsertRecordRow(ROW_PRODUCT_NAME, L"产品信息");
    SetRowItemText(ROW_PRODUCT_NAME, COL_SUBTITLE, L"产品名称 :");
    SetRowItemText(ROW_PRODUCT_NAME, COL_DESC, L"");

    // ROW_HW_VER
    InsertRecordRow(ROW_HW_VER, L"");
    SetRowItemText(ROW_HW_VER, COL_SUBTITLE, L"硬件版本 :");
    SetRowItemText(ROW_HW_VER, COL_DESC, L"V1.0");

    // ROW_SYS_VER
    InsertRecordRow(ROW_SYS_VER, L"");
    SetRowItemText(ROW_SYS_VER, COL_SUBTITLE, L"系统版本 :");
    SetRowItemText(ROW_SYS_VER, COL_DESC, L"V1.0");

    // ROW_SW_VER
    InsertRecordRow(ROW_SW_VER, L"");
    SetRowItemText(ROW_SW_VER, COL_SUBTITLE, L"软件版本 :");
    SetRowItemText(ROW_SW_VER, COL_DESC, L"V1.0");

    // ROW_COMPANY_NAME
    InsertRecordRow(ROW_COMPANY_NAME, L"公司信息");
    SetRowItemText(ROW_COMPANY_NAME, COL_SUBTITLE, L"公司名称 :");
    SetRowItemText(ROW_COMPANY_NAME, COL_DESC, L"");

    // ROW_COMPANY_ADDR
    InsertRecordRow(ROW_COMPANY_ADDR, L"");
    SetRowItemText(ROW_COMPANY_ADDR, COL_SUBTITLE, L"公司地址 :");
    SetRowItemText(ROW_COMPANY_ADDR, COL_DESC, L"");

    // ROW_SUPPORT
    InsertRecordRow(ROW_SUPPORT, L"");
    SetRowItemText(ROW_SUPPORT, COL_SUBTITLE, L"技术支持 :");
    SetRowItemText(ROW_SUPPORT, COL_DESC, L"");

    // ROW_BOT_BLANK
    InsertRecordRow(ROW_BOT_BLANK, L"");

    // 设置标题颜色
    {
        CELRecord*  rowProductInfo = GetRecordRow(ROW_PRODUCT_NAME);
        CELRecordItem*  itemProductTitle = rowProductInfo->GetItem(COL_TITLE);
        itemProductTitle->SetColor(ELT_ABT_COLTITLE_COLOR, EL_COLOR_DEFAULT);
    }

    // 设置标题颜色
    {
        CELRecord*  rowCompanyInfo = GetRecordRow(ROW_COMPANY_NAME);
        CELRecordItem*  itemCompanyTitle = rowCompanyInfo->GetItem(COL_TITLE);
        itemCompanyTitle->SetColor(ELT_ABT_COLTITLE_COLOR, EL_COLOR_DEFAULT);
    }
}


void  CListCtrlT_AboutCtrl::FillGridline (CDC* pDC)
{
    COLORREF  gridColor = m_Header.GetHdrGridlineColor();
    CRect  clientRect;
    GetClientRect(&clientRect);

    int  centerX = (clientRect.left + clientRect.right) / 2;  // 计算中点


    // 绘制中间的水平分割线
    int  itemCount = GetItemCount();
    int  midItemIndex = itemCount / 2 + 1;
    CRect  midItemSub0Rect = GetRowItemRect(midItemIndex, 0);
    
    // 分两段绘制
    CRect  gridRectLeft(clientRect.left, midItemSub0Rect.top, centerX, midItemSub0Rect.top+1);
    CRect  gridRectRight(centerX, midItemSub0Rect.top, clientRect.right, midItemSub0Rect.top+1);
    CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
    CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
}