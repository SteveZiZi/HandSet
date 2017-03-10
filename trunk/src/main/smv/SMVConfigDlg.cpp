/**
 * \file
 *      SMVConfigDlg.cpp
 *
 * \brief
 *      配置页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */

// SMVConfigDlg.cpp :  实现文件
//

#include "stdafx.h"
#include "SMVConfigDlg.h"


#include "src/main/eventproc.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/smvdualphasedlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src\service\datalayer\utility\data_utility.h"
#include "src/service/datalayer/config/smvrecvconfig.h"


//////////////////////////////////////////////////////////////////////////
// smv - 设置
#define DSM_SMV_CONFIG_INDEX                50              ///< 索引
#define DSM_SMV_CONFIG_DESC                 129            ///< 通道描述
#define DSM_SMV_CONFIG_TYPE                 70              ///< 通道类型
#define DSM_SMV_CONFIG_PHASE                60              ///< 通道相位
#define DSM_SMV_CONFIG_RATE1                110             ///< 通道一次值
#define DSM_SMV_CONFIG_RATE2                110             ///< 通道二次值


#define DSM_SMV_CONFIG_NAVI_BASE            0x01            ///< 左侧导航-基本data
#define DSM_SMV_CONFIG_NAVI_CHANNEL         0x02            ///< 左侧导航-通道设置data

#define SMVCONFIG_CAPTION                   0               ///< 标题列
#define SMVCONFIG_VALUE                     1               ///< 值列


/**
 *      smv设置 - 通道设置
 *
 *  \note
 *      序号、通道描述、类型、相别、一次额定值、二次额定值
 */
class CDsmSmvChannelConfigRecord : public CELRecord
{
public:
    CDsmSmvChannelConfigRecord(CSmvRecvChannelInfo* pInfo);
    virtual ~CDsmSmvChannelConfigRecord() {};

public:

    /**
     *      获取记录关联的通道信息指针
     */
    inline CSmvRecvChannelInfo* GetInfo()
    {
        return m_pInfo;
    }

private:
    CSmvRecvChannelInfo*        m_pInfo;
};

CDsmSmvChannelConfigRecord::CDsmSmvChannelConfigRecord( CSmvRecvChannelInfo* pInfo )
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    int nType, nPhasae;

    CString csPrimary;
    CString csSecond;

    // 通道类型
    switch(pInfo->enChannelType)
    {
        // 电流类型
    case CHANNEL_TYPE_SMV_CURRENT:
        nType = DSM_STR_TYPE_DATA_AMPER;
        csPrimary.Format(L"%.3f", m_pInfo->fPrimaryRatedValue);
        csSecond.Format(L"%.3f", m_pInfo->fSecondaryRatedValue);
        break;

        // 电压类型
    case CHANNEL_TYPE_SMV_VOLTAGE:
        nType = DSM_STR_TYPE_DATA_VOLT;
        csPrimary.Format(L"%.3f", (float)(m_pInfo->fPrimaryRatedValue / 1000.0f));
        csSecond.Format(L"%.3f", m_pInfo->fSecondaryRatedValue);
        break;

        // 时间类型
    case CHANNEL_TYPE_SMV_TIME:
        nType = DSM_STR_TYPE_DATA_TIMETICK;
        csPrimary.Format(L"%.0f", m_pInfo->fPrimaryRatedValue);
        csSecond.Format(L"%.0f", m_pInfo->fSecondaryRatedValue);
        break;

        // unknown
    default:
        nType = DSM_STR_TYPE_DATA_UNKNOWN;
        break;
    }

    // 相别信息
    switch(pInfo->enChannelPhase)
    {
    case CHANNEL_PHASE_A:
        nPhasae = DSM_STR_TYPE_PHASE_A;
        break;

    case CHANNEL_PHASE_B:
        nPhasae = DSM_STR_TYPE_PHASE_B;
        break;

    case CHANNEL_PHASE_C:
        nPhasae = DSM_STR_TYPE_PHASE_C;
        break;

    case CHANNEL_PHASE_X:
        nPhasae = DSM_STR_TYPE_PHASE_X;
        break;
    case CHANNEL_PHASE_N:
        nPhasae = DSM_STR_TYPE_PHASE_N;
        break;

    default:
        nPhasae = DSM_STR_TYPE_PHASE_UNKNOWN;
        break;
    }

    // 通道类型
    CString csType(dsm_stringMgr::Get(nType));

    if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType && CHANNEL_VALUE_TYPE_MEASURE == pInfo->enChannelValueType)
    {
        csType.Format(L"%s (%s)", csType, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_MEASURE));
    }

    AddItem(new CDsmRecordItem(m_pInfo->nIndex + 1));                   // 序号
    AddItem(new CDsmRecordItem(m_pInfo->strChannelDesc.c_str()));       // 描述

    CELRecordItem* pItemType    = AddItem(new CDsmRecordItem(csType));       // 类型

    if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
    {
        nPhasae = DSM_STR_TYPE_PHASE_UNKNOWN;
    }

    CELRecordItem* pItemPhasae  = AddItem(new CDsmRecordItem(dsm_stringMgr::Get(nPhasae)));     // 相别

    CELRecordItem* pItem1       = dynamic_cast<CELRecordItemText*>(AddItem(new CELRecordItemText(csPrimary)));      // 一次额定值
    CELRecordItem* pItem2       = dynamic_cast<CELRecordItemText*>(AddItem(new CELRecordItemText(csSecond)));       // 二次额定值

    // 根据通道类型设置其单位
    switch(m_pInfo->enChannelType)
    {
    case CHANNEL_TYPE_SMV_CURRENT:
        pItem1->SetFormatString(L"%s A");
        pItem2->SetFormatString(L"%s A");
        break;

    case CHANNEL_TYPE_SMV_VOLTAGE:
        pItem1->SetFormatString(L"%s kV");
        pItem2->SetFormatString(L"%s V");
        break;

    case CHANNEL_TYPE_SMV_TIME:
        pItem1->SetFormatString(L"%s μs");
        pItem2->SetFormatString(L"%s μs");
        break;

    default:
        break;
    }

    // 添加类型列表 电流、电压、时间戳
    pItemType->SetEditable(TRUE);
    pItemType->GetEditOptions(NULL)->AddComboButton();
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER), CHANNEL_TYPE_SMV_CURRENT);
    pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_VOLT), CHANNEL_TYPE_SMV_VOLTAGE);

    if(pInfo->pOwnerCtrlBlk)
    {
        if(FRAME_TYPE_NET_9_1 == pInfo->pOwnerCtrlBlk->enFrameType || FRAME_TYPE_NET_9_2 == pInfo->pOwnerCtrlBlk->enFrameType)
        {
            pItemType->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_DATA_TIMETICK), CHANNEL_TYPE_SMV_TIME);
        }
    }

    // 添加相别列表 A相、B相、C相
    pItemPhasae->GetEditOptions(NULL)->AddComboButton();
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_A), CHANNEL_PHASE_A);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_B), CHANNEL_PHASE_B);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_C), CHANNEL_PHASE_C);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_N), CHANNEL_PHASE_N);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_X), CHANNEL_PHASE_X);

    BOOL bEdit = TRUE;

    // 若是时间类型，则相别、一次、二次值不允许focus
    if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
    {
        bEdit = FALSE;
    }

    pItem1->SetEditable(bEdit);
    pItem2->SetEditable(bEdit);
    pItemPhasae->SetEditable(bEdit);
}



