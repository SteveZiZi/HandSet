/**
 * \file
 *      SMVConfigDlg.cpp
 *
 * \brief
 *      ����ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/22
 */

// SMVConfigDlg.cpp :  ʵ���ļ�
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
// smv - ����
#define DSM_SMV_CONFIG_INDEX                50              ///< ����
#define DSM_SMV_CONFIG_DESC                 129            ///< ͨ������
#define DSM_SMV_CONFIG_TYPE                 70              ///< ͨ������
#define DSM_SMV_CONFIG_PHASE                60              ///< ͨ����λ
#define DSM_SMV_CONFIG_RATE1                110             ///< ͨ��һ��ֵ
#define DSM_SMV_CONFIG_RATE2                110             ///< ͨ������ֵ


#define DSM_SMV_CONFIG_NAVI_BASE            0x01            ///< ��ർ��-����data
#define DSM_SMV_CONFIG_NAVI_CHANNEL         0x02            ///< ��ർ��-ͨ������data

#define SMVCONFIG_CAPTION                   0               ///< ������
#define SMVCONFIG_VALUE                     1               ///< ֵ��


/**
 *      smv���� - ͨ������
 *
 *  \note
 *      ��š�ͨ�����������͡����һ�ζֵ�����ζֵ
 */
class CDsmSmvChannelConfigRecord : public CELRecord
{
public:
    CDsmSmvChannelConfigRecord(CSmvRecvChannelInfo* pInfo);
    virtual ~CDsmSmvChannelConfigRecord() {};

public:

    /**
     *      ��ȡ��¼������ͨ����Ϣָ��
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

    // ͨ������
    switch(pInfo->enChannelType)
    {
        // ��������
    case CHANNEL_TYPE_SMV_CURRENT:
        nType = DSM_STR_TYPE_DATA_AMPER;
        csPrimary.Format(L"%.3f", m_pInfo->fPrimaryRatedValue);
        csSecond.Format(L"%.3f", m_pInfo->fSecondaryRatedValue);
        break;

        // ��ѹ����
    case CHANNEL_TYPE_SMV_VOLTAGE:
        nType = DSM_STR_TYPE_DATA_VOLT;
        csPrimary.Format(L"%.3f", (float)(m_pInfo->fPrimaryRatedValue / 1000.0f));
        csSecond.Format(L"%.3f", m_pInfo->fSecondaryRatedValue);
        break;

        // ʱ������
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

    // �����Ϣ
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

    // ͨ������
    CString csType(dsm_stringMgr::Get(nType));

    if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType && CHANNEL_VALUE_TYPE_MEASURE == pInfo->enChannelValueType)
    {
        csType.Format(L"%s (%s)", csType, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_MEASURE));
    }

    AddItem(new CDsmRecordItem(m_pInfo->nIndex + 1));                   // ���
    AddItem(new CDsmRecordItem(m_pInfo->strChannelDesc.c_str()));       // ����

    CELRecordItem* pItemType    = AddItem(new CDsmRecordItem(csType));       // ����

    if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
    {
        nPhasae = DSM_STR_TYPE_PHASE_UNKNOWN;
    }

    CELRecordItem* pItemPhasae  = AddItem(new CDsmRecordItem(dsm_stringMgr::Get(nPhasae)));     // ���

    CELRecordItem* pItem1       = dynamic_cast<CELRecordItemText*>(AddItem(new CELRecordItemText(csPrimary)));      // һ�ζֵ
    CELRecordItem* pItem2       = dynamic_cast<CELRecordItemText*>(AddItem(new CELRecordItemText(csSecond)));       // ���ζֵ

    // ����ͨ�����������䵥λ
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
        pItem1->SetFormatString(L"%s ��s");
        pItem2->SetFormatString(L"%s ��s");
        break;

    default:
        break;
    }

    // ��������б� ��������ѹ��ʱ���
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

    // �������б� A�ࡢB�ࡢC��
    pItemPhasae->GetEditOptions(NULL)->AddComboButton();
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_A), CHANNEL_PHASE_A);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_B), CHANNEL_PHASE_B);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_C), CHANNEL_PHASE_C);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_N), CHANNEL_PHASE_N);
    pItemPhasae->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_TYPE_PHASE_X), CHANNEL_PHASE_X);

    BOOL bEdit = TRUE;

    // ����ʱ�����ͣ������һ�Ρ�����ֵ������focus
    if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
    {
        bEdit = FALSE;
    }

    pItem1->SetEditable(bEdit);
    pItem2->SetEditable(bEdit);
    pItemPhasae->SetEditable(bEdit);
}



// CSMVConfigDlg �Ի���

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


// CSMVConfigDlg ��Ϣ�������

BOOL CSMVConfigDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    if(!m_pCurrentCtrlBlk)
    {
        DSM_DEBUG(L"dsm: null ctrlblk in smv config\n");
    }

    // ��ȡ��һ��ҳ�������
    CPageData* pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    ASSERT(pPageData);

    // ���Ǻ���ҳ�棬�����ú�����������ƿ飬֮����ʾ�������ƿ��ͨ����Ϣ
    if(pPageData && pPageData->m_nPageID == CSMVDualphaseDlg::IDD)
    {
        m_modePage = PAGE_DUALPHASE;        // ���õ�ǰ�Ǻ���ͨ������ģʽ
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
    _createLeftMenu();      // �������˵�

    _enterBase();           // Ĭ�Ͻ���������ù���
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
    // �쳣: OCX ����ҳӦ���� FALSE
}

/**
 *      �������˵�
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

    // ��ӻ���������ͨ�����ò˵�
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_BASIC_CFG), DSM_SMV_CONFIG_NAVI_BASE);
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_CHANNEL_CFG), DSM_SMV_CONFIG_NAVI_CHANNEL);

    m_menuNavi.SetCurSel(0);
    m_menuNavi.SetFocus();
}

#endif
/**
 *      ��������
 *
 *  \param[in]
 *      report          ����������
 *  \param[in]
 *      nID             �����������ID
 */
void CSMVConfigDlg::_createReport( CReportH& report, UINT nID )
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    // �����������ñ���
    if(IDC_SMV_CONFIG_REPORT_BASE == nID)
    {
        rcReport.bottom = rcReport.top + ELT_T1_ROWHEIGHT * 1;

        report.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

        // ��������
        report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
        report.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

        // ���ú��򱨱���ʽ
        report.PostInitTemplateStyle();

        _initBaseReport();
    }
    else if(IDC_SMV_CONFIG_REPORT_CHANNEL == nID || IDC_SMV_CONFIG_REPORT_CHANNEL2 == nID)
    {
        int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
        rcReport.bottom = rcReport.top + ELT_T1_ROWHEIGHT * nRows;

        report.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

        // ��������
        report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
        report.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

        // ���ú��򱨱���ʽ
        report.PostInitTemplateStyle();

        _initChannelReport();
    }
}


