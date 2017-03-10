/// @file
///
/// @brief
///     编辑SMV发送控制块对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///


#include "stdafx.h"
#include "EditSmvSendDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/DsmDataLayer.h"
#include "src/utils/CommonControl/MacEditCtrl/MacEditCtrl.h"
#include "src/service/datalayer/config/ParamConfigSMVSend.h"


#include "BasicParamSmvTxDlg.h"



//////////////////////////////////////////////////////////////////////////
// 编辑SMV控制块报表
//
#if SU_FEI
#define DSM_EDIT_SMV_CTRL_WIDTH_CAPTION           180
#define DSM_EDIT_SMV_CTRL_WIDTH_CONTENT           420
#else

#define DSM_EDIT_SMV_CTRL_WIDTH_CAPTION           180
#define DSM_EDIT_SMV_CTRL_WIDTH_CONTENT           349

#endif
// 编辑SMV通道报表
//
#if SU_FEI
#define DSM_EDIT_SMV_CHAN_WIDTH_INDEX             50
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_DESC         140
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TYPE         90
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_PHASE        60
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_ONCE         100
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TWICE        100
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_MAP          60
#else
#define DSM_EDIT_SMV_CHAN_WIDTH_INDEX             35
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_DESC         110
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TYPE		  75
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_PHASE        50
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_ONCE         103
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TWICE        103
#define DSM_EDIT_SMV_CHAN_WIDTH_CHAN_MAP          53
#endif


enum SERecordFuncType
{
    SEUnknown  =0,
    SECtrlBlockDesc,
    SESendPort,
    SEAPPID,
    SEMac,
#if !CHUN_HUA
	SEVlanTag,
#endif
    SEVLanID,
    SEVLanPriority,
    SEVersion,
    SERatedPhaseCur,
    SERatedNeutalCur,
    SERatedPhaseVolt,
    SERatedDelay,
    SEStatusWord1,
    SEStatusWord2,
    SESVID,
    SEDataSet,
    SEDataSetOpt,
    SERefreshTimeOpt,
    SESampleRateOpt,
    SELNName,
    SELDName,
    SEDSName,
    SEChanCount,
};

//
//通道编辑 列索引
//
#define DSM_ESS_EDIT_CHAN_INDEX                       0
#define DSM_ESS_EDIT_CHAN_DESC                        1
#define DSM_ESS_EDIT_CHAN_TYPE                        2
#define DSM_ESS_EDIT_CHAN_PHASE                       3
#define DSM_ESS_EDIT_CHAN_ONCE                        4
#define DSM_ESS_EDIT_CHAN_TWICE                       5
#define DSM_ESS_EDIT_CHAN_MAP                         6


#define DSM_SMV_9_1_CHAN_MAX                               12
#define DSM_SMV_9_2_CHAN_MIN                               1
#define DSM_SMV_FT3_CHAN_MAX                               12
#define DSM_SMV_FT3_EXT_CHAN_MAX                           22



// CEditSmvSendDlg 对话框

IMPLEMENT_DYNAMIC(CEditSmvSendDlg, CBaseDlg)

CEditSmvSendDlg::CEditSmvSendDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CEditSmvSendDlg::IDD, DSM_STR_EDIT_SMV_CTRL_BLOCK_CAPTION, pParent)
{
    m_eShowReportType = ESSShowCtrlBlockReport;
    m_pBaseCb = NULL;
    m_nMenuID = IDR_MENU_EDIT_SMV_CTRL;
	m_pPopupMenu = new CNaviMenu;
}

CEditSmvSendDlg::~CEditSmvSendDlg()
{
	if (m_pPopupMenu)
	{
		delete m_pPopupMenu;
	}
}

void CEditSmvSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditSmvSendDlg, CBaseDlg)
    ON_COMMAND(ID_MENU_EDIT_SMV_CTRL_F1, &CEditSmvSendDlg::OnMenuEditSmvCtrlF1)
#if !SU_FEI
	ON_COMMAND(ID_MENU_EDIT_SMV_CTRL_F2, &CEditSmvSendDlg::OnMenuEditSmvCtrlF2)
#endif
    ON_COMMAND(ID_MENU_EDIT_SMV_CTRL_F6, &CEditSmvSendDlg::OnMenuEditSmvCtrlF6)
    ON_COMMAND_RANGE(ID_MENU_EDIT_SMV_CHAN_F1, ID_MENU_EDIT_SMV_CHAN_F6, &CEditSmvSendDlg::OnNaviMenuEditSmvChan)

    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_EDIT_SMV_CTRL_BLOCK_RET, &CEditSmvSendDlg::OnSmvCtrlItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_EDIT_SMV_CTRL_BLOCK_RET, &CEditSmvSendDlg::OnSmvCtrlItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_EDIT_SMV_CTRL_BLOCK_RET, &CEditSmvSendDlg::OnSmvCtrlItemValueChanged)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_EDIT_SMV_CTRL_BLOCK_RET, &CEditSmvSendDlg::OnSmvCtrlInplaceButtonClick)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_EDIT_SMV_CTRL_BLOCK_RET,  &CEditSmvSendDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_EDIT_SMV_CTRL_BLOCK_RET,  &CEditSmvSendDlg::OnInplaceEditAction)

    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_EDIT_SMV_CHAN_RET, &CEditSmvSendDlg::OnSmvChanItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_EDIT_SMV_CHAN_RET, &CEditSmvSendDlg::OnSmvChanItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_EDIT_SMV_CHAN_RET,  &CEditSmvSendDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_EDIT_SMV_CHAN_RET,  &CEditSmvSendDlg::OnInplaceEditAction)
    ON_WM_TIMER()

#if !CHUN_HUA
	ON_COMMAND_RANGE(ID__ABC,ID__ALL_VOLATE, &CEditSmvSendDlg::OnPopUpMenu)
#endif
	

END_MESSAGE_MAP()


// CEditSmvSendDlg 消息处理程序



BOOL CEditSmvSendDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();


	//ASSERT(pPageData->m_nPageID == CBasicParamDlg::IDD);//问题出在这里？？？？？？？？？
	

   if (pPageData == NULL || 
        pPageData->m_nPageID != CBasicParamSmvTxDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
    {
        //
        //参数配置进入（goose编辑或查看）
        //
        return FALSE;
    }

    CBasicParamPageDataExtraSmv* pDataExtra = dynamic_cast<CBasicParamPageDataExtraSmv*>(pPageData->m_pPageDataExtra);
    if (pDataExtra == NULL || pDataExtra->m_pSmvSendSelectedCb == NULL)
    {
        return FALSE;
    }
    m_pBaseCb = pDataExtra->m_pSmvSendSelectedCb;
    if (m_pBaseCb == NULL)
    {
        return FALSE;
    }
    
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return FALSE;
    }
    
    m_nSmvSendType = pParamConfigSMVSend->GetSMVType();

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    if(!pData || CEditSmvSendDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }
    if(pData->m_eAction == PageEnter)
    {
        //新进入
        _showCtrlBlockReport();
    }
    else
    {
        _restorePage();
    }
    _showCtrlBlockReport();
    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


/**
 *      显示控制块报表
 */
void CEditSmvSendDlg::_showCtrlBlockReport()
{
    if(m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        m_ctlCtrlBlockReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createCtrlBlockReport();
    }
    _updateCtrlBlockChanCount();
    m_eShowReportType = ESSShowCtrlBlockReport;

    m_ctlCtrlBlockReport.SetFocus();

    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_HIDE);
    }
    _reloadNaviMenu(IDR_MENU_EDIT_SMV_CTRL);
    _resetMenu();
    CString csCaption;
    csCaption.Format(L"%s", dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CTRL_BLOCK_CAPTION));
    CStatusMgr::SetTitle(csCaption);
    

    UpdateData(FALSE);
}


void CEditSmvSendDlg::_createCtrlBlockReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_EDIT_SMV_CTRL_BLOCK_RET);

    // 设置字体
    m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlCtrlBlockReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlCtrlBlockReport.InsertColumn(0, 
        _T(""),
        EL_LEFT,
        DSM_EDIT_SMV_CTRL_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlCtrlBlockReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_EDIT_SMV_CTRL_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlCtrlBlockReport.SetRowSelectionVisible(FALSE);
    m_ctlCtrlBlockReport.SetRowFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetSubItemFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetFreezeColumnsCount(1);
    m_ctlCtrlBlockReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetFocusedColIndex(1);
    m_ctlCtrlBlockReport.SetFocus();

#if !SU_FEI
	m_ctlCtrlBlockReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
    //
    //根据SMV类型不同初始化为不同的报表
    //
    switch(m_nSmvSendType)
    {
    case FRAME_TYPE_NET_9_1:
        {

            _initCtrlReportWithIEC6185091();
        }
        break;
    case FRAME_TYPE_NET_9_2:
        {
            _initCtrlReportWithIEC6185092();

        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            _initCtrlReportWithIEC60044FT3();

        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            _initCtrlReportWithIEC60044FT3Ext();
        }
        break;
    }

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    if(!pData || CEditSmvSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    if(pData->m_eAction == PageEnter)
    {
        
        m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
    }

    m_ctlCtrlBlockReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i < m_ctlCtrlBlockReport.GetRecordRowCount()-1; ++i)
    {
        m_ctlCtrlBlockReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
    //if (m_nSmvSendType == FRAME_TYPE_NET_9_2)
    //{
    //    //9-2特殊
    //    m_ctlCtrlBlockReport.GetRecordRow(m_ctlCtrlBlockReport.GetRecordRowCount()-1)->GetItem(1)->SetEditable(TRUE);
    //}
}


/**
 *      初始化控制块报表 9-1
 */
void CEditSmvSendDlg::_initCtrlReportWithIEC6185091()
{
    ASSERT(m_nSmvSendType == FRAME_TYPE_NET_9_1);

    if (m_nSmvSendType != FRAME_TYPE_NET_9_1)
    {
        return;
    }

    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }
    m_ctlCtrlBlockReport.DeleteAllRecordRow();
    
    CIEC9_1Cb* pIEC91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
    if (pIEC91Cb == NULL)
    {
        return;
    }

    CString strContent = _T("");


    m_ctlCtrlBlockReport.InsertRecordRow(0, 
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CTRL_BLOCK_DESC),
        pIEC91Cb->GetDescription().c_str(), SECtrlBlockDesc));

    m_ctlCtrlBlockReport.InsertRecordRow(1,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_PORT),
        pIEC91Cb->GetPortName(), SESendPort));

    CELRecordItemEditOptions* pOption  = m_ctlCtrlBlockReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX1), NET_PORT_LIGHT_A);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX2), NET_PORT_LIGHT_B);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ELEC_PORT), NET_PORT_ELECTRONIC);

    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(pIEC91Cb->GetPortName());
    if (pItemCon != NULL)
    {
        CELRecordItemText* pItemSendPort =  dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(1 )->GetItem(1));
        ASSERT(pItemSendPort);
        pItemSendPort->SetValue(pItemCon->GetConstraintCaption());
    }
    
    strContent.Format(_T("%04X"), pIEC91Cb->GetAPPID());
    m_ctlCtrlBlockReport.InsertRecordRow(2,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_APPID),
        strContent, SEAPPID));

    unsigned char ucMac[6];
    pIEC91Cb->GetDesMAC(ucMac, 6);

    CMacEditCtrl::MacAddr2Str(ucMac, strContent);

    m_ctlCtrlBlockReport.InsertRecordRow(3,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_MAC),
        strContent, SEMac));
    m_ctlCtrlBlockReport.GetRecordRow(3)->GetItem(1)->GetEditOptions(NULL)->AddExpandButton();

    m_ctlCtrlBlockReport.InsertRecordRow(5,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VLAN_ID),
        pIEC91Cb->GetVlanID(), SEVLanID));

    m_ctlCtrlBlockReport.InsertRecordRow(6,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VLAN_PRIORITY),
        pIEC91Cb->GetVLanPriority(), SEVLanPriority));

