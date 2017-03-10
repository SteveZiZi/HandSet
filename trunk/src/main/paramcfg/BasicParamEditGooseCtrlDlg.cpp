/// @file
///
/// @brief
///     编辑goose对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///


#include "stdafx.h"
///#include "BasicParamDlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/paramcfg/common_cfg.h"
#include "BasicParamEditGooseCtrlDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/config/goosecb.h"
#include "src/utils/CommonControl/MacEditCtrl/MacEditCtrl.h"
#include "src/service/datalayer/config/ParamConfigGOOSESend.h"

#include "BasicParamGooseTxDlg.h"


//////////////////////////////////////////////////////////////////////////
// 编辑GOOSE通道报表
//
#if SU_FEI
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_INDEX   50
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_DESC    290
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_TYPE    180
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_MAP     80
#else
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_INDEX   60
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_DESC    210
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_TYPE    179
#define DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_MAP     80

#endif
#define DSM_GOOSE_CHAN_MIN                      1

///
/// 报表显示的类型
///
enum EGShowReportType
{
    EGShowChannelReport = 0,                                ///< 通道报表
    EGShowCtrlBlockReport                                   ///< 控制块信息报表
};



#define MSGPARAM_EDIT_GOOSE_CTRL_BLOCK_DESC       0         ///< 控制块描述
#define MSGPARAM_EDIT_GOOSE_SEND_PORT             1         ///< 
#define MSGPARAM_EDIT_GOOSE_APPID                 2         ///< 
#define MSGPARAM_EDIT_GOOSE_MAC_ADDR              3         ///< 
#define MSGPARAM_EDIT_GOOSE_VLAN_TAG              4         ///< 
#define MSGPARAM_EDIT_GOOSE_VLAN_ID				  5
#define MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY         6         ///< 
#define MSGPARAM_EDIT_GOOSE_CFG_VERSION           7         ///< 
#define MSGPARAM_EDIT_GOOSE_TEST_FLAG             8         ///< 
#define MSGPARAM_EDIT_GOOSE_NEED_COMM             9        ///< 
#define MSGPARAM_EDIT_GOOSE_GOCB_REF              10         ///< 
#define MSGPARAM_EDIT_GOOSE_DATA_SET              11        ///< 
#define MSGPARAM_EDIT_GOOSE_GO_ID                 12        ///< 
#define MSGPARAM_EDIT_GOOSE_CHAN_COUNT            13        ///< 

//
//报表列索引
//
#define MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX        0
#define MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC         1
#define MSGPARAM_EDIT_GOOSE_COL_CHAN_TYPE         2
#define MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP          3



// CBasicParamEditGooseCtrlDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamEditGooseCtrlDlg, CBaseDlg)

CBasicParamEditGooseCtrlDlg::CBasicParamEditGooseCtrlDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamEditGooseCtrlDlg::IDD, DSM_STR_BASIC_PARAM_GOOSE_CTRL_BLOCK_CAPTION, pParent)
    , m_nShowReportType(EGShowCtrlBlockReport)
    , m_pGooseCb(NULL)
{
    m_nMenuID = IDR_MENU_MSG_PARAM_GOOSE_CTRL_BLOCK;
    
}

CBasicParamEditGooseCtrlDlg::~CBasicParamEditGooseCtrlDlg()
{
}

void CBasicParamEditGooseCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamEditGooseCtrlDlg, CBaseDlg)
    
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET, &CBasicParamEditGooseCtrlDlg::OnGooseCtrlInplaceButtonClick)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET, &CBasicParamEditGooseCtrlDlg::OnGooseCtrlItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET, &CBasicParamEditGooseCtrlDlg::OnGooseCtrlItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET, &CBasicParamEditGooseCtrlDlg::OnGooseCtrlItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET,  &CBasicParamEditGooseCtrlDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET,  &CBasicParamEditGooseCtrlDlg::OnInplaceEditAction)

    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_PARAM_EDIT_GOOSE_CHAN_RET, &CBasicParamEditGooseCtrlDlg::OnGooseChanItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_EDIT_GOOSE_CHAN_RET, &CBasicParamEditGooseCtrlDlg::OnGooseChanItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_EDIT_GOOSE_CHAN_RET,  &CBasicParamEditGooseCtrlDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_EDIT_GOOSE_CHAN_RET,  &CBasicParamEditGooseCtrlDlg::OnInplaceEditAction)

    ON_COMMAND(ID_MENU_PARAM_GOOSES_CTRL_F1, &CBasicParamEditGooseCtrlDlg::OnMenuParamEditGoosesCtrlF1)
#if !SU_FEI
	ON_COMMAND(ID_MENU_PARAM_GOOSES_CTRL_F2, &CBasicParamEditGooseCtrlDlg::OnMenuParamEditGoosesCtrlF2)
		