// CSMVConfigDlg 对话框

IMPLEMENT_DYNAMIC(CSMVConfigDlg, DSM_SMVBASEDLG)

CSMVConfigDlg::CSMVConfigDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVConfigDlg::IDD, DSM_STR_SMV_CONFIG_CAPTION, pParent)
    , m_pCurrentReport(NULL)
    , m_modePage(PAGE_NORMAL)
    , m_modeCtrl(CTRL_ONE)
    , m_pCurCtrlBlk(NULL)
    , m_pCtrlBlk2(NULL)
    , m_pCtrlBlk1(NULL)
    , m_pF6SubTemplate(NULL)
    , m_nBeginID(0)
    , m_nEndID(0)
#if !SU_FEI
	, m_modeSetting(SETTING_BASIC)
#endif
{
    m_nMenuID = IDR_MENU_SMV_CONFIG;
}

CSMVConfigDlg::~CSMVConfigDlg()
{

}

void CSMVConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVConfigDlg, DSM_SMVBASEDLG)
#if SU_FEI
    ON_LBN_SELCHANGE(IDC_SMV_CONFIG_MENU, &CSMVConfigDlg::OnMenuChanged)
#else
	ON_COMMAND(ID_SMV_CONFIG_F3, &CSMVConfigDlg::OnMenuF3)
	ON_COMMAND(ID_SMV_CONFIG_F6, &CSMVConfigDlg::OnMenuF6)
#endif
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_CONFIG_REPORT_BASE, &CSMVConfigDlg::OnItemSelectChangedBase)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_CONFIG_REPORT_CHANNEL, &CSMVConfigDlg::OnItemSelectChangedChannel)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_SMV_CONFIG_REPORT_CHANNEL, &CSMVConfigDlg::OnItemValueChangedChannel)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_CONFIG_REPORT_CHANNEL2, &CSMVConfigDlg::OnItemSelectChangedChannel)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_SMV_CONFIG_REPORT_CHANNEL2, &CSMVConfigDlg::OnItemValueChangedChannel)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_SMV_CONFIG_REPORT_CHANNEL,  &CSMVConfigDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_SMV_CONFIG_REPORT_CHANNEL,  &CSMVConfigDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_SMV_CONFIG_REPORT_CHANNEL2,  &CSMVConfigDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_SMV_CONFIG_REPORT_CHANNEL2,  &CSMVConfigDlg::OnInplaceEditAction)
    ON_COMMAND(ID_SMV_CONFIG_F1, &CSMVConfigDlg::OnMenuF1)
    ON_COMMAND(ID_SMV_CONFIG_F2, &CSMVConfigDlg::OnMenuF2)
END_MESSAGE_MAP()


// CSMVConfigDlg 消息处理程序

BOOL CSMVConfigDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    if(!m_pCurrentCtrlBlk)
    {
        DSM_DEBUG(L"dsm: null ctrlblk in smv config\n");
    }

    // 获取上一个页面的数据
    CPageData* pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    ASSERT(pPageData);

    // 若是核相页面，则设置核相的两个控制块，之后显示两个控制块的通道信息
    if(pPageData && pPageData->m_nPageID == CSMVDualphaseDlg::IDD)
    {
        m_modePage = PAGE_DUALPHASE;        // 设置当前是核相通道设置模式
        CPageDataExtraDualPhase* pPageDataExtra = dynamic_cast<CPageDataExtraDualPhase*>(pPageData->m_pPageDataExtra);
        ASSERT(pPageDataExtra);

        m_pCtrlBlk1 = m_pDataLayer->GetSelectCtrlBlkByID(pPageDataExtra->nSmv1);
        m_pCtrlBlk2 = m_pDataLayer->GetSelectCtrlBlkByID(pPageDataExtra->nSmv2);

        m_pCurCtrlBlk = m_pCtrlBlk1;
    }
    else
    {
        m_pCtrlBlk1     = m_pCurrentCtrlBlk;
        m_pCurCtrlBlk   = m_pCtrlBlk1;
    }
#if SU_FEI
    _createLeftMenu();      // 创建左侧菜单

    _enterBase();           // 默认进入基本设置功能
#else
	if (SETTING_BASIC == m_modeSetting)
	{
		_enterBase();
	}
	
	if (SETTING_CHANNEL == m_modeSetting)
	{
		_enterChannel();
	}
#endif
    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

/**
 *      创建左侧菜单
 */

#if SU_FEI
void CSMVConfigDlg::_createLeftMenu()
{
    CRect rcNavi(DSM_BASICCFG_RCLEFT);
    DSM_ParentToChild(this, rcNavi);

    m_menuNavi.Create(WS_TABSTOP | WS_CHILD | WS_VISIBLE, rcNavi, this, IDC_SMV_CONFIG_MENU);

    m_menuNavi.SetFont(dsm_fontmgr::GetInstance()->GetFont(LBT_3L_TEXTLFHEIGHT));
    m_menuNavi.SetHeight(LBT_3L_ITEMHEIGHT);
    m_menuNavi.PostInitTemplateStyle();

    // 添加基本设置与通道设置菜单
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_BASIC_CFG), DSM_SMV_CONFIG_NAVI_BASE);
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_CHANNEL_CFG), DSM_SMV_CONFIG_NAVI_CHANNEL);

    m_menuNavi.SetCurSel(0);
    m_menuNavi.SetFocus();
}

#endif
/**
 *      创建报表
 *
 *  \param[in]
 *      report          待创建报表
 *  \param[in]
 *      nID             待创建报表的ID
 */
void CSMVConfigDlg::_createReport( CReportH& report, UINT nID )
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    // 创建基本设置报表
    if(IDC_SMV_CONFIG_REPORT_BASE == nID)
    {
        rcReport.bottom = rcReport.top + ELT_T1_ROWHEIGHT * 1;

        report.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

        // 设置字体
        report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
        report.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

        // 设置横向报表样式
        report.PostInitTemplateStyle();

        _initBaseReport();
    }
    else if(IDC_SMV_CONFIG_REPORT_CHANNEL == nID || IDC_SMV_CONFIG_REPORT_CHANNEL2 == nID)
    {
        int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
        rcReport.bottom = rcReport.top + ELT_T1_ROWHEIGHT * nRows;

        report.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

        // 设置字体
        report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
        report.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

        // 设置横向报表样式
        report.PostInitTemplateStyle();

        _initChannelReport();
    }
}


/**
 *      初始化基本报表
 */
