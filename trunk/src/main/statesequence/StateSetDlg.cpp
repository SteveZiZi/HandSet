// StateSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StateSetDlg.h"
#include "StateSequenceDlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/manual/ManualSmvChanMapDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "../popdlg/MsgBtnDlg.h"

//////////////////////////////////////////////////////////////////////////
//状态序列-状态设置
//
#if SHENG_ZE
#define DSM_STATE_SET_WIDTH_CAPTION            180
#define DSM_STATE_SET_WIDTH_CONTENT            349
#else
#define DSM_STATE_SET_WIDTH_CAPTION            227
#define DSM_STATE_SET_WIDTH_CONTENT            302
#define DSM_STATE_SET_WIDTH_DI				   75
#endif

#define DSM_STATE_SET_DO_COUNT                  6

#define DSM_STATE_SET_DO_START_INDEX            0
#define DSM_STATE_SET_CHANGE_TYPE_INDEX         6
#define DSM_STATE_SET_DI_CHANGE_TYPE_INDEX      7
#define DSM_STATE_SET_CHANGE_DELAY_INDEX        7
#define DSM_STATE_SET_DI_START_INDEX            8

//状态序列-状态数据
//
#define DSM_STATE_DATA_WIDTH_CHAN              79
#define DSM_STATE_DATA_WIDTH_AMPL              150
#define DSM_STATE_DATA_WIDTH_PHASE             120
#define DSM_STATE_DATA_WIDTH_FREQUENCE         180

#define SSS_COL_INDEX_CHAN           0
#define SSS_COL_INDEX_AMPL           1
#define SSS_COL_INDEX_PHASE          2
#define SSS_COL_INDEX_FREQUENCE      3


#if !CHUN_HUA

//状态序列-谐波
//
#define SH_COL_INDEX_CHAN                          0
#define SH_COL_INDEX_HORMANIC                      1
#define SH_COL_INDEX_AMPL                          2
#define SH_COL_INDEX_PHASE_ANGLE                   3


#define DSM_STATE_HARMONIC_WIDTH_CHAN              94
#define DSM_STATE_HARMONIC_WIDTH_HORMANIC          145
#define DSM_STATE_HARMONIC_WIDTH_AMPL              145
#define DSM_STATE_HARMONIC_WIDTH_PHASE_ANGLE       145



// 状态序列- SMV映射表
//
#define DSM_MANUAL_SMV_MAP_WIDTH_CHAN            104
#define DSM_MANUAL_SMV_MAP_WIDTH_MAP_TO_SMV      215
#define DSM_MANUAL_SMV_MAP_WIDTH_TRANS_RATIO     210

////////////////////////短路故障///////////////////////////////////////
#define DSM_STATE_FAULT_CALC_WIDTH_CAPTION            180
#define DSM_STATE_FAULT_CALC_WIDTH_CONTENT            349

#define DSM_SFC_INDEX_FAULTTYPE                       0
#define DSM_SFC_INDEX_CALC_MODE_TYPE                  1
#define DSM_SFC_INDEX_DIRECTION                       2
#define DSM_SFC_INDEX_RATED_VOLTAGE                   3
#define DSM_SFC_INDEX_LOAD_CURRENT                    4
#define DSM_SFC_INDEX_LOAD_ANGLE                      5
#define DSM_SFC_INDEX_LINE_IMPEDANCE_Z1               6
#define DSM_SFC_INDEX_NEXT                            7

//
//故障设置
//
enum SFCRecordFuncType
{
	SFCUnknown = 0,
	SFCFaultType ,                                  ///< SMV类型
	SFCCalcModeType,
	SFCDirection,
	SFCRatedVoltage,
	SFCLoadCurrent,
	SFCLoadAngle,
	SFCLineImpedanceZ1,
	SFCLineCompensateK1,
	SFCSystemImpdanceZs,
	SFCSystemCompensateKs,
	SFCFaultVoltage,
	SFCFaultCurrent
};

// 提示区域
#define DSM_STATE_FAULT_CALC_RC      CRect(110, 332, 640, 444)     ///< 页面区域
#define DSM_STATE_FAULT_CALC_TIP     CRect(30, 15, 640, 90)
#if !SHENG_ZE
#define DSM_STATE_MAIN_SET_DLG		 CRect(110, 36, 640, 332)
#define DSM_STATE_FAULT_CALC_DI      CRect(110, 332, 640, 444)
#endif
#endif

//
//焦点列
//
enum SSSmvFocusColType
{
    SSSAmpl = 0,
    SSSPhase,
    SSSOther,
};

//
//状态序列记录项功能
//
enum SSSRecordFuncType
{
    SSSUnknown = 0,
    SSSDOSet,                                        ///< DO设置
    SSSChangeType ,                                  ///< 状态切换类型
    SSSDIChangeType,                                 ///< 开入量逻辑
    SSSDISet,                                        ///< DI设置
    SSSDelay,                                        ///< 切换时间
};






IMPLEMENT_DYNAMIC(CStateSetDlg, CBaseDlg)

CStateSetDlg::CStateSetDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateSetDlg::IDD, DSM_STR_STATE_SET_CAPTION, pParent)
    , m_pStatus(NULL)
    , m_nIndex(0)
#if CHUN_HUA
    , m_eShowReportType(SSShowSetReport)
#else
	, m_eShowReportType(SSShowNone)
#endif
    , m_pConfigStateSequ(NULL)
#if !SHENG_ZE
	, m_DIFlag(false)
	, m_flag(false)
#endif
{
    m_nMenuID = IDR_MENU_STATE_SET;
}

CStateSetDlg::~CStateSetDlg()
{
    m_statusMap.clear();
}

void CStateSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateSetDlg, CBaseDlg)
    ON_COMMAND_RANGE(ID_MENU_STATE_SET_STATE_START,   ID_MENU_STATE_SET_STATE_END, &CStateSetDlg::OnNaviMenuChangeState)
    ON_COMMAND_RANGE(ID_MENU_STATE_SET_CMD_MIN,   ID_MENU_STATE_SET_CMD_MAX, &CStateSetDlg::OnNaviMenuStateSet)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_STATE_SET_RET, &CStateSetDlg::OnStateSetItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_SET_RET, &CStateSetDlg::OnStateSetItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATE_SET_RET, &CStateSetDlg::OnStateSetItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_SMV_DATA_RET, &CStateSetDlg::OnStateSmvDataItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_SMV_DATA_RET,  &CStateSetDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_SMV_DATA_RET,  &CStateSetDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_SET_RET,  &CStateSetDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_SET_RET,  &CStateSetDlg::OnInplaceEditAction)
#if CHUN_HUA
	ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_SET_RET, &CStateSetDlg::OnDOItemOkClick)
#else
	ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_SET_RET, &CStateSetDlg::OnDOItemOkClick)
	ON_NOTIFY(NM_RETURN, IDC_SMV_CHAN_MAP_TREE,&CStateSetDlg::OnTreeBoxClick)


	///////////////////////////////////谐波计算///////////////////////////////////////
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_HARMONIC_RET, &CStateSetDlg::OnHarmonicItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATE_HARMONIC_RET, &CStateSetDlg::OnHarmonicItemSelectChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_HARMONIC_RET,  &CStateSetDlg::OnInplaceHarmonicEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_HARMONIC_RET,  &CStateSetDlg::OnInplaceHarmonicEditAction)
	ON_COMMAND_RANGE(ID_MENU_STATE_HARMONIC_F1, ID_MENU_STATE_HARMONIC_F4, &CStateSetDlg::OnStateHarmonicContextMenu)

	//////////////////////////////////////短路计算////////////////////////////////////
	ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATE_FAULT_CALC_RET, &CStateSetDlg::OnFaultCalcItemSelectChanged)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_FAULT_CALC_RET, &CStateSetDlg::OnFaultCalcItemValueChanged)
	ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_FAULT_CALC_RET, &CStateSetDlg::OnFaultCalcItemOkClick)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_FAULT_CALC_RET,  &CStateSetDlg::OnFaultCalcInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_FAULT_CALC_RET,  &CStateSetDlg::OnFaultCalcInplaceEditAction)
#endif
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CStateSetDlg 消息处理程序

BOOL CStateSetDlg::OnInitDialog()
{

    //
    //保存所有状态，动态添加到菜单 放在基类初始化函数前，这个要用于_modify对话框
    //
    m_pConfigStateSequ = CParamConfigStateSequence::getInstance();
    if (m_pConfigStateSequ == NULL)
    {
        return FALSE;
    }
#if !CHUN_HUA
	m_pConfigManualTest = CParamconfigManualTest::getInstance();
	if (NULL == m_pConfigManualTest)
	{
		return FALSE;
	}

#endif

    int nCount = ID_MENU_STATE_SET_STATE_END - ID_MENU_STATE_SET_STATE_START + 1;
    ASSERT(m_pConfigStateSequ->GetStatusCount() <= nCount);
    int nIndex = 0;
    CStatus* pStatus = m_pConfigStateSequ->FirstStatus();
    while(pStatus && nIndex < nCount)
    {
        m_statusMap.insert(std::pair<UINT, CStatus*>(ID_MENU_STATE_SET_STATE_START + nIndex, pStatus));
        pStatus = m_pConfigStateSequ->NextStatus(pStatus);
        ++ nIndex;
    }
    //
    //
    //


    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }

    if(pData->m_eAction == PageBack)
    {
        //
        //从其他页面恢复过来
        //
        _restorePage();
    }
    else
    {
        //
        //从状态列表中进入
        //
       CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
        if (pPageData == NULL || 
            pPageData->m_nPageID != CStateSequenceDlg::IDD || 
            pPageData->m_pPageDataExtra == NULL)
        {
            //
            //参数配置进入（goose编辑或查看）
            //
            return FALSE;
        }

        CStateSequenceDataExtra* pDataExtra = dynamic_cast<CStateSequenceDataExtra*>(pPageData->m_pPageDataExtra);
        if (pDataExtra == NULL || pDataExtra->m_pStatus == NULL)
        {
            return FALSE;
        }

        m_pStatus = pDataExtra->m_pStatus;
        m_nIndex = pDataExtra->m_nFocusRowIndex;
        
        if (m_pStatus == NULL)
        {
            return FALSE;
        }

       // m_eShowReportType = SSShowSetReport;
        _showSetReport(TRUE);
    }

    CString strTitle = _T("");
    strTitle.Format(_T("%s%d%s"), 
        dsm_stringMgr::Get(DSM_STR_STATE_SET_STATE),
        m_nIndex + 1,
        dsm_stringMgr::Get(DSM_STR_STATE_SET_SET)
        );
    CStatusMgr::SetTitle(strTitle);
    
    _highlightF2();
	SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CStateSetDlg::_createSetReport()
{
    if (m_pStatus == NULL)
    {
        return;
    }
#if SHENG_ZE
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStateSetReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_SET_RET);
#else
	CRect rcReport(DSM_STATE_MAIN_SET_DLG);
	DSM_ParentToChild(this, rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = 7;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlStateSetReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_STATE_SET_RET);