#endif
    ON_COMMAND_RANGE(ID_MENU_PARAM_EDIT_GOOSES_CHAN_F1, ID_MENU_PARAM_EDIT_GOOSES_CHAN_F6, &CBasicParamEditGooseCtrlDlg::OnNaviMenuParamEditGoosesChan)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CBasicParamEditGooseCtrlDlg 消息处理程序

BOOL CBasicParamEditGooseCtrlDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
#if SU_FEI
    if (pPageData == NULL || 
        pPageData->m_nPageID != CBasicParamDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
#else
	if (pPageData == NULL ||
		pPageData->m_nPageID != CBasicParamGooseTxDlg::IDD ||
		pPageData->m_pPageDataExtra == NULL)	
#endif
    {
        //
        //参数配置进入（goose编辑或查看）
        //
        return FALSE;
    }
#if SU_FEI
    CBasicParamPageDataExtra* pDataExtra = dynamic_cast<CBasicParamPageDataExtra*>(pPageData->m_pPageDataExtra);
#else
	CBasicParamPageDataExtraGoose* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGoose*>(pPageData->m_pPageDataExtra);
#endif
    if (pDataExtra == NULL || pDataExtra->m_pGooseSendSelectedCb == NULL)
    {
        return FALSE;
    }
    m_pGooseCb = pDataExtra->m_pGooseSendSelectedCb;
    if (m_pGooseCb == NULL)
    {
        return FALSE;
    }


    _showCtrlBlockReport();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CBasicParamEditGooseCtrlDlg::_createCtrlBlockReport()
{
    ASSERT(m_pGooseCb);
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_EDIT_GOOSE_CTRL_BLOCK_RET);

    // 隐藏表头
    m_ctlCtrlBlockReport.SetHeaderHeight(0);

    // 设置字体
    m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlCtrlBlockReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlCtrlBlockReport.InsertColumn(0, 
        _T(""), 
        EL_LEFT,
        DSM_BASIC_EDIT_GOOSE_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);

    m_ctlCtrlBlockReport.InsertColumn(1, 
        _T(""), 
        EL_LEFT,
        DSM_BASIC_EDIT_GOOSE_WIDTH_CONTENT);


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

    CELRecordItemEditOptions* pOptions = NULL;
    CELRecordItemText* pItem = NULL;
    CString  strTemp = _T("");

    // 额定通道类型
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_CTRL_BLOCK_DESC, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_BLOCK_DESC),
       m_pGooseCb->GetDescription().c_str()));

    //发送端口
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_SEND_PORT, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SEND_PORT),
        _T("")));
    pItem = dynamic_cast<CELRecordItemText *>(m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_SEND_PORT)->GetItem(1));

    pItem->SetEditable(TRUE);
    pOptions = pItem->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX2), NET_PORT_LIGHT_B);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX1), NET_PORT_LIGHT_A);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ELEC_PORT), NET_PORT_ELECTRONIC);
    
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(m_pGooseCb->GetPortName());
    if (pItemCon)
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    strTemp.Format(_T("%04X"), m_pGooseCb->GetAPPID());
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_APPID, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_APPID_HEX),
        strTemp));

    unsigned char ucMac[6];
    m_pGooseCb->GetDesMAC(ucMac, 6);

    CMacEditCtrl::MacAddr2Str(ucMac, strTemp);
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_MAC_ADDR, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAC_ADDR),
        strTemp));
    

    m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_MAC_ADDR)->GetItem(1)->GetEditOptions(NULL)->AddExpandButton();

	m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_ID, new CDsmGooseCtrlInfoRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VLAN_ID),
		m_pGooseCb->GetVlanID()));

	m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY, new CDsmGooseCtrlInfoRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VLAN_PRIORITY),
		m_pGooseCb->GetVLanPriority()));

#if !CHUN_HUA
	m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_TAG, new CDsmGooseCtrlInfoRecord(
		/*dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VLAN_TAG),*/
		_T("是否带VLAN标志"),
		_T("")));
	pItem = dynamic_cast<CELRecordItemText *>(m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_TAG)->GetItem(1));
	pItem->HasCheckbox(TRUE);
	pItem->SetChecked(m_pGooseCb->GetVlanFlag());

	if (!m_pGooseCb->GetVlanFlag())
	{
		m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_ID)->SetFocusable(FALSE);
		m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY)->SetFocusable(FALSE);

	}
 