#if !CHUN_HUA
	m_ctlCtrlBlockReport.InsertRecordRow(4,
		new CDsmEditSmvSendCtrlBlockRecord(_T("是否带VLAN标志"),
		_T(""), SEVlanTag));
	CELRecordItem* pItem1 = m_ctlCtrlBlockReport.GetRecordRow(4)->GetItem(1);
	if (pItem1)
	{
		pItem1->HasCheckbox(TRUE);
		pItem1->SetChecked(pIEC91Cb->GetVlanFlag());
		if (!pIEC91Cb->GetVlanFlag())
		{
			m_ctlCtrlBlockReport.GetRecordRow(5)->SetFocusable(FALSE);
			m_ctlCtrlBlockReport.GetRecordRow(6)->SetFocusable(FALSE);
		}
	}
#endif    

    m_ctlCtrlBlockReport.InsertRecordRow(7,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VERSION),
        pIEC91Cb->GetConfigRevision(), SEVersion));

    strContent.Format(_T("%.03lf"), pIEC91Cb->GetRatedPhaseCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(8,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_CURRENT),
        strContent, SERatedPhaseCur));

    strContent.Format(_T("%.03lf"), pIEC91Cb->GetRatedNeutralCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(9,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_NEUTAL_CURRENT),
        strContent, SERatedNeutalCur));

    strContent.Format(_T("%.03lf"), pIEC91Cb->GetRatedPhaseVoltage());
    m_ctlCtrlBlockReport.InsertRecordRow(10,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_VOLTAGE),
        strContent, SERatedPhaseVolt));

    m_ctlCtrlBlockReport.InsertRecordRow(11,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_DELAY),
        pIEC91Cb->GetRatedDelay(), SERatedDelay));

    strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord1());
    m_ctlCtrlBlockReport.InsertRecordRow(12,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD1),
       strContent, SEStatusWord1));

    strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord2());
    m_ctlCtrlBlockReport.InsertRecordRow(13,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD2),
        strContent, SEStatusWord2 ));
    //
    //9-1通道数不可修改
    //
    m_ctlCtrlBlockReport.InsertRecordRow(14,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        pIEC91Cb->GetChannelCount(), SEChanCount ));



}
/**
 *      初始化控制块报表 9-2
 */
void CEditSmvSendDlg::_initCtrlReportWithIEC6185092()
{
    if (m_nSmvSendType != FRAME_TYPE_NET_9_2)
    {
        return;
    }
    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }
    m_ctlCtrlBlockReport.DeleteAllRecordRow();

    CIEC9_2Cb* pIEC92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
    if (pIEC92Cb == NULL)
    {
        return;
    }

    CString strContent = _T("");


    m_ctlCtrlBlockReport.InsertRecordRow(0, 
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CTRL_BLOCK_DESC),
        pIEC92Cb->GetDescription().c_str(), SECtrlBlockDesc));

    m_ctlCtrlBlockReport.InsertRecordRow(1,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_PORT),
        pIEC92Cb->GetPortName(), SESendPort));

    CELRecordItemEditOptions* pOption  = m_ctlCtrlBlockReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX1), NET_PORT_LIGHT_A);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_NET_PORT_TX2), NET_PORT_LIGHT_B);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ELEC_PORT), NET_PORT_ELECTRONIC);

    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(pIEC92Cb->GetPortName());
    if (pItemCon != NULL)
    {
        CELRecordItemText* pItemSendPort =  dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(1 )->GetItem(1));
        ASSERT(pItemSendPort);
        pItemSendPort->SetValue(pItemCon->GetConstraintCaption());
    }

    strContent.Format(_T("%04X"), pIEC92Cb->GetAPPID());
    m_ctlCtrlBlockReport.InsertRecordRow(2,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_APPID),
        strContent, SEAPPID));

    unsigned char ucMac[6];
    pIEC92Cb->GetDesMAC(ucMac, 6);

    CMacEditCtrl::MacAddr2Str(ucMac, strContent);

    m_ctlCtrlBlockReport.InsertRecordRow(3,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_MAC),
        strContent, SEMac));
    m_ctlCtrlBlockReport.GetRecordRow(3)->GetItem(1)->GetEditOptions(NULL)->AddExpandButton();

    m_ctlCtrlBlockReport.InsertRecordRow(5,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VLAN_ID),
        pIEC92Cb->GetVlanID(), SEVLanID));

    m_ctlCtrlBlockReport.InsertRecordRow(6,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VLAN_PRIORITY),
        pIEC92Cb->GetVLanPriority(), SEVLanPriority));

#if !CHUN_HUA
	m_ctlCtrlBlockReport.InsertRecordRow(4,
		new CDsmEditSmvSendCtrlBlockRecord(_T("是否带VLAN标志"),
		_T(""), SEVlanTag));
	CELRecordItem* pItem1 = m_ctlCtrlBlockReport.GetRecordRow(4)->GetItem(1);
	if (pItem1)
	{
		pItem1->HasCheckbox(TRUE);
		pItem1->SetChecked(pIEC92Cb->GetVlanFlag());
		if (!pIEC92Cb->GetVlanFlag())
		{
			m_ctlCtrlBlockReport.GetRecordRow(5)->SetFocusable(FALSE);
			m_ctlCtrlBlockReport.GetRecordRow(6)->SetFocusable(FALSE);
		}
	}
#endif

    m_ctlCtrlBlockReport.InsertRecordRow(7,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_VERSION),
        pIEC92Cb->GetConfigRevision(), SEVersion));

    m_ctlCtrlBlockReport.InsertRecordRow(8,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_SVID),
        pIEC92Cb->GetSvID().c_str(), SESVID));

    m_ctlCtrlBlockReport.InsertRecordRow(9,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_DATSET),
        pIEC92Cb->GetDataSetRef().c_str(), SEDataSet));

    m_ctlCtrlBlockReport.InsertRecordRow(10,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_DATA_SET_OPT),
        _T(""), SEDataSetOpt));
    CELRecordItem* pItem = m_ctlCtrlBlockReport.GetRecordRow(10)->GetItem(1);
    if (pItem)
    {
        pItem->HasCheckbox(TRUE);
        pItem->SetChecked(pIEC92Cb->GetDataSetOpt());
    }

    m_ctlCtrlBlockReport.InsertRecordRow(11,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_SAMPLE_RATE_OPT),
        _T(""), SESampleRateOpt));
    pItem = m_ctlCtrlBlockReport.GetRecordRow(11)->GetItem(1);
    if (pItem)
    {
        pItem->HasCheckbox(TRUE);
        pItem->SetChecked(pIEC92Cb->GetSmpRateOpt());
    }

    //
    //9-2通道数，可修改
    //
    m_ctlCtrlBlockReport.InsertRecordRow(12,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        pIEC92Cb->GetChannelCount(), SEChanCount ));
    m_ctlCtrlBlockReport.GetRecordRow(12)->GetItem(1)->SetEditable(TRUE);
}

/**
 *      初始化控制块报表 ft3
 */
void CEditSmvSendDlg::_initCtrlReportWithIEC60044FT3()
{
    if (m_nSmvSendType != FRAME_TYPE_FT3_NANRUI)
    {
        return;
    }

    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }
    m_ctlCtrlBlockReport.DeleteAllRecordRow();
    
    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
    if (pFt3Cb == NULL)
    {
        return;
    }

    CString strContent = _T("");

    m_ctlCtrlBlockReport.InsertRecordRow(0,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CTRL_BLOCK_DESC),
        pFt3Cb->GetDescription().c_str(), SECtrlBlockDesc));

    m_ctlCtrlBlockReport.InsertRecordRow(1,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_PORT),
        _T(""),SESendPort ));
    CELRecordItemEditOptions* pOption  = m_ctlCtrlBlockReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX), FT3_PORT_LIGHT_C);
    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(pFt3Cb->GetPortName());
    if (pItemCon != NULL)
    {
        CELRecordItemText* pItemSendPort =  dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(1 )->GetItem(1));
        ASSERT(pItemSendPort);
        pItemSendPort->SetValue(pItemCon->GetConstraintCaption());
    }
    
    strContent.Format(_T("%02X"), pFt3Cb->GetLNName());
    m_ctlCtrlBlockReport.InsertRecordRow(2,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_LN_NAME),
        strContent, SELNName));

    strContent.Format(_T("%04X"), pFt3Cb->GetLDName());
    m_ctlCtrlBlockReport.InsertRecordRow(3,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_LD_NAME),
        strContent, SELDName));

    strContent.Format(_T("%02X"), pFt3Cb->GetDSName());
    m_ctlCtrlBlockReport.InsertRecordRow(4,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_DS_NAME),
        strContent, SEDSName));

    strContent.Format(_T("%.03lf"), pFt3Cb->GetRatedPhaseCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(5,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_CURRENT),
        strContent, SERatedPhaseCur));

    strContent.Format(_T("%.03lf"), pFt3Cb->GetRatedNeutralCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(6,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_NEUTAL_CURRENT),
        strContent, SERatedNeutalCur));

    strContent.Format(_T("%.03lf"), pFt3Cb->GetRatedPhaseVoltage());
    m_ctlCtrlBlockReport.InsertRecordRow(7,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_VOLTAGE),
        strContent, SERatedPhaseVolt));

    m_ctlCtrlBlockReport.InsertRecordRow(8,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_DELAY),
        pFt3Cb->GetRatedDelay() , SERatedDelay));
    
    strContent.Format(_T("%04X"), pFt3Cb->GetStatusWord1());
    
    m_ctlCtrlBlockReport.InsertRecordRow(9,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD1),
        strContent, SEStatusWord1 ));

    strContent.Format(_T("%04X"), pFt3Cb->GetStatusWord2());
    m_ctlCtrlBlockReport.InsertRecordRow(10,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD2),
        strContent, SEStatusWord2 ));
    //
    //ft3通道数，不可修改
    //
    m_ctlCtrlBlockReport.InsertRecordRow(11,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        pFt3Cb->GetChannelCount(), SEChanCount ));
}
/**
 *      初始化控制块报表 ft3扩展
 */
