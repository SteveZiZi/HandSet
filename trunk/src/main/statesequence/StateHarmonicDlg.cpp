// StateHarmonicDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StateHarmonicDlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/statesequence/StateSetDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/mgr/StatusMgr.h"

#define SH_COL_INDEX_CHAN                          0
#define SH_COL_INDEX_HORMANIC                      1
#define SH_COL_INDEX_AMPL                          2
#define SH_COL_INDEX_PHASE_ANGLE                   3

//状态序列-谐波
//
#define DSM_STATE_HARMONIC_WIDTH_CHAN              90
#define DSM_STATE_HARMONIC_WIDTH_HORMANIC          170
#define DSM_STATE_HARMONIC_WIDTH_AMPL              170
#define DSM_STATE_HARMONIC_WIDTH_PHASE_ANGLE       170

// CStateHarmonicDlg 对话框

IMPLEMENT_DYNAMIC(CStateHarmonicDlg, CBaseDlg)

CStateHarmonicDlg::CStateHarmonicDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateHarmonicDlg::IDD, DSM_STR_STATE_HARMONIC_CAPTION, pParent)
    , m_pStatus(NULL)
{
    m_nMenuID = IDR_MENU_STATE_SET;//IDR_MENU_STATE_HARMONIC;
}

CStateHarmonicDlg::~CStateHarmonicDlg()
{
}

void CStateHarmonicDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateHarmonicDlg, CBaseDlg)
    ON_COMMAND_RANGE(ID_MENU_STATE_SET_F1, ID_MENU_STATE_SET_F5, &CStateHarmonicDlg::OnNaviMenuStateHarmonic)

    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_HARMONIC_RET, &CStateHarmonicDlg::OnHarmonicItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATE_HARMONIC_RET, &CStateHarmonicDlg::OnHarmonicItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_HARMONIC_RET,  &CStateHarmonicDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_HARMONIC_RET,  &CStateHarmonicDlg::OnInplaceEditAction)
END_MESSAGE_MAP()


// CStateHarmonicDlg 消息处理程序

BOOL CStateHarmonicDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    

    //
    //从状态列表中进入
    //
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL || 
        pPageData->m_nPageID != CStateSetDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
    {
        //
        //从设置 进入
        //
        return FALSE;
    }

    CStateSetDataExtra* pDataExtra = dynamic_cast<CStateSetDataExtra*>(pPageData->m_pPageDataExtra);
    if (pDataExtra == NULL || pDataExtra->m_pStatus == NULL)
    {
        return FALSE;
    }

    m_pStatus = pDataExtra->m_pStatus;

    if (m_pStatus == NULL)
    {
        return FALSE;
    }

    _createReport();
    _resetMenu();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}



void  CStateHarmonicDlg::_createReport()
{
    ASSERT(m_pStatus);

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlHarmonicReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_HARMONIC_RET);

    // 设置字体
    m_ctlHarmonicReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlHarmonicReport.SetHeaderFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlHarmonicReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlHarmonicReport.InsertColumn(SH_COL_INDEX_CHAN, L"   " + dsm_stringMgr::Get(DSM_STR_STATE_CHAN), EL_CENTER, DSM_STATE_HARMONIC_WIDTH_CHAN);
    m_ctlHarmonicReport.InsertColumn(SH_COL_INDEX_HORMANIC, dsm_stringMgr::Get(DSM_STR_STATE_HARMONIC), EL_LEFT, DSM_STATE_HARMONIC_WIDTH_HORMANIC);
    m_ctlHarmonicReport.InsertColumn(SH_COL_INDEX_AMPL, dsm_stringMgr::Get(DSM_STR_STATE_AMPL), EL_LEFT, DSM_STATE_HARMONIC_WIDTH_AMPL);
    m_ctlHarmonicReport.InsertColumn(SH_COL_INDEX_PHASE_ANGLE, dsm_stringMgr::Get(DSM_STR_STATE_PHASE_ANGLE), EL_LEFT, DSM_STATE_HARMONIC_WIDTH_PHASE_ANGLE);

    // 设置选择焦点样式
    m_ctlHarmonicReport.SetRowSelectionVisible(FALSE);
    m_ctlHarmonicReport.SetRowFocusEnable(TRUE);
    m_ctlHarmonicReport.SetSubItemFocusEnable(TRUE);
    m_ctlHarmonicReport.SetFocusedColIndex(1);
    m_ctlHarmonicReport.SetFreezeColumnsCount(1);
   // m_ctlHarmonicReport.SetFreezeSelectionVisible(TRUE);
    
    _recontentReport();
    m_ctlHarmonicReport.GetColumnExAttr(SH_COL_INDEX_HORMANIC)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlHarmonicReport.GetColumnExAttr(SH_COL_INDEX_AMPL)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlHarmonicReport.GetColumnExAttr(SH_COL_INDEX_PHASE_ANGLE)->GetEditOptions()->SetAllowEdit(TRUE);
}

