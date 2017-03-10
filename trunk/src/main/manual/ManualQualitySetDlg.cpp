/// @file
///
/// @brief
///     品质设置 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///
#include "stdafx.h"
#include "ManualQualitySetDlg.h"
#include "src/main/manual/manualsmvchanqualitydlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/popdlg/MsgDlg.h"

#define DSM_MQS_WIDTH_CAPTION           180
#define DSM_MQS_WIDTH_CONTENT           349

// CManualQualitySetDlg 对话框

IMPLEMENT_DYNAMIC(CManualQualitySetDlg, CBaseDlg)

CManualQualitySetDlg::CManualQualitySetDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualQualitySetDlg::IDD, DSM_STR_MANUAL_QUALITY_SET_CAPTION, pParent)
    , m_pIEC92Cb(NULL)
    , m_pChannel(NULL)
{
    m_nMenuID = IDR_MENU_MANUAL_QUALITY_SET;
}

CManualQualitySetDlg::~CManualQualitySetDlg()
{
}

void CManualQualitySetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualQualitySetDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDD_MANUAL_QUALITY_SET_RET, &CManualQualitySetDlg::OnQualitySetItemSelectChanged)
    ON_COMMAND_RANGE(ID_MUNU_MQS_F1,   ID_MUNU_MQS_F2, &CManualQualitySetDlg::OnNaviMenuQualitySet)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualQualitySetDlg 消息处理程序

BOOL CManualQualitySetDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    
    //从状态列表中进入
    
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL || 
        pPageData->m_nPageID != CManualSmvChanQualityDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
    {
        return FALSE;
    }

    CManualSmvChanQualityDataExtra* pDataExtra = dynamic_cast<CManualSmvChanQualityDataExtra*>(pPageData->m_pPageDataExtra);
    if (pDataExtra == NULL || 
        pDataExtra->m_pBaseCb == NULL || 
        pDataExtra->m_pChannel == NULL)
    {
        return FALSE;
    }
    m_pIEC92Cb = dynamic_cast<CIEC9_2Cb*>(pDataExtra->m_pBaseCb);
    m_pChannel = pDataExtra->m_pChannel;
    if (m_pIEC92Cb == NULL || m_pChannel == NULL)
    {
        return FALSE;
    }

#if !CHUN_HUA
	if (pDataExtra->m_pChannelList.GetCount()>0)
	{
		
		CChannel *pChannel = pDataExtra->m_pChannelList.GetHead();
		POSITION psn = pDataExtra->m_pChannelList.GetHeadPosition();
		for (int j= 0; j<pDataExtra->m_pChannelList.GetCount();j++)
		{
			pChannel = pDataExtra->m_pChannelList.GetNext(psn);
			m_pChannelList.AddTail(pChannel);			
		}
	}
#endif

    _createReport();
    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CManualQualitySetDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlQualityReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDD_MANUAL_QUALITY_SET_RET);

    // 设置字体
    m_ctlQualityReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlQualityReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlQualityReport.InsertColumn(0, 
        _T(""),
        EL_LEFT,
        DSM_MQS_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlQualityReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_MQS_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlQualityReport.SetRowSelectionVisible(FALSE);
    m_ctlQualityReport.SetRowFocusEnable(TRUE);
    m_ctlQualityReport.SetSubItemFocusEnable(TRUE);
    m_ctlQualityReport.SetFreezeColumnsCount(1);
    m_ctlQualityReport.SetFreezeSelectionVisible(TRUE);
    m_ctlQualityReport.SetFocusedColIndex(1);
    m_ctlQualityReport.SetFocus();

    int nRepInitInfo[12][3] =
    {
        {DSM_STR_MANUAL_OVERFLOW_CAPTION, DSM_STR_MANUAL_NO_OVERFLOW, DSM_STR_MANUAL_OVERFLOW},
        {DSM_STR_MANUAL_OUT_OF_RANGE_CAPTION, DSM_STR_MANUAL_NORMAL, DSM_STR_MANUAL_OUT_OF_RANGE},
        {DSM_STR_MANUAL_BAD_REFERENCE_CAPTION, DSM_STR_MANUAL_NORMAL, DSM_STR_MANUAL_BAD_REFERENCE},
        {DSM_STR_MANUAL_OSCILLATORY_CAPTION, DSM_STR_MANUAL_NORMAL, DSM_STR_MANUAL_OSCILLATORY},

        {DSM_STR_MANUAL_FAILURE_CAPTION, DSM_STR_MANUAL_NO_FAILURE, DSM_STR_MANUAL_FAILURE},
        {DSM_STR_MANUAL_OLD_DATA_CAPTION, DSM_STR_MANUAL_NORMAL, DSM_STR_MANUAL_OLD_DATA},
        {DSM_STR_MANUAL_INCONSISTANT_CAPTION, DSM_STR_MANUAL_CONSISTANT, DSM_STR_MANUAL_INCONSISTANT},
        {DSM_STR_MANUAL_INACCURATE_CAPTION, DSM_STR_MANUAL_ACCURATE, DSM_STR_MANUAL_INACCURATE},

        {DSM_STR_MANUAL_SOURCE_CAPTION, DSM_STR_MANUAL_PROCESS, DSM_STR_MANUAL_SUBSTITUTED},
        {DSM_STR_MANUAL_Q_TEST_CAPTION, DSM_STR_MANUAL_RUN, DSM_STR_MANUAL_TEST},
        {DSM_STR_MANUAL_OPB_CAPTION, DSM_STR_MANUAL_UNLOCK, DSM_STR_MANUAL_LOCK},
        {DSM_STR_MANUAL_DERIVED_CAPTION, DSM_STR_MANUAL_DERIVED_FALSE, DSM_STR_MANUAL_DERIVED_TRUE},
    };

    CString csBit = _T("");
    CString csCaption = _T("");
    CString csContent = _T("");
    CString csTemp = _T("");
    CString csChannel = _T("");

    csBit = dsm_stringMgr::Get(DSM_STR_MSW_BIT);

    unsigned short usQuality = m_pChannel->GetQuality();

    
    //特殊处理bit0-1，两位，四个选项
    m_ctlQualityReport.InsertRecordRow(0, new CDsmManualQualitySetRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_VALIDITY_CAPTION), _T("")));
    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlQualityReport.GetRecordRow(0)->GetItem(1);
    CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_GOOD), 0);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_INVALID), 1);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_RESVERED), 2);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_QUESTIONABLE), 3);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(usQuality& 0x0003);
    if (pItemCon)
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    //后续使用数组添加选项
    for (int i = 0; i < 12; ++i)
    {
        csCaption.Format(_T("%s%d %s"), csBit, i + 2, dsm_stringMgr::Get(nRepInitInfo[i][0]));

        m_ctlQualityReport.InsertRecordRow(i + 1, new CDsmManualQualitySetRecord(csCaption, _T("")));

        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlQualityReport.GetRecordRow(i + 1)->GetItem(1);
        CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
        pOptions->AddComboButton();
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][1]), 0);
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][2]), 1);
        CELRecordItemConstraint* pItemCon = pOptions->FindConstraint((usQuality>>(i+ 2)) & 0x0001);
        if (pItemCon)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }

    m_ctlQualityReport.SetFocusedSelectionRowIndex(0);
    m_ctlQualityReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for (int i = 0; i < m_ctlQualityReport.GetRecordRowCount(); ++i)
    {
        m_ctlQualityReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
}