void CEditSmvSendDlg::_initCtrlReportWithIEC60044FT3Ext()
{
    if (m_nSmvSendType != FRAME_TYPE_FT3_GUOWANG)
    {
        return;
    }

    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }
    m_ctlCtrlBlockReport.DeleteAllRecordRow();

    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
    if (pFt3ExtCb == NULL)
    {
        return;
    }

    CString strContent = _T("");

    m_ctlCtrlBlockReport.InsertRecordRow(0,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CTRL_BLOCK_DESC),
        pFt3ExtCb->GetDescription().c_str(), SECtrlBlockDesc));

    m_ctlCtrlBlockReport.InsertRecordRow(1,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_PORT),
        _T(""),SESendPort ));
    CELRecordItemEditOptions* pOption  = m_ctlCtrlBlockReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_OPT_SER_PORT_TX), FT3_PORT_LIGHT_C);
    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(pFt3ExtCb->GetPortName());
    if (pItemCon != NULL)
    {
        CELRecordItemText* pItemSendPort =  dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(1 )->GetItem(1));
        ASSERT(pItemSendPort);
        pItemSendPort->SetValue(pItemCon->GetConstraintCaption());
    }

    strContent.Format(_T("%02X"), pFt3ExtCb->GetLNName());
    m_ctlCtrlBlockReport.InsertRecordRow(2,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_LN_NAME),
        strContent, SELNName));

    strContent.Format(_T("%04X"), pFt3ExtCb->GetLDName());
    m_ctlCtrlBlockReport.InsertRecordRow(3,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_LD_NAME),
        strContent, SELDName));

    strContent.Format(_T("%02X"), pFt3ExtCb->GetDSName());
    m_ctlCtrlBlockReport.InsertRecordRow(4,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_DS_NAME),
        strContent, SEDSName));

    strContent.Format(_T("%.03lf"), pFt3ExtCb->GetRatedPhaseCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(5,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_CURRENT),
        strContent, SERatedPhaseCur));

    strContent.Format(_T("%.03lf"), pFt3ExtCb->GetRatedNeutralCurrent());
    m_ctlCtrlBlockReport.InsertRecordRow(6,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_NEUTAL_CURRENT),
        strContent, SERatedNeutalCur));

    strContent.Format(_T("%.03lf"), pFt3ExtCb->GetRatedPhaseVoltage());
    m_ctlCtrlBlockReport.InsertRecordRow(7,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_PHASE_VOLTAGE),
        strContent, SERatedPhaseVolt));

    m_ctlCtrlBlockReport.InsertRecordRow(8,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_RATED_DELAY),
        pFt3ExtCb->GetRatedDelay() , SERatedDelay));

    strContent.Format(_T("%04X"), pFt3ExtCb->GetStatusWord1());

    m_ctlCtrlBlockReport.InsertRecordRow(9,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD1),
        strContent, SEStatusWord1 ));
    strContent.Format(_T("%04X"), pFt3ExtCb->GetStatusWord2());
    m_ctlCtrlBlockReport.InsertRecordRow(10,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_STATUS_WORD2),
        strContent, SEStatusWord2 ));
    
    //
    //ft3扩展通道数，不可修改
    //
    m_ctlCtrlBlockReport.InsertRecordRow(11,
        new CDsmEditSmvSendCtrlBlockRecord(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        pFt3ExtCb->GetChannelCount(), SEChanCount ));
}


/**
 *      显示通道报表
 */
void CEditSmvSendDlg::_updateCtrlBlockChanCount()
{
    if(!m_ctlCtrlBlockReport.GetSafeHwnd() || m_nSmvSendType != FRAME_TYPE_NET_9_2)
    {
        return ;
    }
    int nRowCount = m_ctlCtrlBlockReport.GetRecordRowCount();
    for (int i = nRowCount-1; i >= 0; --i)
    {
        SERecordFuncType rft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(i, 0);
        if (rft == SEChanCount)
        {
            CELRecordItemText* pItem = (CELRecordItemText*)m_ctlCtrlBlockReport.GetRecordRow(i)->GetItem(1);
            if (pItem)
            {
                CString strChanCount = _T("");
                strChanCount.Format(_T("%d"), m_pBaseCb->GetChannelCount());
                pItem->SetValue(strChanCount);
                m_ctlCtrlBlockReport.RedrawRowItem(i, 1);
            }
        }
        break;
    }
}

/**
 *      显示通道报表
 */
void CEditSmvSendDlg::_showChannelReport()
{
    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createChannelReport();
    }
    
    m_ctlChannelReport.SetFocus();
    m_eShowReportType = ESSShowChannelReport;

    if(m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        m_ctlCtrlBlockReport.ShowWindow(SW_HIDE);
    }

    _reloadNaviMenu(IDR_MENU_EDIT_SMV_CHAN);
    _resetMenu();
    
    //
    //F4 修改测试电流显示
    //
    _updateSmvChanMenuF4();

    m_pNaviBar->UpdateNaviMenu();

    CString csCaption;
    csCaption.Format(_T("%s->0x%04X"), dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CHAN_CAPTION), m_pBaseCb->GetAPPID());
    CStatusMgr::SetTitle(csCaption);

    UpdateData(FALSE);
}


void CEditSmvSendDlg::_createChannelReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    if (m_pBaseCb == NULL)
    {
        return ;
    }
    
    //rcReport.top += 15;
    //rcReport.bottom = rcReport.top + 358;
//#if SU_FEI
//    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
//#else
//	rcReport.DeflateRect(10, DSM_MARGIN_IN, 10, DSM_MARGIN_IN);	
//#endif
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlChannelReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_EDIT_SMV_CHAN_RET);

    // 设置字体
    m_ctlChannelReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlChannelReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlChannelReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_INDEX, _T(""),
        EL_CENTER, 
        DSM_EDIT_SMV_CHAN_WIDTH_INDEX);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_DESC, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CHAN_DESC), 
        EL_LEFT,
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_DESC);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_TYPE, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_CHAN_TYPE),
        EL_LEFT,
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TYPE);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_PHASE, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_PHASE),
        EL_LEFT, 
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_PHASE);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_ONCE, L"一次值" ,//dsm_stringMgr::Get(DSM_STR_EDIT_SMV_ONCE_RATED_VALUE),
        EL_LEFT,
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_ONCE);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_TWICE,L"二次值",// dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TWICE_RATED_VALUE),
        EL_LEFT,
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_TWICE);
    m_ctlChannelReport.InsertColumn(DSM_ESS_EDIT_CHAN_MAP, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_MAP),
        EL_LEFT,
        DSM_EDIT_SMV_CHAN_WIDTH_CHAN_MAP);

    // 设置选择焦点样式
    m_ctlChannelReport.SetRowSelectionVisible(FALSE);
    m_ctlChannelReport.SetRowFocusEnable(TRUE);
    m_ctlChannelReport.SetSubItemFocusEnable(TRUE);
    m_ctlChannelReport.SetFreezeSelectionVisible(TRUE);
    m_ctlChannelReport.SetFreezeColumnsCount(1);
    m_ctlChannelReport.SetFocusedColIndex(1);
    m_ctlChannelReport.SetFocus();

    _updateChanReport();

    m_ctlChannelReport.GetColumnExAttr(DSM_ESS_EDIT_CHAN_DESC)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlChannelReport.GetColumnExAttr(DSM_ESS_EDIT_CHAN_ONCE)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlChannelReport.GetColumnExAttr(DSM_ESS_EDIT_CHAN_TWICE)->GetEditOptions()->SetAllowEdit(TRUE);
}


void CEditSmvSendDlg::_updateChanReport()
{
    if (!m_ctlChannelReport.GetSafeHwnd())
    {
        return;
    }

    m_ctlChannelReport.DeleteAllRecordRow();

    int nIndex = 0;
    CChannel* pChannel = m_pBaseCb->FirstChannel();
    while(pChannel)
    {
        m_ctlChannelReport.InsertRecordRow(nIndex, new CDsmEditSmvSendChanRecord(pChannel, nIndex+1));
        m_ctlChannelReport.SetRecordRowData(nIndex, (DWORD_PTR)pChannel);
        pChannel = m_pBaseCb->NextChannel(pChannel);
        ++nIndex;
    }

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    if(!pData || CEditSmvSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    if(pData->m_eAction == PageEnter)
    {
        //从其他页面返回
        if (m_ctlChannelReport.GetRecordRowCount() > 0)
        {
            m_ctlChannelReport.SetFocusedSelectionRowIndex(0);
        }
    }
    m_ctlChannelReport.RedrawRowItems(0, m_ctlChannelReport.GetRecordRowCount()-1);

}

void CEditSmvSendDlg::_resetMenu()
{
    if (m_eShowReportType == ESSShowCtrlBlockReport)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_SEL), NULL, NULL);
#if !SU_FEI
		m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(4, NULL, NULL, NULL);
#endif
    }
    else
    {
        //F1 F2
		
#if SU_FEI
        m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN), NULL, NULL);
#else
		m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_UNSEL), NULL, NULL);
		m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_SEL), NULL, NULL);
