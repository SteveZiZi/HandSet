/// @file
///
/// @brief
///     参数配置对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///     lqx  2013.5.28
///
/// 修改历史：
///

#include "stdafx.h"
#include "BasicParamDlg.h"

#include "src/main/common.h"

#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/popdlg/progressbardlg.h"
#include "src/main/eventproc.h"
#include "src/utils/utility/dsm_utility.h"
//#include "src/main/dlg/timescan1588dlg.h"
#include "src/main/utils/utility.h"

#include "src/service/datalayer/dsmdatalayer.h"
#include "src/service/datalayer/config/goosecb.h"

#include "BasicParamACDCDlg.h"

#define MSGPARAM_BASIC_CAPTION          0
#define MSGPARAM_BASIC_VALUE            1

#define WM_BP_SEL_SCL                   PRIVATE_WND_USERID + 350


//
//基本参数
//
#if SU_FEI
#define MSGPARAM_BASIC_ITEM_SCL         0           ///< SCL
#define MSGPARAM_BASIC_ITEM_RATEFREQ    1           ///< 额定频率
#define MSGPARAM_BASIC_ITEM_CHANTYPE    2           ///< 通道类型
#define MSGPARAM_BASIC_ITEM_VOLT1       3           ///< 电压一次额定值
#define MSGPARAM_BASIC_ITEM_VOLT2       4           ///< 电压二次额定值
#define MSGPARAM_BASIC_ITEM_AMPER1      5           ///< 电流一次额定值
#define MSGPARAM_BASIC_ITEM_AMPER2      6           ///< 电流二次额定值
#define MSGPARAM_BASIC_ITEM_MU          7           ///< MU延时
#define MSGPARAM_BASIC_ITEM_TIMETICK    8           ///< 对时方案
#else
#define MSGPARAM_BASIC_ITEM_SCL         0           ///< SCL
#define MSGPARAM_BASIC_ITEM_TIMETICK    0           ///< 对时方案
#define MSGPARAM_BASIC_ITEM_RATEFREQ    0           ///< 额定频率
#define MSGPARAM_BASIC_ITEM_CHANTYPE    1           ///< 通道类型
#define MSGPARAM_BASIC_ITEM_VOLT1       2           ///< 电压一次额定值
#define MSGPARAM_BASIC_ITEM_VOLT2       3           ///< 电压二次额定值
#define MSGPARAM_BASIC_ITEM_AMPER1      4           ///< 电流一次额定值
#define MSGPARAM_BASIC_ITEM_AMPER2      5           ///< 电流二次额定值
#define MSGPARAM_BASIC_ITEM_MU          6           ///< MU延时
#endif
//
//smv发送
//
#if SU_FEI
enum SSRecordFuncType
{
    RFTUnknown = 0,
    RFTSmvType ,                                    ///< SMV类型
    RFTSampleValDis,                                ///< 采样值显示
    RFTSampleFreq,                                  ///< 采样频率
    RFTASDUCount,                                   ///< ASDU
    RFTACDC,                                        ///< 交直流
    RFTBaudRate,                                    ///< 波特率
    RFTSmvSend                                      ///< SMV发送
};

#endif

//
//
////
////smv发送
////
#define MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX       5        ///< SMV发送起始行(91)
#define MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX       5        ///< SMV发送起始行(92)
#define MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX      5        ///< SMV发送起始行(FT3)
#define MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX  5        ///< SMV发送起始行(FT3扩展)

//
//goose发送
//
#define MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX    0           ///< 心跳间隔T0
#define MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX     1           ///< 最小间隔T1
#if SU_FEI
#define MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX    2     ///< GOOSE发送
#endif

#define MSGPARAM_TICK_SPACE_MIN                  500         ///< 心跳间隔最小值
#define MSGPARAM_TICK_SPACE_MAX                  65535       ///< 心跳间隔最大值

#define MSGPARAM_MIN_SPACE_MIN                  1            ///< 最小间隔最小值
#define MSGPARAM_MIN_SPACE_MAX                  65535        ///< 最小间隔最大值

// CBasicParamDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamDlg, CBaseDlg)

CBasicParamDlg::CBasicParamDlg(CWnd* pParent /*=NULL*/)

	: CBaseDlg(CBasicParamDlg::IDD, DSM_STR_BASIC_PARAM_MSG, pParent)
    , m_pCurReport(NULL)
{
    m_nMenuID    = IDR_MENU_MSG_PARAM;
    m_pCfg       = CDsmDataLayer::getInstance()->GetParamConfigBasic();

  //  m_nIDCaption = DSM_STR_BASIC_PARAM_BASECAPTION;//初始化标题，参数配置 - 基本参数


}

CBasicParamDlg::~CBasicParamDlg()
{
}

void CBasicParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX) ;
}


BEGIN_MESSAGE_MAP(CBasicParamDlg, CBaseDlg)
#if SU_FEI
    ON_LBN_SELCHANGE(IDC_BASICPARAM_MENUNAVI, &CBasicParamDlg::OnMenuChanged)
	///<  针对SCL导入
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_, &CBasicParamDlg::OnInplaceButtonClick)
#else
	///<  针对SCL导入
	ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_BASIC_SCL_RET, &CBasicParamDlg::OnInplaceButtonClick)

	ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_TIME_RET, &CBasicParamDlg::OnItemSelectChanged)
#endif
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_RET, &CBasicParamDlg::OnItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_PARAM_RET, &CBasicParamDlg::OnItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_RET,  &CBasicParamDlg::OnInplaceEditAction)
#if SU_FEI
    ON_NOTIFY(ELNM_OKCLICK, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamDlg::OnSmvSendOkClick)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamDlg::OnSmvSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamDlg::OnSmvSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamDlg::OnSmvSendItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_SMV_SEND_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_SMV_SEND_RET,  &CBasicParamDlg::OnInplaceEditAction)

    ON_COMMAND_RANGE(ID_MENU_PARAM_SMVS_FROM_SCL, ID_MENU_PARAM_SMVS_F6, &CBasicParamDlg::OnNaviMenuParamSmvSend)
	ON_COMMAND_RANGE(ID_MENU_PARAM_SMVS_F1, ID_MENU_PARAM_SMVS_F6, &CBasicParamDlg::OnNaviMenuParamSmvSend)

    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_GOOSE_SEND_RET, &CBasicParamDlg::OnGooseSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_GOOSE_SEND_RET, &CBasicParamDlg::OnGooseSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_GOOSE_SEND_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_GOOSE_SEND_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_COMMAND_RANGE(ID_MENU_PARAM_GOOSES_FROM_SCL, ID_MENU_PARAM_GOOSES_F6, &CBasicParamDlg::OnNaviMenuParamGooseSend)

    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_GOOSE_REC_RET, &CBasicParamDlg::OnGooseRecItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_GOOSE_REC_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_GOOSE_REC_RET,  &CBasicParamDlg::OnInplaceEditAction)
    ON_COMMAND_RANGE(ID_MENU_PARAM_GOOSER_FROM_SCL, ID_MENU_PARAM_GOOSER_F6, &CBasicParamDlg::OnNaviMenuParamGooseRec)
#endif    

	ON_MESSAGE(WM_BP_SEL_SCL, &CBasicParamDlg::OnSelSCL)
    ON_COMMAND(ID_MENU_MSG_PARAM_F6, &CBasicParamDlg::OnMenuF6)

#if !SU_FEI
	ON_COMMAND(ID_MENU_MSG_PARAM_F1, &CBasicParamDlg::OnMenuF1)
	ON_COMMAND(ID_MENU_MSG_PARAM_F2, &CBasicParamDlg::OnMenuF2)
	ON_COMMAND(ID_MENU_MSG_PARAM_F3, &CBasicParamDlg::OnMenuF3)
	ON_COMMAND(ID_MENU_MSG_PARAM_F4, &CBasicParamDlg::OnMenuF4)
	ON_COMMAND(ID_MENU_MSG_PARAM_F5, &CBasicParamDlg::OnMenuF5)
#endif

    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CBasicParamDlg 消息处理程序

BOOL CBasicParamDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
#if SU_FEI
    _initMenuNavi();
#endif
    _restorePage();
    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


#if SU_FEI
void CBasicParamDlg::_initMenuNavi()
{
    CRect rcNavi(DSM_BASICCFG_RCLEFT);
    DSM_ParentToChild(this, rcNavi);

    m_menuNavi.Create(WS_TABSTOP | WS_CHILD | WS_VISIBLE, 
        rcNavi, this, IDC_BASICPARAM_MENUNAVI);

    m_menuNavi.SetFont(dsm_fontmgr::GetInstance()->GetFont(LBT_3L_TEXTLFHEIGHT));
    m_menuNavi.SetHeight(LBT_3L_ITEMHEIGHT);
    m_menuNavi.PostInitTemplateStyle();

    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BASE_CAPTION), DSM_MSG_PARAM_BASE);
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_CAPTION), DSM_MSG_PARAM_SMV);
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_S_CAPTION), DSM_MSG_PARAM_GOOSES);
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION), DSM_MSG_PARAM_GOOSER);

    m_menuNavi.SetCurSel(0);
}

void CBasicParamDlg::OnMenuChanged()
{
    if(m_pCurReport && m_pCurReport->GetSafeHwnd())
    {
        m_pCurReport->ShowWindow(SW_HIDE);
    }

    switch(m_menuNavi.GetItemData(m_menuNavi.GetCurSel()))
    {
    case DSM_MSG_PARAM_BASE:
        _showBase();
        break;

    case DSM_MSG_PARAM_SMV:
        _showSmvSend();
        break;
    case DSM_MSG_PARAM_GOOSES:
        _showGooseSend();
        break;
    case DSM_MSG_PARAM_GOOSER:
        _showGooseRec();
        break;

    default:
        break;
    }
}
#endif
void CBasicParamDlg::_createReport(CReportH& report, UINT nID)
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    report.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, nID);

    // 设置字体
    report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    report.PostInitTemplateStyle();

    // 根据报表需要插入列元素
    switch(nID)
    {
    case IDC_BASIC_PARAM_RET:
#if !SU_FEI
	case IDC_BASIC_SCL_RET:
	case IDC_BASIC_TIME_RET:
#endif
        report.InsertColumn(MSGPARAM_BASIC_CAPTION, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION);    // 标题
        report.InsertColumn(MSGPARAM_BASIC_VALUE, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);      // 内容

        // 设置选择焦点样式
        report.SetRowSelectionVisible(FALSE);
        report.SetRowFocusEnable(TRUE);
        report.SetSubItemFocusEnable(TRUE);
        report.SetFreezeColumnsCount(MSGPARAM_BASIC_VALUE);
        report.SetFreezeSelectionVisible(TRUE);
        report.SetFocusedColIndex(MSGPARAM_BASIC_VALUE);
        break;

#if SU_FEI
    case IDC_BASIC_PARAM_SMV_SEND_RET:
    case IDC_BASIC_PARAM_GOOSE_REC_RET:
    case IDC_BASIC_PARAM_GOOSE_SEND_RET:
        report.InsertColumn(0, L"", EL_CENTER, DSM_REPORT_WIDTH_MARGIN);    // 边框
        report.InsertColumn(1, L"", EL_LEFT, 230);                          // 标题
        report.InsertColumn(2, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);     // 内容

        // 设置选择焦点样式
        report.SetRowSelectionVisible(FALSE);
        report.SetRowFocusEnable(TRUE);
        report.SetSubItemFocusEnable(TRUE);
        report.SetFreezeColumnsCount(2);
        report.SetFreezeSelectionVisible(TRUE);
        report.SetFocusedColIndex(2);
        break;


#endif
    default:
        ASSERT(FALSE);
        break;
    }
}