void CManualQualitySetDlg::OnQualitySetItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    //
    //报表中每个item对应相应的BIT
    //
    int nRowSel = m_ctlQualityReport.GetFocusedRowIndex();
    DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();

    if (nRowSel == 0)
    {
        unsigned short usQuality = m_pChannel->GetQuality();
        usQuality = (usQuality & 0xFFFC) | dwValue;
        m_pChannel->SetQuality(usQuality);
    }
    else if (nRowSel <= 12)
    {
        unsigned short usQuality = m_pChannel->GetQuality();
        usQuality = (usQuality & ~(0x0001 << (nRowSel + 1))) | (dwValue << (nRowSel + 1));
        m_pChannel->SetQuality(usQuality);
    }
}

void showMsg(CString msg, CWnd* pParent)
{
	CMsgDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, 0, pParent);

	msgdlg._SetMsg(msg);
	msgdlg._SetBitmapDisp(false);
	msgdlg._SetTextRC( CRect(60, 51, 314, 71));
	msgdlg._ResetF5Text(_T(""));
	msgdlg._ResetF6Text(_T("确定"));
	msgdlg._EnableCountDown();
	HWND hWnd = ::GetFocus();
	INT_PTR nRet = msgdlg.DoModal();
	msgdlg._ResetMenu();

	if(nRet == IDOK)
	{
		// 设置默认时区
		//int j = 0;

	}
	::SetFocus(hWnd);
}

void CManualQualitySetDlg::OnNaviMenuQualitySet( UINT nID )
{
    switch (nID)
    {
#if !CHUN_HUA
	case  ID_MUNU_MQS_F1:
		{
			if (m_pChannelList.GetCount()>0)
			{
				CChannel* pChannel = m_pChannelList.GetHead();
				POSITION psn = m_pChannelList.GetHeadPosition();
				for (int j= 0; j<m_pChannelList.GetCount();j++)
				{
					pChannel = m_pChannelList.GetNext(psn);
					pChannel->SetQuality(m_pChannel->GetQuality());
				}
			}
			 
			showMsg(_T("品质值将应用在选中通道中！"),this);
		}
		break;
#endif
    case  ID_MUNU_MQS_F2:
        {
            if (m_pIEC92Cb)
            {
                CChannel* pChannel = m_pIEC92Cb->FirstChannel();
                while(pChannel)
                {
                    pChannel->SetQuality(m_pChannel->GetQuality());
                    pChannel = m_pIEC92Cb->NextChannel(pChannel);
                }
            }
			//Cmsgdlg
			showMsg(_T("品质值将应用在所有通道中！"),this);
        }
        break;
    }
	//CString csTip(_T("  设置成功"));
	//CStatusMgr::SetTips(csTip);
	//delete pdlg;

}


void CManualQualitySetDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (nIDEvent == 1)
    {
        _SavePage();
    }
    CBaseDlg::OnTimer(nIDEvent);
}


void CManualQualitySetDlg::_SavePage()
{
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();
}