void CStateHarmonicDlg::_recontentReport()
{
    m_ctlHarmonicReport.SetRedraw(FALSE);
    int nRowSel = m_ctlHarmonicReport.GetFocusedRowIndex();

    m_ctlHarmonicReport.DeleteAllRecordRow();

    CSMVSendVirtualChannel* pVirChannel = m_pStatus->FirstSMVSendVirtualChannel();
    int nIndex = 0;
    while(pVirChannel != NULL)
    {
        //
        //通道只有电压和电流可以添加谐波
        //
        CString strChanName = _T("");
        strChanName = pVirChannel->GetName().c_str();
        if (strChanName.GetLength() > 0)
        {
            if(strChanName[0] == _T('U') || strChanName[0] == _T('I'))
            {
                m_ctlHarmonicReport.InsertRecordRow(nIndex, new CDsmStateHarmonicRecord(pVirChannel));
                m_ctlHarmonicReport.SetRecordRowData(nIndex, (DWORD_PTR)pVirChannel);
                ++nIndex;
            }
        }

        pVirChannel = m_pStatus->NextSMVSendVirtualChannel(pVirChannel);
    }
    if (nRowSel >= 0)
    {
        m_ctlHarmonicReport.SetFocusedSelectionRowIndex(nRowSel);
    }
    else
    {
        m_ctlHarmonicReport.SetFocusedSelectionRowIndex(0);
    }
    m_ctlHarmonicReport.SetRedraw(TRUE);
}

void  CStateHarmonicDlg::_resetMenu()
{
#ifdef ZY_PRJ
	int  nRowIndex = m_ctlHarmonicReport.GetFocusedRowIndex();
    
    if (nRowIndex >= 0)
    {
        CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlHarmonicReport.GetRecordRowData(nRowIndex);
        if (pVirChannel)
        {
            CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
            ASSERT(pHarmonic);
            if (pHarmonic == NULL)
            {
                return;
            }
            
            //等幅值
            if (pHarmonic->GetHarmonicType() == HARMONIC_NONE)
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_STATE_EQUAL_AMPL));
            }

            //等相角
            if (pHarmonic->GetHarmonicType() == HARMONIC_NONE || 
                pHarmonic->GetHarmonicType() == HARMONIC_DC)
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_STATE_EQUAL_PHASE_ANGLE));
            }
            m_pNaviBar->UpdateNaviMenu();
        }
    }
#else
	
#endif
}

