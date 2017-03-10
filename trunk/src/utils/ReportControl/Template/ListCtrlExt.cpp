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
// ��ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create֮����������֮��
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLine::PostInitTemplateStyle ()
{
    // ���ñ�ͷ��ʽ
    SetHeaderHeight(ELT_T1_HDRHEIGHT);                                  // ����ͷ���߶�
    SetHeaderGridlineColor(ELT_T1_HDRGRIDCOLOR);
    SetHeaderBkColor(ELT_T1_HDRBKCOLOR);                                // ����ͷ������ɫ
    SetHeaderTextColor(ELT_T1_HDRTEXTCOLOR);                            // ����ͷ��������ɫ

    // ���ñ�����ʽ
    SetRowHeight(ELT_T1_ROWHEIGHT);
#if SU_FEI
    SetHasGridline(FALSE, TRUE);
#else
	SetHasGridline(TRUE, FALSE);
#endif
    SetGradientGridline(FALSE, FALSE);                                  // ���ý����������
    SetBkColor(ELT_T1_BKCOLOR);                                         // ���ñ���ɫ
    SetTextColor(ELT_T1_TEXTCOLOR);                                     // �����ı���ɫ
    SetSelectionRowHighlightColor(ELT_T1_HLCOLOR);                      // ����ѡ�����
    SetTextHighlightColor(ELT_T1_HLTEXTCOLOR);
    SetSelectionRowLowlightColor(ELT_T1_LLCOLOR);                       // ���ö�����ʱѡ���е�����ɫ
    SetFocusedSubItemHighlightColor(ELT_T1_SUBHLCOLOR);                 // ����item������ɫ

    // ����inplace�ؼ���ɫ
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
// ��ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create֮����������֮��
//-----------------------------------------------------------------------------
//
void  CListCtrlT_HLine::PostInitTemplateStyle ()
{
    // ���ñ�ͷ��ʽ
    SetHeaderHeight(0);                                  // ����ͷ���߶�
    SetHeaderGridlineColor(ELT_T1_HDRGRIDCOLOR);
    SetHeaderBkColor(ELT_T1_HDRBKCOLOR);                                // ����ͷ������ɫ
    SetHeaderTextColor(ELT_T1_HDRTEXTCOLOR);                            // ����ͷ��������ɫ

    // ���ñ�����ʽ
    SetRowHeight(ELT_T1_ROWHEIGHT);
    SetHasGridline(TRUE, FALSE);
    SetGradientGridline(TRUE, TRUE);                                    // ���ý����������
    SetBkColor(ELT_T1_BKCOLOR);                                         // ���ñ���ɫ
    SetTextColor(ELT_T1_TEXTCOLOR);                                     // �����ı���ɫ
    SetSelectionRowHighlightColor(ELT_T1_HLCOLOR);                      // ����ѡ�����
    SetTextHighlightColor(ELT_T1_HLTEXTCOLOR);
    SetSelectionRowLowlightColor(ELT_T1_LLCOLOR);                       // ���ö�����ʱѡ���е�����ɫ
    SetFocusedSubItemHighlightColor(ELT_T1_SUBHLCOLOR);                 // ����item������ɫ

    // ����inplace�ؼ���ɫ
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
// �ӳٳ�ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create, ����������֮��
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLineMidH::PostInitTemplateStyle ()
{
    CListCtrlT_VLine::PostInitTemplateStyle();
}

//
//-----------------------------------------------------------------------------
// ���»��Ʊ���, ʹ�ó��˻��ƴ�ֱ��, ���ڱ����м仭����ˮƽ�ָ���
//-----------------------------------------------------------------------------
//
void  CListCtrlT_VLineMidH::FillGridline (CDC* pDC)
{
    COLORREF  gridColor = m_Header.GetHdrGridlineColor();
    CRect  clientRect;
    GetClientRect(&clientRect);

    // ���ƴ�ֱ������
    if (m_hasVGridline)
    {
        int  colCount = m_Header.GetItemCount();
        int  curColPos = 0;

        // �Ƿ���ƽ����������
        if (m_gradientVGridline)
        {
            // ���಻֧�ֽ���ģʽ
            ASSERT_BREAK(FALSE);
        }
        else
        {
            // �����м��ˮƽ�ָ���, �ҵ��м��һ�б��, Ȼ�������е����¸���һ��ˮƽ��
            int  itemCount = GetItemCount();
            int  midItemIndex = itemCount / 2;
            CRect  midItemSub0Rect = GetRowItemRect(midItemIndex, 0);
            pDC->FillSolidRect(clientRect.left, midItemSub0Rect.top, clientRect.Width(), 1, gridColor);
            pDC->FillSolidRect(clientRect.left, midItemSub0Rect.bottom, clientRect.Width(), 1, gridColor);


            // ���ƴ�ֱʵ��, ����������ˮƽ�߷ָ���
            for (int i = 0; i < colCount; i++)
            {
                int  colWidth = ELSUPER::GetColumnWidth(i);
                curColPos += colWidth;

                // �ж������߻����Ƿ�Ϸ�, ͨ�������ʹ������ӵı���������ᳬ��һ��Ļ
                ASSERT_BREAK(midItemSub0Rect.top-clientRect.top > 0);
                ASSERT_BREAK(clientRect.bottom-midItemSub0Rect.bottom > 0);
                pDC->FillSolidRect(curColPos, clientRect.top, 1, midItemSub0Rect.top-clientRect.top, gridColor);
                pDC->FillSolidRect(curColPos, midItemSub0Rect.bottom, 1, clientRect.bottom-midItemSub0Rect.bottom, gridColor);
            }

            // ����ˮƽ�����
            pDC->FillSolidRect(clientRect.left, clientRect.bottom-1, clientRect.Width(), 1, gridColor);

            // ����ˮƽ�ⶥ��
            pDC->FillSolidRect(clientRect.left, clientRect.top+m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
        }
    }

	// ����ˮƽ������
	if (m_hasHGridline)
	{
#if !SU_FEI	
		pDC->FillSolidRect(clientRect.left, clientRect.top + m_Header.GetHdrHeight(), clientRect.Width(), 1, gridColor);
#endif
		// ÿҳ������
		int  toprow = ELSUPER::GetTopIndex();
		int  chunk_height = ELSUPER::GetCountPerPage();
		int  itemCount = GetItemCount();
		int  hlineCount = min(chunk_height, itemCount);

		// �Ƿ���ƽ����������
		if (m_gradientVGridline)
		{
			// ���಻֧�ֽ���ģʽ
			ASSERT_BREAK(FALSE);
		}
		else
		{
			// ����ʵ��
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
// CTreeBoxT_VLine    ��ֱ�ָ��ߵ�����TreeBox
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
// �ӳٳ�ʼ����ģ�����ʽ, 
// ���ñ�������Ҫ�ڴ���Create, ����������֮��
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

    // ������Ԫ��
    InsertColumn(COL_TITLE,_T(""), EL_LEFT, ELT_ABT_COLTITLE_WIDTH, FALSE, EL_LEFT, EL_COLOR_DEFAULT, ELT_ABT_COLTITLE_MARGIN);
    InsertColumn(COL_SUBTITLE,_T(""), EL_LEFT, ELT_ABT_COLSUBTITLE_WIDTH);
    InsertColumn(COL_DESC,_T(""), EL_LEFT, rcWnd.Width()-ELT_ABT_COLTITLE_WIDTH-ELT_ABT_COLSUBTITLE_WIDTH);

    // ROW_TOP_BLANK
    InsertRecordRow(ROW_TOP_BLANK, L"");

    // ROW_PRODUCT_NAME
    InsertRecordRow(ROW_PRODUCT_NAME, L"��Ʒ��Ϣ");
    SetRowItemText(ROW_PRODUCT_NAME, COL_SUBTITLE, L"��Ʒ���� :");
    SetRowItemText(ROW_PRODUCT_NAME, COL_DESC, L"");

    // ROW_HW_VER
    InsertRecordRow(ROW_HW_VER, L"");
    SetRowItemText(ROW_HW_VER, COL_SUBTITLE, L"Ӳ���汾 :");
    SetRowItemText(ROW_HW_VER, COL_DESC, L"V1.0");

    // ROW_SYS_VER
    InsertRecordRow(ROW_SYS_VER, L"");
    SetRowItemText(ROW_SYS_VER, COL_SUBTITLE, L"ϵͳ�汾 :");
    SetRowItemText(ROW_SYS_VER, COL_DESC, L"V1.0");

    // ROW_SW_VER
    InsertRecordRow(ROW_SW_VER, L"");
    SetRowItemText(ROW_SW_VER, COL_SUBTITLE, L"����汾 :");
    SetRowItemText(ROW_SW_VER, COL_DESC, L"V1.0");

    // ROW_COMPANY_NAME
    InsertRecordRow(ROW_COMPANY_NAME, L"��˾��Ϣ");
    SetRowItemText(ROW_COMPANY_NAME, COL_SUBTITLE, L"��˾���� :");
    SetRowItemText(ROW_COMPANY_NAME, COL_DESC, L"");

    // ROW_COMPANY_ADDR
    InsertRecordRow(ROW_COMPANY_ADDR, L"");
    SetRowItemText(ROW_COMPANY_ADDR, COL_SUBTITLE, L"��˾��ַ :");
    SetRowItemText(ROW_COMPANY_ADDR, COL_DESC, L"");

    // ROW_SUPPORT
    InsertRecordRow(ROW_SUPPORT, L"");
    SetRowItemText(ROW_SUPPORT, COL_SUBTITLE, L"����֧�� :");
    SetRowItemText(ROW_SUPPORT, COL_DESC, L"");

    // ROW_BOT_BLANK
    InsertRecordRow(ROW_BOT_BLANK, L"");

    // ���ñ�����ɫ
    {
        CELRecord*  rowProductInfo = GetRecordRow(ROW_PRODUCT_NAME);
        CELRecordItem*  itemProductTitle = rowProductInfo->GetItem(COL_TITLE);
        itemProductTitle->SetColor(ELT_ABT_COLTITLE_COLOR, EL_COLOR_DEFAULT);
    }

    // ���ñ�����ɫ
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

    int  centerX = (clientRect.left + clientRect.right) / 2;  // �����е�


    // �����м��ˮƽ�ָ���
    int  itemCount = GetItemCount();
    int  midItemIndex = itemCount / 2 + 1;
    CRect  midItemSub0Rect = GetRowItemRect(midItemIndex, 0);
    
    // �����λ���
    CRect  gridRectLeft(clientRect.left, midItemSub0Rect.top, centerX, midItemSub0Rect.top+1);
    CRect  gridRectRight(centerX, midItemSub0Rect.top, clientRect.right, midItemSub0Rect.top+1);
    CommonEL::DrawGradient(pDC, gridRectLeft, GetBkColor(), gridColor);
    CommonEL::DrawGradient(pDC, gridRectRight, gridColor, GetBkColor());
}