#endif
    // 设置字体
    m_ctlStateSetReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlStateSetReport.PostInitTemplateStyle();
    // 插入列元素
    m_ctlStateSetReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STATE_SET_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlStateSetReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STATE_SET_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlStateSetReport.SetRowSelectionVisible(FALSE);
    m_ctlStateSetReport.SetRowFocusEnable(TRUE);
    m_ctlStateSetReport.SetSubItemFocusEnable(TRUE);
    m_ctlStateSetReport.SetFreezeColumnsCount(1);
    m_ctlStateSetReport.SetFreezeSelectionVisible(TRUE);
    m_ctlStateSetReport.SetFocusedColIndex(1);
    m_ctlStateSetReport.SetFocus();
    
    CString strCaption = _T("");
    CString strContent = _T("");

    for(int i = 0; i < DSM_STATE_SET_DO_COUNT; ++i)
    {
        strCaption.Format(_T("DO%d"), i+1);
        bool bDO = m_pStatus->GetDOValue((LPCTSTR)strCaption);
        if (bDO)
        {
            strContent = dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON);
        }
        else
        {
            strContent = dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF);
        }
        m_ctlStateSetReport.InsertRecordRow(DSM_STATE_SET_DO_START_INDEX + i, new CDsmStateSetRecord(strCaption, strContent));
        m_ctlStateSetReport.SetRowItemData(DSM_STATE_SET_DO_START_INDEX + i, 0, SSSDOSet);
    }
    
    //状态切换类型
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SET_CHANGE_TYPE);
    
    eSwitchType switchType = m_pStatus->GetSwitchType();
    switch(switchType)
    {
    case SWITCH_TYPE_TIME:
        {
            strContent = dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_TIME_CHANGE);
        }
        break;
    case SWITCH_TYPE_DI:
        {
            strContent = dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_DI_CHANGE);
        }
        break;
    case SWITCH_TYPE_MANUAL:
        {
            strContent = dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_MANUAL_CHANGE);
        }
        break;
    default:
        break;
    }
    m_ctlStateSetReport.InsertRecordRow(DSM_STATE_SET_CHANGE_TYPE_INDEX, new CDsmStateSetRecord(strCaption, strContent));
    m_ctlStateSetReport.SetRowItemData(DSM_STATE_SET_CHANGE_TYPE_INDEX, 0, SSSChangeType);
    
    CELRecordItemEditOptions* pOptions = m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_CHANGE_TYPE_INDEX)->GetItem(1)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_TIME_CHANGE), SWITCH_TYPE_TIME);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_DI_CHANGE), SWITCH_TYPE_DI);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_MANUAL_CHANGE), SWITCH_TYPE_MANUAL);

#if !SHENG_ZE
	_createSetReportDI();
#endif
    _updateOnSwitchTypeChange();

    m_ctlStateSetReport.SetFocusedSelectionRowIndex(0);
    m_ctlStateSetReport.SetFocus();
    m_ctlStateSetReport.ShowWindow(SW_SHOW);

#if SHENG_ZE
	m_ctlStateSetReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif

}



#if !SHENG_ZE
void CStateSetDlg::_createSetReportDI()
{
	CRect rcReport(DSM_STATE_FAULT_CALC_DI);
	DSM_ParentToChild(this, rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = 2;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlStateSetReportDI.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_STATE_SET_RET);

	// 设置字体
	m_ctlStateSetReportDI.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlStateSetReportDI.PostInitTemplateStyle();
	// 插入列元素
	m_ctlStateSetReportDI.InsertColumn(0,
		_T(""),
		EL_LEFT,
		DSM_STATE_SET_WIDTH_CAPTION);
	m_ctlStateSetReportDI.InsertColumn(1,
		_T(""),
		EL_LEFT,
		DSM_STATE_SET_WIDTH_DI);
	m_ctlStateSetReportDI.InsertColumn(2,
		_T(""),
		EL_LEFT,
		DSM_STATE_SET_WIDTH_DI+1);
	m_ctlStateSetReportDI.InsertColumn(3,
		_T(""),
		EL_LEFT,
		DSM_STATE_SET_WIDTH_DI);
	m_ctlStateSetReportDI.InsertColumn(4,
		_T(""),
		EL_LEFT,
		DSM_STATE_SET_WIDTH_DI+1);

	// 设置选择焦点样式
	m_ctlStateSetReportDI.SetRowSelectionVisible(FALSE);
	m_ctlStateSetReportDI.SetRowFocusEnable(TRUE);
	m_ctlStateSetReportDI.SetSubItemFocusEnable(TRUE);
	m_ctlStateSetReportDI.SetFreezeColumnsCount(1);
	m_ctlStateSetReportDI.SetFreezeSelectionVisible(TRUE);
	m_ctlStateSetReportDI.SetFocusedColIndex(1);

	m_ctlStateSetReportDI.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

	CString strDI[2][4] =
	{
		{_T("DI1"),_T("DI2"),_T("DI3"),_T("DI4")},
		{_T("DI5"),_T("DI6"),_T("DI7"),_T("DI8")}
	};
	for (int i = 0; i < 2; i++)
	{
		m_ctlStateSetReportDI.InsertRecordRow( i, new CDsmStateSetRecordDI(_T(""),strDI[i][0], strDI[i][1],strDI[i][2], strDI[i][3]));
		m_ctlStateSetReportDI.SetRowItemData( i, 0, SSSDISet);
		CELRecordItem* pItem = NULL;
		for (int j = 0; j < 4; j++) {
			pItem = m_ctlStateSetReportDI.GetRecordRow(i)->GetItem(j+1);
			pItem->HasCheckbox(TRUE);
			pItem->SetEditable(TRUE);
			if (m_pStatus->IsDIChecked((LPCTSTR)strDI[i][j]))
			{
				pItem->SetChecked(TRUE);
			}
		}
	}

	m_ctlStateSetReportDI.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlStateSetReportDI.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlStateSetReportDI.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlStateSetReportDI.GetColumnExAttr(4)->GetEditOptions()->SetAllowEdit(TRUE);
	for (int i = 0; i < m_ctlStateSetReportDI.GetRecordRowCount(); ++i)
	{
		m_ctlStateSetReportDI.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
		m_ctlStateSetReportDI.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
		m_ctlStateSetReportDI.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
		m_ctlStateSetReportDI.GetRecordRow(i)->GetItem(4)->SetEditable(TRUE);
	}
	m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
	m_DIFlag = false;
}
#endif

void CStateSetDlg::_createSMVDataReport()
{
    if (m_pStatus == NULL)
    {
        return;
    }
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStateSMVDataReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_SMV_DATA_RET);

    // 设置字体
    m_ctlStateSMVDataReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlStateSMVDataReport.SetHeaderFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlStateSMVDataReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlStateSMVDataReport.InsertColumn(SSS_COL_INDEX_CHAN, L"  " + dsm_stringMgr::Get(DSM_STR_STATE_SET_CHAN), EL_CENTER, DSM_STATE_DATA_WIDTH_CHAN);
    m_ctlStateSMVDataReport.InsertColumn(SSS_COL_INDEX_AMPL, dsm_stringMgr::Get(DSM_STR_STATE_SET_AMPL), EL_LEFT, DSM_STATE_DATA_WIDTH_AMPL);
    m_ctlStateSMVDataReport.InsertColumn(SSS_COL_INDEX_PHASE, dsm_stringMgr::Get(DSM_STR_STATE_SET_PHASE), EL_LEFT, DSM_STATE_DATA_WIDTH_PHASE);
    m_ctlStateSMVDataReport.InsertColumn(SSS_COL_INDEX_FREQUENCE, dsm_stringMgr::Get(DSM_STR_STATE_SET_FREQUENCE), EL_LEFT, DSM_STATE_DATA_WIDTH_FREQUENCE);

    // 设置选择焦点样式
    m_ctlStateSMVDataReport.SetRowSelectionVisible(FALSE);
    m_ctlStateSMVDataReport.SetRowFocusEnable(TRUE);
    m_ctlStateSMVDataReport.SetSubItemFocusEnable(TRUE);
    m_ctlStateSMVDataReport.SetFocusedColIndex(1);
    m_ctlStateSMVDataReport.SetFreezeSelectionVisible(TRUE);
    m_ctlStateSMVDataReport.SetFreezeColumnsCount(1);

    CString strCaption = _T("");
    CString strContent = _T("");

    //状态切换类型
    
    //手动试验参数配置管理器
    
    _updateSmvDataReport();
    
    m_ctlStateSMVDataReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlStateSMVDataReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlStateSMVDataReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);


    m_ctlStateSMVDataReport.SetFocus();
    m_ctlStateSMVDataReport.ShowWindow(SW_SHOW);
}

void CStateSetDlg::_updateOnSwitchTypeChange()
{   

    //m_ctlStateSetReport.SetRedraw(FALSE);
    while(m_ctlStateSetReport.GetRecordRowCount() > DSM_STATE_SET_DI_CHANGE_TYPE_INDEX)
    {
        m_ctlStateSetReport.DeleteRecordRow(DSM_STATE_SET_DI_CHANGE_TYPE_INDEX);
    }

    CString strCaption = _T("");
    CString strContent = _T("");
#if !SHENG_ZE
	m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
	m_DIFlag = false;
#endif
    eSwitchType switchType = m_pStatus->GetSwitchType();
    //
    if (switchType == SWITCH_TYPE_TIME)
    {
        //限时切换

        strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SET_CHANGE_DELAY);
        strContent.Format(_T("%d"), m_pStatus->GetDelay());
        
        m_ctlStateSetReport.InsertRecordRow(DSM_STATE_SET_CHANGE_DELAY_INDEX, new CDsmStateSetRecord(strCaption, strContent));
        
        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_CHANGE_DELAY_INDEX)->GetItem(1));
        pItem->SetFormatString(_T("%s ms"));
        m_ctlStateSetReport.SetRowItemData(DSM_STATE_SET_CHANGE_DELAY_INDEX, 0, SSSDelay);
    }
    else if (switchType == SWITCH_TYPE_DI)
    {
        //DI切换
        strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CHANGE_TYPE);
        strContent = _T("");
        eLogicalOperator logicOperator = m_pStatus->GetLogicalOperator();
        switch(logicOperator)
        {
        case LOGICAL_AND:
            {
                strContent = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_AND);
            }
            break;
        case LOGICAL_OR:
            {
                strContent = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_OR);
            }
            break;
        }
        m_ctlStateSetReport.InsertRecordRow(DSM_STATE_SET_DI_CHANGE_TYPE_INDEX, new CDsmStateSetRecord(strCaption, strContent));
        m_ctlStateSetReport.SetRowItemData(DSM_STATE_SET_DI_CHANGE_TYPE_INDEX, 0, SSSDIChangeType);
        CELRecordItemEditOptions* pOptions = m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_DI_CHANGE_TYPE_INDEX)->GetItem(1)->GetEditOptions(NULL);
        pOptions->AddComboButton();
        pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_AND), LOGICAL_AND);
        pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_OR), LOGICAL_OR);

#if SHENG_ZE
		CString strDI = _T("");
		for (int i = 0; i < 8; ++i)
		{
			strContent.Format(_T("DI%d"), i + 1);
			m_ctlStateSetReport.InsertRecordRow(DSM_STATE_SET_DI_START_INDEX + i, new CDsmStateSetRecord(_T(""), strContent));
			m_ctlStateSetReport.SetRowItemData(DSM_STATE_SET_DI_START_INDEX + i, 0, SSSDISet);
			CELRecordItem* pItem = m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_DI_START_INDEX + i)->GetItem(1);
			pItem->HasCheckbox(TRUE);
			pItem->SetEditable(TRUE);
			if (m_pStatus->IsDIChecked((LPCTSTR)strContent))
			{
				pItem->SetChecked(TRUE);
			}
		}
#else
		m_ctlStateSetReportDI.ShowWindow(SW_SHOW);
		m_DIFlag = true;
#endif
    }

    m_ctlStateSetReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for (int i = DSM_STATE_SET_CHANGE_TYPE_INDEX; i < m_ctlStateSetReport.GetRecordRowCount(); ++i)
    {
        m_ctlStateSetReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
    //m_ctlStateSetReport.SetRedraw(TRUE);

    //修改菜单
    _resetMenu();

}