void CSMVConfigDlg::_initBaseReport()
{
    m_reportBase.SetHeaderHeight(0);

    m_reportBase.InsertColumn(SMVCONFIG_CAPTION, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION);   // 标题
    m_reportBase.InsertColumn(SMVCONFIG_VALUE, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);     // 内容

    // 第一行 采样值显示
   /* m_reportBase.InsertRecordRow(0, new CDsmRecord(
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_VAL), 
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ONCE)));*/

    //// 第二行 采样值频率
    //m_reportBase.InsertRecordRow(1, new CDsmRecord(
    //    dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_FREQU), 
    //    dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_4K)));

    // 第三行 采样点对齐方式
		m_reportBase.InsertRecordRow(0, new CDsmRecord(
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ALIGN), 
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI)));
#if SU_FEI  
    // 设置采样值显示inplacelist
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ONCE), SMV_VALUE_TYPE_PRIMARY);
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_TWICE), SMV_VALUE_TYPE_SECONDARY);
  
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SOURCE), SMV_VALUE_TYPE_ORIGINANL);

    // 设置采样值频率inplacelist
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_4K), SMV92_SAMPLE_RATE_4K);
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_12P8K), SMV92_SAMPLE_RATE_12P8K);

    // 设置采样点对齐方式inplacelist
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI), SMV_PHASE_CHECK_ALIGN_INDEX);
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI), SMV_PHASE_CHECK_ALIGN_TIME);
#else
	// 设置采样点对齐方式inplacelist
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI), SMV_PHASE_CHECK_ALIGN_INDEX);
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI), SMV_PHASE_CHECK_ALIGN_TIME);

#endif
    // 设置选择焦点样式
    m_reportBase.SetRowSelectionVisible(FALSE);
    m_reportBase.SetRowFocusEnable(TRUE);
    m_reportBase.SetSubItemFocusEnable(TRUE);
    m_reportBase.SetFreezeColumnsCount(0);
    m_reportBase.SetFreezeSelectionVisible(TRUE);
    m_reportBase.SetFocusedColIndex(SMVCONFIG_VALUE);

    if(m_reportBase.GetRecordRowCount() > 0)
    {
        m_reportBase.SetFocusedSelectionRowIndex(0);
    }
}

/**
 *      初始化通道报表
 */
void CSMVConfigDlg::_initChannelReport()
{
    ASSERT(m_pCurrentReport->GetSafeHwnd());

    m_pCurrentReport->InsertColumn(0, L"", EL_CENTER, DSM_SMV_CONFIG_INDEX);   // 索引
    m_pCurrentReport->InsertColumn(1, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_DESC), EL_LEFT, DSM_SMV_CONFIG_DESC);       // 通道描述
    m_pCurrentReport->InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_TYPE), EL_CENTER, DSM_SMV_CONFIG_TYPE);     // 类型
    m_pCurrentReport->InsertColumn(3, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_PHASE), EL_CENTER, DSM_SMV_CONFIG_PHASE);   // 相别
    m_pCurrentReport->InsertColumn(4, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_RATE1), EL_LEFT, DSM_SMV_CONFIG_RATE1);     // 一次额定值
    m_pCurrentReport->InsertColumn(5, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_RATE2), EL_LEFT, DSM_SMV_CONFIG_RATE2);     // 二次额定值

    // 设置选择焦点样式
    m_pCurrentReport->SetRowSelectionVisible(FALSE);
    m_pCurrentReport->SetRowFocusEnable(TRUE);
    m_pCurrentReport->SetSubItemFocusEnable(TRUE);
    m_pCurrentReport->SetFreezeSelectionVisible(FALSE);
    m_pCurrentReport->SetFreezeColumnsCount(2);
    m_pCurrentReport->SetFocusedColIndex(2);
    m_pCurrentReport->GetColumnExAttr(4)->GetEditOptions()->SetAllowEdit(TRUE);
    m_pCurrentReport->GetColumnExAttr(5)->GetEditOptions()->SetAllowEdit(TRUE);
}



#if SU_FEI
void CSMVConfigDlg::OnMenuChanged()
{
    if(m_pCurrentReport && m_pCurrentReport->GetSafeHwnd())
    {
        m_pCurrentReport->ShowWindow(SW_HIDE);
    }

    switch(m_menuNavi.GetCurSel())
    {
        // 进入基本设置页面
    case 0:
        _enterBase();
        break;

    case 1:
        // 进入通道设置页面
        _enterChannel();
        break;
    }
}
#endif

/**
 *      进入基本设置
 */
void CSMVConfigDlg::_enterBase()
{
    CString csCaption;
    csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_CAPTION), dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_BASIC_CFG));
    CStatusMgr::SetTitle(csCaption);

#if SU_FEI
    m_menuMain.SetMenuItemBitmaps(0, NULL, NULL, NULL);
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, L"");
#else
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_BASICSETTING_SEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_CHANNELSETTING_UNSEL), NULL, NULL);
#endif

    m_pNaviBar->UpdateNaviMenu();
	m_modeSetting = SETTING_BASIC;
    _updateMenu();


    m_pCurrentReport = &m_reportBase;

    if(m_reportBase.GetSafeHwnd())
    {
        m_reportBase.ShowWindow(SW_SHOW);
    }
    else
    {
        _createReport(m_reportBase, IDC_SMV_CONFIG_REPORT_BASE);
        _updateBase();
    }

#if !SU_FEI
	
	m_pCurrentReport->SetFocus();
	m_modeSetting = SETTING_BASIC;


	if(m_pF6SubTemplate)
	{
		m_menuMain.SetMenuSubItem(5, NULL);
		m_menuMain.SetMenuItemCaption(5, L"");

		m_pF6SubTemplate = NULL;
	}

#endif
}

/**
 *      进入通道设置
 */
void CSMVConfigDlg::_enterChannel()
{
    m_pCurCtrlBlk = CTRL_ONE == m_modeCtrl ? m_pCtrlBlk1 : m_pCtrlBlk2;
    m_pCurrentReport = CTRL_ONE == m_modeCtrl ? &m_reportChannel : &m_reportChannel2;

	m_modeSetting = SETTING_CHANNEL;
    // 更新标题
    _updateCtrlBlkCaption();
    _updateMenu();

    // 显示第一个控制块
    if(m_modeCtrl == CTRL_ONE)
    {
        if(m_reportChannel.GetSafeHwnd())
        {
            m_reportChannel.ShowWindow(SW_SHOW);
        }
        else
        {
            _createReport(m_reportChannel, IDC_SMV_CONFIG_REPORT_CHANNEL);
            _updateChannel();
        }
    }
    // 显示第二个控制块
    else
    {
        if(m_reportChannel2.GetSafeHwnd())
        {
            m_reportChannel2.ShowWindow(SW_SHOW);
        }
        else
        {
            _createReport(m_reportChannel2, IDC_SMV_CONFIG_REPORT_CHANNEL2);
            _updateChannel();
        }
    }
#if !SU_FEI
	
	m_pCurrentReport->SetFocus();

	m_modeSetting = SETTING_CHANNEL;

	/*m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FT3_SEL), NULL, NULL);*/
#if !SU_FEI
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_BASICSETTING_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_CHANNELSETTING_SEL), NULL, NULL);

	m_pNaviBar->UpdateNaviMenu();