#endif

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_CFG_VERSION, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CFG_VERSION),
        m_pGooseCb->GetConfigRevision()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_TEST_FLAG, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TEST_FLAG),
        _T("")));
    pItem = dynamic_cast<CELRecordItemText *>(m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_TEST_FLAG)->GetItem(1));
    pItem->HasCheckbox(TRUE);
    pItem->SetChecked(m_pGooseCb->GetTestFlag());

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_NEED_COMM, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NEED_COMM),
        _T("")));

    pItem = dynamic_cast<CELRecordItemText *>(m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_NEED_COMM)->GetItem(1));
    pItem->HasCheckbox(TRUE);
    pItem->SetChecked(m_pGooseCb->GetNeedComm());

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_GOCB_REF, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOCB_REF),
        m_pGooseCb->GetGocbRef().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_DATA_SET, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DATA_SET),
        m_pGooseCb->GetDataSet().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_GO_ID, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GO_ID),
        m_pGooseCb->GetGoID().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_EDIT_GOOSE_CHAN_COUNT, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        m_pGooseCb->GetChannelCount()));

    m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
    m_ctlCtrlBlockReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = MSGPARAM_EDIT_GOOSE_CTRL_BLOCK_DESC; i <= MSGPARAM_EDIT_GOOSE_CHAN_COUNT; i++)
    {
        m_ctlCtrlBlockReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
}

void CBasicParamEditGooseCtrlDlg::_createChannelReport()
{

    ASSERT(m_pGooseCb);
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlChannelReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_EDIT_GOOSE_CHAN_RET);
    
    // 设置字体
    m_ctlChannelReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlChannelReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlChannelReport.PostInitTemplateStyle();

    // 插入列元素

    m_ctlChannelReport.InsertColumn(MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, _T(""), EL_CENTER, DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_INDEX);
    m_ctlChannelReport.InsertColumn(MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_DESC), EL_CENTER, DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_DESC);
    m_ctlChannelReport.InsertColumn(MSGPARAM_EDIT_GOOSE_COL_CHAN_TYPE, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_TYPE), EL_CENTER, DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_TYPE);
    m_ctlChannelReport.InsertColumn(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP), EL_CENTER, DSM_BASIC_EDIT_GOOSE_WIDTH_CHAN_MAP);

    // 设置选择焦点样式

    m_ctlChannelReport.SetRowSelectionVisible(FALSE);
    m_ctlChannelReport.SetRowFocusEnable(TRUE);
    m_ctlChannelReport.SetSubItemFocusEnable(TRUE);
    m_ctlChannelReport.SetFreezeSelectionVisible(TRUE);
    m_ctlChannelReport.SetFreezeColumnsCount(1);
    m_ctlChannelReport.SetFocusedColIndex(1);
    m_ctlChannelReport.SetFocus();

    _updateChanReport();


    m_ctlChannelReport.GetColumnExAttr(MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlChannelReport.GetColumnExAttr(MSGPARAM_EDIT_GOOSE_COL_CHAN_TYPE)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlChannelReport.GetColumnExAttr(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP)->GetEditOptions()->SetAllowEdit(TRUE);
}


void CBasicParamEditGooseCtrlDlg::_updateChanReport()
{
    if (!m_ctlChannelReport.GetSafeHwnd())
    {
        return;
    }
    m_ctlChannelReport.DeleteAllRecordRow();

    int nRowIndex = 0;
    int nIndex = 1;
    int nSubIndex = 1;
    CString strIndex = _T("");
    CString strTemp = _T("");
    CChannel* pChannel = NULL;
    CChannel* pSubChannel = NULL;

    pChannel = m_pGooseCb->FirstChannel();
    while(pChannel)
    {

        //
        //通道,将通道号和子通道号分别设置到0、1列 
        //
        m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pChannel, nIndex));
        m_ctlChannelReport.SetRecordRowData(nRowIndex, (DWORD_PTR)pChannel);
        m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
        m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, 0);
        ++nRowIndex;

        if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
        {
            //
            //子通道
            //
            nSubIndex = 1;
            pSubChannel = pChannel->FirstSubChannel();
            while(pSubChannel)
            {
                m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pSubChannel, nIndex, nSubIndex));
                m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);
                m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
                m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, nSubIndex);
                ++nRowIndex;
                ++nSubIndex;
                pSubChannel = pChannel->NextSubChannel(pSubChannel);
            }
        }
        ++nIndex;
        pChannel = m_pGooseCb->NextChannel(pChannel);
    }

    if (m_ctlChannelReport.GetRecordRowCount() > 0 )
    {
        m_ctlChannelReport.SetFocusedSelectionRowIndex(0);
    }
}
/**
 *      刷新导航菜单
 */
void CBasicParamEditGooseCtrlDlg::_showCtrlBlockReport()
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
    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_HIDE);
    }
    m_ctlCtrlBlockReport.SetFocus();
    _reloadNaviMenu(IDR_MENU_MSG_PARAM_GOOSE_CTRL_BLOCK);
#if SU_FEI
    m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL), NULL, NULL);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_SEL), NULL, NULL);
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_UNSEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(4, NULL, NULL, NULL);
#endif
    m_pNaviBar->UpdateNaviMenu();

    CString csCaption;
    csCaption.Format(L"%s", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_CTRL_BLOCK_CAPTION));
    CStatusMgr::SetTitle(csCaption);

}