/**
 *      ��ʼ����������
 */
void CSMVConfigDlg::_initBaseReport()
{
    m_reportBase.SetHeaderHeight(0);

    m_reportBase.InsertColumn(SMVCONFIG_CAPTION, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION);   // ����
    m_reportBase.InsertColumn(SMVCONFIG_VALUE, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);     // ����

    // ��һ�� ����ֵ��ʾ
   /* m_reportBase.InsertRecordRow(0, new CDsmRecord(
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_VAL), 
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ONCE)));*/

    //// �ڶ��� ����ֵƵ��
    //m_reportBase.InsertRecordRow(1, new CDsmRecord(
    //    dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_FREQU), 
    //    dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_4K)));

    // ������ ��������뷽ʽ
		m_reportBase.InsertRecordRow(0, new CDsmRecord(
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ALIGN), 
        dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI)));
#if SU_FEI  
    // ���ò���ֵ��ʾinplacelist
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_ONCE), SMV_VALUE_TYPE_PRIMARY);
    m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_TWICE), SMV_VALUE_TYPE_SECONDARY);
  
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SOURCE), SMV_VALUE_TYPE_ORIGINANL);

    // ���ò���ֵƵ��inplacelist
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_4K), SMV92_SAMPLE_RATE_4K);
    m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_12P8K), SMV92_SAMPLE_RATE_12P8K);

    // ���ò�������뷽ʽinplacelist
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI), SMV_PHASE_CHECK_ALIGN_INDEX);
    m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI), SMV_PHASE_CHECK_ALIGN_TIME);
#else
	// ���ò�������뷽ʽinplacelist
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddComboButton();
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_SEQUALI), SMV_PHASE_CHECK_ALIGN_INDEX);
	m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_SAMPLE_CLOCKALI), SMV_PHASE_CHECK_ALIGN_TIME);

#endif
    // ����ѡ�񽹵���ʽ
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
 *      ��ʼ��ͨ������
 */