#endif

	if(!m_pF6SubTemplate && m_pCurCtrlBlk->enFrameType != FRAME_TYPE_NET_9_1)
	{
		CMenu menu;
		VERIFY(menu.CreatePopupMenu());
		int nItemID = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN;

		// 判断协议类型加载模板子菜单

		// FT3通道模板
		if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_NANRUI || m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_GUOWANG)
		{
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A));      // 博电默认模板
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B));      // 凯默默认模板
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C));      // IEC60044
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A));      // 博电扩展
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B));      // 凯默扩展
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C));      // 本设备FT3

			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN]             = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN ;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 1]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 2]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 3]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 4]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 5]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK;

			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN]      = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 1]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 2]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 3]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 4]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 5]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C;

			m_nBeginID  = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN;
			m_nEndID    = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 6;
		}
		else if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
		{
			nItemID = ID_MENU_SMV_CFG_CHANN_92_BEGIN;

			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG));           // SCL模板
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A));           // 博电92
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D));          // 凯默92
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E));          // 本设备（通道小于13）
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A));          // 《智能变电站原理及测试技术》P62页的18通道模板
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A));          // 本设备

			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN]               = SMV_RECV_CONFIG_TEMPLATE_FROME_SCL;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 1]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 2]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 3]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 4]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK;
			m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 5]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2;

			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN]        = DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 1]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 2]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 3]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 4]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A;
			m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 5]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A;

			m_nBeginID  = ID_MENU_SMV_CFG_CHANN_92_BEGIN;
			m_nEndID    = ID_MENU_SMV_CFG_CHANN_92_BEGIN + 6;
		}

		m_pF6SubTemplate = new CNaviMenu;

		m_pF6SubTemplate->LoadMenu(&menu);
		menu.DestroyMenu();

		m_menuMain.SetMenuSubItem(5, m_pF6SubTemplate);
		CNaviBarMgr::GetInstance()->HighlightMenuItem(5, 0, TRUE);
		m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_TEMPLATE));
	}

	m_pNaviBar->UpdateNaviMenu();
#endif

}



BOOL CSMVConfigDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类


    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_RIGHT:
#if	SU_FEI
			if(GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd() &&
                m_pCurrentReport)
#else
			if (GetFocus()->GetSafeHwnd() == m_pNaviBar->GetSafeHwnd()  && m_pCurrentReport)
#endif
            {
                m_pCurrentReport->SetFocus();
                return TRUE;
            }
            break;

        case VK_LEFT:

            break;

        default:
            break;
        }
    }

    if(GetFocus()->GetSafeHwnd() == m_reportChannel.GetSafeHwnd() || GetFocus()->GetSafeHwnd() == m_reportChannel2.GetSafeHwnd())
    {
        // 根据情况显示F2菜单标题
        _updateMenuF2();
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      根据当前控制块更新smv基本设置报表数据
 */
void CSMVConfigDlg::_updateBase()
{
    if(!m_pDataLayer || !m_pCurCtrlBlk || !m_pDataLayer->GetSmvConfigPublic())
    {
        return;
    }

   // int nDis = 0, nAlign = 0;
	int  nAlign = 0;
    CSmvRecvConfigPublic* pConfig = m_pDataLayer->GetSmvConfigPublic();

    // 更新92类型信息
    if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType)
    {
#if SU_FEI        
		// 一次二次值
        switch(pConfig->GetSmvDispType())
        {
        case SMV_VALUE_TYPE_PRIMARY:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_ONCE;
            break;
        case SMV_VALUE_TYPE_SECONDARY:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_TWICE;
            break;

        case SMV_VALUE_TYPE_ORIGINANL:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_SOURCE;
            break;

        default:
            break;
        }
#endif
        // 对齐方式
        switch(pConfig->GetPhaseCheckAlign())
        {
        case SMV_PHASE_CHECK_ALIGN_TIME:
            nAlign = DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI;
            break;

        case SMV_PHASE_CHECK_ALIGN_INDEX:
            nAlign = DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI;
            break;
        default:
            break;
        }
#if SU_FEI
        CString csSample;
        csSample.Format(L"%u", m_pCurCtrlBlk->nSampleRate);

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);        // 92也允许修改显示值类型
        m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->SetEditable(FALSE);
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nDis));
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));
        m_reportBase.SetRowItemText(1, SMVCONFIG_VALUE, csSample);
#else
		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);        // 92也允许修改显示值类型
		
		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));
#endif
    }
    // 更新91与ft3类型信息
    else if(FRAME_TYPE_NET_9_1 == m_pCurCtrlBlk->enFrameType || 
        FRAME_TYPE_FT3_GUOWANG == m_pCurCtrlBlk->enFrameType || 
        FRAME_TYPE_FT3_NANRUI == m_pCurCtrlBlk->enFrameType)
    {
#if SU_FEI        
		// 一次二次值
        switch(pConfig->GetSmvDispType())
        {
        case SMV_VALUE_TYPE_PRIMARY:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_ONCE;
            break;
        case SMV_VALUE_TYPE_SECONDARY:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_TWICE;
            break;

        case SMV_VALUE_TYPE_ORIGINANL:
            nDis = DSM_STR_SMV_CONFIG_SAMPLE_SOURCE;
            break;

        default:
            break;
        }
#endif
        // 对齐方式
        switch(pConfig->GetPhaseCheckAlign())
        {
        case SMV_PHASE_CHECK_ALIGN_TIME:
            nAlign = DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI;
            break;

        case SMV_PHASE_CHECK_ALIGN_INDEX:
            nAlign = DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI;
            break;
        default:
            break;
        }
#if SU_FEI
        CString csSample;
        csSample.Format(L"%u", m_pCurCtrlBlk->nSampleRate);

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
        m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->SetEditable(FALSE);
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nDis));
        m_reportBase.SetRowItemText(1, SMVCONFIG_VALUE, csSample);
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));
#else
		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);

		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));

#endif
    }
}

/**
 *      根据当前控制块更新smv通道设置数据
 */