void CBasicParamDlg::_showBase()
{
#if !SU_FEI
	if (m_ctlImportSCLReport.GetSafeHwnd())
	{
		m_ctlImportSCLReport.ShowWindow(SW_HIDE);
	}

	if (m_ctlTimeReport.GetSafeHwnd())
	{
		m_ctlTimeReport.ShowWindow(SW_HIDE);
	}
	
#endif
    if(m_ctlBaseReport.GetSafeHwnd())
    {
        m_ctlBaseReport.ShowWindow(SW_SHOW);
		m_ctlBaseReport.SetFocus();
    }
    else
    {
        _createBaseReport();
    }

   // _reloadNaviMenu(IDR_MENU_MSG_PARAM);

#if !SU_FEI
	m_ctlBaseReport.SetFocusedSelectionRowIndex(0);
	m_ctlBaseReport.GetColumnExAttr(MSGPARAM_BASIC_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);

	for(int i = 0; i < m_ctlBaseReport.GetItemCount(); i++)
	{
		m_ctlBaseReport.GetRecordRow(i)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(TRUE);
	}

	m_ctlBaseReport.GetRecordRow(MSGPARAM_BASIC_ITEM_RATEFREQ)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(FALSE);
#endif

    CString csCaption;

    csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BASE_CAPTION));

    CStatusMgr::SetTitle(csCaption);


	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_BASIC_PARAM_SEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_MENU_IMPORT_SCL_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_TIME_WAY_UNSEL), NULL, NULL);

	m_pNaviBar->UpdateNaviMenu();


    m_pCurReport = &m_ctlBaseReport;
}

void CBasicParamDlg::_createBaseReport()
{
    _createReport(m_ctlBaseReport, IDC_BASIC_PARAM_RET);

#if SU_FEI
    CString csSCLFile(m_pCfg->GetSclFile().c_str());

    if(!CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
    {
        if(m_pCfg->GetSclFile().empty())
        {
            csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
        }
        else
        {
            csSCLFile.Format(L"%s  (%s)", m_pCfg->GetSclFile().c_str(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
        }
    }

    // SCL文件
    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_SCL, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SCL_IMPRORT), 
        csSCLFile));
#endif
    // 额定频率
    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_RATEFREQ, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_FREQ), 
        m_pCfg->GetRatedFreq().c_str()));

    CString csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHANNEL_ONE);

    if(SMV_VALUE_TYPE_SECONDARY == m_pCfg->GetFrameValueType())
    {
        csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHANNEL_TWO);
    }

    // 额定通道类型
    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_CHANTYPE, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_CHAN_TYPE),
        csValue));

    CString csfval;

    // 电压一次额定值
    csfval.Format(L"%.3f", m_pCfg->GetRatedPrimaryVoltage());

    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_VOLT1, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_VOLT_1),
        csfval));

    // 电压二次额定值
    csfval.Format(L"%.3f", m_pCfg->GetRatedSecondaryVoltage());

    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_VOLT2, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_VOLT_2),
        csfval));

    // 电流一次额定值
    csfval.Format(L"%.3f", m_pCfg->GetRatedPrimaryCurrent());

    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_AMPER1, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_AMP_1),
        csfval));

    // 电流二次额定值
    csfval.Format(L"%.3f", m_pCfg->GetRatedSecondaryCurrent());

    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_AMPER2, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_AMP_2),
        csfval));

    // MU额定延时
    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_MU, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_MU),
        m_pCfg->GetMuRatedDelay().c_str()));

#if SU_FEI
    csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_B);

    if(FPGA_TIME_SYNC_IRIGB_NEG == m_pCfg->GetTimeSyncMode())
    {
        csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_NB);
    }
//     else if(FPGA_TIME_SYNC_IEEE1588_A == m_pCfg->GetTimeSyncMode())
//     {
//         csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_1588);
//     }

    // 对时方案
    m_ctlBaseReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_TIMETICK, new CDsmBasicRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_TICK),
        csValue));


    m_ctlBaseReport.SetFocusedSelectionRowIndex(0);
    m_ctlBaseReport.GetColumnExAttr(MSGPARAM_BASIC_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i < m_ctlBaseReport.GetItemCount(); i++)
    {
        m_ctlBaseReport.GetRecordRow(i)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(TRUE);
    }

    m_ctlBaseReport.GetRecordRow(MSGPARAM_BASIC_ITEM_RATEFREQ)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(FALSE);


    // SCL文件导入按钮
    m_ctlBaseReport.GetRecordRow(MSGPARAM_BASIC_ITEM_SCL)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL)->AddExpandButton();
#endif


    // 额定通道类型
    CELRecordItemEditOptions* pOption = m_ctlBaseReport.GetRecordRow(MSGPARAM_BASIC_ITEM_CHANTYPE)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHANNEL_ONE), SMV_VALUE_TYPE_PRIMARY);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHANNEL_TWO), SMV_VALUE_TYPE_SECONDARY);

#if SU_FEI
    // 对时方案
    pOption = m_ctlBaseReport.GetRecordRow(MSGPARAM_BASIC_ITEM_TIMETICK)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_B), FPGA_TIME_SYNC_IRIGB_POS);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_NB), FPGA_TIME_SYNC_IRIGB_NEG);
    //pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_1588), FPGA_TIME_SYNC_IEEE1588_A);
#endif
}


void CBasicParamDlg::OnInplaceButtonClick( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;
    ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify->pButton);

    //CString csFile = pItemNotify->pItem->GetCaption();

    // scl文件
    if(MSGPARAM_BASIC_ITEM_SCL == pItemNotify->rowIndex)
    {
        PostMessage(WM_BP_SEL_SCL, 0, 0); // 发送消息到系统 ，系统收到消息创建一个浏览文件窗口


         //CString csSCL;
 
         //// 获取SD目录失败
         //if(!DSM_GetSDDriver(csSCL))
         //{
         //    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SCLF));
         //    return;
         //}
 
         //csSCL += DSM_CFG_SCL_FILE_FOLDER;
 
         //// SCL目录不存在则创建
         //if(!DSM_CreateSDPath(csSCL))
         //{
         //    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CREATE_FOLDER_FAILED_SCL));
         //    return;
         //}
 
         //CString csExt(DSM_CFG_SCL_FILE_EXTENSION);
 
         //CDsmFileDlg dlg(0, csSCL, csFile, csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));
 
         //HWND hWnd = ::GetFocus();
         //if(IDOK == dlg.DoModal())
         //{
         //    CProgressBarDlg dlgPro(this);
 
         //    // 加载SCL
         //    _SCLTaskArgs arg;
         //    arg.csFile = dlg._GetFilePath();
         //    arg.m_pWnd = &dlgPro;
 
         //    CString csTips;
 
         //    dlgPro._SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL_TIPS));
         //    dlgPro._SetTitle(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL));
         //    dlgPro._SetTask(_LoadSCLTask, &arg);
         //    if(IDOK == dlgPro.DoModal())
         //    {
         //        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_SUCCESS), dlg._GetFile());
         //        CStatusMgr::SetTips(csTips);
 
         //        CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
         //        if(pItemText)
         //        {
         //            pItemText->SetValue(dlg._GetFile());
         //            m_pCfg->SetSclFile((LPCTSTR)dlg._GetFile());
         //        }
         //    }
         //    else
         //    {
         //        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILED), dlg._GetFile());
         //        CStatusMgr::SetTips(csTips);
         //    }
         //}
 
         //::SetFocus(hWnd);
#if !SHENG_ZE
		m_sclModel = SclFromSd;
#endif
        *pResult = TRUE;
    }

#if !SHENG_ZE
	if((MSGPARAM_BASIC_ITEM_SCL+1) == pItemNotify->rowIndex)
	{
		PostMessage(WM_BP_SEL_SCL, 0, 0);
		*pResult = TRUE;
		m_sclModel = SclFromU;
	}
#endif
}


void CBasicParamDlg::OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    ASSERT(pItem);

    float fVal = 0.0f;
    CString csVal;

    switch(pItemNotify->rowIndex)
    {
        // 额定频率
    case MSGPARAM_BASIC_ITEM_RATEFREQ:
        m_pCfg->SetRatedFreq((LPCTSTR)pItem->GetValue());
        pItem->SetValue(m_pCfg->GetRatedFreq().c_str());
        *pResult = TRUE;
        break;

        // 电压一次额定值
    case MSGPARAM_BASIC_ITEM_VOLT1:
        if(DSM_Str2Float((LPCTSTR)pItem->GetValue(), fVal, DSM_FLOAT_VOLT_1_INT, DSM_FLOAT_VOLT_1_DECI))
        {
            m_pCfg->SetRatedPrimaryVoltage(fVal);
        }

        csVal.Format(L"%.3f", m_pCfg->GetRatedPrimaryVoltage());
        pItem->SetValue(csVal);
        *pResult = TRUE;

        break;

        // 电压二次额定值
    case MSGPARAM_BASIC_ITEM_VOLT2:
        if(DSM_Str2Float((LPCTSTR)pItem->GetValue(), fVal, DSM_FLOAT_VOLT_2_INT, DSM_FLOAT_VOLT_2_DECI))
        {
            m_pCfg->SetRatedSecondaryVoltage(fVal);
        }

        csVal.Format(L"%.3f", m_pCfg->GetRatedSecondaryVoltage());
        pItem->SetValue(csVal);
        *pResult = TRUE;

        break;

        // 电流一次额定值
    case MSGPARAM_BASIC_ITEM_AMPER1:
        if(DSM_Str2Float((LPCTSTR)pItem->GetValue(), fVal, DSM_FLOAT_AMPER_1_INT, DSM_FLOAT_AMPER_1_DECI))
        {
            m_pCfg->SetRatedPrimaryCurrent(fVal);
        }

        csVal.Format(L"%.3f", m_pCfg->GetRatedPrimaryCurrent());
        pItem->SetValue(csVal);
        *pResult = TRUE;

        break;

        // 电流二次额定值
    case MSGPARAM_BASIC_ITEM_AMPER2:
        if(DSM_Str2Float((LPCTSTR)pItem->GetValue(), fVal, DSM_FLOAT_AMPER_2_INT, DSM_FLOAT_AMPER_2_DECI))
        {
            m_pCfg->SetRatedSecondaryCurrent(fVal);
        }

        csVal.Format(L"%.3f", m_pCfg->GetRatedSecondaryCurrent());
        pItem->SetValue(csVal);
        *pResult = TRUE;

        break;

        // MU额定延时
    case MSGPARAM_BASIC_ITEM_MU:
        m_pCfg->SetMuRatedDelay((LPCTSTR)pItem->GetValue());
        pItem->SetValue(m_pCfg->GetMuRatedDelay().c_str());
        *pResult = TRUE;
        break;

    default:
        break;
    }
}

void CBasicParamDlg::OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    switch(pItemNotify->rowIndex)
    {
    // 通道类型
    case MSGPARAM_BASIC_ITEM_CHANTYPE:
        m_pCfg->SetFrameValueType((enumSmvValueType)pItemNotify->pConstraint->GetConstraintData());
        *pResult = TRUE;
        break;

        // 对时方案
    case MSGPARAM_BASIC_ITEM_TIMETICK:
        m_pCfg->SetTimeSyncMode((FpgaTimeSyncMode)pItemNotify->pConstraint->GetConstraintData());
        *pResult = TRUE;
        break;

    default:
        break;
    }
}


void CBasicParamDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit弹出时, 切换菜单
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_pCurReport->GetInplaceEdit());
        *pResult = TRUE;
    }
    // inplace退出时，切换菜单
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}
#if SU_FEI
void CBasicParamDlg::_showSmvSend()
{
    if(m_ctlSmvSendReport.GetSafeHwnd())
    {
        m_ctlSmvSendReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createSmvSendReport();
    }
    
    _reloadNaviMenu(IDR_MENU_MSG_PARAM_SMV_SEND);
    _refreshMenu();

    CString csCaption;
    csCaption.Format(L"%s - %s",
        dsm_stringMgr::Get(DSM_STR_BASIC_IEC_SETTING),
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_CAPTION));

    CStatusMgr::SetTitle(csCaption);
    m_pCurReport = &m_ctlSmvSendReport;
}


void CBasicParamDlg::OnNaviMenuParamSmvSend(UINT nID)
{
    switch (nID)
    {

    case ID_MENU_PARAM_SMVS_FROM_SCL:
        {
            //全站配置添加smv
            CEventProc::GetInstance()->ShowPage(IDD_SEL_SMV_FROM_SCL_DLG);
        }
        break;
    case ID_MENU_PARAM_SMVS_FROM_SCAN:
        {
            //从扫描中添加SMV
            CEventProc::GetInstance()->ShowPage(IDD_SEL_SMV_FROM_SCAN_DLG);
        }
        break;
    case ID_MENU_PARAM_SMVS_FROM_MANUAL:
        {
            //手动添加
            CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
            if (NULL == pParamConfigSMVSend)
            {
                return ;
            }
            

            //手动添加一条记录
            int nRowIndex = m_ctlSmvSendReport.GetRecordRowCount();
            //send索引
            int nIndex = 0;   

            CBaseCb* pBaseCb = NULL;
            switch (pParamConfigSMVSend->GetSMVType())
            {
            case FRAME_TYPE_NET_9_1:
                {
                    nIndex = nRowIndex - MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX + 1;
                    pBaseCb = pParamConfigSMVSend->AddIEC9_1Cb();
                }
                break;
            case FRAME_TYPE_NET_9_2:
                {
                    nIndex = nRowIndex - MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX + 1;
                    pBaseCb = pParamConfigSMVSend->AddIEC9_2Cb();
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    nIndex = nRowIndex - MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX + 1;
                    pBaseCb = pParamConfigSMVSend->AddFt3Cb();
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    nIndex = nRowIndex - MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX + 1;
                    pBaseCb = pParamConfigSMVSend->AddFt3ExpandCb();
                }
                break;
            default:
                break;
            }
            ASSERT(pBaseCb);
            if (pBaseCb == NULL)
            {
                return;
            }

            CString strCaption = _T("");
            CString strContent = _T("");
            strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), nIndex);
            strContent.Format(_T("0x%04X-%s"), pBaseCb->GetAPPID(), pBaseCb->GetDescription().c_str());

            m_ctlSmvSendReport.InsertRecordRow(nRowIndex, new CDsmBasicSmvSendRecord(strCaption , strContent, RFTSmvSend));
            m_ctlSmvSendReport.SetFocusedSelectionRowIndex(nRowIndex);
            m_ctlSmvSendReport.EnsureVisible(nRowIndex, FALSE);

            CELRecordItem * pItem = m_ctlSmvSendReport.GetRecordRow(nRowIndex)->GetItem(2);
            if (pItem != NULL)
            {
                pItem->HasCheckbox(TRUE);
                pItem->SetChecked(pBaseCb->IsSelected());
                pItem->SetItemData((DWORD_PTR)pBaseCb);
            }
            _refreshMenu();
        }
        break;

    case ID_MENU_PARAM_SMVS_F2:///< 编辑
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlSmvSendReport.GetSafeHwnd())
            {
                return;
            }
            //编辑
            int nRowSel = m_ctlSmvSendReport.GetFocusedRowIndex();
            if (nRowSel >= 0)
            {
                SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(nRowSel, 0);
                if (ssRft == RFTSmvSend)
                {
                    CEventProc::GetInstance()->ShowPage(IDD_EDIT_SMV_SEND_DLG);
                }
            }
        }
        break;
    case ID_MENU_PARAM_SMVS_F3:///< 删除
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlSmvSendReport.GetSafeHwnd())
            {
                return;
            }
            m_ctlSmvSendReport.SetRedraw(FALSE);
            //删除
            int nRowSel = m_ctlSmvSendReport.GetFocusedRowIndex();
            if (nRowSel >= 0)
            {
                SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(nRowSel, 0);
                if (ssRft == RFTSmvSend)
                {
                    CBaseCb* pBaseCb = (CBaseCb*) m_ctlSmvSendReport.GetRowItemData(nRowSel, 2);
                    ASSERT(pBaseCb);
                    if (pBaseCb != NULL)
                    {
                        
                        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
                        if (NULL == pParamConfigSMVSend)
                        {
                            m_ctlSmvSendReport.SetRedraw(TRUE);
                            return ;
                        }
                        switch (pParamConfigSMVSend->GetSMVType())
                        {
                        case FRAME_TYPE_NET_9_1:
                            {
                                pParamConfigSMVSend->DeleteIEC9_1Cb((CIEC9_1Cb*)pBaseCb);
                            }
                            break;
                        case FRAME_TYPE_NET_9_2:
                            {
                                pParamConfigSMVSend->DeleteIEC9_2Cb((CIEC9_2Cb*)pBaseCb);
                            }
                            break;
                        case FRAME_TYPE_FT3_NANRUI:
                            {
                                pParamConfigSMVSend->DeleteFt3Cb((CFT3Cb*)pBaseCb);
                            }
                            break;
                        case FRAME_TYPE_FT3_GUOWANG:
                            {
                                pParamConfigSMVSend->DeleteFt3ExpandCb((CFT3ExpandCb*)pBaseCb);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    m_ctlSmvSendReport.DeleteRecordRow(nRowSel);
                    
                    CString strCaption = _T(""); 
                    CELRecordItemText* pItemCaption = NULL;
                    int nIndex = 1;
                    for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                    {
                        ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
                        if (ssRft == RFTSmvSend)
                        {
                            strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), nIndex);
                            pItemCaption = (CELRecordItemText*) m_ctlSmvSendReport.GetRecordRow(i)->GetItem(1);
                            pItemCaption->SetValue(strCaption);
                            m_ctlSmvSendReport.RedrawRowItem(i, 1);
                            ++nIndex;
                        }
                    }
                }
            }
            m_ctlSmvSendReport.SetRedraw(TRUE);
            _refreshMenu();
        }
        break;
    case ID_MENU_PARAM_SMVS_F4:///< 清空
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlSmvSendReport.GetSafeHwnd())
            {
                return;
            }

            CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
            if (NULL == pParamConfigSMVSend)
            {
                return ;
            }
            switch(pParamConfigSMVSend->GetSMVType())
            {
            case FRAME_TYPE_NET_9_1:
                {
                    while(MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX < m_ctlSmvSendReport.GetRecordRowCount())
                    {
                        m_ctlSmvSendReport.DeleteRecordRow(MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX);
                    }
                    pParamConfigSMVSend->ClearAllIEC9_1Cb();
                }
                break;
            case FRAME_TYPE_NET_9_2:
                {
                    while(MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX < m_ctlSmvSendReport.GetRecordRowCount())
                    {
                        m_ctlSmvSendReport.DeleteRecordRow(MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX);
                    }
                    pParamConfigSMVSend->ClearAllIEC9_2Cb();
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    while(MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX < m_ctlSmvSendReport.GetRecordRowCount())
                    {
                        m_ctlSmvSendReport.DeleteRecordRow(MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX);
                    }
                    pParamConfigSMVSend->ClearAllFt3Cb();
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    while(MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX < m_ctlSmvSendReport.GetRecordRowCount())
                    {
                        m_ctlSmvSendReport.DeleteRecordRow(MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX);
                    }
                    pParamConfigSMVSend->ClearAllFt3ExpandCb();
                }
                break;
            default:
                break;
            }
            _refreshMenu();
        }
        break;
    case ID_MENU_PARAM_SMVS_F5:
        {

        }
        break;
    case ID_MENU_PARAM_SMVS_F6:
        {

        }
        break;
    default:
        break;
    }
}
/**
 *      创建SMV发送参数报表
 */
void CBasicParamDlg::_createSmvSendReport()
{
    _createReport(m_ctlSmvSendReport, IDC_BASIC_PARAM_SMV_SEND_RET);

    //
    //根据SMV类型调用
    //
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    enumFrameType nSmvSendType = pParamConfigSMVSend->GetSMVType();

    // SMV类型
    m_ctlSmvSendReport.InsertRecordRow(0, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_TYPE),
        _T(""), RFTSmvType));

    CELRecordItemEditOptions* pOption  = m_ctlSmvSendReport.GetRecordRow(0)->GetItem(2)->GetEditOptions(NULL);
    pOption->AddComboButton();
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC61850_92), FRAME_TYPE_NET_9_2);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC61850_91), FRAME_TYPE_NET_9_1);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC60044_FT3), FRAME_TYPE_FT3_NANRUI);
    pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC60044_FT3_EXT), FRAME_TYPE_FT3_GUOWANG);

    CELRecordItemConstraint * pItemCon = pOption->FindConstraint(nSmvSendType);
    if (pItemCon != NULL)
    {
        CELRecordItemText* pItemType =  dynamic_cast<CELRecordItemText*>(m_ctlSmvSendReport.GetRecordRow(0 )->GetItem(2));
        ASSERT(pItemType);
        pItemType->SetValue(pItemCon->GetConstraintCaption());
    }
    
    switch(nSmvSendType)
    {
    case FRAME_TYPE_NET_9_1:
        {
            _refreshSmvReportWithIEC6185091();
        }
        break;
    case FRAME_TYPE_NET_9_2:
        {
            _refreshSmvReportWithIEC6185092();
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            _refreshSmvReportWithIEC60044FT3();
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            _refreshSmvReportWithIEC60044FT3Ext();
        }
        break;
    default:
        break;
    }

    m_ctlSmvSendReport.SetFocusedSelectionRowIndex(0);
}


CString CBasicParamDlg::_getACDCFormatString()
{
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return _T("");
    }
    
    CString strACDC[16] = {_T("Ua1"),_T("Ub1"), _T("Uc1"),_T("Ux1"), _T("Ua2"),_T("Ub2"), _T("Uc2"), _T("Ux2"),
         _T("Ia1"), _T("Ib1"), _T("Ic1"), _T("Ix1"), _T("Ia2"), _T("Ib2"), _T("Ic2"), _T("Ix2")
    };
    CString strFormat = _T("");
    int nACCount = 0;
    int nDCCount = 0;
    
    strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_CUR_CHAN);
    for (int i = 0; i < 16; ++i)
    {
        if (pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i]) == CURRENT_TYPE_DC)
        {
            if (nDCCount > 0)
            {
                strFormat += _T(",");
            }
            strFormat += strACDC[i];
            ++nDCCount;
        }
        else
        {
            ++nACCount;
        }
    }
    
    if (nACCount == 16)
    {
        strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_ALL_ACUR);
    }
    else if(nDCCount == 16)
    {
        strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_ALL_DCUR);
    }
    return strFormat;
}