void CSMVConfigDlg::_initChannelReport()
{
    ASSERT(m_pCurrentReport->GetSafeHwnd());

    m_pCurrentReport->InsertColumn(0, L"", EL_CENTER, DSM_SMV_CONFIG_INDEX);   // ����
    m_pCurrentReport->InsertColumn(1, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_DESC), EL_LEFT, DSM_SMV_CONFIG_DESC);       // ͨ������
    m_pCurrentReport->InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_TYPE), EL_CENTER, DSM_SMV_CONFIG_TYPE);     // ����
    m_pCurrentReport->InsertColumn(3, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_PHASE), EL_CENTER, DSM_SMV_CONFIG_PHASE);   // ���
    m_pCurrentReport->InsertColumn(4, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_RATE1), EL_LEFT, DSM_SMV_CONFIG_RATE1);     // һ�ζֵ
    m_pCurrentReport->InsertColumn(5, dsm_stringMgr::Get(DSM_STR_SMV_CONFIG_R_RATE2), EL_LEFT, DSM_SMV_CONFIG_RATE2);     // ���ζֵ

    // ����ѡ�񽹵���ʽ
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
        // �����������ҳ��
    case 0:
        _enterBase();
        break;

    case 1:
        // ����ͨ������ҳ��
        _enterChannel();
        break;
    }
}
#endif

/**
 *      �����������
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
 *      ����ͨ������
 */
void CSMVConfigDlg::_enterChannel()
{
    m_pCurCtrlBlk = CTRL_ONE == m_modeCtrl ? m_pCtrlBlk1 : m_pCtrlBlk2;
    m_pCurrentReport = CTRL_ONE == m_modeCtrl ? &m_reportChannel : &m_reportChannel2;

	m_modeSetting = SETTING_CHANNEL;
    // ���±���
    _updateCtrlBlkCaption();
    _updateMenu();

    // ��ʾ��һ�����ƿ�
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
    // ��ʾ�ڶ������ƿ�
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

		// �ж�Э�����ͼ���ģ���Ӳ˵�

		// FT3ͨ��ģ��
		if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_NANRUI || m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_GUOWANG)
		{
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A));      // ����Ĭ��ģ��
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B));      // ��ĬĬ��ģ��
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C));      // IEC60044
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A));      // ������չ
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B));      // ��Ĭ��չ
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C));      // ���豸FT3

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

			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG));           // SCLģ��
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A));           // ����92
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D));          // ��Ĭ92
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E));          // ���豸��ͨ��С��13��
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A));          // �����ܱ��վԭ�����Լ�����P62ҳ��18ͨ��ģ��
			menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A));          // ���豸

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
    // TODO: �ڴ����ר�ô����/����û���


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
        // ���������ʾF2�˵�����
        _updateMenuF2();
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      ���ݵ�ǰ���ƿ����smv�������ñ�������
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

    // ����92������Ϣ
    if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType)
    {
#if SU_FEI        
		// һ�ζ���ֵ
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
        // ���뷽ʽ
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

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);        // 92Ҳ�����޸���ʾֵ����
        m_reportBase.GetRecordRow(1)->GetItem(SMVCONFIG_VALUE)->SetEditable(FALSE);
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);

        m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nDis));
        m_reportBase.GetRecordRow(2)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));
        m_reportBase.SetRowItemText(1, SMVCONFIG_VALUE, csSample);
#else
		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetEditable(TRUE);        // 92Ҳ�����޸���ʾֵ����
		
		m_reportBase.GetRecordRow(0)->GetItem(SMVCONFIG_VALUE)->SetCaption(dsm_stringMgr::Get(nAlign));