void CBasicParamEditGooseCtrlDlg::_updateCtrlBlockChanCount()
{
    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }

    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_CHAN_COUNT)->GetItem(1);
    if (pItem)
    {

        CString strChanCount = _T("");
        strChanCount.Format(_T("%d"), m_pGooseCb->GetChannelCount());
        pItem->SetValue(strChanCount);
        m_ctlCtrlBlockReport.RedrawRowItem(MSGPARAM_EDIT_GOOSE_CHAN_COUNT, 1);
    }
}

/**
 *      刷新导航菜单
 */
void CBasicParamEditGooseCtrlDlg::_showChannelReport()
{
    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createChannelReport();
    }

    if(m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        m_ctlCtrlBlockReport.ShowWindow(SW_HIDE);
    }
    m_ctlChannelReport.SetFocus();
    
    //
    //更新菜单
    //
    _reloadNaviMenu(IDR_MENU_MSG_PARAM_EDIT_GOOSE_CHAN);
#if SU_FEI
    m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN), NULL, NULL);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_UNSEL), NULL, NULL);
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_SEL), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
#endif
    m_pNaviBar->UpdateNaviMenu();

    CString csCaption;
    csCaption.Format(_T("%s->0x%04X"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_EDIT_CHAN_CAPTION), m_pGooseCb->GetAPPID());
    CStatusMgr::SetTitle(csCaption);
}

#if SU_FEI
void CBasicParamEditGooseCtrlDlg::OnMenuParamEditGoosesCtrlF1()
{
    m_nShowReportType = EGShowChannelReport;
    _showChannelReport();
}
#else

void CBasicParamEditGooseCtrlDlg::OnMenuParamEditGoosesCtrlF1()
{
	m_nShowReportType = EGShowCtrlBlockReport;
	_showCtrlBlockReport();
}

void CBasicParamEditGooseCtrlDlg::OnMenuParamEditGoosesCtrlF2()
{
	m_nShowReportType = EGShowChannelReport;
	_showChannelReport();
}

#endif
void CBasicParamEditGooseCtrlDlg::OnNaviMenuParamEditGoosesChan(UINT nID)
{
    ASSERT(m_pGooseCb);

    switch (nID)
    {
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F1:
        {
            m_nShowReportType = EGShowCtrlBlockReport;
            _showCtrlBlockReport();
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F2:
        {
#if SU_FEI
            //添加
            CChannel* pChannel = m_pGooseCb->AddChannel();
            if (pChannel != NULL)
            {
                int nRowCount = m_ctlChannelReport.GetRecordRowCount();
                int nRowIndex = nRowCount;
                if(nRowCount == 0)
                {
                    //
                    //没有通道
                    //
                    m_ctlChannelReport.InsertRecordRow(0, new CDsmEditGooseSendChannelRecord(pChannel, 1, 0) );
                    m_ctlChannelReport.SetRecordRowData(0, (DWORD_PTR)pChannel);
                    m_ctlChannelReport.SetRowItemData(0, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, 1);
                    m_ctlChannelReport.SetRowItemData(0, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, 0);
                }
                else
                {
                    //
                    //添加通道
                    //
                    int nIndex = m_ctlChannelReport.GetRowItemData(nRowIndex -1, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                    ++nIndex;
                    m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pChannel, nIndex, 0));
                    m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pChannel);

                    m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
                    m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, 0);

                    if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
                    {
                        //
                        //如果是结构类型，添加子通道
                        //
                        ++nRowIndex;
                        int nSubIndex = 1;
                        CChannel * pSubChannel = pChannel->FirstSubChannel();
                        while(pSubChannel)
                        {
                            m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pSubChannel, nIndex, nSubIndex));
                            m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);
                            m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
                            m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, nSubIndex);
                            ++nRowIndex;
                            ++nSubIndex;
                            pSubChannel = pChannel->NextSubChannel(pSubChannel);
                        }
                    }
                }
                m_ctlChannelReport.SetFocusedSelectionRowIndex(m_ctlChannelReport.GetRecordRowCount()-1);
                m_ctlChannelReport.EnsureVisible(m_ctlChannelReport.GetRecordRowCount()-1, FALSE);
            }
#else
			m_nShowReportType = EGShowChannelReport;
			_showChannelReport();