void CBasicParamDlg::_refreshSmvReportWithIEC6185091()
{
    if(!m_ctlSmvSendReport.GetSafeHwnd())
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSmvSendReport.SetRedraw(FALSE);

    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSmvSendReport.GetRecordRowCount() > 1)
    {
        m_ctlSmvSendReport.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSmvSendReport.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSmvSendReport.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // ASDU数目
    m_ctlSmvSendReport.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ASDU_COUNT),
        pParamConfigSMVSend->GetASDUCount(),RFTASDUCount));

    
    // 交直流设置
    m_ctlSmvSendReport.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));
    
    //
    //添加SMV发送
    //
    int i = 1;
    CString strCaption = _T("");
    CString strContent = _T("");

    CIEC9_1Cb* pIEC91Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
    while(pIEC91Cb)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
        strContent.Format(_T("0x%04X-%s"), pIEC91Cb->GetAPPID(), pIEC91Cb->GetDescription().c_str());

        m_ctlSmvSendReport.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
            strCaption , strContent, RFTSmvSend));

        CELRecordItem * pItem = m_ctlSmvSendReport.GetRecordRow(4 + i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pIEC91Cb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pIEC91Cb);
        }

        ++i;
        pIEC91Cb = pParamConfigSMVSend->NextIEC9_1Cb(pIEC91Cb);
    }

    m_ctlSmvSendReport.SetFocusedSelectionRowIndex(0);
    m_ctlSmvSendReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i < m_ctlSmvSendReport.GetItemCount(); i++)
    {
        m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }
    m_ctlSmvSendReport.SetRedraw(TRUE);
}
void CBasicParamDlg::_refreshSmvReportWithIEC6185092()
{
    if(!m_ctlSmvSendReport.GetSafeHwnd())
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSmvSendReport.SetRedraw(FALSE);
    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSmvSendReport.GetRecordRowCount() > 1)
    {
        m_ctlSmvSendReport.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSmvSendReport.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSmvSendReport.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // ASDU数目
    m_ctlSmvSendReport.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ASDU_COUNT),
        pParamConfigSMVSend->GetASDUCount(),RFTASDUCount));


    // 交直流设置
    m_ctlSmvSendReport.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));

    //
    //添加SMV发送
    //
    int i = 1;
    CString strCaption = _T("");
    CString strContent = _T("");

    CIEC9_2Cb* pIEC92Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
    while(pIEC92Cb)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
        strContent.Format(_T("0x%04X-%s"), pIEC92Cb->GetAPPID(), pIEC92Cb->GetDescription().c_str());

        m_ctlSmvSendReport.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
            strCaption , strContent, RFTSmvSend));

        CELRecordItem * pItem = m_ctlSmvSendReport.GetRecordRow(4 + i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pIEC92Cb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pIEC92Cb);
        }

        ++i;
        pIEC92Cb = pParamConfigSMVSend->NextIEC9_2Cb(pIEC92Cb);
    }

    m_ctlSmvSendReport.SetFocusedSelectionRowIndex(0);
    m_ctlSmvSendReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i < m_ctlSmvSendReport.GetItemCount(); i++)
    {
        m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }

    m_ctlSmvSendReport.SetRedraw(TRUE);
}
void CBasicParamDlg::_refreshSmvReportWithIEC60044FT3()
{
    if(!m_ctlSmvSendReport.GetSafeHwnd())
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSmvSendReport.SetRedraw(FALSE);
    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSmvSendReport.GetRecordRowCount() > 1)
    {
        m_ctlSmvSendReport.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSmvSendReport.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSmvSendReport.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 交直流设置
    m_ctlSmvSendReport.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));

    // 波特率设置
    m_ctlSmvSendReport.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE),
        _T(""), RFTBaudRate));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(4)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_2_5MBPS), FT3_BAUD_RATE_2P5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_5MBPS), FT3_BAUD_RATE_5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_10MBPS), FT3_BAUD_RATE_10M);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetBaudRate());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(4)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    //
    //添加SMV发送
    //
    int i = 1;
    CString strCaption = _T("");
    CString strContent = _T("");

    CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
    while(pFt3Cb)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
        strContent.Format(_T("0x%04X-%s"), pFt3Cb->GetAPPID(), pFt3Cb->GetDescription().c_str());

        m_ctlSmvSendReport.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
            strCaption , strContent, RFTSmvSend));

        CELRecordItem * pItem = m_ctlSmvSendReport.GetRecordRow(4 + i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pFt3Cb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pFt3Cb);
        }

        ++i;
        pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
    }

    m_ctlSmvSendReport.SetFocusedSelectionRowIndex(0);
    m_ctlSmvSendReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i < m_ctlSmvSendReport.GetItemCount(); i++)
    {
        m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }

    m_ctlSmvSendReport.SetRedraw(TRUE);
}
void CBasicParamDlg::_refreshSmvReportWithIEC60044FT3Ext()
{
    if(!m_ctlSmvSendReport.GetSafeHwnd())
    {
        return;
    }
    
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSmvSendReport.SetRedraw(FALSE);
    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSmvSendReport.GetRecordRowCount() > 1)
    {
        m_ctlSmvSendReport.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSmvSendReport.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSmvSendReport.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 交直流设置
    m_ctlSmvSendReport.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));

    // 波特率设置
    m_ctlSmvSendReport.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE),
        _T(""), RFTBaudRate));
    pOptions  = m_ctlSmvSendReport.GetRecordRow(4)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_5MBPS), FT3_BAUD_RATE_5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_10MBPS), FT3_BAUD_RATE_10M);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetBaudRate());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(4)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    //
    //添加SMV发送
    //
    int i = 1;
    CString strCaption = _T("");
    CString strContent = _T("");

    CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
    while(pFt3ExpandCb)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
        strContent.Format(_T("0x%04X-%s"), pFt3ExpandCb->GetAPPID(), pFt3ExpandCb->GetDescription().c_str());

        m_ctlSmvSendReport.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
            strCaption , strContent, RFTSmvSend));

        CELRecordItem * pItem = m_ctlSmvSendReport.GetRecordRow(4 + i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pFt3ExpandCb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pFt3ExpandCb);
        }

        ++i;
        pFt3ExpandCb = pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpandCb);
    }

    m_ctlSmvSendReport.SetFocusedSelectionRowIndex(0);
    m_ctlSmvSendReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i < m_ctlSmvSendReport.GetItemCount(); i++)
    {
        m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }

    m_ctlSmvSendReport.SetRedraw(TRUE);
}


void CBasicParamDlg::OnSmvSendOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    int nRowIndex = m_ctlSmvSendReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }
    
    SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(nRowIndex, 0);
    
    switch (ssRft)
    {
    case RFTACDC:
        {
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ACDC_DLG);
            *pResult = TRUE;
        }
        break;
    } 
}

void CBasicParamDlg::OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(pItemNotify->rowIndex, 0);

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

    switch(ssRft)
    {
    case RFTASDUCount:
        {
            CString strValue = pItem->GetValue();
            int nValue = _ttoi(strValue);
            pParamConfigSMVSend->SetASDUCount(nValue);
            strValue.Format(_T("%d"), pParamConfigSMVSend->GetASDUCount());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case RFTSmvSend:
        {
            if(pItem->GetHasCheckbox())
            {
                CBaseCb* pBaseCb = (CBaseCb*)pItem->GetItemData();
                if (pBaseCb != NULL)
                {
                    bool bCheck = pItem->IsChecked() ? true: false;

                    if (bCheck)
                    {
                        //
                        //判断已经选中多少个控制块
                        //
                        enumFrameType smvType = pParamConfigSMVSend->GetSMVType();
                        switch(smvType)
                        {
                        case FRAME_TYPE_NET_9_1:
                            {

                                FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();

                                if (sampRate == FT3_SAMP_RATE_4K)
                                {
                                    if (pParamConfigSMVSend->GetSelectedIEC9_1CbCount() < MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX)
                                    {
                                        pBaseCb->Selected(bCheck);
                                    }
                                    else 
                                    {
                                        pItem->SetChecked(FALSE);
                                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_91_4K_CTRL_SEL_MAX), 3000);
                                    }
                                }
                                else if (sampRate == FT3_SAMP_RATE_12P8K)
                                {
                                    if (pParamConfigSMVSend->GetSelectedIEC9_1CbCount() < MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX)
                                    {
                                        pBaseCb->Selected(bCheck);
                                    }
                                    else 
                                    {
                                        pItem->SetChecked(FALSE);
                                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_91_128K_CTRL_SEL_MAX), 3000);
                                    }
                                }
                            }
                            break;
                        case FRAME_TYPE_NET_9_2:
                            {
                                FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();

                                if (sampRate == FT3_SAMP_RATE_4K)
                                {
                                    if (pParamConfigSMVSend->GetSelectedIEC9_2CbCount() < MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX)
                                    {
                                        pBaseCb->Selected(bCheck);
                                    }
                                    else 
                                    {
                                        pItem->SetChecked(FALSE);
                                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_4K_CTRL_SEL_MAX), 3000);
                                    }
                                }
                                else if (sampRate == FT3_SAMP_RATE_12P8K)
                                {
                                    if (pParamConfigSMVSend->GetSelectedIEC9_2CbCount() < MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX)
                                    {
                                        pBaseCb->Selected(bCheck);
                                    }
                                    else 
                                    {
                                        pItem->SetChecked(FALSE);
                                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_128K_CTRL_SEL_MAX), 3000);
                                    }
                                }
                            }
                            break;
                        case FRAME_TYPE_FT3_NANRUI:
                            {
                                if (pParamConfigSMVSend->GetSelectedFt3CbCount() > 0)
                                {
                                    //只允许选中一个
                                    for(int i = MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                                    {
                                        CBaseCb* pFt3Cb = (CBaseCb*)m_ctlSmvSendReport.GetRowItemData(i, 2);
                                        if (pFt3Cb != pBaseCb && pFt3Cb->IsSelected())
                                        {
                                            pFt3Cb->Selected(false);
                                            m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetChecked(FALSE);
                                            m_ctlSmvSendReport.RedrawRowItem(i, 2);
                                        }
                                    }
                                }
                                pBaseCb->Selected(bCheck);
                            }
                            break;
                        case FRAME_TYPE_FT3_GUOWANG:
                            { 
                                if (pParamConfigSMVSend->GetSelectedFt3ExpandCbCount() > 0)
                                {
                                    //只允许选中一个
                                    for(int i = MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                                    {
                                        CBaseCb* pFt3ExtCb = (CBaseCb*)m_ctlSmvSendReport.GetRowItemData(i, 2);
                                        if (pFt3ExtCb != pBaseCb && pFt3ExtCb->IsSelected())
                                        {
                                            pFt3ExtCb->Selected(false);
                                            m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->SetChecked(FALSE);
                                            m_ctlSmvSendReport.RedrawRowItem(i, 2);
                                        }
                                    }
                                }
                                pBaseCb->Selected(bCheck);
                            }
                        }
                    }
                    else
                    {
                        pBaseCb->Selected(bCheck);
                        CStatusMgr::SetTips(_T(""), 100);
                    }
                }
            }
            *pResult = TRUE;
        }
    }
}