void CStateSetDlg::_updateStateSetReport()
{
    ASSERT(m_pStatus);
    if (!m_ctlStateSetReport.GetSafeHwnd())
    {
        return;
    }
    if (m_pStatus == NULL)
    {
        return;
    }
    //DO
    CString strCaption = _T("");
    CString strContent = _T("");
    CELRecordItemText* pItem = NULL;
    for(int i = 0; i < DSM_STATE_SET_DO_COUNT; ++i)
    {
        strCaption.Format(_T("DO%d"), i+1);
        bool bDO = m_pStatus->GetDOValue((LPCTSTR)strCaption);
        pItem = (CELRecordItemText*)m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_DO_START_INDEX + i)->GetItem(1);
        if (pItem)
        {
            CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
            if (pOptions)
            {
                CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(bDO);
                pItem->SetValue(pItemCon->GetConstraintCaption());
            }
        }
    }
    //

    //状态切换类型
    eSwitchType switchType = m_pStatus->GetSwitchType();
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_CHANGE_TYPE_INDEX)->GetItem(1));
    if (pItem)
    {
        CELRecordItemConstraint* pItemCon =  pItem->GetEditOptions(NULL)->FindConstraint(switchType);
        if (pItemCon)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
        else
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_UNKNOWN));
        }
        m_ctlStateSetReport.RedrawRowItem(DSM_STATE_SET_CHANGE_TYPE_INDEX ,1);
    }
    //
    _updateOnSwitchTypeChange();

    m_ctlStateSetReport.RedrawRowItems(0 , m_ctlStateSetReport.GetRecordRowCount());

}
void CStateSetDlg::_updateSmvDataReport()
{
    ASSERT(m_pStatus);
    if (!m_ctlStateSMVDataReport.GetSafeHwnd())
    {
        return;
    }
    if (m_pStatus == NULL)
    {
        return;
    }
    
    int nPreSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();

    m_ctlStateSMVDataReport.DeleteAllRecordRow();

    int nIndex = 0;
    // 遍历虚拟通道
    CSMVSendVirtualChannel* pVirChannel = m_pStatus->FirstSMVSendVirtualChannel();
    while(pVirChannel)
    {
        m_ctlStateSMVDataReport.InsertRecordRow(nIndex, new CDsmStateSmvDataRecord(pVirChannel));
        m_ctlStateSMVDataReport.SetRecordRowData(nIndex, (DWORD_PTR)pVirChannel);
        pVirChannel = m_pStatus->NextSMVSendVirtualChannel(pVirChannel);
        ++nIndex;
    }
    
    //
    //处理选中行
    //
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    

    if ( nPreSelRow >=0 && nPreSelRow < m_ctlStateSMVDataReport.GetRecordRowCount())
    {
        m_ctlStateSMVDataReport.SetFocusedSelectionRowIndex(nPreSelRow);
        m_ctlStateSMVDataReport.EnsureVisible(nPreSelRow, FALSE);
    }
    else if (m_ctlStateSMVDataReport.GetRecordRowCount() > 0)
    {
        if ((pData->m_eAction == PageEnter) ||
            (pData->m_eAction == PageBack && pData->m_nCtrlID != m_ctlStateSMVDataReport.GetDlgCtrlID()))
        {
            m_ctlStateSMVDataReport.SetFocusedSelectionRowIndex(0);
        }
    }

    m_ctlStateSMVDataReport.RedrawRowItems(0, m_ctlStateSMVDataReport.GetRecordRowCount()-1);

}

void CStateSetDlg::_showSetReport(bool bIsHide)
{
    if(m_ctlStateSetReport.GetSafeHwnd())
    {
        m_ctlStateSetReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createSetReport();
    }
#if !CHUN_HUA
	if (bIsHide)
	{
		_hideReport();
	}

#endif

   /* if(m_ctlStateSMVDataReport.GetSafeHwnd())
    {
        m_ctlStateSMVDataReport.ShowWindow(SW_HIDE);
    }*/

    m_eShowReportType = SSShowSetReport;
    _resetMenu();
    m_ctlStateSetReport.SetFocus();
}

void CStateSetDlg::_highlightF2(BOOL bHighLight/* = TRUE*/)
{
#ifdef ZY_PRJ
	CUINTStatusMap::iterator iter;
    for (iter = m_statusMap.begin(); iter != m_statusMap.end(); ++iter)
    {
        if (m_pStatus == iter->second)
        {
            CNaviBarMgr::GetInstance()->HighlightMenuItem(1, iter->first - ID_MENU_STATE_SET_STATE_START, bHighLight);
        }
    }
#endif
}

void CStateSetDlg::_showSmvDataReport(bool bIsHide)
{
    if(m_ctlStateSMVDataReport.GetSafeHwnd())
    {
        m_ctlStateSMVDataReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createSMVDataReport();
    }
#if !CHUN_HUA
	if (bIsHide)
	{
		_hideReport();
	}

#endif

    if(m_ctlStateSetReport.GetSafeHwnd())
    {
        m_ctlStateSetReport.ShowWindow(SW_HIDE);
    }

    m_eShowReportType = SSShowSmvDataReport;
    _resetMenu();
    m_ctlStateSMVDataReport.SetFocus();
}

void CStateSetDlg::_changeStateTo(int nStateIndex)
{
    if (nStateIndex < m_pConfigStateSequ->GetStatusCount())
    {
        CString strContent = _T("");
        int nIndex = 0;
        CStatus* pStatus = m_pConfigStateSequ->FirstStatus();
        while(pStatus)
        {
            if (nIndex == nStateIndex)
            {
                m_pStatus = pStatus;
                //状态切换类型
                eSwitchType switchType = m_pStatus->GetSwitchType();
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_CHANGE_TYPE_INDEX)->GetItem(1));
                if (pItem)
                {
                    CELRecordItemConstraint* pItemCon =  pItem->GetEditOptions(NULL)->FindConstraint(switchType);
                    if (pItemCon)
                    {
                        pItem->SetValue(pItemCon->GetConstraintCaption());
                    }
                    else
                    {
                        pItem->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_UNKNOWN));
                    }
                    m_ctlStateSetReport.RedrawRowItem(DSM_STATE_SET_CHANGE_TYPE_INDEX ,1);
                }

                _updateOnSwitchTypeChange();
                _updateSmvDataReport();

                m_nIndex = nIndex;
                CString strTitle = _T("");
                strTitle.Format(_T("%s%d%s"), 
                    dsm_stringMgr::Get(DSM_STR_STATE_SET_STATE),
                    m_nIndex + 1,
                    dsm_stringMgr::Get(DSM_STR_STATE_SET_SET)
                    );
                CStatusMgr::SetTitle(strTitle);

                break;
            }
            pStatus = m_pConfigStateSequ->NextStatus(pStatus);
            ++nIndex;
        }
    }
}