void CSMVConfigDlg::_updateChannel()
{
    if(!m_pCurrentReport || !m_pCurrentReport->GetSafeHwnd())
    {
        return;
    }

    CSmvRecvConfig* pConfig = m_pCurCtrlBlk->pRecvConfig;

    if(!pConfig)
    {
        return;
    }

    int nFocus      = m_pCurrentReport->GetFocusedRowIndex();       // 焦点行
    int nTop        = m_pCurrentReport->GetTopRowIndex();           // 顶部可见行
    int nPageCounts = m_pCurrentReport->GetCountPerPage();          // 每页显示行数
    int nVisible    = nTop + nPageCounts - 1;                           // 保持可见行

    if(nVisible >= m_pCurrentReport->GetRecordRowCount())
    {
        nVisible = m_pCurrentReport->GetRecordRowCount() - 1;
    }


    m_pCurrentReport->DeleteAllRecordRow();
    size_t nCounts = pConfig->GetChannelCount();

    CSmvRecvChannelInfo* pInfo = NULL;

    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(i);

        if(!pInfo)
        {
            DSM_DEBUG(L"dsm: null channel info from GetOneChannelInfo(%d)", i);
            continue;
        }

        m_pCurrentReport->InsertRecordRow(m_pCurrentReport->GetRecordRowCount(), new CDsmSmvChannelConfigRecord(pInfo));
    }

    if(nFocus >= 0 && nFocus < m_pCurrentReport->GetRecordRowCount())
    {
        m_pCurrentReport->EnsureRowVisibleScrollRow(nVisible);
        m_pCurrentReport->SetFocusedSelectionRowIndex(nFocus);
    }
    else if(m_pCurrentReport->GetRecordRowCount() > 0)
    {
        m_pCurrentReport->SetFocusedSelectionRowIndex(0);
    }
}

void CSMVConfigDlg::OnItemSelectChangedBase( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    *pResult = TRUE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();
    ASSERT(pCfg);

    // 响应选中下拉列表
    if(ELNM_ITEM_CONSTRAINTCHANGED == pItemNotify->hdr.code)
    {
#if SU_FEI        
		// 修改采样值显示
        if(0 == pItemNotify->rowIndex)
        {
            pCfg->SetSmvDispType((enumSmvValueType)pItemNotify->pConstraint->GetConstraintData());
        }

        // 修改采样频率
        else if(1 == pItemNotify->rowIndex)
        {
            pCfg->SetSmv92SampleRate((SMV92_SAMPLE_RATE)pItemNotify->pConstraint->GetConstraintData());
        }
        // 修改采样点对齐方式
        else if(2 == pItemNotify->rowIndex)
        {
            pCfg->SetPhaseCheckAlign((SMV_PHASE_CHECK_ALIGNMENT)pItemNotify->pConstraint->GetConstraintData());
        }
#else
		// 修改采样点对齐方式
		if(0 == pItemNotify->rowIndex)
		{
			pCfg->SetPhaseCheckAlign((SMV_PHASE_CHECK_ALIGNMENT)pItemNotify->pConstraint->GetConstraintData());
		}

#endif
    }
}

void CSMVConfigDlg::OnItemSelectChangedChannel( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    // 响应选中某项下拉列表
    if(ELNM_ITEM_CONSTRAINTCHANGED == pItemNotify->hdr.code)
    {
        bool bResetDesc     = false;
        bool bOldMeasure    = false;
        bool bNewMeasure    = false;

        CDsmSmvChannelConfigRecord* pRecord = dynamic_cast<CDsmSmvChannelConfigRecord*>(m_pCurrentReport->GetRecordRow(pItemNotify->rowIndex));
        ASSERT(pRecord);
        CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
        ASSERT(pInfo);

        std::wstring sOldDesc   = pInfo->strChannelDesc;
        enumChannelType oldType = pInfo->enChannelType;
        ChannelPhase oldPhase   = (oldType == CHANNEL_TYPE_SMV_TIME) ? CHANNEL_PHASE_UNKOWN : pInfo->enChannelPhase;

        if(oldType == CHANNEL_TYPE_SMV_CURRENT && pInfo->enChannelValueType == CHANNEL_VALUE_TYPE_MEASURE)
        {
            bOldMeasure = true;
        }

        switch(pItemNotify->colIndex)
        {
            // 类型
        case 2:
            bResetDesc = true;
            _changedChannelType(pItemNotify, pResult);
            break;

            // 相别
        case 3:
            bResetDesc = true;
            _changedChannelPhase(pItemNotify, pResult);
            break;

        default:
            break;
        }

        if(bResetDesc)
        {
            enumChannelType newType = pInfo->enChannelType;
            ChannelPhase newPhase   = (newType == CHANNEL_TYPE_SMV_TIME) ? CHANNEL_PHASE_UNKOWN : pInfo->enChannelPhase;

            if(newType == CHANNEL_TYPE_SMV_CURRENT && pInfo->enChannelValueType == CHANNEL_VALUE_TYPE_MEASURE)
            {
                bNewMeasure = true;
            }

            pInfo->strChannelDesc = GenerateChannelDes(sOldDesc, oldType, newType, oldPhase, newPhase, bOldMeasure, bNewMeasure);

            m_pCurrentReport->SetRowItemText(pItemNotify->rowIndex, 1, pInfo->strChannelDesc.c_str());
        }
    }
}

/**
 *      修改通道类型数据
 */