#endif
    }
    // ����91��ft3������Ϣ
    else if(FRAME_TYPE_NET_9_1 == m_pCurCtrlBlk->enFrameType || 
        FRAME_TYPE_FT3_GUOWANG == m_pCurCtrlBlk->enFrameType || 
        FRAME_TYPE_FT3_NANRUI == m_pCurCtrlBlk->enFrameType)
    {
#if SU_FEI        
		// һ�ζ���ֵ
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
        // ���뷽ʽ
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
 *      ���ݵ�ǰ���ƿ����smvͨ����������
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

    int nFocus      = m_pCurrentReport->GetFocusedRowIndex();       // ������
    int nTop        = m_pCurrentReport->GetTopRowIndex();           // �����ɼ���
    int nPageCounts = m_pCurrentReport->GetCountPerPage();          // ÿҳ��ʾ����
    int nVisible    = nTop + nPageCounts - 1;                           // ���ֿɼ���

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

    // ��Ӧѡ�������б�
    if(ELNM_ITEM_CONSTRAINTCHANGED == pItemNotify->hdr.code)
    {
#if SU_FEI        
		// �޸Ĳ���ֵ��ʾ
        if(0 == pItemNotify->rowIndex)
        {
            pCfg->SetSmvDispType((enumSmvValueType)pItemNotify->pConstraint->GetConstraintData());
        }

        // �޸Ĳ���Ƶ��
        else if(1 == pItemNotify->rowIndex)
        {
            pCfg->SetSmv92SampleRate((SMV92_SAMPLE_RATE)pItemNotify->pConstraint->GetConstraintData());
        }
        // �޸Ĳ�������뷽ʽ
        else if(2 == pItemNotify->rowIndex)
        {
            pCfg->SetPhaseCheckAlign((SMV_PHASE_CHECK_ALIGNMENT)pItemNotify->pConstraint->GetConstraintData());
        }
#else
		// �޸Ĳ�������뷽ʽ
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

    // ��Ӧѡ��ĳ�������б�
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
            // ����
        case 2:
            bResetDesc = true;
            _changedChannelType(pItemNotify, pResult);
            break;

            // ���
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
 *      �޸�ͨ����������
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

    // �л�ʱ�������
    if(CHANNEL_TYPE_SMV_TIME == eType)
    {
        // ����ʱ���Ĭ��ֵ
        pInfo->fPrimaryRatedValue      = _ttoi(pParamCfg->GetMuRatedDelay().c_str()) * 1.0f;
        pInfo->fSecondaryRatedValue    = _ttoi(pParamCfg->GetMuRatedDelay().c_str()) * 1.0f;

        // ����ʱ�䵥λ
        pText1->SetFormatString(L"%s ��s");
        pText2->SetFormatString(L"%s ��s");

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

        //���õ����ĵ�λĬ��ֵ
        if(CHANNEL_TYPE_SMV_CURRENT == eType)
        {
            pText1->SetFormatString(L"%s A");
            pText2->SetFormatString(L"%s A");

            pInfo->fPrimaryRatedValue   = pParamCfg->GetRatedPrimaryCurrent();
            pInfo->fSecondaryRatedValue = pParamCfg->GetRatedSecondaryCurrent();

            pInfo->enChannelValueType   = CHANNEL_VALUE_TYPE_PROTECT;
        }
        // ���õ�ѹ�ĵ�λ��Ĭ��ֵ
        else
        {
            pText1->SetFormatString(L"%s kV");
            pText2->SetFormatString(L"%s V");

            pInfo->fPrimaryRatedValue   = pParamCfg->GetRatedPrimaryVoltage() * 1000;
            pInfo->fSecondaryRatedValue = pParamCfg->GetRatedSecondaryVoltage();
        }

        // ��ѹһ��ֵ����
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
 *      �޸�ͨ���������
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

    // һ�ζֵ
    if(4 == pItemNotify->colIndex)
    {
        CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
        ASSERT(pInfo);

        float fVal;
        CString cs(L"");

        // ���õ�ѹһ�ζֵ
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_VOLT_1_INT, DSM_FLOAT_VOLT_1_DECI))
            {
                pInfo->fPrimaryRatedValue = fVal * 1000.0f;
            }

            cs.Format(L"%.3f", (float)(pInfo->fPrimaryRatedValue / 1000.0f));
        }
        // ���õ���һ�ζֵ
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
    // ���ζֵ
    else if(5 == pItemNotify->colIndex)
    {
        CSmvRecvChannelInfo* pInfo = pRecord->GetInfo();
        ASSERT(pInfo);

        float fVal;
        CString cs(L"");

        // ���õ�ѹ���ζֵ
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            if(DSM_Str2Float(pItem->GetValue(), fVal, DSM_FLOAT_VOLT_2_INT, DSM_FLOAT_VOLT_2_DECI))
            {
                pInfo->fSecondaryRatedValue = fVal;
            }
        }
        // ���õ������ζֵ
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
 *     �жϱ���ؼ��Ƿ�����������
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

    // ��������
    if(DSM_SMV_CONFIG_NAVI_BASE == dwData)
    {
        // 0��1����
        if(nFocCol > 2)
        {
            return false;
        }
    }
    // ͨ������
    else if(DSM_SMV_CONFIG_NAVI_CHANNEL == dwData)
    {
        // 0, 1����
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
            // �л������������ƿ���Ϣ����ʾ
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
 *      ���¿��ƿ����
 */
void CSMVConfigDlg::_updateCtrlBlkCaption()
{
    ASSERT(m_pCurCtrlBlk);

    CString csCaption;
    CString csDesc = L"";

    // 9X����
    if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType || FRAME_TYPE_NET_9_1 == m_pCurCtrlBlk->enFrameType)
    {
        CNetDetectItem* pNetCtrlBlk = dynamic_cast<CNetDetectItem*>(m_pCurCtrlBlk);
        ASSERT(pNetCtrlBlk);

        // 92���Դ�scl���ö�ȡ������Ϣ
        if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType)
        {
            CIEC9_2Cb* pCB = m_pDataLayer->GetSclManager()->GetIEC9_2Cb(pNetCtrlBlk->appId);

            if(pCB)
            {
                csDesc = pCB->GetDescription().c_str();
            }
        }

        // ��������ϢΪ�գ���ȡappid : port ��Ϣ
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
 *      ���µײ��˵���ʾ
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
        // ȡ��F6��ģ���Ӳ˵�
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

            // �ж�Э�����ͼ���ģ���Ӳ˵�

            // FT3ͨ��ģ��
            if(m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_NANRUI || m_pCurCtrlBlk->enFrameType == FRAME_TYPE_FT3_GUOWANG)
            {
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12A));      // ����Ĭ��ģ��
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12B));      // ��ĬĬ��ģ��
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12C));      // IEC60044
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22A));      // ������չ
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22B));      // ��Ĭ��չ
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_22C));      // ���豸FT3

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

                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_SCL_CFG));           // SCLģ��
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_8A));           // ����92
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12D));          // ��Ĭ92
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_12E));          // ���豸��ͨ��С��13��
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_18A));          // �����ܱ��վԭ�����Լ�����P62ҳ��18ͨ��ģ��
                menu.AppendMenu(MF_STRING, nItemID++, dsm_stringMgr::Get(DSM_STR_EDIT_SMV_TEMPLATE_CHAN_30A));          // ���豸

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
    //  ����ͨ�������򷵻�
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

    // ���û�ȡ��������������
    if(CHANNEL_VALUE_TYPE_MEASURE == pChannel->enChannelValueType)
    {
        pChannel->enChannelValueType = CHANNEL_VALUE_TYPE_PROTECT;
    }
    else
    {
        pChannel->enChannelValueType = CHANNEL_VALUE_TYPE_MEASURE;
    }

    // ���õ���ͨ����������Ϣ
    CString csChannelDesc;

    if(CHANNEL_TYPE_SMV_CURRENT == pChannel->enChannelType)
    {
        // ��������
        if(CHANNEL_VALUE_TYPE_PROTECT == pChannel->enChannelValueType)
        {
            m_pCurrentReport->SetRowItemText(nSelRow, 2, dsm_stringMgr::Get(DSM_STR_TYPE_DATA_AMPER));
        }
        // ���Ե���
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
*�˺��������壿��
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

    //// �ж��Ƿ�ѡ��ͨ��������
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
    // TODO: �ڴ����ר�ô����/����û���

    int nMenuID = LOWORD(wParam);

    if((DWORD)nMenuID >= m_nBeginID && (DWORD)nMenuID < m_nEndID)
    {
        // ����ͨ��ģ��
        if(_changeTemplate(nMenuID))
        {
            // ���²˵�
            _updateChannel();
        }
        // ����ͨ��ģ��ʧ��
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
    // TODO: �ڴ����ר�ô����/����û���

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
 *      ����ͨ����ģ��
 *
 *  \param[in]
 *      nMenuID     ����ID
 *  \return
 *      bool        ���ĳɹ�����true��ʧ�ܷ���false
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
        // inplaceEdit����ʱ, ����༭�˵�
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_pCurrentReport->GetInplaceEdit());
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace�˳�ʱ���˳��༭�˵�
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}


/**
*	F3���ܰ���
*
*/
#if !SU_FEI
void CSMVConfigDlg::OnMenuF3()
{
	if(m_modeSetting == SETTING_CHANNEL)
	{
		if(m_modePage == PAGE_DUALPHASE)
		{
			// �л������������ƿ���Ϣ����ʾ
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