#endif
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F3:
        {
#if SU_FEI
            //删除
            
            if (m_pGooseCb->GetChannelCount() <= DSM_GOOSE_CHAN_MIN)
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_CHAN_MIN), 3000);
                return;
            }
            else
            {
                int nRowSelIndex = m_ctlChannelReport.GetFocusedRowIndex();
                if (nRowSelIndex < 0)
                {
                    return;
                }
                CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowSelIndex);
                ASSERT(pChannel);
                
                //
                int nIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                int nSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);
                
                if (nSubIndex > 0)
                {
                    //选中子通道，不允许删除
                    return;
                }

                m_ctlChannelReport.SetRedraw(FALSE);
                enumChannelType nChanType = pChannel->GetChannelType();

                m_pGooseCb->DeleteChannel(pChannel);
                m_ctlChannelReport.DeleteRecordRow(nRowSelIndex);
                if (nChanType == CHANNEL_TYPE_GOOSE_STRUCT)
                {
                    //判断下一行是不是子通道，如果是则删除；
                    int nNextIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                    int nNextSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);
                    while(nNextIndex == nIndex && nNextSubIndex > 0)
                    {
                        m_ctlChannelReport.DeleteRecordRow(nRowSelIndex);
                        if (nRowSelIndex < m_ctlChannelReport.GetRecordRowCount())
                        {
                            nNextIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                            nNextSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                m_ctlChannelReport.SetRedraw(TRUE);
            }
#else
			//添加
			CChannel* pChannel = m_pGooseCb->AddChannel();
			if (pChannel != NULL)
			{
				int nRowCount = m_ctlChannelReport.GetRecordRowCount();
				int nRowIndex = nRowCount;
				if(nRowCount == 0)
				{
					//
					//没有通道
					//
					m_ctlChannelReport.InsertRecordRow(0, new CDsmEditGooseSendChannelRecord(pChannel, 1, 0) );
					m_ctlChannelReport.SetRecordRowData(0, (DWORD_PTR)pChannel);
					m_ctlChannelReport.SetRowItemData(0, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, 1);
					m_ctlChannelReport.SetRowItemData(0, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, 0);
				}
				else
				{
					//
					//添加通道
					//
					int nIndex = m_ctlChannelReport.GetRowItemData(nRowIndex -1, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
					++nIndex;
					m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pChannel, nIndex, 0));
					m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pChannel);

					m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
					m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, 0);

					if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
					{
						//
						//如果是结构类型，添加子通道
						//
						++nRowIndex;
						int nSubIndex = 1;
						CChannel * pSubChannel = pChannel->FirstSubChannel();
						while(pSubChannel)
						{
							m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pSubChannel, nIndex, nSubIndex));
							m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);
							m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
							m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, nSubIndex);
							++nRowIndex;
							++nSubIndex;
							pSubChannel = pChannel->NextSubChannel(pSubChannel);
						}
					}
				}
				m_ctlChannelReport.SetFocusedSelectionRowIndex(m_ctlChannelReport.GetRecordRowCount()-1);
				m_ctlChannelReport.EnsureVisible(m_ctlChannelReport.GetRecordRowCount()-1, FALSE);
			}

#endif
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_DO_SQT:
        {
            m_pGooseCb->DO_s_q_t();
            _updateChanReport();
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_DA_SQT:
        {
            m_pGooseCb->DA_s_q_t();
            _updateChanReport();
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_DA_SQ:
        {
            m_pGooseCb->DA_s_q();
            _updateChanReport();
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_DA_S:
        {
            m_pGooseCb->DA_SinglePoint();
            _updateChanReport();
        }
        break;
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_DA_DP:
        {
            m_pGooseCb->DA_DoublePoint();
            _updateChanReport();
        }
        break;
#if SU_FEI
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F5:
        {
            //
        }
        break;
#else
	case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F4:
		{
			//删除

			if (m_pGooseCb->GetChannelCount() <= DSM_GOOSE_CHAN_MIN)
			{
				CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_CHAN_MIN), 3000);
				return;
			}
			else
			{
				int nRowSelIndex = m_ctlChannelReport.GetFocusedRowIndex();
				if (nRowSelIndex < 0)
				{
					return;
				}
				CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(nRowSelIndex);
				ASSERT(pChannel);

				//
				int nIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
				int nSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);

				if (nSubIndex > 0)
				{
					//选中子通道，不允许删除
					return;
				}

				m_ctlChannelReport.SetRedraw(FALSE);
				enumChannelType nChanType = pChannel->GetChannelType();

				m_pGooseCb->DeleteChannel(pChannel);
				m_ctlChannelReport.DeleteRecordRow(nRowSelIndex);
				if (nChanType == CHANNEL_TYPE_GOOSE_STRUCT)
				{
					//判断下一行是不是子通道，如果是则删除；
					int nNextIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
					int nNextSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);
					while(nNextIndex == nIndex && nNextSubIndex > 0)
					{
						m_ctlChannelReport.DeleteRecordRow(nRowSelIndex);
						if (nRowSelIndex < m_ctlChannelReport.GetRecordRowCount())
						{
							nNextIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
							nNextSubIndex = m_ctlChannelReport.GetRowItemData(nRowSelIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC);
						}
						else
						{
							break;
						}
					}
				}
				m_ctlChannelReport.SetRedraw(TRUE);
			}
		}
		break;