void CStateSetDlg::OnNaviMenuChangeState(UINT nID)
{
    CUINTStatusMap::iterator iter;
    iter = m_statusMap.find(nID);
    if (iter != m_statusMap.end())
    {
        _highlightF2(FALSE);
        m_pStatus = iter->second;

        //状态切换类型
        eSwitchType switchType = m_pStatus->GetSwitchType();
        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSetReport.GetRecordRow(DSM_STATE_SET_CHANGE_TYPE_INDEX)->GetItem(1));
        if (pItem)
        {
            CELRecordItemConstraint* pItemCon =  pItem->GetEditOptions(NULL)->FindConstraint(switchType);
            if (pItemCon)
            {
                pItem->SetValue(pItemCon->GetConstraintCaption());
            }
            else
            {
                pItem->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_UNKNOWN));
            }
            m_ctlStateSetReport.RedrawRowItem(DSM_STATE_SET_CHANGE_TYPE_INDEX ,1);
        }

        _updateStateSetReport();
        _updateSmvDataReport();

        CString strTitle = _T("");
        strTitle.Format(_T("%s%d%s"), 
            dsm_stringMgr::Get(DSM_STR_STATE_SET_STATE),
            nID - ID_MENU_STATE_SET_STATE_START + 1,
            dsm_stringMgr::Get(DSM_STR_STATE_SET_SET)
            );
        CStatusMgr::SetTitle(strTitle);

        _highlightF2();
    }
}
void CStateSetDlg::OnNaviMenuStateSet(UINT nID)
{
    switch (nID)
    {
#ifdef ZY_PRJ 
   case ID_MENU_STATE_SET_F1:
        {
            //设置数据切换
            if (m_eShowReportType == SSShowSetReport)
            {
                m_eShowReportType = SSShowSmvDataReport;
                _showSmvDataReport();
            }
            else
            {
                m_eShowReportType = SSShowSetReport;
                _showSetReport();
            }
        }
        break;
#else
   case ID_MENU_STATE_SET_F1:
	   {
	    
		  if (m_eShowReportType != SSShowSetReport)
		  {
				_showSetReport(TRUE);
#if !SHENG_ZE
				m_ctlStateSetReportDI.ShowWindow(SW_SHOW);
				m_DIFlag = true;
#endif
		  }
	   }
	   break;
	case ID_MENU_STATE_SET_F2:
		{
			if (m_eShowReportType != SSShowSmvDataReport)
			{
				_showSmvDataReport(TRUE);
#if !SHENG_ZE
				m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
				m_DIFlag = false;
#endif
			}
			
		}		
		break;
#endif
    case ID_MENU_STATE_EQUAL_AMPL:
        {
            //等幅值
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->EqualAmplitude(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_RATED_AMPL:
        {
            //额定值
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->RatedAmplitude(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_THREE_PHASE_BALANCE:
        {
            //三相平衡
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->ThreePhaseBalance(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_EQUAL_PHASE_ANGLE:
        {
            //等相角
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->EqualPhaseAngle(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_PHASE_ANGLE_POS_SEQ:
        {
            //相角正序
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->PhaseAnglePositiveSequence(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_PHASE_ANGLE_REV_SEQ:
        {
            //相角反序
            int nSelRow = m_ctlStateSMVDataReport.GetFocusedRowIndex();
            if (nSelRow >= 0 )
            {
                CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(nSelRow);
                if (pVirChannel)
                {
                    m_pStatus->PhaseAngleReverseSequence(pVirChannel);
                    _updateSmvDataReport();
                }
            }
        }
        break;
    case ID_MENU_STATE_STATUS_WORD_QUALITY:
        {
            if (m_eShowReportType == SSShowSmvDataReport)
            {
                //状态字 通道品质
                CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CTRL_LIST_DLG);
            }
        }
        break;
    case ID_MENU_STATE_EQUAL_SMV_MAP:
        {
            if (m_eShowReportType == SSShowSmvDataReport)
            {
                //映射表
                CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_MAP_DLG);
            }
        }
        break;

    case ID_MENU_STATE_SET_F3:
        {
			//故障计算
			if (m_eShowReportType != SSShowFaultComensateReport)
			{
				//映射表
				_showFaultCalcReport(TRUE);
			}
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
        }
        break;
    case ID_MENU_STATE_SET_F4:
        {
			//谐波设置
			if (m_eShowReportType != SSShowHarmonicReport)
			{

				_showHarmonicReport(TRUE);
			}
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
        }
        break;
	case ID_MENU_STATE_SET_F5:
		{
			//进入状态字或通信品质编辑页面
			CParamConfigSMVSend* m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
			if (m_pParamConfigSMVSend == NULL)
			{
				return ;
			}

			enumFrameType eFrameType = m_pParamConfigSMVSend->GetSMVType();
			if (eFrameType == FRAME_TYPE_NET_9_1 ||
				eFrameType == FRAME_TYPE_FT3_NANRUI ||
				eFrameType == FRAME_TYPE_FT3_GUOWANG )
			{
				if (m_pParamConfigSMVSend->GetSelectedIEC9_1CbCount() > 0 ||	m_pParamConfigSMVSend->GetSelectedFt3CbCount()> 0||m_pParamConfigSMVSend->GetSelectedFt3ExpandCbCount()>0)
				{
					CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_STATUS_WORD_DLG);
				}
				else
				{
					CMsgDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, 0, this);

					msgdlg._SetMsg(_T("提示：还没有添加SMV发送控制块！"));
					msgdlg._SetBitmapDisp(false);
					msgdlg._SetTextRC( CRect(40, 51, 314, 71));
					msgdlg._ResetF5Text(_T(""));
					msgdlg._ResetF6Text(_T("确定"));
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

			}
			else if (eFrameType == FRAME_TYPE_NET_9_2)
			{
				if (m_pParamConfigSMVSend->GetSelectedIEC9_2CbCount()!=0)
				{
					CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_QUALITY_DLG);
				}
				else
				{
					CMsgDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, 0, this);

					msgdlg._SetMsg(_T("提示：还没有添加SMV发送控制块！"));
					msgdlg._SetBitmapDisp(false);
					msgdlg._SetTextRC( CRect(40, 51, 314, 71));
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

			}
		}
		break;
    case ID_MENU_STATE_SET_F6:
        {
			if (m_eShowReportType != SSShowSmvChanMap)
			{
				//映射表
				_showSmvChanMapTree(TRUE);//    CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_MAP_DLG);

			} 
        }
#if !SHENG_ZE
		m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
		m_DIFlag = false;
#endif
        break;

    default:
        break;
    }
}

/*
 *
 *  @Note:  新增m_ctlStateSetReportDI控件，与m_ctlStateSetReport控件公用一个消息处理函数OnStateSetItemValueChanged
 *          函数中根据控件句柄来作区分处理。
 */
void CStateSetDlg::OnStateSetItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
	}

	if (pNotifyStruct->hwndFrom == m_ctlStateSetReport.GetSafeHwnd()) {
		SSSRecordFuncType nRecordType = (SSSRecordFuncType)m_ctlStateSetReport.GetRowItemData(pItemNotify->rowIndex, 0);
		switch(nRecordType)
		{
		case SSSDelay:
			{
				CString strValue = pItem->GetValue();
				int nDelay = _ttoi(strValue);
				if (nDelay > 0)
				{
					m_pStatus->SetDelay(nDelay);
				}
				strValue.Format(_T("%d"), m_pStatus->GetDelay());
				pItem->SetValue(strValue);
				*pResult = TRUE;
			}
			break;
		default:
			break;
		}	
	}
	else if (pNotifyStruct->hwndFrom == m_ctlStateSetReportDI.GetSafeHwnd()) 
	{
		SSSRecordFuncType nRecordTypeDI = (SSSRecordFuncType)m_ctlStateSetReportDI.GetRowItemData(pItemNotify->rowIndex, 0);
		switch(nRecordTypeDI)
		{
		case SSSDISet:
			{
				CString strValue = pItem->GetValue();
				m_pStatus->SetDIChecked((LPCTSTR)strValue, (bool)pItem->IsChecked());
				*pResult = TRUE;
			}
			break;
		default:
			break;
		}
	}
	else {
		//do nothing
	}    
}

void CStateSetDlg::OnStateSetItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

	if (pNotifyStruct->hwndFrom != m_ctlStateSetReport.GetSafeHwnd()) {
		RETAILMSG(1, (_T("[%s] Found Error!!"), _T(__FUNCTION__)));
		return ;
	}

    SSSRecordFuncType nRecordType = (SSSRecordFuncType)m_ctlStateSetReport.GetRowItemData(pItemNotify->rowIndex, 0);
    
    switch(nRecordType)
    {
    //case  SSSDOSet:
    //    {
    //        CELRecordItemText* pItemCaption = (CELRecordItemText*)m_ctlStateSetReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(0);
    //        if (pItemCaption)
    //        {
    //            CString strDO = pItemCaption->GetValue();
    //            m_pStatus->SetDOValue((LPCTSTR)strDO, (bool)pItemNotify->pConstraint->GetConstraintData());
    //        }
    //        *pResult = TRUE;
    //    }
    //    break;
    case SSSChangeType:
        {
            m_pStatus->SetSwitchType((eSwitchType)pItemNotify->pConstraint->GetConstraintData());
            _updateOnSwitchTypeChange();
            *pResult = TRUE;
        }
        break;
    case SSSDIChangeType:
        {
            m_pStatus->SetLogicalOperator((eLogicalOperator)pItemNotify->pConstraint->GetConstraintData());
            *pResult = TRUE;
        } 
        break;
    default:
        break;
    }
}

void CStateSetDlg::OnDOItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    int nRowIndex = m_ctlStateSetReport.GetFocusedRowIndex();
    int nColIndex = m_ctlStateSetReport.GetFocusedColIndex();
    if (nRowIndex <0 || nColIndex < 0)
    {
        return;
    }
#if !SHENG_ZE
	if (true == m_DIFlag && 0 == m_ctlStateSetReportDI.GetFocusedRowIndex() && 0 == m_ctlStateSetReport.GetFocusedRowIndex() && false == m_flag)
	{
		return;
	}
	if (true == m_DIFlag && 1 == m_ctlStateSetReportDI.GetFocusedRowIndex() && 0 == m_ctlStateSetReport.GetFocusedRowIndex() && false == m_flag)
	{
		return;
	}
#endif

    SSSRecordFuncType nRecordType = (SSSRecordFuncType)m_ctlStateSetReport.GetRowItemData(nRowIndex, 0);
    switch(nRecordType)
    {
    case  SSSDOSet:
        {
            CELRecordItemText* pItemCaption = (CELRecordItemText*)m_ctlStateSetReport.GetRecordRow(nRowIndex)->GetItem(0);
            CELRecordItemText* pItemContent = (CELRecordItemText*)m_ctlStateSetReport.GetRecordRow(nRowIndex)->GetItem(1);
            if (pItemCaption)
            {
                CString strDO = pItemCaption->GetValue();
                bool bDO = m_pStatus->GetDOValue((LPCTSTR)strDO);
                bDO = !bDO;
                m_pStatus->SetDOValue((LPCTSTR)strDO, bDO);

                if (bDO)
                {
                    pItemContent->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_POINT_ON));
                }
                else
                {
                    pItemContent->SetValue(dsm_stringMgr::Get(DSM_STR_STATE_POINT_OFF));
                }
                m_ctlStateSetReport.RedrawRowItem(nRowIndex, nColIndex);
            }
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}
void CStateSetDlg::OnStateSmvDataItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pVirChannel == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;

    CString strValue = pItem->GetValue();

    double dValue = 0.0f;
    bool bConvert = false;
    bConvert = DSM_Str2Double(strValue, dValue);

    switch(pItemNotify->colIndex)
    {
    case SSS_COL_INDEX_AMPL:
        {  
            CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
            if (NULL == pParamConfigSMVSend)
            {
                return ;
            }

            CString strChanName = pVirChannel->GetName().c_str();
            if (bConvert)
            {
                if (strChanName.GetLength() > 0)
                {
                    //改变幅值，步长均可编辑
                    if (strChanName[0] == _T('U'))
                    {
                        if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
                        {
                            pVirChannel->SetAmplitude(dValue*1000.0f);
                        }
                        else
                        {
                            pVirChannel->SetAmplitude(dValue);
                        }
                    }
                    else
                    {
                        pVirChannel->SetAmplitude(dValue);
                    }
                }
            }

            dValue = pVirChannel->GetAmplitude();
            if (strChanName.GetLength() > 0)
            {
                //改变幅值，步长均可编辑
                if (strChanName[0] == _T('U'))
                {
                    if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
                    {
                        strValue.Format(_T("%.03lf"), dValue/1000.0f);
                    }
                    else
                    {
                        strValue.Format(_T("%.03lf"), dValue);
                    }
                }
                else
                {
                    strValue.Format(_T("%.03lf"), dValue);
                }
            }
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case SSS_COL_INDEX_PHASE:
        {
            if (bConvert)
            {
                pVirChannel->SetPhaseAngle(dValue);
            }
            dValue = pVirChannel->GetPhaseAngle();
            strValue.Format(_T("%.03lf"), dValue);
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case SSS_COL_INDEX_FREQUENCE:
        {
            if (bConvert)
            {
                pVirChannel->SetFrequency(dValue);
            }
            dValue = pVirChannel->GetFrequency();
            strValue.Format(_T("%.03lf"), dValue);
            pItem->SetValue(strValue);
            *pResult = TRUE;

            //
            //频率全部修改
            //
            if (bConvert)
            {
                for (int i = 0; i < m_ctlStateSMVDataReport.GetRecordRowCount(); ++i)
                {
                    pVirChannel = (CSMVSendVirtualChannel*)m_ctlStateSMVDataReport.GetRecordRowData(i);
                    if (pVirChannel)
                    {
                        pVirChannel->SetFrequency(dValue);
                    }
                    dValue = pVirChannel->GetFrequency();
                    strValue.Format(_T("%.03lf"), dValue);

                    pItem = (CELRecordItemText*)m_ctlStateSMVDataReport.GetRecordRow(i)->GetItem(SSS_COL_INDEX_FREQUENCE);
                    if (pItem)
                    {
                        pItem->SetValue(strValue);
                        m_ctlStateSMVDataReport.RedrawRowItem(i, SSS_COL_INDEX_FREQUENCE);
                    }
                }
                *pResult = TRUE;
            }
        }
        break;
    default:
        break;
    }
}
void CStateSetDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        if (m_eShowReportType == SSShowSmvDataReport)
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlStateSMVDataReport.GetInplaceEdit());
        }
        else 
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlStateSetReport.GetInplaceEdit());
        }
        *pResult = TRUE;
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}
void CStateSetDlg::SetStatus(int nIndex, CStatus* pStatus)
{
    if (nIndex < 0 || pStatus == NULL)
    {
        return;
    }
    m_nIndex = nIndex;
    m_pStatus = pStatus;
}

void CStateSetDlg::_modifyMenu()
{
    __super::_modifyMenu();

	/*m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_SEL), NULL);*/

#ifdef ZY_PRJ

    //修改F1
    CNaviMenu* pSubMenu = m_menuMain.GetSubMenu(1);
    if (!pSubMenu)
    {
        //
        //已经加载过控制块，不再重新加载
        //
        CMenu  menu;
        VERIFY(menu.CreatePopupMenu());
        CUINTStatusMap::iterator iter;
        CString strText = _T("");
        int nIndex = 0;
        for (iter = m_statusMap.begin(); iter != m_statusMap.end(); ++iter)
        {
            strText.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_STATE_SET_STATE), nIndex + 1);
            menu.AppendMenu(MF_STRING, iter->first, strText);
            ++nIndex;
        }
        pSubMenu = new CNaviMenu;
        pSubMenu->LoadMenu(&menu);
        menu.DestroyMenu();
        m_menuMain.SetMenuSubItem(1, pSubMenu);
    }
#endif
	//smv类型
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return;
	}

	if (pParamConfigSMVSend->GetSMVType() == FRAME_TYPE_NET_9_2)
	{

		m_menuMain.SetMenuItemCaption(4,dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_QUALITY));

	}



}