#endif
        //F3 F4
        if (m_nSmvSendType == FRAME_TYPE_NET_9_1 ||
            m_nSmvSendType == FRAME_TYPE_FT3_NANRUI ||
            m_nSmvSendType == FRAME_TYPE_FT3_GUOWANG )
        {
            //9-1 ft3 ft3扩展通道数目不能变化
#if SU_FEI
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(" "));
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(" "));
#else
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(" "));
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(" "));
#endif
        }
        m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
        //F5
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE));
        CMenu menu;
        CString strCaption = _T("");
        VERIFY(menu.CreatePopupMenu());
        switch(m_nSmvSendType)
        {
        case FRAME_TYPE_NET_9_1:
            {
                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_91_12A, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_91_12B, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_91_12C, strCaption);
                
            }
            break;
        case FRAME_TYPE_NET_9_2:
            {
                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_92_8A, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_92_12D, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_92_18A, strCaption);
            }
            break;
        case FRAME_TYPE_FT3_NANRUI:
            {
                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_12A, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_12B, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_12C, strCaption);
            }
            break;
        case FRAME_TYPE_FT3_GUOWANG:
            {
                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22A, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22B, strCaption);

                strCaption = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C);
                menu.AppendMenu(MF_STRING, ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22C, strCaption);
            }
            break;
        }

        CNaviMenu * pSubMenu = new CNaviMenu;
        pSubMenu->LoadMenu(&menu);
        menu.DestroyMenu();
        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(4, pSubMenu);



    }
    
    m_pNaviBar->UpdateNaviMenu();
}
void CEditSmvSendDlg::OnMenuEditSmvCtrlF1()
{
#if SU_FEI
    m_eShowReportType = ESSShowChannelReport;
    _showChannelReport();
#else
	m_eShowReportType = ESSShowCtrlBlockReport;
	_showCtrlBlockReport();
#endif
}

#if !SU_FEI
void CEditSmvSendDlg::OnMenuEditSmvCtrlF2()
{
	m_eShowReportType = ESSShowChannelReport;
	_showChannelReport();
}

#endif
void CEditSmvSendDlg::OnMenuEditSmvCtrlF6()
{
    int nRowIndex = m_ctlCtrlBlockReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }

    SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(nRowIndex, 0);
    switch (seRft)
    {
    case SEStatusWord1:
    case SEStatusWord2:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATUS_WORD_SET_DLG);
        }
        break;
    default:
        break;
    } 
}
void CEditSmvSendDlg::_updateSmvCtrlMenuF6()
{
    int nRowIndex = m_ctlCtrlBlockReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }

    SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(nRowIndex, 0);
    switch (seRft)
    {
    case SEStatusWord1:
    case SEStatusWord2:
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ES_STATUS_WORD_DETAIL_SET));
        }
        break;
    default:
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, _T(""));
        }
        break;
    } 
    m_pNaviBar->UpdateNaviMenu();
}