#endif
    case ID_MENU_PARAM_EDIT_GOOSES_CHAN_F6:
        {
            //清空映射
            for(int i = 0; i < m_ctlChannelReport.GetRecordRowCount(); ++i)
            {
                CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(i);
                if (pChannel != NULL)
                {
                    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(i)->GetItem(3));
                    pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
                    pChannel->SetVirtualChannel((LPCTSTR)dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
                    m_ctlChannelReport.RedrawRowItem(i, 3);
                }
            }
        }
        break;
    default:
        break;
    }
}

void CBasicParamEditGooseCtrlDlg::OnGooseCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult)
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

    switch (pItemNotify->rowIndex )
    {
    case MSGPARAM_EDIT_GOOSE_MAC_ADDR:
        {

            CRect rcItem = m_ctlCtrlBlockReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

            CMacEditCtrl macEdit;
            macEdit.SetBkColor(ELT_T1_EDITBKCOLOR);
            macEdit.SetTextColor(ELT_T1_EDITTEXTCOLOR);

            unsigned char ucMac[6];
            m_pGooseCb->GetDesMAC(ucMac, 6);

            CWnd* pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&macEdit, IDR_MENU_MAC_INPLACEEDIT);
            if (IDOK == macEdit.GoModal(&m_ctlCtrlBlockReport, rcItem, ucMac))
            {
                CString strMac = _T("");
                macEdit.MacAddr2Str(ucMac, strMac);
                pItem->SetValue(strMac);
                m_pGooseCb->SetDesMAC(ucMac, 6);
                m_ctlCtrlBlockReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
            }
            CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}