void CStateSetDlg::_resetMenu()
{
#ifdef ZY_PRJ
    if (m_eShowReportType == SSShowSetReport)
    {
        //
        //F1
        //
        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_STATE_STATEDATA_STATE), NULL, NULL);
        eSwitchType switchType = m_pStatus->GetSwitchType();
        //
        //F4 F5
        //
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, _T(""));
        //
        //F6
        //
        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(5, NULL);
        //修改菜单
        if (switchType == SWITCH_TYPE_DI)
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_MAP));
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(5, TRUE);
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_STATE_SET_EMPTY));
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(5, FALSE);
        }

    }
    else if(m_eShowReportType == SSShowSmvDataReport)
    {
        //
        //F1
        //
        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_STATE_STATEDATA_DATA), NULL, NULL);
        //
        //F4F5
        //
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_STATE_FAULT_CALC));
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_STATE_HARMONIC_SET));

        //
        //F6
        //
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_STATE_SET_EXTEND));

        
        int nSelCol = m_ctlStateSMVDataReport.GetFocusedColIndex();
        //
        //根据选中的列 加载不同的F6菜单
        //
        switch(nSelCol)
        {
        case SSS_COL_INDEX_AMPL:
            {
                _resetMenuSmvF6(SSSAmpl);
            }
            break;
        case SSS_COL_INDEX_PHASE:
            {
                _resetMenuSmvF6(SSSPhase);
            }
            break;
        default:
            {
                _resetMenuSmvF6(SSSOther);
            }
        }
        m_pNaviBar->GetNaviMenu()->EnableMenuItem(5, TRUE);
    }
#else
	if (m_eShowReportType == SSShowSetReport)
	{
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_UNSEL), NULL, NULL);
	}
	else if (m_eShowReportType == SSShowSmvDataReport)
	{
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_UNSEL), NULL, NULL);
	}
	else if (m_eShowReportType == SSShowHarmonicReport)
	{
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_UNSEL), NULL, NULL);
	}
	else if (m_eShowReportType == SSShowFaultComensateReport)
	{
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_UNSEL), NULL, NULL);
	}
	else if (m_eShowReportType == SSShowSmvChanMap)
	{
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_OUT_VALIABLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SAMPLE_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SHORT_CAL_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_MENU_WAVE_OVER_UNSEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_SV_MAPPING_SEL), NULL, NULL);
	}
#endif
    m_pNaviBar->UpdateNaviMenu();
}

void CStateSetDlg::_resetMenuSmvF6(SSSmvFocusColType smvFoucsColType)
{
    if(!m_pNaviBar)
    {   
        return;
    }

    //smvreport没有创建
    if (!m_ctlStateSMVDataReport.GetSafeHwnd())
    {
        return;
    }


    CMenu menu;
    CNaviMenu* pSubMenu = new CNaviMenu;

    menu.LoadMenu(IDR_MENU_STATE_SET_F6);

    switch(smvFoucsColType)
    {
    case SSSAmpl:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(1));
        }
        break;
    case SSSPhase:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(2));
        }
        break;
    default:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(0));
        }
    }

    //smv类型
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return;
    }

    if (pParamConfigSMVSend->GetSMVType() == FRAME_TYPE_NET_9_2)
    {
        if (pSubMenu)
        {
            MM_ItemInfo itemInfo;
            for(int i = 0; i < pSubMenu->GetMenuItemCount(); ++i)
            {
                pSubMenu->GetMenuItemInfo(i, &itemInfo, MM_GET_CMDID);
                if (itemInfo.cmdId == ID_MENU_STATE_STATUS_WORD_QUALITY)
                {
                    pSubMenu->SetMenuItemCaption(i, dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_QUALITY));
                    break;
                }
            }
        }
    }
    ;

    menu.DestroyMenu();
    m_pNaviBar->GetNaviMenu()->SetMenuSubItem(5, pSubMenu);
    m_pNaviBar->UpdateNaviMenu();

}

void CStateSetDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
		if (m_pConfigStateSequ != NULL)
		{
			m_pConfigStateSequ->SaveConfig();
		}

		if (m_pConfigManualTest != NULL)
		{
			m_pConfigManualTest->SaveConfig();
		}
    }
    CBaseDlg::OnTimer(nIDEvent);
}
void CStateSetDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    
    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CStateSetDataExtra(pData);
    }

    CStateSetDataExtra* pDataExtra = dynamic_cast<CStateSetDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }
#if CHUN_HUA
    if (m_eShowReportType == SSShowSetReport)
    {
        pDataExtra->m_nFocusRowIndex = m_ctlStateSetReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusColIndex = m_ctlStateSetReport.GetFocusedColIndex();
    }
    else
    {
        pDataExtra->m_nFocusRowIndex = m_ctlStateSMVDataReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusColIndex = m_ctlStateSMVDataReport.GetFocusedColIndex();
    }
#else
	switch (m_eShowReportType)
	{
	case SSShowSetReport:
		pDataExtra->m_nFocusRowIndex = m_ctlStateSetReport.GetFocusedRowIndex();
		pDataExtra->m_nFocusColIndex = m_ctlStateSetReport.GetFocusedColIndex();
		break;
	case SSShowSmvDataReport:
		pDataExtra->m_nFocusRowIndex = m_ctlStateSMVDataReport.GetFocusedRowIndex();
		pDataExtra->m_nFocusColIndex = m_ctlStateSMVDataReport.GetFocusedColIndex();
		break;
	case SSShowHarmonicReport:
		pDataExtra->m_nFocusRowIndex = m_ctlHarmonicReport.GetFocusedRowIndex();
		pDataExtra->m_nFocusColIndex = m_ctlHarmonicReport.GetFocusedColIndex();
		break;
		break;
	case SSShowSmvChanMap:
		//pDataExtra->m_nFocusRowIndex = m_ctlStateSMVDataReport.GetFocusedRowIndex();
		//pDataExtra->m_nFocusColIndex = m_ctlStateSMVDataReport.GetFocusedColIndex();
		break;
		break;
	case SSShowFaultComensateReport:
		pDataExtra->m_nFocusRowIndex = m_ctlFaultCalcReport.GetFocusedRowIndex();
		pDataExtra->m_nFocusColIndex = m_ctlFaultCalcReport.GetFocusedColIndex();
		pDataExtra->m_eStateFaultSetType = m_eStateFaultType;
		break;
	default:
		break;

	}

#endif
    pDataExtra->m_eShowReportType = m_eShowReportType;
    pDataExtra->m_nIndex = m_nIndex;
    pDataExtra->m_pStatus = m_pStatus;

    if (m_pConfigStateSequ == NULL)
    {
        return;
    }
    m_pConfigStateSequ->SaveConfig();
}

void CStateSetDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CStateSetDataExtra* pDataExtra = dynamic_cast<CStateSetDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        //
        //恢复选中行列
        //
        m_nIndex = pDataExtra->m_nIndex;
        m_pStatus = pDataExtra->m_pStatus;
        m_eShowReportType = pDataExtra->m_eShowReportType;
        if (m_eShowReportType == SSShowSetReport)
        {
            _showSetReport(FALSE);
            if(pDataExtra->m_nFocusRowIndex >= 0 && 
                pDataExtra->m_nFocusRowIndex < m_ctlStateSetReport.GetRecordRowCount())
            {
                m_ctlStateSetReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            }
            m_ctlStateSetReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
            m_ctlStateSetReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
        else if (m_eShowReportType == SSShowSmvDataReport)
        {
            _showSmvDataReport(FALSE);
            if(pDataExtra->m_nFocusRowIndex >= 0 && 
                pDataExtra->m_nFocusRowIndex < m_ctlStateSMVDataReport.GetRecordRowCount())
            {
                m_ctlStateSMVDataReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            }
            m_ctlStateSMVDataReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
            m_ctlStateSMVDataReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
#if !CHUN_HUA
		else if (m_eShowReportType == SSShowFaultComensateReport)
		{
			_showFaultCalcReport(FALSE);
			if(pDataExtra->m_nFocusRowIndex >= 0 && 
				pDataExtra->m_nFocusRowIndex < m_ctlFaultCalcReport.GetRecordRowCount())
			{
				m_ctlFaultCalcReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
			}
			m_ctlFaultCalcReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
			m_ctlFaultCalcReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
		}
		else if (m_eShowReportType == SSShowHarmonicReport)
		{
			_showHarmonicReport(FALSE);
			if(pDataExtra->m_nFocusRowIndex >= 0 && 
				pDataExtra->m_nFocusRowIndex < m_ctlHarmonicReport.GetRecordRowCount())
			{
				m_ctlHarmonicReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
			}
			m_ctlHarmonicReport.SetFocusedColIndex(pDataExtra->m_nFocusColIndex);
			m_ctlHarmonicReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
		}
		else if (m_eShowReportType == SSShowSmvChanMap)
		{
			_showSmvChanMapTree(FALSE);
		}
#endif
        _resetMenu();
    }
}
BOOL CStateSetDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_pConfigStateSequ != NULL)
    {
        m_pConfigStateSequ->SaveConfig();
    }
    return CBaseDlg::DestroyWindow();
}

#if !CHUN_HUA
DWORD WINAPI DsmFaultCalcConfirm(WPARAM wParam , LPARAM/* lParam*/ , void* arg)
{
	CDialog* pDlg = (CDialog*)arg;

	UINT nMenuID = LOWORD(wParam);

	pDlg->EndDialog(nMenuID);

	return 0;
}
#endif

BOOL CStateSetDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
            {
                //if (m_eShowReportType == SSShowSmvDataReport && m_ctlStateSMVDataReport.GetSafeHwnd())
                //{
                //    int nSelCol = m_ctlStateSMVDataReport.GetFocusedColIndex();
                //    //
                //    //在报表处理VK_LEFT之前处理，幅值是最左可选中的列
                //    //
                //    if (nSelCol > SSS_COL_INDEX_AMPL && m_ctlStateSMVDataReport.GetFocusedRowIndex() >= 0)
                //    {
                //        nSelCol-=1;
                //        switch(nSelCol)
                //        {
                //        case SSS_COL_INDEX_AMPL:
                //            {
                //                _resetMenuSmvF6(SSSAmpl);
                //            }
                //            break;
                //        case SSS_COL_INDEX_PHASE:
                //            {
                //                _resetMenuSmvF6(SSSPhase);
                //            }
                //            break;
                //        default:
                //            {
                //                _resetMenuSmvF6(SSSOther);
                //            }
                //            break;
                //        }
                //    }
                //}
            }
            break;
        case VK_RIGHT:
            {
                //if (m_eShowReportType == SSShowSmvDataReport && m_ctlStateSMVDataReport.GetSafeHwnd())
                //{
                //    int nSelCol = m_ctlStateSMVDataReport.GetFocusedColIndex();
                //    //
                //    //在报表处理VK_LEFT之前处理，幅值是最左可选中的列
                //    //
                //    if (nSelCol < SSS_COL_INDEX_FREQUENCE && m_ctlStateSMVDataReport.GetFocusedRowIndex() >= 0)
                //    {
                //        nSelCol+=1;
                //        switch(nSelCol)
                //        {
                //        case SSS_COL_INDEX_AMPL:
                //            {
                //                _resetMenuSmvF6(SSSAmpl);
                //            }
                //            break;
                //        case SSS_COL_INDEX_PHASE:
                //            {
                //                _resetMenuSmvF6(SSSPhase);
                //            }
                //            break;
                //        default:
                //            {
                //                _resetMenuSmvF6(SSSOther);
                //            }
                //            break;
                //        }
                //    }
                //}
            }
            break;