void CBasicParamDlg::OnSmvSendItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(pItemNotify->rowIndex, 0);
    DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();

    switch(ssRft)
    {
    // 通道类型
    case RFTSmvType:
        {
            pParamConfigSMVSend->SetSMVType((enumFrameType)dwValue);
            switch(dwValue)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    _refreshSmvReportWithIEC6185091();
                }
                break;
            case FRAME_TYPE_NET_9_2:
                {
                    _refreshSmvReportWithIEC6185092();
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    _refreshSmvReportWithIEC60044FT3();
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    _refreshSmvReportWithIEC60044FT3Ext();
                }
                break;
            default:
                break;
            }
            *pResult = TRUE;
        }
        break;
    case RFTSampleValDis:
        {
            //采样值显示
            pParamConfigSMVSend->SetSVDisplay((enumSmvValueType)dwValue);
            *pResult = TRUE;
        }
        break;
    case RFTSampleFreq:
        {
            //采样频率
            pParamConfigSMVSend->SetFrequency((FT3_SAMP_RATE)dwValue);
            switch(pParamConfigSMVSend->GetSMVType())
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CELRecordItem* pItem = NULL;
                    CIEC9_1Cb* pIEC91Cb = NULL;
                    SSRecordFuncType ssRft = RFTUnknown;
                    for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                    {
                        ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
                        if (ssRft == RFTSmvSend)
                        {
                            pItem = m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2);
                            if (pItem)
                            {
                                pIEC91Cb = (CIEC9_1Cb*)pItem->GetItemData();
                                pItem->SetChecked(pIEC91Cb->IsSelected());
                                m_ctlSmvSendReport.RedrawRowItem(i, 2);
                            }
                        }
                    }
                }
                break;
            case FRAME_TYPE_NET_9_2:
                {
                    CELRecordItem* pItem = NULL;
                    CIEC9_2Cb* pIEC92Cb = NULL;
                    SSRecordFuncType ssRft = RFTUnknown;
                    for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                    {
                        ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
                        if (ssRft == RFTSmvSend)
                        {
                            pItem = m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2);
                            if (pItem)
                            {
                                pIEC92Cb = (CIEC9_2Cb*)pItem->GetItemData();
                                pItem->SetChecked(pIEC92Cb->IsSelected());
                                m_ctlSmvSendReport.RedrawRowItem(i, 2);
                            }
                        }
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                //{
                //    if (FT3_SAMP_RATE_12P8K == dwValue)
                //    {
                //        pParamConfigSMVSend->SetBaudRate(FT3_BAUD_RATE_10M);

                //        SSRecordFuncType ssRft = RFTUnknown;
                //        
                //        for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                //        {
                //            ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
                //            if (ssRft == RFTBaudRate)
                //            {
                //                CELRecordItemConstraint* pItemCon  = m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->GetEditOptions(NULL)->FindConstraint(pParamConfigSMVSend->GetBaudRate());
                //                if (pItemCon)
                //                {
                //                    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2);
                //                    pItem->SetValue(pItemCon->GetConstraintCaption());
                //                    m_ctlSmvSendReport.RedrawRowItem(i, 2);
                //                }
                //                break;
                //            }
                //        }
                //    }
                //}
                //break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    if (FT3_SAMP_RATE_12P8K == dwValue)
                    {
                        pParamConfigSMVSend->SetBaudRate(FT3_BAUD_RATE_10M);

                        SSRecordFuncType ssRft = RFTUnknown;

                        for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
                        {
                            ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
                            if (ssRft == RFTBaudRate)
                            {
                                CELRecordItemConstraint* pItemCon  = m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->GetEditOptions(NULL)->FindConstraint(pParamConfigSMVSend->GetBaudRate());
                                if (pItemCon)
                                {
                                    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2);
                                    pItem->SetValue(pItemCon->GetConstraintCaption());
                                    m_ctlSmvSendReport.RedrawRowItem(i, 2);
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            }
            *pResult = TRUE;
        }
        break;
    case RFTBaudRate:
        {
            //波特率
            pParamConfigSMVSend->SetBaudRate((FT3_BAUD_RATE)dwValue);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}



void CBasicParamDlg::_showGooseSend()
{
    if(m_ctlGooseSendReport.GetSafeHwnd())
    {
        m_ctlGooseSendReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createGooseSendReport();
    }

    _reloadNaviMenu(IDR_MENU_MSG_PARAM_GOOSE_SEND);
    _refreshMenu();

    CString csCaption;
    csCaption.Format(L"%s - %s",
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG),
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_S_CAPTION));

    CStatusMgr::SetTitle(csCaption);
    m_pCurReport = &m_ctlGooseSendReport;
}


/**
 *      创建Goose发送参数报表
 */
void CBasicParamDlg::_createGooseSendReport()
{

    _createReport(m_ctlGooseSendReport, IDC_BASIC_PARAM_GOOSE_SEND_RET);
    

    CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
    if (pGooseSend == NULL)
    {
        return;
    }
    // 心跳间隔
    m_ctlGooseSendReport.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX, new CDsmBasicGooseSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_SPACE),
        pGooseSend->GetHeartbeatInterval()));

    // 额定频率
    m_ctlGooseSendReport.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX, new CDsmBasicGooseSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MIN_SPACE),
        pGooseSend->GetMinimumInterval()));
    // goose发送
    
    int i = 0;
    CString strCaption = _T("");
    CString strContent = _T("");

    CGooseCb* pGoosecb = NULL;
    pGoosecb = pGooseSend->FirstGocb();

    while (pGoosecb != NULL)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), i + 1);
        strContent.Format(_T("0x%04X-%s"), pGoosecb->GetAPPID(), pGoosecb->GetDescription().c_str());

        m_ctlGooseSendReport.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + i, new CDsmBasicGooseSendRecord(
            strCaption,
            strContent));
        CELRecordItem * pItem = m_ctlGooseSendReport.GetRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pGoosecb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pGoosecb);
        }
        pGoosecb = pGooseSend->NextGocb(pGoosecb);
        ++i;
    }

    m_ctlGooseSendReport.SetFocusedSelectionRowIndex(0);
    m_ctlGooseSendReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i < m_ctlGooseSendReport.GetItemCount(); i++)
    {
        m_ctlGooseSendReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }
}


void CBasicParamDlg::OnNaviMenuParamGooseSend(UINT nID)
{
    switch (nID)
    {
    case ID_MENU_PARAM_GOOSES_FROM_SCL:
        {
            //全站配置
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCL_DLG);
        }
        break;
    case ID_MENU_PARAM_GOOSES_FROM_SCAN:
        {
            //从扫描中添加goose
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_DLG);
        }
        break;
    case ID_MENU_PARAM_GOOSES_FROM_MANUAL:
        {
            m_ctlGooseSendReport.SetRedraw(FALSE);
            CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
            if (pGooseSend == NULL)
            {
                return;
            }
            CGooseCb* pGooseCb = pGooseSend->AddGocb();
            if (pGooseCb == NULL)
            {
                //添加失败;
                return;
            }
            //默认添加一个通道
            if(pGooseCb->GetChannelCount() == 0)
            {
                pGooseCb->AddChannel();
            }

            int nRowCount = m_ctlGooseSendReport.GetRecordRowCount();
            ASSERT(nRowCount >=2 );
            CString strCaption = _T("");
            CString strContent = _T("");

            strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), nRowCount - MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + 1);
            strContent.Format(_T("0x%04X-%s"), pGooseCb->GetAPPID(), pGooseCb->GetDescription().c_str());
            m_ctlGooseSendReport.InsertRecordRow(nRowCount, new CDsmBasicGooseSendRecord(strCaption, strContent));
            m_ctlGooseSendReport.SetFocusedSelectionRowIndex(nRowCount);
            m_ctlGooseSendReport.EnsureVisible(nRowCount, FALSE);
            CELRecordItem * pItem = m_ctlGooseSendReport.GetRecordRow(nRowCount)->GetItem(2);
            if (pItem != NULL)
            {
                pItem->HasCheckbox(TRUE);
                pItem->SetChecked(pGooseCb->IsSelected());
                pItem->SetItemData((DWORD_PTR)pGooseCb);
                pItem->SetEditable(TRUE);
            }
            m_ctlGooseSendReport.SetRedraw(TRUE);
            _refreshMenu();

        }
        break;
    case ID_MENU_PARAM_GOOSES_F2:
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlGooseSendReport.GetSafeHwnd())
            {
                return;
            }
            int nFocusRow = m_ctlGooseSendReport.GetFocusedRowIndex();
            //编辑通道
            if (nFocusRow >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
            {
                CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_EDIT_GOOSE_CTRL_DLG);
            }
        }
        break;
    case ID_MENU_PARAM_GOOSES_F3:
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlGooseSendReport.GetSafeHwnd())
            {
                return;
            }
            m_ctlGooseSendReport.SetRedraw(FALSE);
            //删除goose发送
            int nFocusRow = m_ctlGooseSendReport.GetFocusedRowIndex();
            
            if (nFocusRow >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
            {
                 CGooseCb* pGooseCb = (CGooseCb*)m_ctlGooseSendReport.GetRecordRow(nFocusRow)->GetItem(2)->GetItemData();
                m_ctlGooseSendReport.DeleteRecordRow(nFocusRow);
                
                CParamConfigGOOSESend* pConfigGooseSend = CParamConfigGOOSESend::getInstance();
                if (pConfigGooseSend == NULL)
                {
                    m_ctlGooseSendReport.SetRedraw(TRUE);
                    return;
                }
                pConfigGooseSend->DeleteGocb(pGooseCb);
                //
                //更新caption
                //
                for (int i = MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX; i < m_ctlGooseSendReport.GetRecordRowCount(); ++i)
                {
                    CString strCaption = _T("");
                    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlGooseSendReport.GetRecordRow(i)->GetItem(1);
                    if (pItem)
                    {
                        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), i - 1);
                        pItem->SetValue(strCaption);
                        m_ctlGooseSendReport.RedrawRowItem(i, 1);
                    }
                }
            }
            m_ctlGooseSendReport.SetRedraw(TRUE);
            _refreshMenu();
        }
        break;
    case ID_MENU_PARAM_GOOSES_F4:
        {
            //清空goose发送

            if (GetFocus()->GetSafeHwnd() != m_ctlGooseSendReport.GetSafeHwnd())
            {
                return;
            }

            m_ctlGooseSendReport.SetRedraw(FALSE);

            CParamConfigGOOSESend* pConfigGooseSend = CParamConfigGOOSESend::getInstance();
            if (pConfigGooseSend == NULL)
            {
                return;
            }
            CGooseCb* pGooseCb = NULL;
            while (m_ctlGooseSendReport.GetRecordRowCount() > MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
            {
                pGooseCb = (CGooseCb*)m_ctlGooseSendReport.GetRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)->GetItem(2)->GetItemData();
                m_ctlGooseSendReport.DeleteRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX);
                pConfigGooseSend->DeleteGocb(pGooseCb);
            }

            m_ctlGooseSendReport.SetRedraw(TRUE);
            _refreshMenu();
        }
        break;
    default:
        break;
    }
}