void CEditSmvSendDlg::OnSmvCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if(!pItemNotify || !m_pBaseCb)
    {
        return;
    }

    SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(pItemNotify->rowIndex, 0);
    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex));
    if (pItem == NULL)
    {
        return;
    }
    CString strValue = pItem->GetValue();
    int nValue = 0;
    double dValue = 0.0f;
    
    switch (seRft)
    {
    case SECtrlBlockDesc:
        {
            m_pBaseCb->SetDescription((LPCTSTR)strValue);
            *pResult = TRUE;
        }
        break;
    case SEAPPID:
        {
            nValue = _tcstol(strValue, NULL, 16);
            m_pBaseCb->SetAPPID(nValue);
            strValue.Format(_T("%04X"), m_pBaseCb->GetAPPID());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case SEVLanID:
        {
            CNetCb* pNetCb = dynamic_cast<CNetCb*>(m_pBaseCb);
            ASSERT(pNetCb);
            if (pNetCb != NULL)
            {
                nValue = _ttoi(strValue);
                pNetCb->SetVlanID(nValue);
                strValue.Format(_T("%d"), pNetCb->GetVlanID());
                pItem->SetValue(strValue);
                *pResult = TRUE;
            }

        }
        break;

#if !CHUN_HUA
	case SEVlanTag:
		{
			CNetCb* pNetCb = dynamic_cast<CNetCb*>(m_pBaseCb);
			ASSERT(pNetCb);
			if (pNetCb != NULL)
			{
				if (pItem->GetHasCheckbox())
				{
					pNetCb->SetVlanFlag(pItem->IsChecked() == TRUE ? true : false);
					if (pItem->IsChecked())
					{
						pNetCb->SetVlanFlag(true);
						m_ctlCtrlBlockReport.GetRecordRow(5)->SetFocusable(TRUE);
						m_ctlCtrlBlockReport.GetRecordRow(6)->SetFocusable(TRUE);

					}
					else
					{
						pNetCb->SetVlanFlag(false);
						m_ctlCtrlBlockReport.GetRecordRow(5)->SetFocusable(FALSE);
						m_ctlCtrlBlockReport.GetRecordRow(6)->SetFocusable(FALSE);
					}
				}
			*pResult = TRUE;
			}

		}
		break;
#endif

    case SEVLanPriority:
        {

            CNetCb* pNetCb = dynamic_cast<CNetCb*>(m_pBaseCb);
            ASSERT(pNetCb);
            if (pNetCb != NULL)
            {
                nValue = _ttoi(strValue);
                pNetCb->SetVLanPriority(nValue);
                strValue.Format(_T("%d"), pNetCb->GetVLanPriority());
                pItem->SetValue(strValue);
                *pResult = TRUE;
            }

        }
        break;
    case SEVersion:
        {
            CNetCb* pNetCb = dynamic_cast<CNetCb*>(m_pBaseCb);
            ASSERT(pNetCb);
            if (pNetCb != NULL)
            {
                nValue = _ttoi(strValue);
                pNetCb->SetConfigRevision(nValue);
                strValue.Format(_T("%d"), pNetCb->GetConfigRevision());
                pItem->SetValue(strValue);
                *pResult = TRUE;
            }

        }
        break;
    case SERatedPhaseCur:
        {
            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            p91Cb->SetRatedPhaseCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), p91Cb->GetRatedPhaseCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3Cb->SetRatedPhaseCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3Cb->GetRatedPhaseCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3ExtCb->SetRatedPhaseCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3ExtCb->GetRatedPhaseCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SERatedNeutalCur:
        {
            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            p91Cb->SetRatedNeutralCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), p91Cb->GetRatedNeutralCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3Cb->SetRatedNeutralCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3Cb->GetRatedNeutralCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3ExtCb->SetRatedNeutralCurrent(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3ExtCb->GetRatedNeutralCurrent());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SERatedPhaseVolt:
        {
            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            p91Cb->SetRatedPhaseVoltage(dValue);
                        }
                        strValue.Format(_T("%.03lf"), p91Cb->GetRatedPhaseVoltage());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3Cb->SetRatedPhaseVoltage(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3Cb->GetRatedPhaseVoltage());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        if (DSM_Str2Double(strValue, dValue))
                        {
                            pFt3ExtCb->SetRatedPhaseVoltage(dValue);
                        }
                        strValue.Format(_T("%.03lf"), pFt3ExtCb->GetRatedPhaseVoltage());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SERatedDelay:
        {
            nValue = _ttoi(strValue);

            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        p91Cb->SetRatedDelay(nValue);
                        strValue.Format(_T("%d"), p91Cb->GetRatedDelay());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        pFt3Cb->SetRatedDelay(nValue);
                        strValue.Format(_T("%d"), pFt3Cb->GetRatedDelay());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        pFt3ExtCb->SetRatedDelay(nValue);
                        strValue.Format(_T("%d"), pFt3ExtCb->GetRatedDelay());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SEStatusWord1:
        {
            nValue = _tcstol(strValue, NULL, 16);

            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        p91Cb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), p91Cb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        pFt3Cb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3Cb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        pFt3ExtCb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3ExtCb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SEStatusWord2:
        {
            nValue = _tcstol(strValue, NULL, 16);

            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        p91Cb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), p91Cb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        pFt3Cb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3Cb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        pFt3ExtCb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3ExtCb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case SESVID:
        {

            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb != NULL)
            {
                p92Cb->SetSvID((LPCTSTR)strValue);
                *pResult = TRUE;
            }
        }
        break;
    case SEDataSet:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb != NULL)
            {
                p92Cb->SetDataSetRef((LPCTSTR)strValue);
                *pResult = TRUE;
            }
        }
        break;

    case SEDataSetOpt:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb != NULL)
            {
                p92Cb->SetDataSetOpt(pItem->IsChecked());
                *pResult = TRUE;
            }
        }
        break;
    case SESampleRateOpt:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb != NULL)
            {
                p92Cb->SetSmpRateOpt(pItem->IsChecked());
                *pResult = TRUE;
            }
        }
        break;

    case SELNName:
        {
            ASSERT(m_nSmvSendType == FRAME_TYPE_FT3_NANRUI || m_eShowReportType == FRAME_TYPE_FT3_GUOWANG);

            nValue = _tcstol(strValue, NULL, 16);

            if (m_nSmvSendType == FRAME_TYPE_FT3_NANRUI)
            {
                CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                if (pFt3Cb != NULL)
                {
                    pFt3Cb->SetLNName(nValue);
                    strValue.Format(_T("%02X"), pFt3Cb->GetLNName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
            else if (m_nSmvSendType == FRAME_TYPE_FT3_GUOWANG)
            {
                CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                if (pFt3ExtCb != NULL)
                {
                    pFt3ExtCb->SetLNName(nValue);
                    strValue.Format(_T("%02X"), pFt3ExtCb->GetLNName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
        }
        break;
    case SELDName:
        {
            ASSERT(m_nSmvSendType == FRAME_TYPE_FT3_NANRUI || m_eShowReportType == FRAME_TYPE_FT3_GUOWANG);

            nValue = _tcstol(strValue, NULL, 16);

            if (m_nSmvSendType == FRAME_TYPE_FT3_NANRUI)
            {
                CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                if (pFt3Cb != NULL)
                {
                    pFt3Cb->SetLDName(nValue);
                    strValue.Format(_T("%04X"), pFt3Cb->GetLDName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
            else if (m_nSmvSendType == FRAME_TYPE_FT3_GUOWANG)
            {
                CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                if (pFt3ExtCb != NULL)
                {
                    pFt3ExtCb->SetLDName(nValue);
                    strValue.Format(_T("%04X"), pFt3ExtCb->GetLDName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
        }
        break;
    case SEDSName:
        {
            ASSERT(m_nSmvSendType == FRAME_TYPE_FT3_NANRUI || m_eShowReportType == FRAME_TYPE_FT3_GUOWANG);

            nValue = _tcstol(strValue, NULL, 16);

            if (m_nSmvSendType == FRAME_TYPE_FT3_NANRUI)
            {
                CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                if (pFt3Cb != NULL)
                {
                    pFt3Cb->SetDSName(nValue);
                    strValue.Format(_T("%02X"), pFt3Cb->GetDSName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
            else if (m_nSmvSendType == FRAME_TYPE_FT3_GUOWANG)
            {
                CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                if (pFt3ExtCb != NULL)
                {
                    pFt3ExtCb->SetDSName(nValue);
                    strValue.Format(_T("%02X"), pFt3ExtCb->GetDSName());
                    pItem->SetValue(strValue);
                    *pResult = TRUE;
                }
            }
        }
        break;
    case SEChanCount:
        {
            ASSERT(m_nSmvSendType == FRAME_TYPE_NET_9_2);
            if (m_nSmvSendType == FRAME_TYPE_NET_9_2)
            {
                nValue = _tcstol(strValue, NULL, 10);
                if (nValue > 0 && nValue <= SMV_CHANNEL_NUM_MAX)
                {
                    m_pBaseCb->SetChannelCount(nValue);
                }
                strValue.Format(_T("%d"), m_pBaseCb->GetChannelCount());
                pItem->SetValue(strValue);
                _updateChanReport();
                *pResult = TRUE;
            }
        }
        break;
    default:
        break;
    }
}


void CEditSmvSendDlg::OnSmvCtrlItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;
    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify || !m_pBaseCb)
    {
        return;
    }

    SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(pItemNotify->rowIndex, 0);
    DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();
    
    switch(seRft)
    {
    case SESendPort:
        {
            m_pBaseCb->SetPortName((DsmPortName)dwValue);
            *pResult = TRUE;
        }
        break;
    }
}

void CEditSmvSendDlg::OnSmvCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify->pButton);

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex));
    if (pItem == NULL)
    {
        return;
    }

    SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(pItemNotify->rowIndex, 0);

    switch (seRft)
    {
    case SEMac:
        {

            CRect rcItem = m_ctlCtrlBlockReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

            CMacEditCtrl macEdit;
            macEdit.SetBkColor(ELT_T1_EDITBKCOLOR);
            macEdit.SetTextColor(ELT_T1_EDITTEXTCOLOR);

            CNetCb* pNetCb = dynamic_cast<CNetCb*>(m_pBaseCb);
            ASSERT(pNetCb);
            if (pNetCb != NULL)
            {
                unsigned char ucMac[6];
                pNetCb->GetDesMAC(ucMac, 6);

                CWnd* pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&macEdit, IDR_MENU_MAC_INPLACEEDIT);
                if (IDOK == macEdit.GoModal(&m_ctlCtrlBlockReport, rcItem, ucMac))
                {
                    CString strMac = _T("");
                    macEdit.MacAddr2Str(ucMac, strMac);
                    pItem->SetValue(strMac);
                    pNetCb->SetDesMAC(ucMac, 6);
                    m_ctlCtrlBlockReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
                }
                CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
            }
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}



void CEditSmvSendDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        if (m_eShowReportType == ESSShowCtrlBlockReport)
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlCtrlBlockReport.GetInplaceEdit());
        }
        else 
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlChannelReport.GetInplaceEdit());
        }
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}
void CEditSmvSendDlg::OnNaviMenuEditSmvChan(UINT nID)
{
    switch (nID)
    {
    case ID_MENU_EDIT_SMV_CHAN_F1:
        {
            m_eShowReportType = ESSShowCtrlBlockReport;
            _showCtrlBlockReport();
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F2:
        {
#if SU_FEI
            if(m_nSmvSendType == FRAME_TYPE_NET_9_2)
            {
                //
                //只有9-2可以改变通道个数
                //
                CChannel* pChannel = m_pBaseCb->AddChannel();
                if (pChannel)
                {
                    int nIndex = m_ctlChannelReport.GetRecordRowCount();
                    m_ctlChannelReport.InsertRecordRow(nIndex, new CDsmEditSmvSendChanRecord(pChannel, nIndex+1));
                    m_ctlChannelReport.SetFocusedSelectionRowIndex(nIndex);
                    m_ctlChannelReport.SetRecordRowData(nIndex, (DWORD_PTR)pChannel);
                    m_ctlChannelReport.EnsureVisible(nIndex, FALSE);
                    m_ctlChannelReport.Invalidate(TRUE);

                }
            }
#else
			m_eShowReportType = ESSShowChannelReport;
			_showChannelReport();
#endif
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F3:
        {
#if SU_FEI
            if (m_nSmvSendType == FRAME_TYPE_NET_9_2)
            {
                //
                //只有9-2可以改变通道个数
                //
                if ( m_pBaseCb->GetChannelCount() <= DSM_SMV_9_2_CHAN_MIN)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_CHAN_MIN), 3000);
                    return;
                }
                else
                {
                    //删除通道
                    m_ctlChannelReport.SetRedraw(FALSE);
                    int nSelRow = m_ctlChannelReport.GetFocusedRowIndex();
                    if (nSelRow >= 0)
                    {
                        CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nSelRow);
                        if (pChannel)
                        {
                            m_pBaseCb->DeleteChannel(pChannel);
                        }
                        m_ctlChannelReport.DeleteRecordRow(nSelRow);

                        CString strIndex = _T("");
                        for (int i = nSelRow; i < m_ctlChannelReport.GetRecordRowCount(); ++i)
                        {
                            strIndex.Format(_T("%d"), i + 1);
                            ((CELRecordItemText*)m_ctlChannelReport.GetRecordRow(i)->GetItem(DSM_ESS_EDIT_CHAN_INDEX))->SetValue(strIndex);
                            m_ctlChannelReport.RedrawRowItem(i, DSM_ESS_EDIT_CHAN_INDEX);
                        }
                    }
                    m_ctlChannelReport.SetRedraw(TRUE);
                }
            }
#else
			if(m_nSmvSendType == FRAME_TYPE_NET_9_2)
			{
				//
				//只有9-2可以改变通道个数,添加通道数
				//
				CChannel* pChannel = m_pBaseCb->AddChannel();
				if (pChannel)
				{
					int nIndex = m_ctlChannelReport.GetRecordRowCount();
					m_ctlChannelReport.InsertRecordRow(nIndex, new CDsmEditSmvSendChanRecord(pChannel, nIndex+1));
					m_ctlChannelReport.SetFocusedSelectionRowIndex(nIndex);
					m_ctlChannelReport.SetRecordRowData(nIndex, (DWORD_PTR)pChannel);
					m_ctlChannelReport.EnsureVisible(nIndex, FALSE);
					m_ctlChannelReport.Invalidate(TRUE);

				}
			}
#endif
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F4:
        {
#if SU_FEI
            //测量电流
            int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
            int nColIndex = m_ctlChannelReport.GetFocusedColIndex();
            if (nRowIndex >= 0 && nColIndex == DSM_ESS_EDIT_CHAN_TYPE && m_nSmvSendType != FRAME_TYPE_NET_9_2)
            {
                CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);
                if (pChannel == NULL)
                {
                    break;
                }
                if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
                {
                    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(DSM_ESS_EDIT_CHAN_TYPE));
                    if (pItem)
                    {
                        if (pChannel->IsMeasure())
                        {
                            pChannel->SetMeasure(false);
                            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
                        }
                        else 
                        {
                            pChannel->SetMeasure(true);
                            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER_MEASURE));
                        }
                        m_ctlChannelReport.RedrawRowItem(nRowIndex, DSM_ESS_EDIT_CHAN_TYPE);
                    }
                }
            }
#else
			if (m_nSmvSendType == FRAME_TYPE_NET_9_2)
			{
				//
				//只有9-2可以改变通道个数
				//
				if ( m_pBaseCb->GetChannelCount() <= DSM_SMV_9_2_CHAN_MIN)
				{
					CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_CHAN_MIN), 3000);
					return;
				}
				else
				{
					//删除通道
					m_ctlChannelReport.SetRedraw(FALSE);
					int nSelRow = m_ctlChannelReport.GetFocusedRowIndex();
					if (nSelRow >= 0)
					{
						CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nSelRow);
						if (pChannel)
						{
							m_pBaseCb->DeleteChannel(pChannel);
						}
						m_ctlChannelReport.DeleteRecordRow(nSelRow);

						CString strIndex = _T("");
						for (int i = nSelRow; i < m_ctlChannelReport.GetRecordRowCount(); ++i)
						{
							strIndex.Format(_T("%d"), i + 1);
							((CELRecordItemText*)m_ctlChannelReport.GetRecordRow(i)->GetItem(DSM_ESS_EDIT_CHAN_INDEX))->SetValue(strIndex);
							m_ctlChannelReport.RedrawRowItem(i, DSM_ESS_EDIT_CHAN_INDEX);
						}
					}
					m_ctlChannelReport.SetRedraw(TRUE);
				}
			}
			else  ///< 9-1/FT3/FT3EX通道类型
			{
				//测量电流
				int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
				int nColIndex = m_ctlChannelReport.GetFocusedColIndex();
				if (nRowIndex >= 0 && nColIndex == DSM_ESS_EDIT_CHAN_TYPE && m_nSmvSendType != FRAME_TYPE_NET_9_2)
				{
					CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);
					if (pChannel == NULL)
					{
						break;
					}
					if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
					{
						CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(DSM_ESS_EDIT_CHAN_TYPE));
						if (pItem)
						{
							if (pChannel->IsMeasure())
							{
								pChannel->SetMeasure(false);
								pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
							}
							else 
							{
								pChannel->SetMeasure(true);
								pItem->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER_MEASURE));
							}
							m_ctlChannelReport.RedrawRowItem(nRowIndex, DSM_ESS_EDIT_CHAN_TYPE);
						}
					}
				}
			}