void CBasicParamEditGooseCtrlDlg::OnGooseCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if(!pItemNotify)
    {
        return;
    }
    ASSERT(m_pGooseCb && pItemNotify->colIndex == 1);

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex));
    if (pItem == NULL)
    {
        return;
    }
    CString strValue(_T(""));
    int  nValue(0);

    switch(pItemNotify->rowIndex)
    {
    case MSGPARAM_EDIT_GOOSE_CTRL_BLOCK_DESC:
        {
            m_pGooseCb->SetDescription((LPCTSTR)pItem->GetValue());
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_APPID:
        {
            strValue = pItem->GetValue();
            nValue = _tcstol(strValue, NULL, 16);
            m_pGooseCb->SetAPPID(nValue);
            strValue.Format(_T("%04X"), m_pGooseCb->GetAPPID());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_VLAN_ID:
        {
            strValue = pItem->GetValue();
            nValue = _tcstol(strValue, NULL, 10);
            m_pGooseCb->SetVlanID(nValue);
            strValue.Format(_T("%d"), m_pGooseCb->GetVlanID());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY:
        {
            strValue = pItem->GetValue();
            nValue = _tcstol(strValue, NULL, 10);
            m_pGooseCb->SetVLanPriority(nValue);
            strValue.Format(_T("%d"), m_pGooseCb->GetVLanPriority());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_CFG_VERSION:
        {
            strValue = pItem->GetValue();
            nValue = _tcstol(strValue, NULL, 10);
            m_pGooseCb->SetConfigRevision(nValue);
            strValue.Format(_T("%d"), m_pGooseCb->GetConfigRevision());
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;

    case MSGPARAM_EDIT_GOOSE_TEST_FLAG:
        {
            if (pItem->GetHasCheckbox())
            {
                m_pGooseCb->SetTestFlag(pItem->IsChecked() == TRUE ? true : false);
				
            }
            *pResult = TRUE;
        }
        break;

#if !CHUN_HUA
	case MSGPARAM_EDIT_GOOSE_VLAN_TAG:
		{
			if (pItem->GetHasCheckbox())
			{
				m_pGooseCb->SetVlanFlag(pItem->IsChecked() == TRUE ? true : false);
				if (pItem->IsChecked())
				{
					m_pGooseCb->SetVlanFlag(true);
					m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_ID)->SetFocusable(TRUE);
					m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY)->SetFocusable(TRUE);
 
				}
				else
				{
					m_pGooseCb->SetVlanFlag(false);
					m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_ID)->SetFocusable(FALSE);
					m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_EDIT_GOOSE_VLAN_PRIORITY)->SetFocusable(FALSE);
				}
			}
			*pResult = TRUE;
		}
		break;
#endif

    case MSGPARAM_EDIT_GOOSE_NEED_COMM:
        {
            if (pItem->GetHasCheckbox())
            {
                m_pGooseCb->SetNeedComm(pItem->IsChecked() == TRUE ? true : false);
            }
            *pResult = TRUE;
        }
        break;


    case MSGPARAM_EDIT_GOOSE_GOCB_REF:
        {
            strValue = pItem->GetValue();
            m_pGooseCb->SetGocbRef((LPCTSTR)strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_DATA_SET:
        {
            strValue = pItem->GetValue();
            m_pGooseCb->SetDataSet((LPCTSTR)strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_GO_ID:
        {
            strValue = pItem->GetValue();
            m_pGooseCb->SetGoID((LPCTSTR)strValue);
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_CHAN_COUNT:
        {
            strValue = pItem->GetValue();
            nValue = _tcstol(strValue, NULL, 10);
            if (nValue > 0)
            {
                m_pGooseCb->SetChannelCount(nValue);
            }
            strValue.Format(_T("%d"), m_pGooseCb->GetChannelCount());
            pItem->SetValue(strValue);
            _updateChanReport();
            *pResult = TRUE;
        }
        break;
    }
}


///
/// @brief
///      
/// @param[in]
///      NMHDR * pNotifyStruct    
/// @param[in]
///      LRESULT * pResult    
/// @return
///      void    
///
void CBasicParamEditGooseCtrlDlg::OnGooseCtrlItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    
    if(!pItemNotify || !m_pGooseCb)
    {
        return;
    }

    switch(pItemNotify->rowIndex)
    {
    case MSGPARAM_EDIT_GOOSE_SEND_PORT:
        {
            DsmPortName portName = (DsmPortName)pItemNotify->pConstraint->GetConstraintData();
            m_pGooseCb->SetPortName(portName);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}


void CBasicParamEditGooseCtrlDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        if (m_nShowReportType == EGShowCtrlBlockReport)
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

///
/// @brief
///      
/// @param[in]
///      NMHDR * pNotifyStruct    
/// @param[in]
///      LRESULT * pResult    
/// @return
///      void    
///
void CBasicParamEditGooseCtrlDlg::OnGooseChanItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify || !m_pGooseCb)
    {
        return;
    }

    CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(pItemNotify->rowIndex);

    if (pChannel == NULL)
    {
        return;
    }
    
    int nRowIndex = pItemNotify->rowIndex;
    int nColIndex = pItemNotify->colIndex;

    switch(nColIndex)
    {
    case MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC:
        {
            CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(1));
            pChannel->SetDescription((LPCTSTR)pItem->GetValue());
            *pResult = TRUE;
        }
        break;
    case MSGPARAM_EDIT_GOOSE_COL_CHAN_TYPE:
        {
            // 通道类型
            enumChannelType nOldChanType = pChannel->GetChannelType();

            enumChannelType nChanType = (enumChannelType)pItemNotify->pConstraint->GetConstraintData();
            pChannel->SetChannelType(nChanType);
            
            //
            //调整映射
            //
            if (nChanType == CHANNEL_TYPE_GOOSE_POINT ||
                nChanType == CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
            {
                m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP)->SetEditable(TRUE);
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP));
                if (pChannel->GetVirtualChannel() != _T(""))
                {
                    pItem->SetValue(pChannel->GetVirtualChannel().c_str());
                }
                else
                {
                    pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
                }
            }
            else
            {
                m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP)->SetEditable(FALSE);
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP));
                pItem->SetValue(_T(""));
            }

            //
            //设置通道类型
            //
            if (nOldChanType != CHANNEL_TYPE_GOOSE_STRUCT && nChanType == CHANNEL_TYPE_GOOSE_STRUCT)
            {
                //从非结构改成结构 调整子通道显示
                int nIndex = m_ctlChannelReport.GetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                int nSubIndex = 1;
                
                CChannel* pSubChannel = pChannel->FirstSubChannel();
                while (pSubChannel)
                {
                    ++nRowIndex;
                    m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseSendChannelRecord(pSubChannel, nIndex, nSubIndex));
                    m_ctlChannelReport.SetRecordRowData(nRowIndex, (DWORD_PTR)pSubChannel);
                    m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX, nIndex);
                    m_ctlChannelReport.SetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC, nSubIndex);
                    ++nSubIndex;
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                }
            }
            else if(nOldChanType == CHANNEL_TYPE_GOOSE_STRUCT && nChanType != CHANNEL_TYPE_GOOSE_STRUCT)
            {
                //从结构改成非结构 删除子通道显示
                if(nRowIndex < m_ctlChannelReport.GetRecordRowCount() -1)
                {
                    //下面还有通道显示，
                    DWORD_PTR nNextIndex = m_ctlChannelReport.GetRowItemData(nRowIndex + 1, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);

                    while(nNextIndex == m_ctlChannelReport.GetRowItemData(nRowIndex, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX))
                    {
                        //是子通道，删除之
                        m_ctlChannelReport.DeleteRecordRow(nRowIndex + 1);
                        
                        if(nRowIndex < m_ctlChannelReport.GetRecordRowCount() -1)
                        {
                            nNextIndex = m_ctlChannelReport.GetRowItemData(nRowIndex + 1, MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX);
                        }
                        else
                        {
                            //下面没有通道
                            break;
                        }
                    }
                }
            }
            *pResult = TRUE;
        }
        break;

    case MSGPARAM_EDIT_GOOSE_COL_CHAN_MAP:
        {
            //映射
            if ((LPCTSTR)pItemNotify->pConstraint->GetConstraintCaption()  != dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL))
            {
                pChannel->SetVirtualChannel((LPCTSTR)pItemNotify->pConstraint->GetConstraintCaption());
            }
            else
            {
                pChannel->SetVirtualChannel(_T(""));
            }

            Invalidate(TRUE);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}