#if !SHENG_ZE
		case VK_UP:
			if (m_ctlStateSetReportDI.IsWindowEnabled() && 0 == m_ctlStateSetReportDI.GetFocusedRowIndex())
			{
				m_flag = true;
				m_ctlStateSetReport.EnableWindow(TRUE);

				GetDlgItem(IDC_STATE_SET_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
				m_ctlStateSetReport.SetFocus();

				m_ctlStateSetReport.SetRowFocusEnable(TRUE);					

				m_ctlStateSetReportDI.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

				m_ctlStateSetReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

				m_ctlStateSetReport.RedrawRowItem(m_ctlStateSetReportDI.GetFocusedRowIndex(), 1);
				m_ctlStateSetReport.SetFocusedSelectionRowIndex(7);

				m_ctlStateSetReportDI.SetRowFocusEnable(FALSE);

				m_ctlStateSetReportDI.EnableWindow(FALSE);
			}
			if (m_ctlStateSetReport.IsWindowEnabled() && 0 == m_ctlStateSetReport.GetFocusedRowIndex() && true == m_DIFlag)
			{
				m_flag = false;
				m_ctlStateSetReportDI.EnableWindow(TRUE);

				GetDlgItem(IDC_STATE_SET_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
				
				m_ctlStateSetReportDI.SetFocus();

				m_ctlStateSetReportDI.SetRowFocusEnable(TRUE);					

				m_ctlStateSetReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

				m_ctlStateSetReportDI.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

				m_ctlStateSetReportDI.RedrawRowItem(m_ctlStateSetReportDI.GetFocusedRowIndex(), m_ctlStateSetReportDI.GetFocusedColIndex());
				m_ctlStateSetReportDI.SetFocusedSelectionRowIndex(1);

				m_ctlStateSetReport.SetRowFocusEnable(FALSE);

				m_ctlStateSetReport.EnableWindow(FALSE);
			}
			break;
		case VK_DOWN:
			if (m_ctlStateSetReport.IsWindowEnabled() && 7 == m_ctlStateSetReport.GetFocusedRowIndex() && true == m_DIFlag)
			{
				m_flag = false;
				m_ctlStateSetReportDI.EnableWindow(TRUE);

				GetDlgItem(IDC_STATE_SET_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
				m_ctlStateSetReportDI.SetFocus();

				m_ctlStateSetReportDI.SetRowFocusEnable(TRUE);					

				m_ctlStateSetReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

				m_ctlStateSetReportDI.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

				int ri = m_ctlStateSetReportDI.GetFocusedRowIndex();
				int ci = m_ctlStateSetReportDI.GetFocusedColIndex();
				m_ctlStateSetReportDI.RedrawRowItem(m_ctlStateSetReportDI.GetFocusedRowIndex(), m_ctlStateSetReportDI.GetFocusedColIndex());
				m_ctlStateSetReportDI.SetFocusedSelectionRowIndex(0);

				m_ctlStateSetReport.SetRowFocusEnable(FALSE);

				m_ctlStateSetReport.EnableWindow(FALSE);
			}		
			if (m_ctlStateSetReportDI.IsWindowEnabled() && 1 == m_ctlStateSetReportDI.GetFocusedRowIndex())
			{
				m_flag = true;
				m_ctlStateSetReport.EnableWindow(TRUE);

				GetDlgItem(IDC_STATE_SET_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
				m_ctlStateSetReport.SetFocus();

				m_ctlStateSetReport.SetRowFocusEnable(TRUE);					

				m_ctlStateSetReportDI.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

				m_ctlStateSetReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
					dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

				m_ctlStateSetReport.RedrawRowItem(m_ctlStateSetReportDI.GetFocusedRowIndex(), 1);
				m_ctlStateSetReport.SetFocusedSelectionRowIndex(0);

				m_ctlStateSetReportDI.SetRowFocusEnable(FALSE);

				m_ctlStateSetReportDI.EnableWindow(FALSE);
			}
			break;
#endif
		case VK_F8:
			{
				_showContexMenu();
				return TRUE;
			}
			break;

#if !CHUN_HUA
		case VK_ESCAPE:
			{ 
				if (m_eShowReportType == SSShowFaultComensateReport)
				{
					confirmFaultCalExit();
				}

				CEventProc::GetInstance()->OnKeyEsc();
				return TRUE;
			}
#endif
        default:
            break;
        }
    }
    return CBaseDlg::PreTranslateMessage(pMsg);
}

void CStateSetDlg::_showContexMenu()
{
	//if (m_eShowReportType!=SSShowSmvDataReport)
	//{
	//	return;
	//}	

	//int nRowIndex = m_ctlStateSMVDataReport.GetFocusedRowIndex();
	//int nColIndex = m_ctlStateSMVDataReport.GetFocusedColIndex();
	//if (nRowIndex <0 || nColIndex < 0 )
	//{
	//	return;
	//}

	//CMenu menu;
	//menu.LoadMenu(IDR_MENU_STATE_SET_F6);
	//switch(nColIndex)
	//{
	//case SSS_COL_INDEX_AMPL:
	//	{
	//		m_contextMenu.LoadMenu(menu.GetSubMenu(3));
	//	}
	//	break;
	//case SSS_COL_INDEX_PHASE:
	//	{
	//		 m_contextMenu.LoadMenu(menu.GetSubMenu(4));
	//	}
	//	break;

	//default:
	//	{
	//		 return;
	//	}
	//	break;
	//}

	//
	//
	//CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStateSMVDataReport.GetRecordRow(nRowIndex)->GetItem(nColIndex);
	//CRect rect = m_ctlStateSMVDataReport.GetRowItemRect(nRowIndex,nColIndex);	 


	//if (pItem == NULL )
	//{
	//	return; 
	//}


	////CNaviMenu* pPopup = m_ContexMenu.GetSubMenu(5);

	//CRect rcItem = rect;

	//CNaviPopupBar popupBar;

	//UINT  cmdId = m_contextMenu.TrackPopupMenu(rcItem, this, &popupBar, CNaviMenu::RET_CMDID);
	//if (cmdId )
	//{
	//	this->SendMessage(WM_COMMAND, cmdId);

	//} 
	switch (m_eShowReportType)
	{
	case SSShowHarmonicReport:
		if (m_ctlHarmonicReport.GetSafeHwnd())
		{
			_setHarmonicContextMenu();
		}
		break;
	case SSShowSmvDataReport:
		if (m_ctlStateSMVDataReport.GetSafeHwnd())
		{
			_setSmvContextMenu();
		}
	}


}

#if !CHUN_HUA

void CStateSetDlg::_createHarmonicReport()
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

void CStateSetDlg::_recontentReport()
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

void CStateSetDlg::_showHarmonicReport(bool bIsHide)
{
	if(m_ctlHarmonicReport.GetSafeHwnd())
	{
		m_ctlHarmonicReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createHarmonicReport();
	}
	if (bIsHide)
	{
		_hideReport();
	}

	m_eShowReportType = SSShowHarmonicReport;
	_resetMenu();
	m_ctlHarmonicReport.SetFocus();
}

void CStateSetDlg::_showReport( CListCtrlEL report )
{
	if(report.GetSafeHwnd())
	{
		report.ShowWindow(SW_SHOW);
	}
	else
	{
		_createHarmonicReport();
	}

	_hideReport();

	m_eShowReportType = SSShowHarmonicReport;
	_resetMenu();
	report.SetFocus();
}

void CStateSetDlg::_hideReport()
{
	switch (m_eShowReportType)
	{
	case SSShowSetReport:
		if(m_ctlStateSetReport.GetSafeHwnd())
		{
			m_ctlStateSetReport.ShowWindow(SW_HIDE);
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
		}
		break;
	case SSShowSmvDataReport:
		if(m_ctlStateSMVDataReport.GetSafeHwnd())
		{
			m_ctlStateSMVDataReport.ShowWindow(SW_HIDE);
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
		}
		break;
	case SSShowHarmonicReport:
		if(m_ctlHarmonicReport.GetSafeHwnd())
		{
			m_ctlHarmonicReport.ShowWindow(SW_HIDE);
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
		}	
		break;
	case SSShowSmvChanMap:
		if(m_ctlSmvChanMapTree.GetSafeHwnd())
		{
			m_ctlSmvChanMapTree.ShowWindow(SW_HIDE);
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
		}	
		break;
	case SSShowFaultComensateReport:
		if (m_ctlFaultCalcReport.GetSafeHwnd())
		{
			confirmFaultCalExit();
			m_ctlFaultCalcReport.ShowWindow(SW_HIDE);
#if !SHENG_ZE
			m_ctlStateSetReportDI.ShowWindow(SW_HIDE);
			m_DIFlag = false;
#endif
		}
	default:
		break;
	}
}

void CStateSetDlg::_createSmvChanMapTree()
{
	CRect rcTree;
	GetClientRect(rcTree);

	rcTree.DeflateRect(DSM_MARGIN_IN_RECT);

	int nRows = (rcTree.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcTree.bottom = rcTree.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlSmvChanMapTree.Create(DSM_WINSTYLE_TREE_DEFAULT, rcTree, this, IDC_SMV_CHAN_MAP_TREE);

	m_ctlSmvChanMapTree.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlSmvChanMapTree.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlSmvChanMapTree.PostInitTemplateStyle();

	m_ctlSmvChanMapTree.InsertTreeColumn(0, 
		dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_CHAN);
	m_ctlSmvChanMapTree.InsertTreeColumn(1,
		dsm_stringMgr::Get(DSM_STR_MANUAL_MAP_TO_SMV), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_MAP_TO_SMV);
	m_ctlSmvChanMapTree.InsertTreeColumn(2, 
		dsm_stringMgr::Get(DSM_STR_MANUAL_TRANS_RATIO), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_TRANS_RATIO);

	if (m_pConfigManualTest == NULL)
	{
		return;
	}    

	LBT_HTREEITEM  hDIItem = NULL;
	LBT_HTREEITEM  hChanItem = NULL;
	CChannel* pChannel = NULL;
	int nIndex = 0;
	int nSelIndex = 0;
	CString strTemp = _T("");

	// 遍历虚拟通道
	CSMVSendVirtualChannel* pVirChannel = m_pConfigManualTest->FirstSMVSendVirtualChannel();
	while(pVirChannel)
	{
		hDIItem = m_ctlSmvChanMapTree.InsertTreeItem(pVirChannel->GetName().c_str());
		m_ctlSmvChanMapTree.SetTreeItemData(hDIItem,(DWORD_PTR)pVirChannel);

		nIndex = 0;
		nSelIndex = pVirChannel->GetSelectedIndex();

		pChannel = pVirChannel->FirstChannel();
		while(pChannel)
		{
			strTemp.Format(_T("%d"), nIndex + 1);
			//通道
			hChanItem = m_ctlSmvChanMapTree.InsertTreeItem(strTemp, hDIItem);
			m_ctlSmvChanMapTree.SetTreeItemData(hChanItem, nIndex);
			m_ctlSmvChanMapTree.SetTreeHasCheckbox(hChanItem, TRUE);
			if (nIndex == nSelIndex)
			{
				m_ctlSmvChanMapTree.SetTreeCheck(hChanItem, TRUE);
			}
			//映射到SMV
			CString strIndex = _T("");
			if (pChannel->GetParentChannel() != NULL)
			{
				if (pChannel->GetParentChannel()->GetIndex() >= 0 && pChannel->GetIndex() >=0)
				{
					strIndex.Format(_T("%d.%d"), pChannel->GetParentChannel()->GetIndex() + 1, pChannel->GetIndex()+1);
				}
				else
				{
					strIndex = _T("_._");
				}
			}
			else
			{
				if (pChannel->GetIndex() >= 0)
				{
					strIndex.Format(_T("%d"), pChannel->GetIndex()+1);
				}
				else
				{
					strIndex = _T("_");
				}
			}
			strTemp.Format(_T("0x%04X-%s-%s"), pChannel->GetParentBaseCb()->GetAPPID(), strIndex, pChannel->GetDescription().c_str());
			m_ctlSmvChanMapTree.SetTreeItemText(hChanItem, 1, strTemp);

			//变比
			switch(pChannel->GetChannelType())
			{
			case CHANNEL_TYPE_SMV_CURRENT:
				{
					strTemp.Format(_T("%.03lf A : %.03lf A"), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
				}
				break;

			case CHANNEL_TYPE_SMV_VOLTAGE:
				{
					double dOnceValue = pChannel->GetOnceRatings();
					dOnceValue/=1000.0;

					strTemp.Format(_T("%.03lf kV : %.03lf V"), dOnceValue, pChannel->GetSecondaryRatings());
				}
				break;
			case CHANNEL_TYPE_SMV_TIME:
				{
					strTemp.Format(_T("%.03lf μs : %.03lf μs"), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
				}
				break;
			default:
				{
					strTemp.Format(_T("%.03lf : %.03lf "), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
				}
				break;
			}

			m_ctlSmvChanMapTree.SetTreeItemText(hChanItem, 2, strTemp);

			++nIndex;
			pChannel = pVirChannel->NextChannel(pChannel);
		}
		pVirChannel = m_pConfigManualTest->NextSMVSendVirtualChannel(pVirChannel);
	}
	if (m_ctlSmvChanMapTree.GetCount() > 0)
	{
		m_ctlSmvChanMapTree.SetCurSel(0);
	}
	m_ctlSmvChanMapTree.SetFocus();
	m_ctlSmvChanMapTree.ShowWindow(SW_SHOW);

#if !SU_FEI
	m_ctlSmvChanMapTree.SetTreeCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
}

void CStateSetDlg::_showSmvChanMapTree(bool bIsHide)
{
	if(m_ctlSmvChanMapTree.GetSafeHwnd())
	{
		m_ctlSmvChanMapTree.ShowWindow(SW_SHOW);
	}
	else
	{
		_createSmvChanMapTree();
	}
	if (bIsHide)
	{
		_hideReport();
	}

	m_eShowReportType = SSShowSmvChanMap;
	_resetMenu();
	m_ctlSmvChanMapTree.SetFocus();
}

void CStateSetDlg::OnTreeBoxClick( NMHDR * /*pNotifyStruct*/, LRESULT* pResult )
{
	LBT_HTREEITEM  hItem = m_ctlSmvChanMapTree.GetTreeSelectedItem();

	if (m_ctlSmvChanMapTree.GetTreeParentItem(hItem) == m_ctlSmvChanMapTree.GetTreeRootItem())
	{
		//
		//映射通道
		//
		//m_ctlSmvChanMapTree.ExpandTreeItem(hItem, !hItem->isExpand);
	}
	else
	{
		//
		//真实通道
		//
		if (!m_ctlSmvChanMapTree.GetTreeCheck(hItem))
		{
			CSMVSendVirtualChannel* pVirChannel = NULL;
			int nSelIndex = 0;

			LBT_HTREEITEM hParentItem = m_ctlSmvChanMapTree.GetTreeParentItem(hItem);
			pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvChanMapTree.GetTreeItemData(hParentItem);
			if (pVirChannel == NULL)
			{
				return;
			}
			nSelIndex = m_ctlSmvChanMapTree.GetTreeItemData(hItem);
			pVirChannel->SetSelectedIndex(nSelIndex);

			LBT_HTREEITEM hChildItem = m_ctlSmvChanMapTree.GetTreeChildItem(hParentItem);

			while(hChildItem != NULL)
			{
				if (hChildItem == hItem)
				{
					m_ctlSmvChanMapTree.SetTreeCheck(hChildItem, TRUE);
				}
				else if(hChildItem != hItem && m_ctlSmvChanMapTree.GetTreeCheck(hChildItem))
				{
					m_ctlSmvChanMapTree.SetTreeCheck(hChildItem, FALSE);
				}
				hChildItem = m_ctlSmvChanMapTree.GetTreeNextSiblingItem(hChildItem);
			}
		}
	}
	;
	*pResult = TRUE;
}

void CStateSetDlg::_setHarmonicContextMenu()
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
				m_contextMenu.LoadMenu(menu.GetSubMenu(4));
			}
			//等相角
			else if (pHarmonic->GetHarmonicType() == HARMONIC_DC)
			{
				m_contextMenu.LoadMenu(menu.GetSubMenu(5));
			}
			else
			{
				m_contextMenu.LoadMenu(menu.GetSubMenu(6));
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

	UINT  cmdId = m_contextMenu.TrackPopupMenu(rcItem, &m_ctlHarmonicReport, &popupBar, CNaviMenu::RET_CMDID);
	if (cmdId )
	{
		this->SendMessage(WM_COMMAND, cmdId);

	} 

}

void CStateSetDlg::_setSmvContextMenu()
{
	if(!m_pNaviBar)
	{   
		return;
	}
	int nRowIndex = m_ctlStateSMVDataReport.GetFocusedRowIndex();
	int nColIndex = m_ctlStateSMVDataReport.GetFocusedColIndex();
	if (nRowIndex <0 || nColIndex < 0 )
	{
		return;
	}

	//smv类型
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return;
	}

	CMenu menu;
	//CNaviMenu* pSubMenu = new CNaviMenu;

	menu.LoadMenu(IDR_MENU_STATE_SET_F6);
	switch(nColIndex)
	{
	case SSS_COL_INDEX_AMPL:
		{
			m_contextMenu.LoadMenu(menu.GetSubMenu(3)); 
		}
		break;
	case SSS_COL_INDEX_PHASE:
		{
			m_contextMenu.LoadMenu(menu.GetSubMenu(4));
		}
		break;
	default:
		{
			return;//m_contextMenu.LoadMenu(menu.GetSubMenu(5));
		}
		break;
	}


	CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStateSMVDataReport.GetRecordRow(nRowIndex)->GetItem(nColIndex);
	CRect rect = m_ctlStateSMVDataReport.GetRowItemRect(nRowIndex,nColIndex);	 


	if (pItem == NULL )
	{
		return; 
	}


	//CNaviMenu* pPopup = m_ContexMenu.GetSubMenu(5);

	CRect rcItem = rect;

	CNaviPopupBar popupBar;

	UINT  cmdId = m_contextMenu.TrackPopupMenu(rcItem, &m_ctlStateSMVDataReport, &popupBar, CNaviMenu::RET_CMDID);
	if (cmdId )
	{
		this->SendMessage(WM_COMMAND, cmdId);

	} 
}

void CStateSetDlg::OnHarmonicItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
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

void CStateSetDlg::OnHarmonicItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
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

void CStateSetDlg::OnInplaceHarmonicEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
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



void CStateSetDlg::OnStateHarmonicContextMenu( UINT nID )
{
	int nRowSel = m_ctlHarmonicReport.GetFocusedRowIndex();

	switch (nID)
	{
	case ID_MENU_STATE_HARMONIC_F1:
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
	case ID_MENU_STATE_HARMONIC_F2:
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
	case ID_MENU_STATE_HARMONIC_F3:
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
	case ID_MENU_STATE_HARMONIC_F4:
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



void CStateSetDlg::OnFaultCalcItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

	if(!pItemNotify)
	{
		return;
	}

	SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(0)->GetItemData();


	switch(sfcType)
	{
	case SFCFaultType:
		{
			m_pFaultCalc->SetFaultType((SCCalc::SCType)pItemNotify->pConstraint->GetConstraintData());
			_resetFaultCalcPartReport();
			*pResult = TRUE;
		}
		break;
	case SFCCalcModeType:
		{
			m_pFaultCalc->SetCalcModelType((SCCalc::CalcModelType)pItemNotify->pConstraint->GetConstraintData());
			_resetFaultCalcPartReport();
			*pResult = TRUE;
		}
		break;
	case SFCDirection:
		{
			m_pFaultCalc->SetDirection((SCCalc::DirectionType)pItemNotify->pConstraint->GetConstraintData());
			*pResult = TRUE;
		}
		break;
	}
}

void CStateSetDlg::OnFaultCalcItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
	if (pItemNotify == NULL)
	{
		return;
	}
	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
	if (pItem == NULL)
	{
		return;
	}

	CString strValue = pItem->GetValue();
	double dValue = 0.0f;

	SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(0)->GetItemData();

	switch(sfcType)
	{
	case SFCRatedVoltage:
		{
			if (DSM_Str2Double(strValue, dValue))
			{
				m_pFaultCalc->SetRatedVoltage(dValue);
			}
			strValue.Format(_T("%.03lf"), m_pFaultCalc->GetRatedVoltage());
			pItem->SetValue(strValue);

			*pResult = TRUE;
		}
		break;
	case SFCLoadCurrent:
		{
			if (DSM_Str2Double(strValue, dValue))
			{
				m_pFaultCalc->SetLoadCurrent(dValue);
			}
			strValue.Format(_T("%.03lf"), m_pFaultCalc->GetLoadCurrent());
			pItem->SetValue(strValue);

			*pResult = TRUE;

		}
		break;
	case SFCLoadAngle:
		{
			if (DSM_Str2Double(strValue, dValue))
			{
				m_pFaultCalc->SetLoadAngle(dValue);
			}
			strValue.Format(_T("%.03lf"), m_pFaultCalc->GetLoadAngle());
			pItem->SetValue(strValue);

			*pResult = TRUE;

		}
		break;
	case SFCFaultCurrent:
		{
			if (DSM_Str2Double(strValue, dValue))
			{
				m_pFaultCalc->SetFaultCurrent(dValue);
			}
			strValue.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
			pItem->SetValue(strValue);

			*pResult = TRUE;
		}
		break;
	case SFCFaultVoltage:
		{
			if (DSM_Str2Double(strValue, dValue))
			{
				m_pFaultCalc->SetFaultVoltage(dValue);
			}
			strValue.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
			pItem->SetValue(strValue);

			*pResult = TRUE;
		}
		break;
	}
}

void CStateSetDlg::OnFaultCalcItemOkClick( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(pNotifyStruct);
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	int nRowIndex = m_ctlFaultCalcReport.GetFocusedRowIndex();
	if (nRowIndex < 0)
	{
		return;
	}

	SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(nRowIndex)->GetItem(0)->GetItemData();

	switch (sfcType)
	{
	case SFCLineImpedanceZ1:
		{
			m_eStateFaultType = eSFSTImpedanceZ1;
			CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_IMPEDANCE_DLG);			
			*pResult = TRUE;
		}
		break;
	case SFCLineCompensateK1:
		{
			m_eStateFaultType = eSFSTCompensateK1;
			CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_COMPENSATE_DLG); 			
			*pResult = TRUE;
		}
		break;
	case SFCSystemImpdanceZs:
		{
			m_eStateFaultType = eSFSTImpedanceZs;
			CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_IMPEDANCE_DLG);			
			*pResult = TRUE;
		}
		break;
	case SFCSystemCompensateKs:
		{
			m_eStateFaultType = eSFSTCompensateKs;
			CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_COMPENSATE_DLG);			
			*pResult = TRUE;
		}
		break;
	}
}