#endif
        }
        break;
        //9-2
    case ID_MENU_EDIT_SMV_CHAN_F5_92_8A:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb)
            {
                p92Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_92_12D:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb)
            {
                p92Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_92_18A:
        {
            CIEC9_2Cb* p92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb);
            if (p92Cb)
            {
                p92Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK);
                _updateChanReport();
            }
        }
        break;
        //9-1
    case ID_MENU_EDIT_SMV_CHAN_F5_91_12A:
        {
            CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (p91Cb)
            {
                p91Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_91_12B:
        {
            CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (p91Cb)
            {
                p91Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_91_12C:
        {
            CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (p91Cb)
            {
                p91Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8);
                _updateChanReport();
            }
        }
        break;
        //ft3
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_12A:
        {
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb)
            {
                pFt3Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_12B:
        {
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb)
            {
                pFt3Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_12C:
        {
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb)
            {
                pFt3Cb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8);
                _updateChanReport();
            }
        }
        break;
        //ft3扩展
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22A:
        {
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb)
            {
                pFt3ExpCb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22B:
        {
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb)
            {
                pFt3ExpCb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F5_FT3_EXT_22C:
        {
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb)
            {
                pFt3ExpCb->UseChannelTemplate(SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK);
                _updateChanReport();
            }
        }
        break;
    case ID_MENU_EDIT_SMV_CHAN_F6:
        {
            CChannel* pChannel = NULL;
            //清除映射
            for(int i = 0; i < m_ctlChannelReport.GetRecordRowCount(); ++i)
            {
                pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(i);
                if (pChannel)
                {
                    pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                }
                ((CELRecordItemText*)m_ctlChannelReport.GetRecordRow(i)->GetItem(DSM_ESS_EDIT_CHAN_MAP))->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                m_ctlChannelReport.RedrawRowItem(i, DSM_ESS_EDIT_CHAN_MAP);
            }
         }
        break;
    default:
        break;
    }
}

void CEditSmvSendDlg::OnSmvChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if(!pItemNotify || !m_pBaseCb)
    {
        return;
    } 
    
    CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(pItemNotify->rowIndex);
    ASSERT(pChannel);
    if (pChannel == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

    CString strValue = pItem->GetValue();

    switch(pItemNotify->colIndex)
    {
    case DSM_ESS_EDIT_CHAN_DESC:
        {
            //描述
            pChannel->SetDescription((LPCTSTR)strValue);
            *pResult = TRUE;
        }
        break;
    case DSM_ESS_EDIT_CHAN_ONCE:
        {
            //一次值
            if (pChannel->GetChannelType() == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                //电压 为kV，需要做转换

                double dValue = 0.0f;

                if (DSM_Str2Double(strValue, dValue))
                {
                    pChannel->SetOnceRatings(dValue*1000.0f);
                }
                dValue = pChannel->GetOnceRatings()/1000.0f;
                strValue.Format(_T("%.03lf"), dValue);
                pItem->SetValue(strValue);
            }
            else
            {
                //保持3位小数
                double dValue = 0.0f;
                if (DSM_Str2Double(strValue, dValue))
                {
                    pChannel->SetOnceRatings(dValue);
                }

                strValue.Format(_T("%.03lf"), pChannel->GetOnceRatings());
                pItem->SetValue(strValue);
            }
            *pResult = TRUE;
        }
        break;
    case DSM_ESS_EDIT_CHAN_TWICE:
        {
            double dValue = 0.0f;
            if (DSM_Str2Double(strValue, dValue))
            {
                pChannel->SetSecondaryRatings(dValue);
            }
            strValue.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    }
}
void CEditSmvSendDlg::OnSmvChanItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify || !m_pBaseCb)
    {
        return;
    }

    CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(pItemNotify->rowIndex);
    ASSERT(pChannel);
    if (pChannel == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

    DWORD_PTR dwpConData = pItemNotify->pConstraint->GetConstraintData();
    CString strCaption = pItemNotify->pConstraint->GetConstraintCaption();


    switch(pItemNotify->colIndex)
    {
    case DSM_ESS_EDIT_CHAN_TYPE:
        {
            //通道类型
            enumChannelType nChanType = (enumChannelType)dwpConData;
            
            pChannel->SetChannelType(nChanType);
            if (nChanType == CHANNEL_TYPE_SMV_CURRENT &&
                pChannel->IsMeasure())
            {
                pChannel->SetMeasure(false);
            }
            _updateSmvChanMenuF4();
            //
            //修改相位
            //
            CELRecordItemText* pItemPhase = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_PHASE);
            if (pItemPhase == NULL)
            {
                return;
            }

            if (nChanType == CHANNEL_TYPE_SMV_CURRENT || nChanType == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                CELRecordItemConstraint* pItemCon = pItemPhase->GetEditOptions(NULL)->FindConstraint (pChannel->GetChannelPhase());
                if(pItemCon)
                {
                    pItemPhase->SetValue(pItemCon->GetConstraintCaption());
                }
                pItemPhase->SetEditable(TRUE);
            }
            else
            {
                pItemPhase->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
                pChannel->SetChannelPhase(CHANNEL_PHASE_UNKOWN);
                pItemPhase->SetEditable(FALSE);
            }
            
            //
            //修改额定值
            //
            CParamConfigBasic* pCfg = CDsmDataLayer::getInstance()->GetParamConfigBasic();
            if (pCfg == NULL)
            {
                return;
            }

            CELRecordItemText* pItemDesc = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_DESC);
            CELRecordItemText* pItemOnce = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_ONCE);
            CELRecordItemText* pItemTwice = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_TWICE);

            pItemDesc->SetValue(pChannel->GetDescription().c_str());
            m_ctlChannelReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);

            switch(nChanType)
            {
            case CHANNEL_TYPE_SMV_CURRENT:
                {
                    //重设额定值

                    CString strTemp = _T("");
                    strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings());
                    pItemOnce->SetValue(strTemp);
                    pItemOnce->SetFormatString(_T("%s A"));

                    strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
                    pItemTwice->SetValue(strTemp);
                    pItemTwice->SetFormatString(_T("%s A"));
                    
                }
                break;

            case CHANNEL_TYPE_SMV_VOLTAGE:
                {
                    CString strTemp = _T("");
                    strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings() /1000.0f);
                    pItemOnce->SetValue(strTemp);
                    pItemOnce->SetFormatString(_T("%s kV"));

                    strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
                    pItemTwice->SetValue(strTemp);
                    pItemTwice->SetFormatString(_T("%s V"));
                }
                break;
            case CHANNEL_TYPE_SMV_TIME:
                {
                    CString strDelay = _T("");
                    strDelay.Format(_T("%.03lf"), pChannel->GetOnceRatings());
                    pItemOnce->SetValue(strDelay);
                    pItemOnce->SetFormatString(_T("%s μs"));

                    pItemTwice->SetValue(strDelay);
                    pItemTwice->SetFormatString(_T("%s μs"));
                }
                break;
            default:
                {
                    CString strTemp = _T("");
                    strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings());
                    pItemOnce->SetValue(strTemp);
                    pItemOnce->SetFormatString(_T("%s"));

                    strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
                    pItemTwice->SetValue(_T("1.000"));
                    pItemTwice->SetFormatString(_T("%s"));
                }
                break;
            }

            //
            //修改映射
            //
            CELRecordItemText* pItemMap = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_MAP);
            if (pItemMap == NULL)
            {
                return;
            }
            pItemMap->GetEditOptions(NULL)->GetConstraints()->RemoveAll();
            

            switch(nChanType)
            {
            case CHANNEL_TYPE_SMV_CURRENT:
                {
                    pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ia1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ib1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ic1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ix1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ia2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ib2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ic2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ix2"));
                    pItemMap->SetEditable(TRUE);
                }
                break;
            case CHANNEL_TYPE_SMV_VOLTAGE:
                {
                    pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ua1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ub1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Uc1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ux1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ua2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ub2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Uc2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ux2"));
                    pItemMap->SetEditable(TRUE);
                }
                break;
            case CHANNEL_TYPE_SMV_TIME:
                {
                    pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t1"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t2"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t3"));
                    pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t4"));
                    pItemMap->SetEditable(TRUE);
                }
                break;
            default:
                {
                    //pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                    pItemMap->SetEditable(FALSE);
                }
                break;
            }
            CELRecordItemEditOptions* pItemOptions = pItemMap->GetEditOptions(NULL);
            if (pItemOptions)
            {
                CELRecordItemConstraint* pItemCon = pItemOptions->FindConstraint(pChannel->GetVirtualChannel().c_str());
                if (pItemCon)
                {
                    pItemMap->SetValue(pItemCon->GetConstraintCaption());
                }
                else
                {
                    pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                    pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                }
            }
            else
            {
                pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
                pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
            }
            *pResult = TRUE;
        }
        break;
    case DSM_ESS_EDIT_CHAN_PHASE:
        {
            //
            //相位
            //
            pChannel->SetChannelPhase((ChannelPhase)pItemNotify->pConstraint->GetConstraintData());
            CString strDesc = pChannel->GetDescription().c_str();
            CELRecordItemText* pItemDesc = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_DESC);
            if (pItemDesc)
            {
                pItemDesc->SetValue(strDesc);
                m_ctlChannelReport.RedrawRowItem(pItemNotify->rowIndex, DSM_ESS_EDIT_CHAN_DESC);
            }


            *pResult = TRUE;
        }
        break;
    case DSM_ESS_EDIT_CHAN_MAP:
        {
            //
            //映射
            //
            if (pItemNotify->pConstraint->GetConstraintCaption() != (LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN))
            {
                pChannel->SetVirtualChannel((LPCTSTR)pItemNotify->pConstraint->GetConstraintCaption());
            }
            else
            {
                pChannel->SetVirtualChannel(_T(""));
            }
            *pResult = TRUE;
        }
        break;
    }
}


BOOL CEditSmvSendDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    KillTimer(1);
    return CBaseDlg::DestroyWindow();
}

void CEditSmvSendDlg::_SavePage()
{

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CEditSmvSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CEditSmvSendDataExtra(pData);
    }

    CEditSmvSendDataExtra* pDataExtra = dynamic_cast<CEditSmvSendDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }
    if (m_eShowReportType == ESSShowCtrlBlockReport)
    {
        pDataExtra->m_nFocusRowIndex = m_ctlCtrlBlockReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusColIndex = m_ctlCtrlBlockReport.GetFocusedColIndex();
    }
    else
    {
        pDataExtra->m_nFocusRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusColIndex = m_ctlChannelReport.GetFocusedColIndex();
    }
    pDataExtra->m_eShowReportType = m_eShowReportType;
    pDataExtra->m_pBaseCb = m_pBaseCb;

    if (m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        int nRowIndex = m_ctlCtrlBlockReport.GetFocusedRowIndex();
        SERecordFuncType seRft = (SERecordFuncType)m_ctlCtrlBlockReport.GetRowItemData(nRowIndex, 0);
        if (seRft == SEStatusWord1)
        {
            pDataExtra->m_eStatusWordType = ESSStatusWord1;
        }
        else if (seRft == SEStatusWord2)
        {
            pDataExtra->m_eStatusWordType = ESSStatusWord2;
        }
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();
}

void CEditSmvSendDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CEditSmvSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回

        CEditSmvSendDataExtra* pDataExtra = dynamic_cast<CEditSmvSendDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        m_pBaseCb = pDataExtra->m_pBaseCb;
        if (pDataExtra->m_eShowReportType == ESSShowCtrlBlockReport)
        {
            _showCtrlBlockReport();
            m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            m_ctlCtrlBlockReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
            m_ctlCtrlBlockReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
        else
        {
            _showChannelReport();
            m_ctlChannelReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            m_ctlChannelReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
            m_ctlChannelReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
        

    }
}
void CEditSmvSendDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
        if (NULL == pParamConfigSMVSend)
        {
            return ;
        }
        pParamConfigSMVSend->SaveConfig();
    }
    CBaseDlg::OnTimer(nIDEvent);
}


void CEditSmvSendDlg::_updateSmvChanMenuF4()
{
    if (m_eShowReportType != ESSShowChannelReport)
    {
        return;
    }
    if (m_ctlChannelReport.GetSafeHwnd())
    {
        int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
        int nColIndex = m_ctlChannelReport.GetFocusedColIndex();

        if (nColIndex == DSM_ESS_EDIT_CHAN_TYPE && 
            nRowIndex >= 0 &&
            m_nSmvSendType != FRAME_TYPE_NET_9_2)
        {
            //选择通道
            CChannel* pChan = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);
            if (pChan)
            {
                if (pChan->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT && pChan->IsMeasure())
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_CHAN_MENU_AMPER_PROTECT));
                }
                else if(pChan->GetChannelType() == CHANNEL_TYPE_SMV_CURRENT)
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_CHAN_MENU_AMPER_MEASURE));
                }
                else
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
                }
            }
        }
#if SU_FEI
        else
        {
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
        }
#else
		else if (m_nSmvSendType != FRAME_TYPE_NET_9_2)
		{
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
		}
		else
		{
			m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T("删除"));
		}
	
#endif
        m_pNaviBar->UpdateNaviMenu();
    }
}

BOOL CEditSmvSendDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

#if!CHUN_HUA
	if(WM_KEYDOWN == pMsg->message)
	{
		switch(pMsg->wParam)
		{
	
		case VK_F8:
			{
				_showContexMenu();
				return TRUE;
			}
			break;
		default:
			break;
		}
	}
#endif

    if (m_ctlChannelReport.GetSafeHwnd() == GetFocus()->GetSafeHwnd())
    {
        _updateSmvChanMenuF4();
    }

    BOOL bRet = CBaseDlg::PreTranslateMessage(pMsg);