void CBasicParamDlg::OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    
    CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
    if (pGooseSend == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    ASSERT(pItem);

    switch(pItemNotify->rowIndex)
    {
        // 心跳间隔
    case MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX:
        {
            CString strValue = pItem->GetValue();
            int nTickSpace = _ttoi(strValue);
            if (nTickSpace >= MSGPARAM_TICK_SPACE_MIN && nTickSpace <= MSGPARAM_TICK_SPACE_MAX)
            {
                pGooseSend->SetHeartbeatInterval(nTickSpace);
            }
            strValue.Format(_T("%d"),  pGooseSend->GetHeartbeatInterval());
            pItem->SetValue(strValue);

            *pResult = TRUE;
        }
        break;
        // 心跳间隔
    case MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX:
        {

            CString strValue = pItem->GetValue();
            int nMinSpace = _ttoi(strValue);
            if (nMinSpace >= MSGPARAM_MIN_SPACE_MIN && nMinSpace <= MSGPARAM_MIN_SPACE_MAX)
            {
                pGooseSend->SetMinimumInterval(nMinSpace);
            }
            strValue.Format(_T("%d"), pGooseSend->GetMinimumInterval());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    default:
        {
            //goose发送

            if(pItem->GetHasCheckbox())
            {
                CGooseCb* pGooseCb = (CGooseCb*)pItem->GetItemData();
                ASSERT(pGooseCb);
                if (pGooseCb != NULL)
                {
                    bool bAuto = pItem->IsChecked() ? true: false;
                    
                    if (bAuto)
                    {
                        //
                        //判断已经选中多少个控制块
                        //
                        CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
                        if (pGooseSend == NULL)
                        {
                            return;
                        }
                        int nGooseCbSel = pGooseSend->GetSelectedGocbCount();
                        if (nGooseCbSel < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
                        {
                            pGooseCb->Selected(bAuto);
                        }
                        else
                        {
                            pItem->SetChecked(FALSE);
                            //MessageBox(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND_SEL_MAX), 
                            //    dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND),
                            //    MB_ICONINFORMATION);

                            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND_SEL_MAX), 3000);
                        }
                    }
                    else
                    {
                        pGooseCb->Selected(bAuto);
                    }
                }
            }
            *pResult = TRUE;
        }
        break;
    }

    *pResult = TRUE;
}



void CBasicParamDlg::_showGooseRec()
{
    if(m_ctlGooseRecReport.GetSafeHwnd())
    {
        m_ctlGooseRecReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createGooseRecReport();
    }

    _reloadNaviMenu(IDR_MENU_MSG_PARAM_GOOSE_REC);
    _refreshMenu();
    CString csCaption;
    csCaption.Format(L"%s - %s",
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG),
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION));

    CStatusMgr::SetTitle(csCaption);
    m_pCurReport = &m_ctlGooseRecReport;
}

/**
 *      创建Goose接收参数报表
 */
void CBasicParamDlg::_createGooseRecReport()
{
    _createReport(m_ctlGooseRecReport, IDC_BASIC_PARAM_GOOSE_REC_RET);

    CParamConfigGOOSERecv* pGooseRec = CParamConfigGOOSERecv::getInstance();
    if (pGooseRec == NULL)
    {
        return;
    }

    // goose接收

    int i = 0;
    CString strCaption = _T("");
    CString strContent = _T("");

    CGooseCb* pGoosecb = NULL;
    pGoosecb = pGooseRec->FirstGocb();

    while (pGoosecb != NULL)
    {
        strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC), i + 1);
        strContent.Format(_T("0x%04X-%s"), pGoosecb->GetAPPID(), pGoosecb->GetDescription().c_str());

        m_ctlGooseRecReport.InsertRecordRow(i, new CDsmBasicGooseRecRecord( strCaption, strContent));
        CELRecordItem * pItem = m_ctlGooseRecReport.GetRecordRow(i)->GetItem(2);
        if (pItem != NULL)
        {
            pItem->HasCheckbox(TRUE);
            pItem->SetChecked(pGoosecb->IsSelected());
            pItem->SetItemData((DWORD_PTR)pGoosecb);
        }
        pGoosecb = pGooseRec->NextGocb(pGoosecb);
        ++i;
    }

    m_ctlGooseRecReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i < m_ctlGooseRecReport.GetItemCount(); i++)
    {
        m_ctlGooseRecReport.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }
    if (m_ctlGooseRecReport.GetRecordRowCount() > 0)
    {
        m_ctlGooseRecReport.SetFocusedSelectionRowIndex(0);
    }
}

void CBasicParamDlg::OnNaviMenuParamGooseRec(UINT nID)
{
    switch (nID)
    {
    case ID_MENU_PARAM_GOOSER_FROM_SCL:
        {
            //全站配置添加goose
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCL_DLG);
        }
        break;
    case ID_MENU_PARAM_GOOSER_FROM_SCAN:
        {
            //从扫描中添加goose
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_DLG);
        }
        break;
    case ID_MENU_PARAM_GOOSER_F2:
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlGooseRecReport.GetSafeHwnd())
            {
                return;
            }
            //查看通道
            if (m_ctlGooseRecReport.GetFocusedRowIndex() >=0 )
            {
                CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_VIEW_GOOSE_CTRL_DLG);
            }
        }
        break;
    case ID_MENU_PARAM_GOOSER_F3:
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlGooseRecReport.GetSafeHwnd())
            {
                return;
            }

            m_ctlGooseRecReport.SetRedraw(FALSE);
            int nRowSel = m_ctlGooseRecReport.GetFocusedRowIndex();
            //删除
            if (nRowSel >= 0)
            {
                CGooseCb * pGooseCb = (CGooseCb*)m_ctlGooseRecReport.GetRowItemData(nRowSel, 2);
                if (pGooseCb)
                {
                    CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
                    if (pConfigGooseRec == NULL)
                    {
                        m_ctlGooseRecReport.SetRedraw(TRUE);
                        return;
                    }
                    pConfigGooseRec->DeleteGocb(pGooseCb);
                }
                m_ctlGooseRecReport.DeleteRecordRow(nRowSel);
                //
                //更新caption
                //
                CString strCaption = _T("");
                for (int i = 0; i < m_ctlGooseRecReport.GetRecordRowCount(); ++i)
                {
                    strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC), i + 1);
                    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlGooseRecReport.GetRecordRow(i)->GetItem(1);
                    if (pItem)
                    {
                        pItem->SetValue(strCaption);
                    }
                    m_ctlGooseRecReport.RedrawRowItem(i, 1);
                }
            }
            m_ctlGooseRecReport.SetRedraw(TRUE);
            _refreshMenu();

        }
        break;
    case ID_MENU_PARAM_GOOSER_F4:
        {
            if (GetFocus()->GetSafeHwnd() != m_ctlGooseRecReport.GetSafeHwnd())
            {
                return;
            }

            //清空
            CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
            if (pConfigGooseRec == NULL)
            {
                return;
            }
            CGooseCb* pGooseCb = NULL;
            for (int i = 0; i < m_ctlGooseRecReport.GetRecordRowCount(); ++i)
            {
                pGooseCb = (CGooseCb*)m_ctlGooseRecReport.GetRowItemData(i, 2);
                if (pGooseCb)
                {
                    pConfigGooseRec->DeleteGocb(pGooseCb);
                }
            }
            m_ctlGooseRecReport.DeleteAllRecordRow();
            _refreshMenu();

        }
        break;
    case ID_MENU_PARAM_GOOSER_F5:
        {
            //
        }
        break;
    case ID_MENU_PARAM_GOOSER_F6:
        {
            //开入映射
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DI_MAP_DLG);
        }
        break;
    default:
        break;
    }
}


void CBasicParamDlg::OnGooseRecItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

    if (pItemNotify == NULL)
    {
        return ;
    }
    
    CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
    if (pConfigGooseRec == NULL)
    {
        return;
    }
    CELRecordItem* pItem = m_ctlGooseRecReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex);
    if (pItem && pItem->GetHasCheckbox())
    {
        CGooseCb* pGooseCb = (CGooseCb*)pItem->GetItemData();
        if (!pGooseCb)
        {
            return;
        }

        bool bCheck = pItem->IsChecked() ? true: false;
        if (bCheck)
        {
            //
            //判断已经选中多少个控制块
            //
            if (pConfigGooseRec->GetSelectedGocbCount() < MSGPARAM_GOOSE_REC_CTRL_SELECTED_MAX)
            {
                pGooseCb->Selected(bCheck);
            }
            else
            {
                pItem->SetChecked(FALSE);
                //MessageBox(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC_SEL_MAX), 
                //    dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION),
                //    MB_ICONINFORMATION);

                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC_SEL_MAX), 3000);

            }
        }
        else
        {
            pGooseCb->Selected(bCheck);
        }
    }
    *pResult = TRUE;
}



void  CBasicParamDlg::_refreshMenu()
{
    switch(m_nMenuID)
    {
    case IDR_MENU_MSG_PARAM_SMV_SEND:
        {
            //
            //smv发送
            //
            if (GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd())
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
            }
            else
            {
                int nRow = m_ctlSmvSendReport.GetFocusedRowIndex();
                if (nRow >= 0)
                {
                    SSRecordFuncType nRecType = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(nRow, 0);
                    if (nRecType == RFTSmvSend)
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_EDIT));
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DELETE));
                    }
                    else
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                    }
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CLEAR));
                }
            }
        }
        break;
    case IDR_MENU_MSG_PARAM_GOOSE_SEND:
        {
            //goose发送
            if (GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd())
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
            }
            else
            {
                int nRow = m_ctlGooseSendReport.GetFocusedRowIndex();
                if (nRow >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_EDIT));
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DELETE));
                }
                else
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                }
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CLEAR));
            }
        }
        break;
    case IDR_MENU_MSG_PARAM_GOOSE_REC:
        {
            //
            //goose 接收
            //
            if (GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd())
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
            }
            else
            {
                int nRow = m_ctlGooseRecReport.GetFocusedRowIndex();
                if (nRow >= 0)
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_EDIT));
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DELETE));
                }
                else
                {
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, _T(""));
                    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
                }
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CLEAR));
            }
        }
        break;

    case IDR_MENU_MSG_PARAM:
        if(m_ctlBaseReport.GetFocusedRowIndex() == MSGPARAM_BASIC_ITEM_SCL && GetFocus()->GetSafeHwnd() != m_menuNavi.GetSafeHwnd() && !m_pCfg->GetSclFile().empty())
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD_SCL));
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, L"");
        }

        break;
    }
    m_pNaviBar->UpdateNaviMenu();
}


#endif
BOOL CBasicParamDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
#if SU_FEI
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_RIGHT:
            if(GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd() &&
                m_pCurReport)
            {
                if (m_pCurReport->GetSafeHwnd() && m_pCurReport->GetRecordRowCount() > 0)
                {
                    m_pCurReport->SetFocus();
                    _refreshMenu();
                    return TRUE;
                }
            }
            break;

        case VK_LEFT:
            {
                if(GetFocus()->GetSafeHwnd() != m_menuNavi.GetSafeHwnd())
                {
                    m_menuNavi.SetFocus();
                    _refreshMenu();
                    return TRUE;
                }
                else
                {
                    return TRUE;
                }
            }

            break;
        }
    }

    BOOL bRet = CBaseDlg::PreTranslateMessage(pMsg);
 
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:
        case VK_DOWN:
            {
                if(GetFocus()->GetSafeHwnd() != m_menuNavi.GetSafeHwnd() &&
                    m_pCurReport)
                {
                    _refreshMenu();
                    return TRUE;
                }
            }
        }
    }
	return bRet;
#else
	 return CBaseDlg::PreTranslateMessage(pMsg);
#endif
    
}


void CBasicParamDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    KillTimer(1);
}

void CBasicParamDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CBasicParamDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回


        CBasicParamPageDataExtra* pDataExtra = dynamic_cast<CBasicParamPageDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        //恢复左边选中菜单
#if SU_FEI
        for (int i = 0; i < m_menuNavi.GetCount(); ++i)
        {
            if (m_menuNavi.GetItemData(i) == (unsigned int)pDataExtra->m_nMsgParamType)
            {
                m_menuNavi.SetCurSel(i);
                break;
            }
        }