void CBasicParamEditGooseCtrlDlg::OnGooseChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    
    if(!pItemNotify || !m_pGooseCb)
    {
        return;
    }

    CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(pItemNotify->rowIndex);

    if (pChannel == NULL)
    {
        return;
    }

    int nColIndex = pItemNotify->colIndex;
    int nRowIndex = pItemNotify->rowIndex;

    switch(nColIndex)
    {
    case MSGPARAM_EDIT_GOOSE_COL_CHAN_DESC:
        {
            //描述
            CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChannelReport.GetRecordRow(nRowIndex)->GetItem(MSGPARAM_EDIT_GOOSE_COL_CHAN_INDEX));
            pChannel->SetDescription((LPCTSTR)pItem->GetValue());
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}


BOOL CBasicParamEditGooseCtrlDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    KillTimer(1);
    return CBaseDlg::DestroyWindow();
}


void CBasicParamEditGooseCtrlDlg::_SavePage()
{ 
    CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (NULL == pParamConfigGooseSend)
    {
        return ;
    }
    pParamConfigGooseSend->SaveConfig();

}

void CBasicParamEditGooseCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
        if (NULL == pParamConfigGooseSend)
        {
            return ;
        }
        pParamConfigGooseSend->SaveConfig();
    }
    CBaseDlg::OnTimer(nIDEvent);
}


BOOL CBasicParamEditGooseCtrlDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL nRet = CBaseDlg::PreTranslateMessage(pMsg);


	CChannel* pChannel = NULL;

	if (WM_KEYDOWN == pMsg->message)
	{
		switch(pMsg->wParam)
		{
		case VK_UP:
			{			
				if (m_nShowReportType == EGShowChannelReport)
				{
					if (m_ctlChannelReport.GetFocusedColIndex() == 3)
					{
						int index = m_ctlChannelReport.GetSelectionRowIndex();


						pChannel = m_pGooseCb->GetChannelByIndex(index);

						if (pChannel != NULL)
						{

							if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
							{
								//判断选中项是否可以编辑


								if (index < 0)
								{
									index = m_ctlChannelReport.GetRecordRowCount();
								}
								while(!m_ctlChannelReport.GetRecordRow(index)->GetItem(3)->IsEditable())
								{
									index--;
									if (index <= 0)
									{
										index = m_ctlChannelReport.GetRecordRowCount();
									}
								}
								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);

								break;


							}
							/*else
							{
								pChannel = m_pGooseCb->PrevChannel(pChannel);
								index--;

								while(pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_POINT &&
									pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
								{
									if (pChannel == m_pGooseCb->FirstChannel())
									{
										pChannel = m_pGooseCb->LastChannel();
										index = m_ctlChannelReport.GetRecordRowCount();
									}
									pChannel = m_pGooseCb->PrevChannel(pChannel);
									index--;

								}

								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);
							}					*/

						}
						else
						{
							break;
						}

					}
				}
			}
			break;
		case VK_DOWN:
			{			
				if (m_nShowReportType == EGShowChannelReport)
				{
					if (m_ctlChannelReport.GetFocusedColIndex() == 3)
					{
						int index = m_ctlChannelReport.GetSelectionRowIndex();

						pChannel = m_pGooseCb->GetChannelByIndex(index);

						if (pChannel != NULL)
						{

							if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
							{
								//判断选中项是否可以编辑


								if (index > m_ctlChannelReport.GetRecordRowCount())
								{
									index = 0;
								}
								while(!m_ctlChannelReport.GetRecordRow(index)->GetItem(3)->IsEditable())
								{
									index++;
									if (index >= m_ctlChannelReport.GetRecordRowCount())
									{
										index = 0;
									}
								}
								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);

								break;


							}
							/*else
							{
								pChannel = m_pGooseCb->NextChannel(pChannel);
								index++;

								while(pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_POINT &&
									pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
								{
									if (pChannel == m_pGooseCb->LastChannel())
									{
										pChannel = m_pGooseCb->FirstChannel();
										index = m_ctlChannelReport.GetRecordRowCount();
									}
									pChannel = m_pGooseCb->NextChannel(pChannel);
									index++;

								}

								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);
							}				*/	

						}
						else
						{
							break;
						}

					}
				}
			}
			break;
		default:
			break;
		}
	}

	return nRet;
}