#if!CHUN_HUA
	if(WM_KEYDOWN == pMsg->message)
	{
		switch(pMsg->wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			{
				if (m_eShowReportType == ESSShowChannelReport && m_ctlChannelReport.GetSafeHwnd())
				{
					int nSelCol = m_ctlChannelReport.GetFocusedColIndex();

					switch(nSelCol)
					{
					case 6:
						{
							_updateContexMenu(DSM_EDIT_SMV_CHAN_WIDTH_CHAN_PHASE);
						}
						break;
					case 3:
						{
							_updateContexMenu(DSM_ESS_EDIT_CHAN_MAP);
						}
						break;
					case 2:
						{
							_updateContexMenu(DSM_ESS_EDIT_CHAN_TYPE);
						}
						break;
					default:
						{
							_updateContexMenu(0);
						}
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}
#endif


    if (m_ctlCtrlBlockReport.GetSafeHwnd() && 
        m_ctlCtrlBlockReport.GetSafeHwnd() == GetFocus()->GetSafeHwnd())
    {
        _updateSmvCtrlMenuF6();
    }
    return bRet;
}
#if!CHUN_HUA
void CEditSmvSendDlg::_showContexMenu()
{

	CReportV* cCurrentReport;
	if (m_eShowReportType == ESSShowChannelReport)
	{
		cCurrentReport = &m_ctlChannelReport;
	}
	else  
	{
		return;
	}
	int nRowIndex = cCurrentReport->GetFocusedRowIndex();
	int nColIndex = cCurrentReport->GetFocusedColIndex();
	if (nRowIndex <0 || nColIndex < 0||m_pPopupMenu->GetMenuItemCount()==1)
	{
		return;
	}


	CELRecordItemText* pItem = (CELRecordItemText*)cCurrentReport->GetRecordRow(nRowIndex)->GetItem(nColIndex);
	CRect rect = cCurrentReport->GetRowItemRect(nRowIndex,nColIndex);	 


	if (pItem == NULL )
	{
		return; 
	}


	//CNaviMenu* pPopup = m_ContexMenu.GetSubMenu(5);

	CRect rcItem = rect;

	CNaviPopupBar popupBar;


	UINT  cmdId = m_pPopupMenu->TrackPopupMenu(rcItem, cCurrentReport, &popupBar, CNaviMenu::RET_CMDID);
	if (cmdId )
	{
		this->SendMessage(WM_COMMAND, cmdId);

	}  
}

void CEditSmvSendDlg::_updateContexMenu(int col)
{

	CMenu menu2;
	switch(col)
	{
	
	case 0:
		//CNaviMenu* pSubMenu = new CNaviMenu;
		menu2.LoadMenu(IDR_MENU_SMV_CHANEL_EDIT_POPMENU);
		m_pPopupMenu->LoadMenu(menu2.GetSubMenu(3));
		break;
	case DSM_ESS_EDIT_CHAN_MAP:
		//CNaviMenu* pSubMenu = new CNaviMenu;
		menu2.LoadMenu(IDR_MENU_SMV_CHANEL_EDIT_POPMENU);
		m_pPopupMenu->LoadMenu(menu2.GetSubMenu(0));
		break;
	case DSM_EDIT_SMV_CHAN_WIDTH_CHAN_PHASE:
		//CNaviMenu* pSubMenu = new CNaviMenu;
		menu2.LoadMenu(IDR_MENU_SMV_CHANEL_EDIT_POPMENU);
		m_pPopupMenu->LoadMenu(menu2.GetSubMenu(1));
		break;
	case DSM_ESS_EDIT_CHAN_TYPE:
		//CNaviMenu* pSubMenu = new CNaviMenu;
		menu2.LoadMenu(IDR_MENU_SMV_CHANEL_EDIT_POPMENU);
		m_pPopupMenu->LoadMenu(menu2.GetSubMenu(2));
		break;
	default:
		break;
	}
}

void CEditSmvSendDlg::OnPopUpMenu(UINT nID)
{
	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nColIndex = m_ctlChannelReport.GetFocusedColIndex();
	int nChannelCount = m_pBaseCb->GetChannelCount();
	int nIndex = 0;

	int nPhaseIndexArray[][12] = {	{1,2,3,0,},
									{1,2,3,5,0,},
									{1,2,3,5,4,0,},
									{1,1,2,2,3,3,0,},
									{1,1,2,2,3,3,5,5,0,},
									{1,1,2,2,3,3,5,5,4,4,0,},
									{1,1,1,2,2,2,3,3,3,0},
									{1,1,1,2,2,2,3,3,3,5,5,5}
							   };



	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	switch(nID)
	{
	case ID__ABC:
		
		while(pChannel && nIndex<3)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[0][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}

		break;
	case ID__ABCN:		
			
		while(pChannel && nIndex<4)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[1][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
		break;
	case ID__ABCNX:
		while(pChannel && nIndex<5)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[2][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
		
		break;
	case ID__AABBCC:
		
		while(pChannel && nIndex<6)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[3][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
	
		break;
	case ID__AABBCCNN:
		
		while(pChannel && nIndex<8)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[4][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
		
		break;
	case ID__AABBCCNNXX :
		
		while(pChannel && nIndex<10)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[5][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
		
		break;
	case  ID__AAABBBCCC:
		
		while(pChannel && nIndex<9)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[6][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;
		}
		
		break;
	case ID__AAABBBCCCNNN :
		
		while(pChannel && nIndex<12)
		{	
			pChannel->SetChannelPhase((ChannelPhase)nPhaseIndexArray[7][nIndex]);
			_setPhase(pChannel, nRowIndex, nIndex);

			++nIndex;		
		}
		break;
	case ID__MAP_AUTO :
		_setAutoMap();
		break;
	case ID__MAP_2GROUP1 :
		_setMapGroup1();
		break;
	case ID__MAP_2GROUP2:
		_setMapGroup2();
		break;
	case ID__ALL_CURRENT:
		_setTypeAllCurrent();
		break;
	case ID__ALL_VOLATE:
		_setTypeAllVolate();
		break;

	}
}

void CEditSmvSendDlg::_setPhase( CChannel* &pChannel, int nRowIndex, int nIndex )
{
	
	CString strDesc = pChannel->GetDescription().c_str();
	CELRecordItemText* pItemDesc = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(nRowIndex+nIndex)->GetItem(DSM_ESS_EDIT_CHAN_DESC);
	if (pItemDesc)
	{
		pItemDesc->SetValue(strDesc);
		m_ctlChannelReport.RedrawRowItem(nRowIndex+nIndex, DSM_ESS_EDIT_CHAN_DESC);
	}

	CELRecordItemText* pItemPhase = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(nRowIndex+nIndex)->GetItem(DSM_ESS_EDIT_CHAN_PHASE);
	if (pItemPhase == NULL)
	{
		return;
	}

	CELRecordItemConstraint* pItemCon = pItemPhase->GetEditOptions(NULL)->FindConstraint (pChannel->GetChannelPhase());
	if(pItemCon)
	{
		pItemPhase->SetValue(pItemCon->GetConstraintCaption());
		m_ctlChannelReport.RedrawRowItem(nRowIndex+nIndex, DSM_ESS_EDIT_CHAN_PHASE);
	}
	pItemPhase->SetEditable(TRUE);


	pChannel = m_pBaseCb->NextChannel(pChannel);
	
}

void CEditSmvSendDlg::_setAutoMap()
{


	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nIndex = 0;


	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	while(pChannel)
	{	
		//通道类型
		enumChannelType nChanType = (enumChannelType)pChannel->GetChannelType();
		ChannelPhase nChanPhase =  pChannel->GetChannelPhase();
		//
		//修改映射
		//
		CELRecordItemText* pItemMap = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(nRowIndex+nIndex)->GetItem(DSM_ESS_EDIT_CHAN_MAP);
		if (pItemMap == NULL)
		{
			return;
		}

		CString sLastIndex ;
		if ((LPCTSTR)pItemMap->GetCaption() != (LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN))
		{
			sLastIndex = pItemMap->GetCaption().Right(1) ;

		}

		CString sMapString ;
		switch(nChanType)
		{
		case CHANNEL_TYPE_SMV_CURRENT:
			{
				sMapString = "I";
			}
			break;
		case CHANNEL_TYPE_SMV_VOLTAGE:
			{
				sMapString = "U";
			}
			break;
		case CHANNEL_TYPE_SMV_TIME:
			{
				sMapString = "t";
			}
			break;
		default:
			{
				sMapString = "--";			
			}
			break;
		}

		switch(nChanPhase)
		{
		case CHANNEL_PHASE_A:
			sMapString += "a";
			break;
		case CHANNEL_PHASE_B:
			sMapString += "b";
			break;
		case CHANNEL_PHASE_C:
			sMapString += "c";
			break;
		case CHANNEL_PHASE_N:
		case CHANNEL_PHASE_X:
			sMapString += "x";
			break;
		case CHANNEL_PHASE_UNKOWN:
			break;
		default:
			break;

		}

		if (sLastIndex=="-" && sMapString!="")
		{
			sLastIndex = "1";
		}

		sMapString += sLastIndex;

		pItemMap->SetValue((LPCTSTR)sMapString);

		pChannel->SetVirtualChannel((LPCTSTR)sMapString);
		m_ctlChannelReport.RedrawRowItem(nRowIndex+nIndex, DSM_ESS_EDIT_CHAN_MAP);
		nIndex++;
		pChannel =  m_pBaseCb->NextChannel(pChannel);
	}
	
}

void CEditSmvSendDlg::_setMapGroup1()
{
	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nIndex = 0;


	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	while(pChannel)
	{	
		
		//
		//修改映射
		//
		CELRecordItemText* pItemMap = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(nRowIndex+nIndex)->GetItem(DSM_ESS_EDIT_CHAN_MAP);
		if (pItemMap == NULL)
		{
			return;
		}

		CString sLeftString  = pItemMap->GetCaption();;
		if ((LPCTSTR)sLeftString == (LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN) || sLeftString == "--")
		{
			break;
		}
		sLeftString = sLeftString.Left(sLeftString.GetLength()-1);
		CString sMapString = sLeftString + L"1" ;

		pItemMap->SetValue((LPCTSTR)sMapString);

		pChannel->SetVirtualChannel((LPCTSTR)sMapString);
		m_ctlChannelReport.RedrawRowItem(nRowIndex+nIndex, DSM_ESS_EDIT_CHAN_MAP);
		nIndex++;
		pChannel =  m_pBaseCb->NextChannel(pChannel);
	}
}

void CEditSmvSendDlg::_setMapGroup2()
{
	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nIndex = 0;


	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	while(pChannel)
	{	

		//
		//修改映射
		//
		CELRecordItemText* pItemMap = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(nRowIndex+nIndex)->GetItem(DSM_ESS_EDIT_CHAN_MAP);
		if (pItemMap == NULL)
		{
			return;
		}

		CString sLeftString  = pItemMap->GetCaption();;
		if ((LPCTSTR)sLeftString == (LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN) || sLeftString == "--")
		{
			break;
		}
		sLeftString = sLeftString.Left(sLeftString.GetLength()-1);

		CString sMapString = sLeftString +L"2" ;

		pItemMap->SetValue((LPCTSTR)sMapString);

		pChannel->SetVirtualChannel((LPCTSTR)sMapString);
		m_ctlChannelReport.RedrawRowItem(nRowIndex+nIndex, DSM_ESS_EDIT_CHAN_MAP);
		nIndex++;
		pChannel =  m_pBaseCb->NextChannel(pChannel);
	}
}

void CEditSmvSendDlg::_setTypeAllCurrent()
{
	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nIndex = 0;


	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	while(pChannel)
	{	

		setType(pChannel,CHANNEL_TYPE_SMV_CURRENT,nRowIndex+nIndex);
		nIndex++;
		pChannel =  m_pBaseCb->NextChannel(pChannel);
	}
	
}

void CEditSmvSendDlg::_setTypeAllVolate()
{
	if (m_ctlChannelReport.GetSafeHwnd()==NULL)
	{
		return;
	}
	int nRowIndex = m_ctlChannelReport.GetFocusedRowIndex();
	int nIndex = 0;


	//选择通道
	CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowIndex);

	while(pChannel)
	{	

		setType(pChannel,CHANNEL_TYPE_SMV_VOLTAGE,nRowIndex+nIndex);
		
		nIndex++;
		pChannel =  m_pBaseCb->NextChannel(pChannel);
	}
}

void CEditSmvSendDlg::setType(CChannel* pChannel, enumChannelType nChanType, int rowIndex)
{
	
	pChannel->SetChannelType(nChanType);
	if (nChanType == CHANNEL_TYPE_SMV_CURRENT &&
		pChannel->IsMeasure())
	{
		pChannel->SetMeasure(false);
	}
	_updateSmvChanMenuF4();
	//
	//修改类型
	//
	CELRecordItemText* pItemType = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_TYPE);
	if (pItemType == NULL)
	{
		return;
	}
	switch(nChanType)
	{
	case CHANNEL_TYPE_SMV_CURRENT:
		pItemType->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
		break;
	case CHANNEL_TYPE_SMV_VOLTAGE:
		pItemType->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_VOLT));
		break;
	}
	

	//
	//修改相位
	//
	CELRecordItemText* pItemPhase = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_PHASE);
	if (pItemPhase == NULL)
	{
		return;
	}

	if (nChanType == CHANNEL_TYPE_SMV_CURRENT || nChanType == CHANNEL_TYPE_SMV_VOLTAGE)
	{
		CELRecordItemConstraint* pItemCon = pItemPhase->GetEditOptions(NULL)->FindConstraint (pChannel->GetChannelPhase());
		if(pItemCon)
		{
			pItemPhase->SetValue(pItemCon->GetConstraintCaption());
		}
		pItemPhase->SetEditable(TRUE);
	}
	else
	{
		pItemPhase->SetValue(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_UNKNOWN));
		pChannel->SetChannelPhase(CHANNEL_PHASE_UNKOWN);
		pItemPhase->SetEditable(FALSE);
	}

	//
	//修改额定值
	//
	CParamConfigBasic* pCfg = CDsmDataLayer::getInstance()->GetParamConfigBasic();
	if (pCfg == NULL)
	{
		return;
	}

	CELRecordItemText* pItemDesc = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_DESC);
	CELRecordItemText* pItemOnce = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_ONCE);
	CELRecordItemText* pItemTwice = (CELRecordItemText*)m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_TWICE);

	pItemDesc->SetValue(pChannel->GetDescription().c_str());
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_TYPE);

	switch(nChanType)
	{
	case CHANNEL_TYPE_SMV_CURRENT:
		{
			//重设额定值

			CString strTemp = _T("");
			strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings());
			pItemOnce->SetValue(strTemp);
			pItemOnce->SetFormatString(_T("%s A"));

			strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
			pItemTwice->SetValue(strTemp);
			pItemTwice->SetFormatString(_T("%s A"));

		}
		break;

	case CHANNEL_TYPE_SMV_VOLTAGE:
		{
			CString strTemp = _T("");
			strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings() /1000.0f);
			pItemOnce->SetValue(strTemp);
			pItemOnce->SetFormatString(_T("%s kV"));

			strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
			pItemTwice->SetValue(strTemp);
			pItemTwice->SetFormatString(_T("%s V"));
		}
		break;
	case CHANNEL_TYPE_SMV_TIME:
		{
			CString strDelay = _T("");
			strDelay.Format(_T("%.03lf"), pChannel->GetOnceRatings());
			pItemOnce->SetValue(strDelay);
			pItemOnce->SetFormatString(_T("%s μs"));

			pItemTwice->SetValue(strDelay);
			pItemTwice->SetFormatString(_T("%s μs"));
		}
		break;
	default:
		{
			CString strTemp = _T("");
			strTemp.Format(_T("%.03lf"), pChannel->GetOnceRatings());
			pItemOnce->SetValue(strTemp);
			pItemOnce->SetFormatString(_T("%s"));

			strTemp.Format(_T("%.03lf"), pChannel->GetSecondaryRatings());
			pItemTwice->SetValue(_T("1.000"));
			pItemTwice->SetFormatString(_T("%s"));
		}
		break;
	}

	//
	//修改映射
	//
	CELRecordItemText* pItemMap = (CELRecordItemText* )m_ctlChannelReport.GetRecordRow(rowIndex)->GetItem(DSM_ESS_EDIT_CHAN_MAP);
	if (pItemMap == NULL)
	{
		return;
	}
	pItemMap->GetEditOptions(NULL)->GetConstraints()->RemoveAll();


	switch(nChanType)
	{
	case CHANNEL_TYPE_SMV_CURRENT:
		{
			pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ia1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ib1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ic1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ix1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ia2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ib2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ic2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ix2"));
			pItemMap->SetEditable(TRUE);
		}
		break;
	case CHANNEL_TYPE_SMV_VOLTAGE:
		{
			pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ua1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ub1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Uc1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ux1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ua2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ub2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Uc2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("Ux2"));
			pItemMap->SetEditable(TRUE);
		}
		break;
	case CHANNEL_TYPE_SMV_TIME:
		{
			pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t1"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t2"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t3"));
			pItemMap->GetEditOptions(NULL)->AddConstraint(_T("t4"));
			pItemMap->SetEditable(TRUE);
		}
		break;
	default:
		{
			//pItemMap->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
			pItemMap->SetEditable(FALSE);
		}
		break;
	}
	CELRecordItemEditOptions* pItemOptions = pItemMap->GetEditOptions(NULL);
	if (pItemOptions)
	{
		CELRecordItemConstraint* pItemCon = pItemOptions->FindConstraint(pChannel->GetVirtualChannel().c_str());
		if (pItemCon)
		{
			pItemMap->SetValue(pItemCon->GetConstraintCaption());
		}
		else
		{
			pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
			pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
		}
	}
	else
	{
		pItemMap->SetValue(dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
		pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN));
	}
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_DESC);
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_TYPE);
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_PHASE);
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_ONCE);
	m_ctlChannelReport.RedrawRowItem(rowIndex,DSM_ESS_EDIT_CHAN_TWICE);
}
#endif