void CStateSetDlg::OnFaultCalcInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(pResult);

	*pResult = FALSE;
	static CWnd* pSrc = NULL;

	ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

	// inplaceEdit弹出时, 切换菜单
	if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
	{
		pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlFaultCalcReport.GetInplaceEdit());
		*pResult = TRUE;
	}
	// inplace退出时，切换菜单
	else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
	{
		CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
		*pResult = TRUE;
	}
}

void CStateSetDlg::_createFaultCalcReport()
{
	m_pConfigStateSequ = CParamConfigStateSequence::getInstance();
	if (m_pConfigStateSequ)
	{
		m_pFaultCalc = m_pConfigStateSequ->GetFaultCalc();
	}
	if (m_pFaultCalc == NULL)
	{
		return ;
	}

	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

	m_ctlFaultCalcReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_STATE_FAULT_CALC_RET);

	// 设置字体
	m_ctlFaultCalcReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlFaultCalcReport.PostInitTemplateStyle();
	// 插入列元素
	m_ctlFaultCalcReport.InsertColumn(0,
		_T(""),
		EL_LEFT,
		DSM_STATE_FAULT_CALC_WIDTH_CAPTION,
		FALSE,
		EL_CENTER,
		DSM_REPORT_COLOR_CAPTION_BK);
	m_ctlFaultCalcReport.InsertColumn(1,
		_T(""),
		EL_LEFT,
		DSM_STATE_FAULT_CALC_WIDTH_CONTENT);

	// 设置选择焦点样式
	m_ctlFaultCalcReport.SetRowSelectionVisible(FALSE);
	m_ctlFaultCalcReport.SetRowFocusEnable(TRUE);
	m_ctlFaultCalcReport.SetSubItemFocusEnable(TRUE);
	m_ctlFaultCalcReport.SetFreezeColumnsCount(1);
	m_ctlFaultCalcReport.SetFreezeSelectionVisible(TRUE);
	m_ctlFaultCalcReport.SetFocusedColIndex(1);
	m_ctlFaultCalcReport.SetFocus();

	CString strCaption = _T("");
	CString strContent = _T("");
	CELRecordItemText* pItem = NULL;
	//故障类型
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_TYPE);
	SCCalc::SCType faultType = m_pFaultCalc->GetFaultType();
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_FAULTTYPE, new CDsmStateFaultCalcRecord(strCaption, _T("")));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_FAULTTYPE, 0, SFCFaultType);
	CELRecordItemEditOptions* pOptions = m_ctlFaultCalcReport.GetRecordRow(0)->GetItem(1)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_A_GROUND), SCCalc::A_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_B_GROUND), SCCalc::B_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_C_GROUND), SCCalc::C_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_AB_SHORT_CIRCUIT), SCCalc::AB_SHORT_CIRCUIT);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_BC_SHORT_CIRCUIT), SCCalc::BC_SHORT_CIRCUIT);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CA_SHORT_CIRCUIT), SCCalc::CA_SHORT_CIRCUIT);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_AB_GROUND), SCCalc::AB_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_BC_GROUND), SCCalc::BC_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CA_GROUND), SCCalc::CA_GROUND);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_ABC_SHORT_CIRCUIT), SCCalc::ABC_SHORT_CIRCUIT);
	//pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SINGLE_PHASE_SYSTEM), SCCalc::SINGLE_PHASE_SYSTEM);

	CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(faultType);
	if (pItemCon)
	{
		pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(0)->GetItem(1);
		if (pItem)
		{
			pItem->SetValue(pItemCon->GetConstraintCaption());
		}
	}

	//计算模型
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_CALC_MODE_TYPE);
	SCCalc::CalcModelType calcModeType = m_pFaultCalc->GetCalcModelType();
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_CALC_MODE_TYPE, new CDsmStateFaultCalcRecord(strCaption, _T("")));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_CALC_MODE_TYPE, 0, SFCCalcModeType);
	pOptions = m_ctlFaultCalcReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CURRENT_FIXED), SCCalc::CURRENT_FIXED);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_VOLTAGE_FIXED), SCCalc::VOLTAGE_FIXED);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_FIXED), SCCalc::SYSTEM_IMPEDANCE_FIXED);

	pItemCon = pOptions->FindConstraint(calcModeType);
	if (pItemCon)
	{
		pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(1)->GetItem(1);
		if (pItem)
		{
			pItem->SetValue(pItemCon->GetConstraintCaption());
		}
	}

	//方向
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_DIRECTION);
	SCCalc::DirectionType directionType = m_pFaultCalc->GetDirection();
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_DIRECTION, new CDsmStateFaultCalcRecord(strCaption, _T("")));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_DIRECTION, 0, SFCDirection);
	pOptions = m_ctlFaultCalcReport.GetRecordRow(2)->GetItem(1)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_POSITIVE), SCCalc::POSITIVE);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_NEGATIVE), SCCalc::NEGATIVE);

	pItemCon = pOptions->FindConstraint(directionType);
	if (pItemCon)
	{
		pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(2)->GetItem(1);
		if (pItem)
		{
			pItem->SetValue(pItemCon->GetConstraintCaption());
		}
	}

	//额定电压
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_RATED_VOLTAGE);
	strContent.Format(_T("%.03lf"), m_pFaultCalc->GetRatedVoltage());
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_RATED_VOLTAGE, new CDsmStateFaultCalcRecord(strCaption, strContent));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_RATED_VOLTAGE, 0, SFCRatedVoltage);

	//负荷电流
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LOAD_CURRENT);
	strContent.Format(_T("%.03lf"), m_pFaultCalc->GetLoadCurrent());
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LOAD_CURRENT, new CDsmStateFaultCalcRecord(strCaption, strContent));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LOAD_CURRENT, 0, SFCLoadCurrent);

	//负荷功角
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LOAD_ANGLE);
	strContent.Format(_T("%.03lf"), m_pFaultCalc->GetLoadAngle());
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LOAD_ANGLE, new CDsmStateFaultCalcRecord(strCaption, strContent));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LOAD_ANGLE, 0, SFCLoadAngle);


	//线路阻抗z1
	strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LINE_IMPEDANCE_Z1);
	CImpedance impdance = m_pFaultCalc->GetLineImpedanceZl();

	strContent.Format(_T("%.03lf∠%.03lf°"), impdance.GetImpedance(), impdance.GetAngle());
	m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LINE_IMPEDANCE_Z1, new CDsmStateFaultCalcRecord(strCaption, strContent));
	m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LINE_IMPEDANCE_Z1, 0, SFCLineImpedanceZ1);

	_resetFaultCalcPartReport();

	m_ctlFaultCalcReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
	for(int i = 0; i <m_ctlFaultCalcReport.GetRecordRowCount(); i++)
	{
		m_ctlFaultCalcReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
	}

	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageEnter)
	{
		m_ctlFaultCalcReport.SetFocusedSelectionRowIndex(0);
	}
	m_ctlFaultCalcReport.SetFocus();
	m_ctlFaultCalcReport.ShowWindow(SW_SHOW);

}