void CSMVConfigDlg::_changedChannelType( ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult )
{
    CDsmSmvChannelConfigRecord* pRecord = 
        dynamic_cast<CDsmSmvChannelConfigRecord*>(
        m_pCurrentReport->GetRecordRow(pNotifyStruct->rowIndex));

    if(!pRecord || !pRecord->GetInfo())
    {
        return;
    }

    CString csValue1, csValue2;
    CELRecordItemText* pText1   = NULL;
    CELRecordItemText* pText2   = NULL;
    CSmvRecvChannelInfo* pInfo  = pRecord->GetInfo();
    enumChannelType eType           = (enumChannelType)pNotifyStruct->pConstraint->GetConstraintData();

    pText1 = dynamic_cast<CELRecordItemText*>(pRecord->GetItem(4));
    pText2 = dynamic_cast<CELRecordItemText*>(pRecord->GetItem(5));

    CParamConfigBasic* pParamCfg = m_pDataLayer->GetParamConfigBasic();
    ASSERT(pParamCfg);

    // 切换时间戳类型
    if(CHANNEL_TYPE_SMV_TIME == eType)
    {
        // 设置时间的默认值
        pInfo->fPrimaryRatedValue      = _ttoi(pParamCfg->GetMuRatedDelay().c_str()) * 1.0f;
        pInfo->fSecondaryRatedValue    = _ttoi(pParamCfg->GetMuRatedDelay().c_str()) * 1.0f;

        // 设置时间单位
        pText1->SetFormatString(L"%s μs");
        pText2->SetFormatString(L"%s μs");

        csValue1.Format(L"%.0f", pInfo->fPrimaryRatedValue);
        csValue2.Format(L"%.0f", pInfo->fSecondaryRatedValue);

        pRecord->GetItem(3)->SetCaption(g_csBlankValue);
        pText1->SetValue(csValue1);
        pText2->SetValue(csValue2);

        pRecord->GetItem(3)->SetEditable(FALSE);
        pText1->SetEditable(FALSE);
        pText2->SetEditable(FALSE);

        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 3);
        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 4);
        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 5);
    }
    else if(CHANNEL_TYPE_SMV_CURRENT == eType || 
        CHANNEL_TYPE_SMV_VOLTAGE == eType)
    {
        int nPhasae;
        CString csDesc(pInfo->strChannelDesc.c_str());

        switch(pInfo->enChannelPhase)
        {
        case CHANNEL_PHASE_A:
            nPhasae = DSM_STR_TYPE_PHASE_A;
            pInfo->enChannelPhase = CHANNEL_PHASE_A;
            break;
        case CHANNEL_PHASE_B:
            nPhasae = DSM_STR_TYPE_PHASE_B;
            pInfo->enChannelPhase = CHANNEL_PHASE_B;
            break;
        case CHANNEL_PHASE_C:
            nPhasae = DSM_STR_TYPE_PHASE_C;
            pInfo->enChannelPhase = CHANNEL_PHASE_C;
            break;
        default:
            nPhasae = DSM_STR_TYPE_PHASE_A;
            pInfo->enChannelPhase = CHANNEL_PHASE_A;
            break;
        }

        //设置电流的单位默认值
        if(CHANNEL_TYPE_SMV_CURRENT == eType)
        {
            pText1->SetFormatString(L"%s A");
            pText2->SetFormatString(L"%s A");

            pInfo->fPrimaryRatedValue   = pParamCfg->GetRatedPrimaryCurrent();
            pInfo->fSecondaryRatedValue = pParamCfg->GetRatedSecondaryCurrent();

            pInfo->enChannelValueType   = CHANNEL_VALUE_TYPE_PROTECT;
        }
        // 设置电压的单位与默认值
        else
        {
            pText1->SetFormatString(L"%s kV");
            pText2->SetFormatString(L"%s V");

            pInfo->fPrimaryRatedValue   = pParamCfg->GetRatedPrimaryVoltage() * 1000;
            pInfo->fSecondaryRatedValue = pParamCfg->GetRatedSecondaryVoltage();
        }

        // 电压一次值换算
        if(CHANNEL_TYPE_SMV_VOLTAGE == eType)
        {
            csValue1.Format(L"%.3f", (float)(pInfo->fPrimaryRatedValue / 1000));
        }
        else
        {
            csValue1.Format(L"%.3f", (float)(pInfo->fPrimaryRatedValue));
        }

        csValue2.Format(L"%.3f", pInfo->fSecondaryRatedValue);

        pRecord->GetItem(3)->SetCaption(dsm_stringMgr::Get(nPhasae));
        pText1->SetValue(csValue1);
        pText2->SetValue(csValue2);

        pRecord->GetItem(3)->SetEditable(TRUE);
        pRecord->GetItem(4)->SetEditable(TRUE);
        pRecord->GetItem(5)->SetEditable(TRUE);

        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 3);
        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 4);
        m_pCurrentReport->RedrawRowItem(pNotifyStruct->rowIndex, 5);

        m_pCurrentReport->SetRowItemText(pNotifyStruct->rowIndex, 1, csDesc);
    }

    pInfo->enChannelType = (enumChannelType)pNotifyStruct->pConstraint->GetConstraintData();

    _updateMenuF2();

    *pResult = TRUE;
}


/**
 *      修改通道相别数据
 */
void CSMVConfigDlg::_changedChannelPhase( ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult )
{
    CDsmSmvChannelConfigRecord* pRecord = 
        dynamic_cast<CDsmSmvChannelConfigRecord*>(
        m_pCurrentReport->GetRecordRow(pNotifyStruct->rowIndex));

    if(!pRecord)
    {
        return;
    }

    pRecord->GetInfo()->enChannelPhase = (ChannelPhase)pNotifyStruct->pConstraint->GetConstraintData();

    *pResult = TRUE;
}

void CSMVConfigDlg::OnItemValueChangedChannel( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;
    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);

    CDsmSmvChannelConfigRecord* pRecord = dynamic_cast<CDsmSmvChannelConfigRecord*>(m_pCurrentReport->GetRecordRow(pItemNotify->rowIndex));

    if(!pItem || !pRecord || !pRecord->GetInfo())
    {
        return;
    }

    // 一次额定值
    if(4 == pItemNotify->colIndex)
    {
        CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
        ASSERT(pInfo);

        float fVal;
        CString cs(L"");

        // 设置电压一次额定值
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_VOLT_1_INT, DSM_FLOAT_VOLT_1_DECI))
            {
                pInfo->fPrimaryRatedValue = fVal * 1000.0f;
            }

            cs.Format(L"%.3f", (float)(pInfo->fPrimaryRatedValue / 1000.0f));
        }
        // 设置电流一次额定值
        else if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_AMPER_1_INT, DSM_FLOAT_AMPER_1_DECI))
            {
                pInfo->fPrimaryRatedValue = fVal;
            }

            cs.Format(L"%.3f", pInfo->fPrimaryRatedValue);
        }
        else
        {
            ASSERT(FALSE);
            cs.Format(L"%.3f", pInfo->fPrimaryRatedValue);
        }

        pItem->SetValue(cs);
    }
    // 二次额定值
    else if(5 == pItemNotify->colIndex)
    {
        CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
        ASSERT(pInfo);

        float fVal;
        CString cs(L"");

        // 设置电压二次额定值
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_VOLT_2_INT, DSM_FLOAT_VOLT_2_DECI))
            {
                pInfo->fSecondaryRatedValue = fVal;
            }
        }
        // 设置电流二次额定值
        else if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_AMPER_2_INT, DSM_FLOAT_AMPER_2_DECI))
            {
                pInfo->fSecondaryRatedValue = fVal;
            }
        }
        else
        {
            ASSERT(FALSE);
        }

        cs.Format(L"%.3f", pInfo->fSecondaryRatedValue);
        pItem->SetValue(cs);
    }

    *pResult = TRUE;
}


/**
 *     判断报表控件是否允许交出焦点
 */
bool CSMVConfigDlg::_allowKillFocus()
{
    if(!m_pCurrentReport->IsWindowVisible())
    {
        return true;
    }

#if SU_FEI
    int nSel = m_menuNavi.GetCurSel();

    ASSERT(nSel >= 0 && nSel < m_menuNavi.GetCount());

    int nFocCol         = m_pCurrentReport->GetFocusedColIndex();
    DWORD_PTR dwData    = m_menuNavi.GetItemData(nSel);

    // 基本设置
    if(DSM_SMV_CONFIG_NAVI_BASE == dwData)
    {
        // 0，1冻结
        if(nFocCol > 2)
        {
            return false;
        }
    }
    // 通道设置
    else if(DSM_SMV_CONFIG_NAVI_CHANNEL == dwData)
    {
        // 0, 1冻结
        if(nFocCol > 2)
        {
            return false;
        }
    }
#endif
    return true;
}