#endif
        //显示对应报表
        switch(pDataExtra->m_nMsgParamType)
        {
        case DSM_MSG_PARAM_BASE:
            _showBase();
            break;
#if SU_FEI
        case DSM_MSG_PARAM_SMV:
            _showSmvSend();
            break;
        case DSM_MSG_PARAM_GOOSER:
            _showGooseRec();
            break;
        case DSM_MSG_PARAM_GOOSES:
            _showGooseSend();
            break;
#else
		case DSM_MSG_PARAM_IMPORT_SCL:
			_showSCLReport();
			break;
		case DSM_MSG_PARAM_TIME_WAY:
			_showTimeReport();
			break;
#endif
        default:
            break;
        }
        //
        if (m_pCurReport->GetRecordRowCount() > pDataExtra->m_nRightReportFocus && pDataExtra->m_nRightReportFocus >= 0)
        {
            m_pCurReport->SetFocusedSelectionRowIndex(pDataExtra->m_nRightReportFocus);
            m_pCurReport->EnsureVisible(pDataExtra->m_nRightReportFocus, FALSE);
        }
        
		
#if SU_FEI
        if (pData->m_nCtrlID != m_menuNavi.GetDlgCtrlID())
        {
            m_pCurReport->SetFocus();
            _refreshMenu();
        }
        else
        {
            m_menuNavi.SetFocus();
            _refreshMenu();
        }
#else
			m_pCurReport->SetFocus();	
#endif
    }
    else
    {
        _showBase();
#if SU_FEI
		m_menuNavi.SetFocus();
#else
		m_pCurReport->SetFocus();
#endif
    }
}

void CBasicParamDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CBasicParamDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CBasicParamPageDataExtra(pData);
    }

    CBasicParamPageDataExtra* pDataExtra = dynamic_cast<CBasicParamPageDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }

    //
    //恢复左右的选中行
    //
#if SU_FEI
    pDataExtra->m_nMsgParamType = m_menuNavi.GetItemData(m_menuNavi.GetCurSel());

    pDataExtra->m_nRightReportFocus = m_pCurReport->GetFocusedRowIndex();

    int nRowSel = 0;

    //
    //选中的SMV发送控制块，用于编辑SMV发送
    //
    if(m_ctlSmvSendReport.GetSafeHwnd())
    {
        nRowSel = m_ctlSmvSendReport.GetFocusedRowIndex();
        if (nRowSel >=0)
        {
            SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(nRowSel, 0);
            if (ssRft == RFTSmvSend)
            {
                pDataExtra->m_pSmvSendSelectedCb = (CBaseCb*) m_ctlSmvSendReport.GetRowItemData(nRowSel, 2);
            }
            else
            {
                pDataExtra->m_pSmvSendSelectedCb = NULL;
            }
        }
    }
    else
    {
        pDataExtra->m_pSmvSendSelectedCb = NULL;
    }

    //
    //选中的goose发送控制块，用于编辑goose控制块信息
    //
    if (m_ctlGooseSendReport.GetSafeHwnd())
    {
        nRowSel = m_ctlGooseSendReport.GetFocusedRowIndex();
        if (nRowSel >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
        {
            pDataExtra->m_pGooseSendSelectedCb = (CGooseCb*)m_ctlGooseSendReport.GetRecordRow(nRowSel)->GetItem(2)->GetItemData();
        }
        else
        {
            pDataExtra ->m_pGooseSendSelectedCb = NULL;
        }
    }
    else
    {
        pDataExtra ->m_pGooseSendSelectedCb = NULL;
    }

    
    //
    //选中的goose接收控制块，用于查看goose控制块信息
    //
    if (m_ctlGooseRecReport.GetSafeHwnd())
    {
        nRowSel = m_ctlGooseRecReport.GetFocusedRowIndex();
        if (nRowSel >= 0)
        {
            pDataExtra->m_pGooseRecSelectedCb = (CGooseCb*)m_ctlGooseRecReport.GetRecordRow(nRowSel)->GetItem(2)->GetItemData();
        }
        else
        {
            pDataExtra ->m_pGooseRecSelectedCb = NULL;
        }
    }
    else
    {
        pDataExtra ->m_pGooseRecSelectedCb = NULL;
    }
    
    //
    //保存文件
    //
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();

    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        return ;
    }
    pParamConfigGooseSend->SaveConfig();

    CParamConfigGOOSERecv* pParamConfigGooseRec= CParamConfigGOOSERecv::getInstance();
    if (NULL == pParamConfigGooseRec)
    {
        return ;
    }
    pParamConfigGooseRec->SaveConfig();
#endif
}

void CBasicParamDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
#if SU_FEI
    if (nIDEvent == 1)
    {
        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
        if (NULL == pParamConfigSMVSend)
        {
            return ;
        }
        pParamConfigSMVSend->SaveConfig();

        CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
        if (NULL == pParamConfigGooseSend)
        {
            return ;
        }
        pParamConfigGooseSend->SaveConfig();

        CParamConfigGOOSERecv* pParamConfigGooseRec= CParamConfigGOOSERecv::getInstance();
        if (NULL == pParamConfigGooseRec)
        {
            return ;
        }
        pParamConfigGooseRec->SaveConfig();
    }
#endif
	

    CBaseDlg::OnTimer(nIDEvent);
}

LRESULT CBasicParamDlg::OnSelSCL( WPARAM, LPARAM )
{
#if SHENG_ZE
    CString csSCL;
    CString csFile = m_ctlBaseReport.GetRecordRow(0)->GetItem(1)->GetCaption();

    // 获取SD目录失败
    if(!DSM_GetSDDriver(csSCL))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SCLF));
        return 1;
    }

    csSCL += DSM_CFG_SCL_FILE_FOLDER;

    // SCL目录不存在则创建
    if(!DSM_CreateSDPath(csSCL))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CREATE_FOLDER_FAILED_SCL));
        return 1;
    }
	//csSCL = "\\User\\";

    CString csExt(DSM_CFG_SCL_FILE_EXTENSION);

    CString csTitle(CStatusMgr::GetTitle());

    CDsmFileDlg dlg(0, csSCL, csFile, csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

    HWND hWnd       = ::GetFocus();
    INT_PTR nRet    = dlg.DoModal();
    if(nRet == IDOK)
    {
        CProgressBarDlg* pDlgPro = new CProgressBarDlg(this);

        // 加载SCL  SHENG_ZE
        _SCLTaskArgs arg;
        arg.csFile = dlg._GetFilePath();
        arg.m_pWnd = pDlgPro;

        CString csTips;

        pDlgPro->_SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL_TIPS));
        pDlgPro->_SetTitle(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL));
        pDlgPro->_SetTask(_LoadSCLTask, &arg);

        CRect rc(pDlgPro->_GetPicRC());
        rc.OffsetRect(-26, 0);
        pDlgPro->_SetPicRC(rc);
        rc = pDlgPro->_GetTextRC();
        rc.OffsetRect(-26, 0);
        pDlgPro->_SetTextRC(rc);

        INT_PTR nRet = pDlgPro->DoModal();
        delete pDlgPro;
        pDlgPro = NULL;

        if(nRet == IDOK)
        {
            csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_SUCCESS), dlg._GetFile());
            CStatusMgr::SetTips(csTips);

            CDsmDataLayer::getInstance()->ClearAppidSmvRecvCache();
        }
        else
        {
            csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILEDF), dlg._GetFile());
            CStatusMgr::SetTips(csTips);
        }

        m_pCfg->SetSclFile((LPCTSTR)dlg._GetFile());

        CString csSCLFile(m_pCfg->GetSclFile().c_str());

        if(!CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
        {
            if(m_pCfg->GetSclFile().empty())
            {
                csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
            }

            else
            {
                csSCLFile.Format(L"%s  (%s)", m_pCfg->GetSclFile().c_str(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
            }
        }

        CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlBaseReport.GetRecordRow(0)->GetItem(1));
        if(pItemText)
        {
            pItemText->SetValue(csSCLFile);
        }
#if SU_FEI
        _refreshMenu();
#endif
    }

    if(nRet == IDBACKTOMAIN)
    {
        return 0;
    }

    if(IsWindow(hWnd))
    {
        CStatusMgr::SetTitle(csTitle);
        ::SetFocus(hWnd);
    }
    else
    {
        DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    }
    return 0;
#else
	if (m_sclModel == SclFromSd)
	{
		CString csSCL;
		CString csFile = m_ctlBaseReport.GetRecordRow(0)->GetItem(1)->GetCaption();

		// 获取SD目录失败
		if(!DSM_GetSDDriver(csSCL))
		{
			CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SCLF));
			return 1;
		}

		csSCL += DSM_CFG_SCL_FILE_FOLDER;

		// SCL目录不存在则创建
		if(!DSM_CreateSDPath(csSCL))
		{
			CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CREATE_FOLDER_FAILED_SCL));
			return 1;
		}
		//csSCL = "\\User\\";

		CString csExt(DSM_CFG_SCL_FILE_EXTENSION);

		CString csTitle(CStatusMgr::GetTitle());

		CDsmFileDlg dlg(0, csSCL, csFile, csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

		HWND hWnd       = ::GetFocus();
		INT_PTR nRet    = dlg.DoModal();
		if(nRet == IDOK)
		{
			CProgressBarDlg* pDlgPro = new CProgressBarDlg(this);

			// 加载SCL  SHENG_ZE
			_SCLTaskArgs arg;
			arg.csFile = dlg._GetFilePath();
			arg.m_pWnd = pDlgPro;

			CString csTips;

			pDlgPro->_SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL_TIPS));
			pDlgPro->_SetTitle(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL));
			pDlgPro->_SetTask(_LoadSCLTask, &arg);

			CRect rc(pDlgPro->_GetPicRC());
			rc.OffsetRect(-26, 0);
			pDlgPro->_SetPicRC(rc);
			rc = pDlgPro->_GetTextRC();
			rc.OffsetRect(-26, 0);
			pDlgPro->_SetTextRC(rc);

			INT_PTR nRet = pDlgPro->DoModal();
			delete pDlgPro;
			pDlgPro = NULL;

			if(nRet == IDOK)
			{
				csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_SUCCESS), dlg._GetFile());
				CStatusMgr::SetTips(csTips);

				CDsmDataLayer::getInstance()->ClearAppidSmvRecvCache();

#if !CHUN_HUA
				if (m_ctlImportSCLReport.GetSafeHwnd())
				{

					CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlImportSCLReport.GetRecordRow(0)->GetItem(1));
					if(pItemText)
					{
						pItemText->SetValue(dlg._GetFile());
						m_ctlImportSCLReport.RedrawRowItem(0,1);
					}

				}