void CStateHarmonicDlg::OnNaviMenuStateHarmonic(UINT nID)
{
    
    int nRowSel = m_ctlHarmonicReport.GetFocusedRowIndex();

    switch (nID)
    {
    case ID_MENU_STATE_SET_F1:
        {
            //清除
            CSMVSendVirtualChannel* pVirChannel = m_pStatus->FirstSMVSendVirtualChannel();
            while(pVirChannel != NULL)
            {
                //
                //通道只有电压和电流可以添加谐波
                //
                CString strChanName = _T("");
                strChanName = pVirChannel->GetName().c_str();
                if (strChanName.GetLength() > 0)
                {
                    pVirChannel->GetHarmonic()->SetHarmonicType(HARMONIC_NONE);
                }
                pVirChannel = m_pStatus->NextSMVSendVirtualChannel(pVirChannel);
            }

            _recontentReport();
        }
        break;
    case ID_MENU_STATE_SET_F2:
        {
            //等谐波
            if (nRowSel < 0)
            {
                return;
            }
            
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel* )m_ctlHarmonicReport.GetRecordRowData(nRowSel);
            if (pVirChannel == NULL)
            {
                return;
            }
            
            m_pStatus->EqualHarmonicType(pVirChannel);

            _recontentReport();
        }
        break;
    case ID_MENU_STATE_SET_F3:
        {
            //等幅值
            if (nRowSel < 0)
            {
                return;
            }

            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel* )m_ctlHarmonicReport.GetRecordRowData(nRowSel);
            if (pVirChannel)
            {
                CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
                ASSERT(pHarmonic);
                if (pHarmonic == NULL)
                {
                    return;
                }

                //等幅值
                if (pHarmonic->GetHarmonicType() != HARMONIC_NONE)
                {
                    m_pStatus->EqualHarmonicAmplitude(pVirChannel);
                    _recontentReport();
                }
                return;
            }
        }
        break;
    case ID_MENU_STATE_SET_F4:
        {

            //等相角
            if (nRowSel < 0)
            {
                return;
            }
            
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel* )m_ctlHarmonicReport.GetRecordRowData(nRowSel);
            if (pVirChannel)
            {
                CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
                ASSERT(pHarmonic);
                if (pHarmonic == NULL)
                {
                    return;
                }
                //等相角
                if (pHarmonic->GetHarmonicType() != HARMONIC_NONE && 
                    pHarmonic->GetHarmonicType() != HARMONIC_DC)
                {
                    m_pStatus->EqualHarmonicPhaseAngle(pVirChannel);
                    _recontentReport();
                }
                return;
            }
        }
        break;
    }
}


void CStateHarmonicDlg::OnHarmonicItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{

    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }
    ASSERT(pItemNotify->rowIndex >= 0);

    //虚拟通道
    CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel* )m_ctlHarmonicReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pVirChannel == NULL)
    {
        return;
    }
    CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
    if (pHarmonic == NULL)
    {
        return;
    }
    DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();
    CString strChanName = pVirChannel->GetName().c_str();

    switch(pItemNotify->colIndex)
    {
    case SH_COL_INDEX_HORMANIC:
        {
            //谐波
            pHarmonic->SetHarmonicType((enumHarmonicType)dwValue);
            _resetMenu();

            CELRecordItemText * pItemAmpl = dynamic_cast<CELRecordItemText*>(m_ctlHarmonicReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(SH_COL_INDEX_AMPL));
            CELRecordItemText * pItemPhaseAngle = dynamic_cast<CELRecordItemText*>(m_ctlHarmonicReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(SH_COL_INDEX_PHASE_ANGLE));
            if (pItemAmpl == NULL || pItemPhaseAngle == NULL)
            {
                return;
            }
            
            //
            //幅值
            //
            if (dwValue == HARMONIC_NONE)
            {
                pItemAmpl->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_EMPTY));
                pItemAmpl->SetFormatString(_T(""));
                pItemAmpl->SetEditable(FALSE);
            }
            else
            {    //幅值
                CString strContent = _T("");

                strContent.Format(_T("%.03lf"), pHarmonic->GetAmplitude());
                pItemAmpl->SetValue(strContent);
                pItemAmpl->SetEditable(TRUE);
                if (strChanName.GetLength() == 0)
                {
                    return;
                }

                if (strChanName[0] == _T('U'))
                {
                    //电压
                    pItemAmpl->SetFormatString(_T("%s V"));
                }
                else if (strChanName[0] == _T('I'))
                {
                    //电流
                    pItemAmpl->SetFormatString(_T("%s A"));
                }
            }

            //
            //相角
            //
            if (dwValue == HARMONIC_NONE || dwValue == HARMONIC_DC)
            {
                pItemPhaseAngle->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_EMPTY));
                pItemPhaseAngle->SetFormatString(_T("%s"));
                pItemPhaseAngle->SetEditable(FALSE);
            }
            else
            {    //幅值
                CString strContent = _T("");
                //
                //相角
                //
                strContent.Format(_T("%.03lf"), pHarmonic->GetPhaseAngle());
                pItemPhaseAngle->SetValue(strContent);
                pItemPhaseAngle->SetFormatString(_T("%s°"));
                pItemPhaseAngle->SetEditable(TRUE);
            }
        }
        break;
    }

}