void CSMVConfigDlg::OnMenuF1()
{
#if SU_FEI
    if(m_menuNavi.GetCurSel() == 1)
   {
        if(m_modePage == PAGE_DUALPHASE)
        {
            // 切换核相两个控制块信息的显示
            m_modeCtrl = m_modeCtrl == CTRL_ONE ? CTRL_TWO : CTRL_ONE;

            if(m_pCurrentReport && m_pCurrentReport->GetSafeHwnd())
            {
                m_pCurrentReport->ShowWindow(SW_HIDE);
            }

            _enterChannel();
            m_pCurrentReport->SetFocus();
        }
    }
#else
	if(m_pCurrentReport && m_pCurrentReport->GetSafeHwnd())
	{
		m_pCurrentReport->ShowWindow(SW_HIDE);
	}
	
	
		_enterBase();
		m_pCurrentReport->SetFocus();
	

	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_DEFNAVIBK), dsm_bmpmgr::GetCB(DSM_BMP_DEFNAVIBK), NULL);

#endif
	
}




void CSMVConfigDlg::_modifyMenu()
{

	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_BASICSETTING_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_BASICSETTING_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_CHANNELSETTING_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_CHANNELSETTING_SEL), NULL);

}


/**
 *      更新控制块标题
 */
void CSMVConfigDlg::_updateCtrlBlkCaption()
{
    ASSERT(m_pCurCtrlBlk);

    CString csCaption;
    CString csDesc = L"";

    // 9X标题
    if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType || FRAME_TYPE_NET_9_1 == m_pCurCtrlBlk->enFrameType)
    {
        CNetDetectItem* pNetCtrlBlk = dynamic_cast<CNetDetectItem*>(m_pCurCtrlBlk);
        ASSERT(pNetCtrlBlk);

        // 92可以从scl配置读取描述信息
        if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType)
        {
            CIEC9_2Cb* pCB = m_pDataLayer->GetSclManager()->GetIEC9_2Cb(pNetCtrlBlk->appId);

            if(pCB)
            {
                csDesc = pCB->GetDescription().c_str();
            }
        }

        // 若描述信息为空，则取appid : port 信息
        if(csDesc.IsEmpty())
        {
            csDesc.Format(L"0x%04X : %s", pNetCtrlBlk->appId, DSM_GetPortDesc(m_pCurCtrlBlk->enPortName));
        }
    }
    else if(FRAME_TYPE_FT3_NANRUI == m_pCurCtrlBlk->enFrameType || FRAME_TYPE_FT3_GUOWANG == m_pCurCtrlBlk->enFrameType)
    {
        CFt3DetectItem* pFT3CtrlBlk = dynamic_cast<CFt3DetectItem*>(m_pCurCtrlBlk);
        ASSERT(pFT3CtrlBlk);

        csDesc.Format(L"0x%04X : %s", pFT3CtrlBlk->nLDName, DSM_GetPortDesc(m_pCurCtrlBlk->enPortName));
    }

    csCaption.Format(L"%s->%s [%s]", dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_CAPTION), dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_CHANNEL_CFG), csDesc);

    CStatusMgr::SetTitle(csCaption);
}

/**
 *      更新底部菜单显示
 */


void CSMVConfigDlg::_updateMenu()
{
    if(PAGE_NORMAL == m_modePage)
    {
#if SU_FEI
        m_menuMain.SetMenuItemBitmaps(0, NULL, NULL, NULL);
#else
		m_menuMain.SetMenuItemBitmaps(2, NULL, NULL, NULL);
#endif
    }
    else
    {
		if (m_modeSetting == SETTING_CHANNEL)
		{
		

			if(CTRL_ONE == m_modeCtrl)
			{
#if SU_FEI
				m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV1SMV2_SMV1), NULL, NULL);
#else
				m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SMV1_SEL), NULL, NULL);
#endif
			}
			else
			{
#if SU_FEI
				m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV1SMV2_SMV1), NULL, NULL);
#else
				m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SMV2_SEL), NULL, NULL);
#endif
			}
		}
		else
		{

				m_menuMain.SetMenuItemBitmaps(2, NULL, NULL, NULL);
		}
    }

#if SU_FEI
    int nSel = m_menuNavi.GetCurSel();
    if(nSel == 0)
    {
        // 取消F6的模板子菜单
        if(m_pF6SubTemplate)
        {
            m_menuMain.SetMenuSubItem(5, NULL);
            m_menuMain.SetMenuItemCaption(5, L"");

            m_pF6SubTemplate = NULL;
        }
    }
    else if(nSel == 1)
    {
        if(!m_pF6SubTemplate && m_pCurCtrlBlk->enFrameType != FRAME_TYPE_NET_9_1)
        {
            CMenu menu;
            VERIFY(menu.CreatePopupMenu());
            int nItemID = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN;

            // 判断协议类型加载模板子菜单

            // FT3通道模板
            if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_NANRUI || m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_GUOWANG)
            {
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A));      // 博电默认模板
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B));      // 凯默默认模板
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C));      // IEC60044
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A));      // 博电扩展
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B));      // 凯默扩展
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C));      // 本设备FT3

                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN]             = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_BODIAN ;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 1]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_KAIMO;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 2]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_STD_60044_8;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 3]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BODIAN;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 4]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_KAIMO;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 5]         = SMV_RECV_CONFIG_TEMPLATE_TYPE_FT3_EXT_BOOK;

                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN]      = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 1]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 2]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 3]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 4]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 5]  = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C;

                m_nBeginID  = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN;
                m_nEndID    = ID_MENU_SMV_CFG_CHANN_FT3_BENGIN + 6;
            }
            else if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
            {
                nItemID = ID_MENU_SMV_CFG_CHANN_92_BEGIN;

                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG));           // SCL模板
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A));           // 博电92
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D));          // 凯默92
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E));          // 本设备（通道小于13）
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A));          // 《智能变电站原理及测试技术》P62页的18通道模板
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A));          // 本设备

                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN]               = SMV_RECV_CONFIG_TEMPLATE_FROME_SCL;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 1]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BODIAN;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 2]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_KAIMO;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 3]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_LESS_CHANNEL;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 4]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2_BOOK;
                m_mapTemplate[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 5]           = SMV_RECV_CONFIG_TEMPLATE_TYPE_9_2;

                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN]        = DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 1]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 2]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 3]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 4]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A;
                m_mapTemplateCaption[ID_MENU_SMV_CFG_CHANN_92_BEGIN + 5]    = DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A;

                m_nBeginID  = ID_MENU_SMV_CFG_CHANN_92_BEGIN;
                m_nEndID    = ID_MENU_SMV_CFG_CHANN_92_BEGIN + 6;
            }

            m_pF6SubTemplate = new CNaviMenu;

            m_pF6SubTemplate->LoadMenu(&menu);
            menu.DestroyMenu();

            m_menuMain.SetMenuSubItem(5, m_pF6SubTemplate);
            CNaviBarMgr::GetInstance()->HighlightMenuItem(5, 0);
            m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_TEMPLATE));
        }
    }
#endif

    m_pNaviBar->UpdateNaviMenu();
}