#endif
			}
			else
			{
				csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILEDF), dlg._GetFile());
				CStatusMgr::SetTips(csTips);
			}

			m_pCfg->SetSclFile((LPCTSTR)dlg._GetFile());
			m_pCfg->SetSclPath((LPCTSTR)dlg._GetFilePath());

			CString csSCLFile(m_pCfg->GetSclFile().c_str());

			if(!CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
			{
				if(m_pCfg->GetSclFile().empty())
				{
					csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
				}
				//  IEC设置-SCL导入显示框显示文件名 SHENG_ZE
				else
				{
					csSCLFile.Format(L"%s  (%s)", m_pCfg->GetSclFile().c_str(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
				}
			}

			CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlBaseReport.GetRecordRow(0)->GetItem(1));
			if(pItemText)
			{
				pItemText->SetValue(csSCLFile);
			}

		}

		if(nRet == IDBACKTOMAIN)
		{
			return 0;
		}

		if(IsWindow(hWnd))
		{
			CStatusMgr::SetTitle(csTitle);
			::SetFocus(hWnd);
		}
		else
		{
			DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
		}
		return 0;
	}
	else
	{
		CString csSCL;
		CString csFile = m_ctlBaseReport.GetRecordRow(1)->GetItem(1)->GetCaption();

		
		csSCL = "\\硬盘";  // 暂时的U盘的路径
		// 获取SD目录失败
		if(!CStatusMgr::GetUSBDisk())
		{
			CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_USBDISK_NOT_FOUND_SCLF));
			return 1;
		}

		csSCL += DSM_CFG_SCL_FILE_FOLDER;


		CString csExt(DSM_CFG_SCL_FILE_EXTENSION);

		CString csTitle(CStatusMgr::GetTitle());

		CDsmFileDlg dlg(0, csSCL, csFile, csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

		HWND hWnd       = ::GetFocus();
		INT_PTR nRet    = dlg.DoModal();
		if(nRet == IDOK)
		{
			CProgressBarDlg* pDlgPro = new CProgressBarDlg(this);

			// 加载SCL  SHENG_ZE
			_SCLTaskArgs arg;
			arg.csFile = dlg._GetFilePath();
			arg.m_pWnd = pDlgPro;

			CString csTips;

			pDlgPro->_SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL_TIPS));
			pDlgPro->_SetTitle(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL));
			pDlgPro->_SetTask(_LoadSCLTask, &arg);

			CRect rc(pDlgPro->_GetPicRC());
			rc.OffsetRect(-26, 0);
			pDlgPro->_SetPicRC(rc);
			rc = pDlgPro->_GetTextRC();
			rc.OffsetRect(-26, 0);
			pDlgPro->_SetTextRC(rc);

			INT_PTR nRet = pDlgPro->DoModal();
			delete pDlgPro;
			pDlgPro = NULL;

			if(nRet == IDOK)
			{
				csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_SUCCESS), dlg._GetFile());
				CStatusMgr::SetTips(csTips);

				CDsmDataLayer::getInstance()->ClearAppidSmvRecvCache();

#if !CHUN_HUA
				if (m_ctlImportSCLReport.GetSafeHwnd())
				{

					CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlImportSCLReport.GetRecordRow(1)->GetItem(1));
					if(pItemText)
					{
						pItemText->SetValue(dlg._GetFile());
						m_ctlImportSCLReport.RedrawRowItem(1,1);
					}

				}
#endif
			}
			else
			{
				csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILEDF), dlg._GetFile());
				CStatusMgr::SetTips(csTips);
			}

			m_pCfg->SetSclFile((LPCTSTR)dlg._GetFile());
			m_pCfg->SetSclPath((LPCTSTR)dlg._GetFilePath());

			CString csSCLFile(m_pCfg->GetSclFile().c_str());

			if(!CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
			{
				if(m_pCfg->GetSclFile().empty())
				{
					csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
				}

				else
				{
					csSCLFile.Format(L"%s  (%s)", m_pCfg->GetSclFile().c_str(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
				}
			}

			CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlBaseReport.GetRecordRow(0)->GetItem(1));
			if(pItemText)
			{
				pItemText->SetValue(csSCLFile);
			}

		}

		if(nRet == IDBACKTOMAIN)
		{
			return 0;
		}

		if(IsWindow(hWnd))
		{
			CStatusMgr::SetTitle(csTitle);
			::SetFocus(hWnd);
		}
		else
		{
			DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
		}
		return 0;
	}
#endif
}

void CBasicParamDlg::OnMenuF6()
{
#if SU_FEI
    int nSel = m_ctlBaseReport.GetFocusedRowIndex();

    if(nSel == MSGPARAM_BASIC_ITEM_SCL && !m_pCfg->GetSclFile().empty())
    {
        if(CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
        {
            CDsmDataLayer::getInstance()->GetSclManager()->UnLoad();
        }

        CString csSCLFile;
        csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
        m_pCfg->SetSclFile(L"");

        CELRecordItemText* pItemText = dynamic_cast<CELRecordItemText*>(m_ctlBaseReport.GetRecordRow(0)->GetItem(1));
        if(pItemText)
        {
            pItemText->SetValue(csSCLFile);
            m_ctlBaseReport.Invalidate();
        }

        _refreshMenu();
    }
#else
	_showTimeReport();

#endif
}

#if !SU_FEI
/**
**   创建SCL表格
*
*/
void CBasicParamDlg::_createSCLReport()
{
	_createReport(m_ctlImportSCLReport, IDC_BASIC_SCL_RET);

	CString csSCLFile(m_pCfg->GetSclFile().c_str());

	if(!CDsmDataLayer::getInstance()->GetSclManager()->IsSclLoaded())
	{
		if(m_pCfg->GetSclFile().empty())
		{
			csSCLFile.Format(L"(%s)", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
		}
		else
		{
			csSCLFile.Format(L"%s  (%s)", m_pCfg->GetSclFile().c_str(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_UNLOAD));
		}
	}
#if SHENG_ZE
	// SCL文件
	m_ctlImportSCLReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_SCL, new CDsmBasicRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SCL_IMPRORT_FROMSD), 
		csSCLFile));
#else
	m_ctlImportSCLReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_SCL, new CDsmBasicRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SCL_IMPRORT_FROMU), 
		csSCLFile));
#endif

#if !SHENG_ZE  // 插入一条记录, 并指定第一列的文本,
	m_ctlImportSCLReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_SCL, new CDsmBasicRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SCL_IMPRORT_FROMSD), 
		csSCLFile));
#endif
	
	//m_ctlImportSCLReport.InsertRecordRow(1, L"");

	m_ctlImportSCLReport.GetRecordRow(MSGPARAM_BASIC_ITEM_SCL)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL)->AddExpandButton();
#if !SHENG_ZE  // 为新增的信息行增加一个扩展按钮
	m_ctlImportSCLReport.GetRecordRow(MSGPARAM_BASIC_ITEM_SCL+1)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL)->AddExpandButton();
#endif
}


/*
**   显示SCL表格
*
*/
void CBasicParamDlg::_showSCLReport()
{
	m_pCurReport = &m_ctlImportSCLReport;

	if (m_ctlTimeReport.GetSafeHwnd())
	{
		m_ctlTimeReport.ShowWindow(SW_HIDE);
	}

	if(m_ctlBaseReport.GetSafeHwnd())
	{
		m_ctlBaseReport.ShowWindow(SW_HIDE);
	}

	if (m_ctlImportSCLReport.GetSafeHwnd())
	{
		m_ctlImportSCLReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createSCLReport();
	}

	//_reloadNaviMenu(IDR_MENU_MSG_PARAM);

	m_ctlImportSCLReport.SetFocus();
	m_ctlImportSCLReport.SetFocusedSelectionRowIndex(0); // 焦点聚焦到第一行
	
	// 获得控件坐标
	m_ctlImportSCLReport.GetColumnExAttr(MSGPARAM_BASIC_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlImportSCLReport.GetRecordRow(0)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(TRUE);

#if !SHENG_ZE
	m_ctlImportSCLReport.GetColumnExAttr(MSGPARAM_BASIC_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlImportSCLReport.GetRecordRow(1)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(TRUE);
#endif

	CString csCaption;
	csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SCL_IMPRORT));

	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_BASIC_PARAM_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_MENU_IMPORT_SCL_SEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_TIME_WAY_UNSEL), NULL, NULL);

	m_pNaviBar->UpdateNaviMenu();

	CStatusMgr::SetTitle(csCaption);
	
}


/**
**   创建对时表格
*
*/
void CBasicParamDlg::_createTimeReport()
{
	_createReport(m_ctlTimeReport, IDC_BASIC_TIME_RET);

	CString csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_B);

	if(FPGA_TIME_SYNC_IRIGB_NEG == m_pCfg->GetTimeSyncMode())
	{
		csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_NB);
	}
	//     else if(FPGA_TIME_SYNC_IEEE1588_A == m_pCfg->GetTimeSyncMode())
	//     {
	//         csValue = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_1588);
	//     }

	// 对时方案
	m_ctlTimeReport.InsertRecordRow(MSGPARAM_BASIC_ITEM_TIMETICK, new CDsmBasicRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_TICK),
		csValue));

//	m_ctlTimeReport.InsertRecordRow(1, L"");

	// 对时方案
	CELRecordItemEditOptions* pOption = m_ctlTimeReport.GetRecordRow(MSGPARAM_BASIC_ITEM_TIMETICK)->GetItem(MSGPARAM_BASIC_VALUE)->GetEditOptions(NULL);
	pOption->AddComboButton();
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_B), FPGA_TIME_SYNC_IRIGB_POS);
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_IRIG_NB), FPGA_TIME_SYNC_IRIGB_NEG);
	//pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_1588), FPGA_TIME_SYNC_IEEE1588_A);
}


/*
**   显示对时表格
*
*/
void CBasicParamDlg::_showTimeReport()
{
	m_pCurReport = &m_ctlTimeReport;

	if (m_ctlImportSCLReport.GetSafeHwnd())
	{
		m_ctlImportSCLReport.ShowWindow(SW_HIDE);
	}

	if(m_ctlBaseReport.GetSafeHwnd())
	{
		m_ctlBaseReport.ShowWindow(SW_HIDE);
	}

	if (m_ctlTimeReport.GetSafeHwnd())
	{
		m_ctlTimeReport.ShowWindow(SW_SHOW);
		m_ctlTimeReport.SetFocus();
	}
	else
	{
		_createTimeReport();
	}

	//_reloadNaviMenu(IDR_MENU_MSG_PARAM);

	m_ctlTimeReport.SetFocus();
	m_ctlTimeReport.SetFocusedSelectionRowIndex(0);
	
	m_ctlTimeReport.GetColumnExAttr(MSGPARAM_BASIC_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlTimeReport.GetRecordRow(0)->GetItem(MSGPARAM_BASIC_VALUE)->SetEditable(TRUE);

	CString csCaption;
	csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_RATED_TICK));


	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_BASIC_PARAM_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_MENU_IMPORT_SCL_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_TIME_WAY_SEL), NULL, NULL);

	m_pNaviBar->UpdateNaviMenu();
	CStatusMgr::SetTitle(csCaption);
	
}




void CBasicParamDlg::OnMenuF1()
{
	_showBase();
}
void CBasicParamDlg::OnMenuF2()
{
	CEventProc::GetInstance()->ShowPage(IDD_PARAM_SMV_TX_DLG);
}
void CBasicParamDlg::OnMenuF3()
{
	CEventProc::GetInstance()->ShowPage(IDD_PARAM_GOOSE_TX_DLG);
}
void CBasicParamDlg::OnMenuF4()
{
	CEventProc::GetInstance()->ShowPage(IDD_PARAM_GOOSE_RX_DLG);
}
void CBasicParamDlg::OnMenuF5()
{
	_showSCLReport();
}


//void CBasicParamDlg::_modifyMenu()
//{
//	__super::_modifyMenu();
//	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_MENU_BASIC_PARAM_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_BASIC_PARAM_SEL), NULL);
//	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_MENU_IMPORT_SCL_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_IMPORT_SCL_SEL), NULL);
//	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_MENU_TIME_WAY_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_MENU_TIME_WAY_SEL), NULL);
//}
#endif