void CStateSetDlg::_resetFaultCalcPartReport()
{
	CString strCaption = _T("");
	CString strContent = _T("");

	m_ctlFaultCalcReport.SetRedraw(FALSE);

	while(m_ctlFaultCalcReport.GetRecordRowCount() > DSM_SFC_INDEX_NEXT)
	{
		m_ctlFaultCalcReport.DeleteRecordRow(DSM_SFC_INDEX_NEXT);
	}

	int nNextIndex = DSM_SFC_INDEX_NEXT;
	SCCalc::SCType faultType = m_pFaultCalc->GetFaultType();
	SCCalc::CalcModelType calcModeType = m_pFaultCalc->GetCalcModelType();

	if (faultType == SCCalc::A_GROUND  ||
		faultType == SCCalc::B_GROUND  ||
		faultType == SCCalc::C_GROUND  ||
		faultType == SCCalc::AB_GROUND ||
		faultType == SCCalc::BC_GROUND ||
		faultType == SCCalc::CA_GROUND )
	{

		//有线路零序补偿系数K1
		strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LINE_COMPENSATE_K1);
		CCompensate compensate = m_pFaultCalc->GetLineCompensateKl();
		strContent.Format(_T("%.03lf∠%.03lf"), compensate.GetModelValue(), compensate.GetAngle());
		m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
		m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCLineCompensateK1);
		++nNextIndex;

		switch (calcModeType)
		{
		case SCCalc::SYSTEM_IMPEDANCE_FIXED:
			{
				//Zs
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_ZS);
				CImpedance impedance = m_pFaultCalc->GetSystemImpedanceZs();
				strContent.Format(_T("%.03lf∠%.03lf"), impedance.GetImpedance(), impedance.GetAngle());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemImpdanceZs);
				++nNextIndex;

				//Ks
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_COMPENSTATE_KS);
				CCompensate compensate = m_pFaultCalc->GetSystemCompensateKs();
				strContent.Format(_T("%.03lf∠%.03lf"), compensate.GetModelValue(), compensate.GetAngle());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemCompensateKs);

			}
			break;
		case SCCalc::VOLTAGE_FIXED:
			{
				//故障电压
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_VOLTAGE);
				strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultVoltage);
			}
			break;
		case SCCalc::CURRENT_FIXED:
			{
				//故障电流
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_CURRENT);
				strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultCurrent);
			}
			break;
		}

	}
	else
	{
		switch (calcModeType)
		{
		case SCCalc::SYSTEM_IMPEDANCE_FIXED:
			{
				//Zs
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_ZS);
				CImpedance impedance = m_pFaultCalc->GetSystemImpedanceZs();
				strContent.Format(_T("%.03lf∠%.03lf"), impedance.GetImpedance(), impedance.GetAngle());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemImpdanceZs);
				++nNextIndex;
			}
			break;
		case SCCalc::VOLTAGE_FIXED:
			{
				//故障电压
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_VOLTAGE);
				strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultVoltage);
			}
			break;
		case SCCalc::CURRENT_FIXED:
			{
				//故障电流
				strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_CURRENT);
				strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
				m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
				m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultCurrent);
			}
			break;
		}
	}
	for (int i = DSM_SFC_INDEX_NEXT; i < m_ctlFaultCalcReport.GetRecordRowCount(); ++i)
	{
		m_ctlFaultCalcReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
	}
	m_ctlFaultCalcReport.SetRedraw(TRUE);
}

void CStateSetDlg::_showFaultCalcReport(BOOL bIsHide)
{
	if(m_ctlFaultCalcReport.GetSafeHwnd())
	{
		m_ctlFaultCalcReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createFaultCalcReport();
	}
	if (bIsHide)
	{
		_hideReport();
	}

	m_eShowReportType = SSShowFaultComensateReport;
	_resetMenu();
	m_ctlFaultCalcReport.SetFocus();
}

void CStateSetDlg::confirmFaultCalExit()
{
 
	CMsgDlg msgdlg(DSM_STR_STATE_CONFIRM_FAULT_CALC, 0, this); 

	msgdlg._SetRect(DSM_STATE_FAULT_CALC_RC);
	msgdlg._SetTextRC(DSM_STATE_FAULT_CALC_TIP); 
	msgdlg._SetMenu(IDR_MENU_STATE_FAULT_CALC_CONFIRM);
	msgdlg._ResetF5Text(_T("2"));
	msgdlg._ResetF6Text(_T("取消"));
	msgdlg._SetBitmapDisp(false);
	msgdlg._SetCommandRange(ID_MENU_STATE_FAULT_CALC_CONFIRM_F4, ID_MENU_STATE_FAULT_CALC_CONFIRM_F6, DsmFaultCalcConfirm, (void*)&msgdlg);
	INT_PTR nRet = msgdlg.DoModal();
	msgdlg._ResetMenu();
	switch(nRet)
	{
	case ID_MENU_STATE_FAULT_CALC_CONFIRM_F4:
		{
			m_pConfigStateSequ->UseFaultCalc(m_pStatus, 1);
		}
		break;
	case ID_MENU_STATE_FAULT_CALC_CONFIRM_F5:
		{
			m_pConfigStateSequ->UseFaultCalc(m_pStatus, 2);
		}
		break;
	case ID_MENU_STATE_FAULT_CALC_CONFIRM_F6: 
		break;
	default:
		break;
	} 
}


#endif