void CSMVConfigDlg::OnMenuF2()
{
    //  不是通道设置则返回
#if SU_FEI
    if(m_menuNavi.GetCurSel() != 1 || GetFocus()->GetSafeHwnd() != m_pCurrentReport->GetSafeHwnd())
    {
        return;
    }

    if(FRAME_TYPE_NET_9_1 != m_pCurCtrlBlk->enFrameType && 
        FRAME_TYPE_FT3_NANRUI != m_pCurCtrlBlk->enFrameType && 
        FRAME_TYPE_FT3_GUOWANG != m_pCurCtrlBlk->enFrameType)
    {
        return;
    }

    ASSERT(m_pCurrentReport);

    int nSelRow = m_pCurrentReport->GetFocusedRowIndex();
    ASSERT(nSelRow >=0 && nSelRow < m_pCurrentReport->GetRecordRowCount());

    CDsmSmvChannelConfigRecord* pRecord = dynamic_cast<CDsmSmvChannelConfigRecord*>(m_pCurrentReport->GetRecordRow(nSelRow));
    ASSERT(pRecord);

    CSmvRecvChannelInfo* pChannel = pRecord->GetInfo();
    ASSERT(pChannel);

    if(CHANNEL_TYPE_SMV_CURRENT != pChannel->enChannelType)
    {
        return;
    }

    // 设置或取消测量电流属性
    if(CHANNEL_VALUE_TYPE_MEASURE == pChannel->enChannelValueType)
    {
        pChannel->enChannelValueType = CHANNEL_VALUE_TYPE_PROTECT;
    }
    else
    {
        pChannel->enChannelValueType = CHANNEL_VALUE_TYPE_MEASURE;
    }

    // 设置电流通道的描述信息
    CString csChannelDesc;

    if(CHANNEL_TYPE_SMV_CURRENT == pChannel->enChannelType)
    {
        // 保护电流
        if(CHANNEL_VALUE_TYPE_PROTECT == pChannel->enChannelValueType)
        {
            m_pCurrentReport->SetRowItemText(nSelRow, 2, dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
        }
        // 测试电流
        else if(CHANNEL_VALUE_TYPE_MEASURE == pChannel->enChannelValueType)
        {
            CString csType;
            csType.Format(L"%s (%s)", dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER), dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_MEASURE));

            m_pCurrentReport->SetRowItemText(nSelRow, 2, csType);
        }
    }
#else
	
	if(m_pCurrentReport && m_pCurrentReport->GetSafeHwnd())
	{
		m_pCurrentReport->ShowWindow(SW_HIDE);

		
	}
	
		
	_enterChannel();
	
	
	m_pCurrentReport->SetFocus();
	
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

#endif



}
/***
*
*此函数的意义？？
*
*****/

void CSMVConfigDlg::_updateMenuF2()
{
    if(FRAME_TYPE_NET_9_1 != m_pCurCtrlBlk->enFrameType && 
        FRAME_TYPE_FT3_NANRUI != m_pCurCtrlBlk->enFrameType && 
        FRAME_TYPE_FT3_GUOWANG != m_pCurCtrlBlk->enFrameType)
    {
        return;
    }

  /*  int nSelRow = m_pCurrentReport->GetFocusedRowIndex();
    ASSERT(nSelRow >=0 && nSelRow < m_pCurrentReport->GetRecordRowCount());

    CDsmSmvChannelConfigRecord* pRecord = dynamic_cast<CDsmSmvChannelConfigRecord*>(m_pCurrentReport->GetRecordRow(nSelRow));
    if(!pRecord)
    {
        return;
    }

    CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
    ASSERT(pInfo);

    if(CHANNEL_TYPE_SMV_CURRENT != pRecord->GetInfo()->enChannelType)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, L"");
        m_pNaviBar->UpdateNaviMenu();
        return;
    }*/
	
    //int nCol = m_pCurrentReport->GetFocusedColIndex();

    //// 判断是否选中通道属性列
    //if(nCol == 2)
    //{
    //    if(CHANNEL_VALUE_TYPE_PROTECT == pInfo->enChannelValueType)
    //    {
    //        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_MEASUREAMPER));
    //    }
    //    else if(CHANNEL_VALUE_TYPE_MEASURE == pInfo->enChannelValueType)
    //    {
    //        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_PROTECTAMPER));
    //    }
    //}
    //else
    //{
    //    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, L"");
    //}

    //m_pNaviBar->UpdateNaviMenu();
}



BOOL CSMVConfigDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    int nMenuID = LOWORD(wParam);

    if((DWORD)nMenuID >= m_nBeginID && (DWORD)nMenuID < m_nEndID)
    {
        // 更换通道模板
        if(_changeTemplate(nMenuID))
        {
            // 更新菜单
            _updateChannel();
        }
        // 更换通道模板失败
        else
        {
            CString csMsg(dsm_stringMgr::Get(DSM_STR_INFO_TEMPLATE_SET_ERROR));
            mapTemplateCap::iterator it = m_mapTemplateCaption.find(nMenuID);

            if(it != m_mapTemplateCaption.end())
            {
                csMsg.Format(dsm_stringMgr::Get(DSM_STR_INFO_TEMPLATE_SET_ERROR_TIP), dsm_stringMgr::Get(it->second));
            }

            CStatusMgr::SetTips(csMsg);
        }

        return TRUE;
    }

    return __super::OnCommand(wParam, lParam);
}

BOOL CSMVConfigDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= m_nBeginID && nID < m_nEndID)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/**
 *      更改通道的模板
 *
 *  \param[in]
 *      nMenuID     命令ID
 *  \return
 *      bool        更改成功返回true，失败返回false
 */
bool CSMVConfigDlg::_changeTemplate( UINT nMenuID )
{
    mapTemplate::iterator it = m_mapTemplate.find(nMenuID);

    if(it != m_mapTemplate.end())
    {
        return m_pDataLayer->SetSmvRecvChannelTemplate(it->second, m_pCurCtrlBlk->pRecvConfig);
    }

    return false;
}

void CSMVConfigDlg::OnInplaceEditAction( NMHDR * pNotifyStruct, LRESULT* pResult )
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_pCurrentReport->GetInplaceEdit());
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}


/**
*	F3功能按键
*
*/
#if !SU_FEI
void CSMVConfigDlg::OnMenuF3()
{
	if(m_modeSetting == SETTING_CHANNEL)
	{
		if(m_modePage == PAGE_DUALPHASE)
		{
			// 切换核相两个控制块信息的显示
			m_modeCtrl = m_modeCtrl == CTRL_ONE ? CTRL_TWO : CTRL_ONE;

			if(m_pCurrentReport && m_pCurrentReport->GetSafeHwnd())
			{
				m_pCurrentReport->ShowWindow(SW_HIDE);
			}

			_enterChannel();
			m_pCurrentReport->SetFocus();
		}
	}
}


void CSMVConfigDlg::OnMenuF6()
{
	if(m_modeSetting == SETTING_CHANNEL)
	{
		_enterChannel();
		m_pCurrentReport->SetFocus();
			
	}
}

#endif