void CStateHarmonicDlg::OnHarmonicItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }
    ASSERT(pItemNotify->rowIndex >= 0);

    //虚拟通道
    CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel* )m_ctlHarmonicReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pVirChannel == NULL)
    {
        return;
    }
    CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
    if (pHarmonic == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

    switch(pItemNotify->colIndex)
    {

    case SH_COL_INDEX_AMPL:
        {

            double dValue = 0.0f;
            CString strValue = pItem->GetValue();
            if (DSM_Str2Double(strValue, dValue))
            {
                pHarmonic->SetAmplitude(dValue);
            }
            strValue.Format(_T("%.03lf"), pHarmonic->GetAmplitude());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case SH_COL_INDEX_PHASE_ANGLE:
        {
            double dValue = 0.0f;
            CString strValue = pItem->GetValue();
            if (DSM_Str2Double(strValue, dValue))
            {
                pHarmonic->SetPhaseAngle(dValue);
            }
            strValue.Format(_T("%.03lf"), pHarmonic->GetPhaseAngle());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    }


}


void CStateHarmonicDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit弹出时, 切换菜单
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlHarmonicReport.GetInplaceEdit());
        *pResult = TRUE;
    }
    // inplace退出时，切换菜单
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}

BOOL CStateHarmonicDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    BOOL bRet = CBaseDlg::PreTranslateMessage(pMsg);
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
            {
                _resetMenu();
            }
            break;
		case VK_F8:
			{
				_showContexMenu();
				return TRUE;
			}
			break;
        }
    }
    return bRet;

}
void CStateHarmonicDlg::_showContexMenu()
{ 
	int nRowIndex = m_ctlHarmonicReport.GetFocusedRowIndex();
	int nColIndex = m_ctlHarmonicReport.GetFocusedColIndex();
	if (nRowIndex <0 || nColIndex < 0 )
	{
		return;
	}

	CMenu menu;
	menu.LoadMenu(IDR_MENU_STATE_HARMONIC);;

	if (nRowIndex >= 0)
	{
		CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlHarmonicReport.GetRecordRowData(nRowIndex);
		if (pVirChannel)
		{
			CHarmonic* pHarmonic = pVirChannel->GetHarmonic();
			ASSERT(pHarmonic);
			if (pHarmonic == NULL)
			{
				return;
			}

			//等幅值
			if (pHarmonic->GetHarmonicType() == HARMONIC_NONE)
			{
				m_contexMenu.LoadMenu(menu.GetSubMenu(4));
			}
			//等相角
			else if (pHarmonic->GetHarmonicType() == HARMONIC_DC)
			{
				m_contexMenu.LoadMenu(menu.GetSubMenu(5));
			}
			else
			{
				m_contexMenu.LoadMenu(menu.GetSubMenu(6));
			}
		}
	}


	CELRecordItemText* pItem = (CELRecordItemText*)m_ctlHarmonicReport.GetRecordRow(nRowIndex)->GetItem(nColIndex);
	CRect rect = m_ctlHarmonicReport.GetRowItemRect(nRowIndex,nColIndex);	 


	if (pItem == NULL )
	{
		return; 
	}


	//CNaviMenu* pPopup = m_ContexMenu.GetSubMenu(5);

	CRect rcItem = rect;

	CNaviPopupBar popupBar;

	UINT  cmdId = m_contexMenu.TrackPopupMenu(rcItem, &m_ctlHarmonicReport, &popupBar, CNaviMenu::RET_CMDID);
	if (cmdId )
	{
		this->SendMessage(WM_COMMAND, cmdId);

	} 

}