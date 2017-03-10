// ListCtrlTestInfo.cpp : implementation file
//

#include "stdafx.h"
#include "FunctionTest.h"
#include "ListCtrlTestInfo.h"


// CListCtrlTestInfo

IMPLEMENT_DYNAMIC(CListCtrlTestInfo, CListCtrl)

CListCtrlTestInfo::CListCtrlTestInfo():
m_iLastSelectItem(-1)
{
    memset(m_crTextColor,0,sizeof(m_crTextColor));    
}

CListCtrlTestInfo::~CListCtrlTestInfo()
{

}

BEGIN_MESSAGE_MAP(CListCtrlTestInfo, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CListCtrlTestInfo::OnNMClick)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CListCtrlTestInfo::OnNMDblclk)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CListCtrlTestInfo::OnNMCustomdraw)
END_MESSAGE_MAP()

void CListCtrlTestInfo::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW*   pNMListView   =   (NM_LISTVIEW*)pNMHDR;  

	//得到当前选中的项
	int iItem=pNMListView->iItem;       

	if (iItem >=0)
	{        
		//去掉上一次选中的项的选中状态
		if (m_iLastSelectItem >=0)
		{
			SetCheck(m_iLastSelectItem,FALSE);
			SetItemState(m_iLastSelectItem,0,LVNI_SELECTED);
		}

		SetCheck(iItem,TRUE);
		SetItemState(iItem,LVNI_SELECTED,LVNI_SELECTED);
		m_iLastSelectItem=iItem;
	}
    else
    {
        SetCheck(m_iLastSelectItem,TRUE);
        SetItemState(m_iLastSelectItem,LVNI_SELECTED,LVNI_SELECTED);
    }

	*pResult = 1;
}

void CListCtrlTestInfo::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_LISTVIEW*   pNMListView   =   (NM_LISTVIEW*)pNMHDR;  
    //得到当前选中的项
    int iItem=pNMListView->iItem;

    if (iItem >=0)
    {

    }
    else
    {
        SetCheck(m_iLastSelectItem,TRUE);
        SetItemState(m_iLastSelectItem,LVNI_SELECTED,LVNI_SELECTED);
    }

    *pResult = 1;
}


BOOL CListCtrlTestInfo::SetCheck(int nItem, BOOL fCheck /* = TRUE */)
{
    if (fCheck)
    {
        m_iLastSelectItem=nItem;
    }

    return CListCtrl::SetCheck(nItem,fCheck);
}
void CListCtrlTestInfo::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{

    NMLVCUSTOMDRAW*   pLVCD   =   reinterpret_cast<NMLVCUSTOMDRAW*>(   pNMHDR   ); 
    //   Take   the   default   processing   unless   we   set   this   to   something   else   below.   
    *pResult   =   0;   

    //   First   thing   -   check   the   draw   stage.   If   it's   the   control's   prepaint   
    //   stage,   then   tell   Windows   we   want   messages   for   every   item.   
    if   (   CDDS_PREPAINT   ==   pLVCD->nmcd.dwDrawStage   )   
    {   
        *pResult   =   CDRF_NOTIFYITEMDRAW;   
    }   
    else   if   (   CDDS_ITEMPREPAINT   ==   pLVCD->nmcd.dwDrawStage   )   
    {   
        //   This   is   the   notification   message   for   an   item.     We'll   request   
        //   notifications   before   each   subitem's   prepaint   stage.   

        *pResult   =   CDRF_NOTIFYSUBITEMDRAW;   
    }   
    else   if   (   (CDDS_ITEMPREPAINT   |   CDDS_SUBITEM)   ==   pLVCD->nmcd.dwDrawStage   )   
    {   
        //   This   is   the   prepaint   stage   for   a   subitem.   Here's   where   we   set   the   
        //   item's   text   and   background   colors.   Our   return   value   will   tell     
        //   Windows   to   draw   the   subitem   itself,   but   it   will   use   the   new   colors   
        //   we   set   here.   

        //if(GetItemText(pLVCD->nmcd.dwItemSpec,14).Find(_T("以太网"))   !=   -1)   {

        //   Store   the   colors   back   in   the   NMLVCUSTOMDRAW   struct.   
        pLVCD->clrText   =   m_crTextColor[pLVCD->nmcd.dwItemSpec];   
        //pLVCD->clrTextBk   =   RGB(0, 0, 0);   
        //}
        //   Tell   Windows   to   paint   the   control   itself.   
        *pResult   =   CDRF_DODEFAULT;   
    }   
}


void CListCtrlTestInfo::SetItemColor(int nItem,COLORREF cr)
{
    if (nItem <0 ||nItem>=MAX_TEST_OBJ)
    {
        return;
    }

    m_crTextColor[nItem]=cr;
}

void CListCtrlTestInfo::ResetAllItemsColor()
{
    memset(m_crTextColor,0,sizeof(m_crTextColor));
}