// ManualSmvGooseSend.cpp : 实现文件
//
#include "stdafx.h"
#include "winbase.h"
#include "ManualSmvGooseSend.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/config/ParamconfigManualTest.h"
#include "src/service/datalayer/config/ParamConfigGooseSend.h"
#include "src/utils/CommonControl/TimeEditCtrl_MS/TimeEditCtrl_MS.h"
#include "src/utils/CommonControl/BitStrEditCtrl/BitStrEditCtrl.h"
#include "src/main/popdlg/MsgDlg.h"
#include "src/main/utils/utility.h"

//////////////////////////////////////////////////////////////////////////
// 手动试验 - goose报表
//
#define DSM_MANUAL_GOOSE_WIDTH_CHAN_INDEX  50
#define DSM_MANUAL_GOOSE_WIDTH_CHAN_DESC   230
#define DSM_MANUAL_GOOSE_WIDTH_CHAN_TYPE   80
#define DSM_MANUAL_GOOSE_WIDTH_CHAN_VALUE  169
//#define ZY_PRJ

//////////////////////////////////////////////////////////////////////////
// 手动试验 - smv报表
//
#define DSM_MANUAL_SMV_WIDTH_CHAN          99
#define DSM_MANUAL_SMV_WIDTH_AMPL          110
#define DSM_MANUAL_SMV_WIDTH_PHASE         110
#define DSM_MANUAL_SMV_WIDTH_FREQUENCE     110
#define DSM_MANUAL_SMV_WIDTH_STEP          100

//
//提醒消息对话框位置
//
#define DSM_MANUAL_TIP_DLG_RC          CRect(0, 333, 640, 441)     ///< 页面区域
#define DSM_MANUAL_TIP_DLG_RCTEXT      CRect(100, 15, 620, 95)     ///< 文字区域

// CManualSmvGooseSend 对话框


enum MSSSmvChanValCalc
{
    SCVCAdd = 0,
    SCVCSub,
};

enum MSSSendStatus
{
    SSInit = 0,
    SSSmv,
    SSGoose,
    SSSmvGoose,
};

enum MSSModifyGooseValType
{
    MGVTAllOn = 0,
    MGVTAllOff,
    MGVTAllRevert,
};


enum MSSSmvFocusColType
{
    SFCAmpl = 0,
    SFCPhase,
    SFCOther,
};

#ifdef ZY_PRJ
#define MMS_COL_INDEX_CHAN           0
//#define MMS_COL_INDEX_SEL            1
#define MMS_COL_INDEX_AMPL           1
#define MMS_COL_INDEX_PHASE          2
#define MMS_COL_INDEX_FREQUENCE      3
#define MMS_COL_INDEX_STEP           4
#else
#define MMS_COL_INDEX_CHAN           0
#define MMS_COL_INDEX_AMPL           1
#define MMS_COL_INDEX_AMPL_STEP      2
#define MMS_COL_INDEX_PHASE          3 
#define MMS_COL_INDEX_PHASE_STEP     4
#define MMS_COL_INDEX_FREQUENCE      5
#endif

#define MMS_FOCUS_ROW_INDEX_TOP      3
#define MMS_FOCUS_ROW_INDEX_BOTTOM   7

IMPLEMENT_DYNAMIC(CManualSmvGooseSendDlg, CBaseDlg)

CManualSmvGooseSendDlg::CManualSmvGooseSendDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualSmvGooseSendDlg::IDD, DSM_STR_MANUAL_SMV_CAPTION, pParent)
    , m_eShowReportType(SRTShowSmvReport)
    , m_pGooseCb(NULL)
    , m_pConfigGooseSend(NULL)
    , m_pConfigManualTest(NULL)
    , m_pDIStateDlg(NULL)
    , m_dwSendStatus(SSInit)
    , m_pDataLayer(NULL)
    , m_bGooseModify(FALSE)
    , m_bSmvModify(FALSE)	
	, m_bIsEditing(FALSE)
{
    m_nMenuID = IDR_MENU_MANUAL_SMV_SEND;
	m_pPopupMenu = new CNaviMenu;
}

CManualSmvGooseSendDlg::~CManualSmvGooseSendDlg()
{
    if (m_pDIStateDlg)
    {
        m_pDIStateDlg->DestroyWindow();
        delete m_pDIStateDlg;
    }
    m_gooseCbMap.clear();
	if (m_pPopupMenu)
	{
		delete m_pPopupMenu;
	}
}

void CManualSmvGooseSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSmvGooseSendDlg, CBaseDlg)
    ON_COMMAND_RANGE(ID_MENU_MANUAL_SMV_GOOSE_CMD_MIN,   ID_MENU_MANUAL_SMV_GOOSE_CMD_MAX, &CManualSmvGooseSendDlg::OnNaviMenuParamSmvGooseSend)
    ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_MANUAL_SMV_GOOSE_CMD_MIN,   ID_MENU_MANUAL_SMV_GOOSE_CMD_MAX, &CManualSmvGooseSendDlg::OnUpdateNaviMenuParamSmvGooseSend)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_MANUAL_SMV_RET, &CManualSmvGooseSendDlg::OnSmvSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_MANUAL_SMV_RET, &CManualSmvGooseSendDlg::OnSmvSendItemValueChanged)
    ON_NOTIFY(ELNM_OKCLICK, IDC_MANUAL_SMV_RET, &CManualSmvGooseSendDlg::OnStepOkClick)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_MANUAL_GOOSE_RET, &CManualSmvGooseSendDlg::OnGooseSendItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_MANUAL_GOOSE_RET, &CManualSmvGooseSendDlg::OnGooseSendItemSelectChanged)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_MANUAL_GOOSE_RET, &CManualSmvGooseSendDlg::OnGooseSendInplaceButtonClick)

    ON_NOTIFY(ELNM_OKCLICK, IDC_MANUAL_GOOSE_RET, &CManualSmvGooseSendDlg::OnGoosePointItemOkClick)
    ON_MESSAGE(WM_DI_CHANGED, &CManualSmvGooseSendDlg::OnDIChanged)
    ON_MESSAGE(WM_MANUAL_TEST_RESULT, &CManualSmvGooseSendDlg::OnManualTestResult)
    ON_COMMAND_RANGE(ID_MENU_MANUAL_GOOSE_SEND_CTRL_MIN,   ID_MENU_MANUAL_GOOSE_SEND_CTRL_MAX, &CManualSmvGooseSendDlg::OnNaviMenuGooseCtrlSel)

    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_MANUAL_SMV_RET,  &CManualSmvGooseSendDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_MANUAL_SMV_RET,  &CManualSmvGooseSendDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_MANUAL_GOOSE_RET,  &CManualSmvGooseSendDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_MANUAL_GOOSE_RET,  &CManualSmvGooseSendDlg::OnInplaceEditAction)

    ON_WM_DESTROY()
    ON_WM_TIMER()

	ON_WM_PAINT()
END_MESSAGE_MAP()


// CManualSmvGooseSend 消息处理程序

BOOL CManualSmvGooseSendDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    m_pDataLayer = CDsmDataLayer::getInstance();
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }
    
    //手动试验参数配置管理器
    m_pConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL == m_pConfigManualTest)
    {
        return FALSE;
    }

    m_pConfigGooseSend = CParamConfigGOOSESend::getInstance();
    if (m_pConfigGooseSend == NULL)
    {
        return FALSE;
    }

    m_pGooseCb = NULL;
    BOOL bFirstSelCb = TRUE;

    ASSERT(m_pConfigGooseSend->GetSelectedGocbCount() <= (ID_MENU_MANUAL_GOOSE_SEND_CTRL_MAX - ID_MENU_MANUAL_GOOSE_SEND_CTRL_MIN + 1));

    int i = 0; 
    CGooseCb *pGooseCb =  m_pConfigGooseSend->FirstGocb();
    while(pGooseCb)
    {
        if (pGooseCb->IsSelected())
        {
            m_gooseCbMap.insert(std::pair<UINT, CGooseCb*>(ID_MENU_MANUAL_GOOSE_SEND_CTRL_MIN + i, pGooseCb));
            if (bFirstSelCb)
            {
                m_pGooseCb = pGooseCb;
                bFirstSelCb = FALSE;
            }
            ++i;
        }
        pGooseCb = m_pConfigGooseSend->NextGocb(pGooseCb);
    }
    
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    if(!pData || CManualSmvGooseSendDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }
    if(pData->m_eAction == PageBack)
    {
        _restorePage();
    }
    else
    {
        _showSmvReport();
    }

    //
    //变更F3控制块焦点
    //
    _highlightGooseF3();

    //
    //是否显示DI
    //
    if (m_pConfigManualTest->GetIsShowDI())
    {
        ShowDI(TRUE);
    }

	SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

 

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

///
/// @brief
///      创建Smv报表
/// 
/// @return
///      void    
///
void CManualSmvGooseSendDlg::_createSmvReport()
{
    ASSERT(m_pConfigManualTest);

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_SMV_RET);

    // 设置字体
    m_ctlSmvReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlSmvReport.PostInitTemplateStyle();

    // 插入列元素
#ifdef ZY_PRJ
    m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_CHAN, dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN), EL_CENTER, DSM_MANUAL_SMV_WIDTH_CHAN);
    //m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_SEL, dsm_stringMgr::Get(DSM_STR_MANUAL_SEL), EL_CENTER, DSM_MANUAL_SMV_WIDTH_SEL);
    m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_AMPL, dsm_stringMgr::Get(DSM_STR_MANUAL_AMPL), EL_CENTER, DSM_MANUAL_SMV_WIDTH_AMPL);
    m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_PHASE, dsm_stringMgr::Get(DSM_STR_MANUAL_PHASE), EL_CENTER, DSM_MANUAL_SMV_WIDTH_PHASE);
    m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_FREQUENCE, dsm_stringMgr::Get(DSM_STR_MANUAL_FREQUENCE), EL_CENTER, DSM_MANUAL_SMV_WIDTH_FREQUENCE);
    m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_STEP, dsm_stringMgr::Get(DSM_STR_MANUAL_STEP), EL_CENTER, DSM_MANUAL_SMV_WIDTH_STEP);
#else
	m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_CHAN, dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN), EL_CENTER, DSM_MANUAL_SMV_WIDTH_CHAN);
	//m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_SEL, dsm_stringMgr::Get(DSM_STR_MANUAL_SEL), EL_CENTER, DSM_MANUAL_SMV_WIDTH_SEL);
	m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_AMPL, dsm_stringMgr::Get(DSM_STR_MANUAL_AMPL), EL_LEFT, DSM_MANUAL_SMV_WIDTH_AMPL);	
	m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_AMPL_STEP, dsm_stringMgr::Get(DSM_STR_MANUAL_STEP), EL_LEFT, DSM_MANUAL_SMV_WIDTH_AMPL);
	m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_PHASE, dsm_stringMgr::Get(DSM_STR_MANUAL_PHASE), EL_LEFT, DSM_MANUAL_SMV_WIDTH_PHASE);
	m_ctlSmvReport.InsertColumn(MMS_COL_INDEX_PHASE_STEP, dsm_stringMgr::Get(DSM_STR_MANUAL_STEP), EL_LEFT, DSM_MANUAL_SMV_WIDTH_STEP);
#endif

    // 设置选择焦点样式
    m_ctlSmvReport.SetRowSelectionVisible(FALSE);
    m_ctlSmvReport.SetRowFocusEnable(TRUE);
    m_ctlSmvReport.SetSubItemFocusEnable(TRUE);
    m_ctlSmvReport.SetFreezeSelectionVisible(TRUE);
    m_ctlSmvReport.SetFocusedColIndex(MMS_COL_INDEX_CHAN);
    m_ctlSmvReport.SetFocus();

#if !SU_FEI
	m_ctlSmvReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
    _updateSmvReport();

#ifdef ZY_PRJ
    //m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_SEL)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_AMPL)->GetEditOptions()->SetAllowEdit(TRUE);   
    m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_FREQUENCE)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_PHASE)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_STEP)->GetEditOptions()->SetAllowEdit(TRUE);
#else
	//m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_SEL)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_AMPL)->GetEditOptions()->SetAllowEdit(TRUE);   
	m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_AMPL_STEP)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_PHASE)->GetEditOptions()->SetAllowEdit(TRUE);
	m_ctlSmvReport.GetColumnExAttr(MMS_COL_INDEX_PHASE_STEP)->GetEditOptions()->SetAllowEdit(TRUE);
#endif
}

void CManualSmvGooseSendDlg::_updateSmvReport()
{
    if (!m_ctlSmvReport.GetSafeHwnd())
    {
        return;
    }
    
    int nPreSelRow = m_ctlSmvReport.GetFocusedRowIndex();

    m_ctlSmvReport.DeleteAllRecordRow();

    //手动试验参数配置管理器
    int nIndex = 0;
    // 遍历虚拟通道
    CSMVSendVirtualChannel* pVirChannel = m_pConfigManualTest->FirstSMVSendVirtualChannel();
    while(pVirChannel)
    {
        m_ctlSmvReport.InsertRecordRow(nIndex, new CDsmManualSmvSendRecord(pVirChannel));
        m_ctlSmvReport.SetRecordRowData(nIndex, (DWORD_PTR)pVirChannel);
        pVirChannel = m_pConfigManualTest->NextSMVSendVirtualChannel(pVirChannel);
        ++nIndex;
    }

    //
    //处理选中行
    //
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvGooseSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if ( nPreSelRow >=0 && nPreSelRow < m_ctlSmvReport.GetRecordRowCount())
    {
        m_ctlSmvReport.SetFocusedSelectionRowIndex(nPreSelRow);
        m_ctlSmvReport.EnsureVisible(nPreSelRow, FALSE);
    }
    else if (m_ctlSmvReport.GetRecordRowCount() > 0)
    {
        if ((pData->m_eAction == PageEnter) ||
            (pData->m_eAction == PageBack && pData->m_nCtrlID != m_ctlSmvReport.GetDlgCtrlID()))
        {
            m_ctlSmvReport.SetFocusedSelectionRowIndex(0);
        }
    }

    m_ctlSmvReport.RedrawRowItems(0, m_ctlSmvReport.GetRecordRowCount()-1);
}
///
/// @brief
///      创建Goose设置报表
/// 
/// @return
///      void    
///
void CManualSmvGooseSendDlg::_createGooseReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlGooseReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_GOOSE_RET);

    // 设置字体
    m_ctlGooseReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlGooseReport.PostInitTemplateStyle();

    // 插入列元素

    m_ctlGooseReport.InsertColumn(0, _T(""), EL_CENTER, DSM_MANUAL_GOOSE_WIDTH_CHAN_INDEX);
    m_ctlGooseReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_DESC), EL_LEFT, DSM_MANUAL_GOOSE_WIDTH_CHAN_DESC);
    m_ctlGooseReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_TYPE), EL_LEFT, DSM_MANUAL_GOOSE_WIDTH_CHAN_TYPE);
    m_ctlGooseReport.InsertColumn(3, L"          " + dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_VALUE), EL_CENTER, DSM_MANUAL_GOOSE_WIDTH_CHAN_VALUE);
    

    // 设置选择焦点样式
    m_ctlGooseReport.SetRowSelectionVisible(FALSE);
    m_ctlGooseReport.SetRowFocusEnable(TRUE);
    m_ctlGooseReport.SetSubItemFocusEnable(TRUE);
    m_ctlGooseReport.SetFreezeColumnsCount(3);
    m_ctlGooseReport.SetFreezeSelectionVisible(TRUE);
    m_ctlGooseReport.SetFocusedColIndex(3);
    m_ctlGooseReport.SetFocus();

    m_ctlGooseReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
    _updateGooseReport();
}
///
/// @brief
///      显示Smv报表
///
/// @return
///      void    
///
void CManualSmvGooseSendDlg::_showSmvReport()
{
    //显示smv报表
    if(m_ctlSmvReport.GetSafeHwnd())
    {
        m_ctlSmvReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createSmvReport();
    }

    if(m_ctlGooseReport.GetSafeHwnd())
    {
        m_ctlGooseReport.ShowWindow(SW_HIDE);
    }

    m_ctlSmvReport.SetFocus();
    m_eShowReportType = SRTShowSmvReport;

    _reloadNaviMenu(IDR_MENU_MANUAL_SMV_SEND);

    CString csCaption;
    csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CAPTION));
    CStatusMgr::SetTitle(csCaption);
}

///
/// @brief
///      显示goose报表
///
/// @return
///      void    
///
void CManualSmvGooseSendDlg::_showGooseReport()
{
    if(m_ctlGooseReport.GetSafeHwnd())
    {
        m_ctlGooseReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createGooseReport();
    }

    if(m_ctlSmvReport.GetSafeHwnd())
    {
        m_ctlSmvReport.ShowWindow(SW_HIDE);
    }
    m_ctlGooseReport.SetFocus();
    m_eShowReportType = SRTShowGooseReport;

    _reloadNaviMenu(IDR_MENU_MANUAL_GOOSE_SEND);

    CString csCaption;
    if (m_pGooseCb)
    {
		csCaption.Format(L"%s->%s: 0x%04X", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION), m_pGooseCb->GetAPPID());
    }
    else
    {
        csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION));
    }
    CStatusMgr::SetTitle(csCaption);
}


void CManualSmvGooseSendDlg::_highlightGooseF3(BOOL bHighLight/* = TRUE*/)
{
    if (m_nMenuID == IDR_MENU_MANUAL_GOOSE_SEND)
    {
        CUINTGooseCbMap::iterator iter;
        for (iter = m_gooseCbMap.begin(); iter != m_gooseCbMap.end(); ++iter)
        {
            if (m_pGooseCb == iter->second)
            {
                CNaviBarMgr::GetInstance()->HighlightMenuItem(2, iter->first - ID_MENU_MANUAL_GOOSE_SEND_CTRL_MIN, bHighLight);
                break;
            }
        }
    }
    ;
}

void CManualSmvGooseSendDlg::_updateGooseReport()
{
    if (m_ctlGooseReport.GetSafeHwnd() == NULL)
    {
        return;
    }

    m_ctlGooseReport.DeleteAllRecordRow();

    if (m_pGooseCb)
    {
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
            m_ctlGooseReport.InsertRecordRow(nRowIndex, new CDsmManualGooseSendRecord(pChannel, nIndex));
            m_ctlGooseReport.SetRecordRowData(nRowIndex, (DWORD_PTR)pChannel);
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
                    m_ctlGooseReport.InsertRecordRow(nRowIndex, new CDsmManualGooseSendRecord(pSubChannel, nIndex, nSubIndex));
                    m_ctlGooseReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);
                    ++nRowIndex;
                    ++nSubIndex;
                    pSubChannel = pChannel->NextSubChannel(pSubChannel);
                }
            }
            ++nIndex;
            pChannel = m_pGooseCb->NextChannel(pChannel);
        }
    }

    if (m_ctlGooseReport.GetRecordRowCount() > 0)
    {
        m_ctlGooseReport.SetFocusedSelectionRowIndex(0);
    }
}

void CManualSmvGooseSendDlg::_resetMenu()
{
	m_menuMain.SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
    //
    //修改F1
    //
    if (m_eShowReportType == SRTShowSmvReport)
    {
        //m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_MANUAL_SMVGSE_SMV), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_STATE_SMV_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, NULL, NULL, NULL);
    }
    else if (m_eShowReportType == SRTShowGooseReport)
    {
        //m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_MANUAL_SMVGSE_GSE), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_STATE_GOOSE_SEL), NULL, NULL);
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
    }
    //
    //根据实验开始方式修改为 全部发送
    //
    if (m_pConfigManualTest == NULL)
    {
        return;
    }
    //
    //修改F2
    //
    eStartMode startMode = m_pConfigManualTest->GetStartMode();

    if (startMode == START_MODE_APART)
    {
        if (m_eShowReportType == SRTShowSmvReport)
        {
            //发送smv界面
            if ( m_dwSendStatus & SSSmv)
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_STOP_SMV));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_SEND_SMV));
            }
        }
        else if (m_eShowReportType == SRTShowGooseReport)
        {
            //发送goose界面
            if ( m_dwSendStatus & SSGoose)
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_STOP_GOOSE));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_SEND_GOOSE));
            }
        }
    }
    else if(startMode == START_MODE_TOGETHER)
    {

        //发送中
        if ( m_dwSendStatus & (SSSmv|SSSmvGoose))
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_STOP_ALL));
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_SEND_ALL));
        }
    }

    //
    //F3
    //
    if (m_eShowReportType == SRTShowGooseReport)
    {
        ASSERT(m_nMenuID == IDR_MENU_MANUAL_GOOSE_SEND);

        CNaviMenu* pSubMenu = m_pNaviBar->GetNaviMenu()->GetSubMenu(2);
        if (!pSubMenu)
        {
            //
            //已经加载过控制块，不再重新加载
            //
			if (m_gooseCbMap.size() != 0)
			{	
				CMenu  menu;
				VERIFY(menu.CreatePopupMenu());
				CUINTGooseCbMap::iterator iter;

				for (iter = m_gooseCbMap.begin(); iter != m_gooseCbMap.end(); ++iter)
				{
					CString strDesc = _T("");
					switch(iter->second->GetPortName())
					{
					case NET_PORT_LIGHT_A:
						{
							strDesc.Format(_T("0x%04X[%s]"), iter->second->GetAPPID(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX1));
						}
						break;
					case NET_PORT_LIGHT_B:
						{
							strDesc.Format(_T("0x%04X[%s]"), iter->second->GetAPPID(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_TX2));
						}
						break;
					case NET_PORT_ELECTRONIC:
						{
							strDesc.Format(_T("0x%04X[%s]"), iter->second->GetAPPID(), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ELEC_PORT));
						}
						break;
					default:
						strDesc.Format(_T("0x%04X"), iter->second->GetAPPID());
						break;
					}
					menu.AppendMenu(MF_STRING, iter->first, strDesc);
				}
				pSubMenu = new CNaviMenu;
				pSubMenu->LoadMenu(&menu);
				menu.DestroyMenu();
				
				m_pNaviBar->GetNaviMenu()->SetMenuSubItem(2, pSubMenu);
			}
        }
    }
    //
    //F5 设置生效菜单，（修改生效或置空）
    //
    eModifyEffectiveMode effMode = m_pConfigManualTest->GetModifyEffectiveMode();
    if (effMode == EFFECTIVE_MODE_MANUALLY)
    {
        if(m_eShowReportType == SRTShowSmvReport)
        {
            if (m_dwSendStatus&SSSmv)
            {
                if (m_bSmvModify)
                {
                    m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, TRUE);
                }
                else
                {
                    m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
                }
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_MANUAL_MODIFY_EFF));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_MANUAL_EMPTY));
            }
        }
        else if(m_eShowReportType == SRTShowGooseReport)
        {
            if (m_dwSendStatus&SSGoose)
            {
                if (m_bGooseModify)
                {
                    m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, TRUE);
                }
                else
                {
                    m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
                }
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_MANUAL_MODIFY_EFF));
            }
            else
            {
                m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_MANUAL_EMPTY));
            }
        }
    }
    else
    {
        m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
    }
    
    //
    //更新F6
    //
    if (m_eShowReportType == SRTShowSmvReport && m_ctlSmvReport.GetSafeHwnd())
    {
        int nSelCol = m_ctlSmvReport.GetFocusedColIndex();
        switch(nSelCol)
        {
        case MMS_COL_INDEX_AMPL:
            {
                _updateMenuSmv(SFCAmpl);
            }
            break;
        case MMS_COL_INDEX_PHASE:
            {
                _updateMenuSmv(SFCPhase);
            }
            break;
        default:
            {
                _updateMenuSmv(SFCOther);
            }
            break;
        }

		//设置使能F6部分菜单
		if (m_dwSendStatus)
		{
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(3, FALSE);
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(4, FALSE);
		}
		else
		{
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(3, TRUE);
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(4, TRUE);
		}

    }
	else if (m_eShowReportType == SRTShowGooseReport)
	{
		CMenu menu;
		//CNaviMenu* pSubMenu = new CNaviMenu;

		menu.LoadMenu(IDR_MENU_MANUAL_GOOSE_SEND_F6);
		m_pPopupMenu->LoadMenu(menu.GetSubMenu(0));
		//设置使能F6部分菜单
		if (m_dwSendStatus)
		{
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(1, FALSE);
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(2, FALSE);
		}
		else
		{
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(1, TRUE);
			m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->EnableMenuItem(2, TRUE);
		}

	}
	
	m_pNaviBar->UpdateNaviMenu();
}




void CManualSmvGooseSendDlg::_updateMenuSmv(MSSSmvFocusColType smvFoucsColType)
{
    if(!m_pNaviBar)
    {   
        return;
    }

    //smvreport没有创建
    if (!m_ctlSmvReport.GetSafeHwnd())
    {
        return;
    }
    
    //F3 F4

    switch(smvFoucsColType)
    {
    case SFCAmpl:
    case SFCPhase:
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_MANUAL_ADD));
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_SUB));
        }
        break;
    default:
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, _T(""));
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, _T(""));
        }
    }




    CMenu menu;
    CNaviMenu* pSubMenu = new CNaviMenu;

    menu.LoadMenu(IDR_MENU_MANUAL_SMV_SEND_F6);

#ifdef ZY_PRJ
    
    switch(smvFoucsColType)
    {
    case SFCAmpl:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(1));
        }
        break;
    case SFCPhase:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(2));
        }
        break;
    default:
        {
            pSubMenu->LoadMenu(menu.GetSubMenu(0));
        }
    }
#else

	pSubMenu->LoadMenu(menu.GetSubMenu(0));
	CNaviMenu naviMenu;
	naviMenu.LoadMenu(&menu);
	switch(smvFoucsColType)
	{
	case SFCAmpl:
		{
			 m_pPopupMenu->LoadMenu(menu.GetSubMenu(3));
		}
		break;
	case SFCPhase:
		{
			m_pPopupMenu->LoadMenu(menu.GetSubMenu(4));
		}
		break;
	default: 
			m_pPopupMenu->LoadMenu(menu.GetSubMenu(5));
		break;
		 
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
        if (pSubMenu)
        {
            MM_ItemInfo itemInfo;
            for(int i = 0; i < pSubMenu->GetMenuItemCount(); ++i)
            {
                pSubMenu->GetMenuItemInfo(i, &itemInfo, MM_GET_CMDID);
                if (itemInfo.cmdId == ID_MENU_MANUAL_STATUS_WORD_QUALITY)
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

BOOL CManualSmvGooseSendDlg::IsDIShowing()
{
    if (m_pDIStateDlg &&
        m_pDIStateDlg->GetSafeHwnd() &&
        m_pDIStateDlg->IsWindowVisible())
    {
        //退出开入量显示
        return TRUE;
    }
    return FALSE;

}

void CManualSmvGooseSendDlg::ShowDI(BOOL bShow)
{
    
    if (bShow)
    {
        if (!m_pDIStateDlg)
        {
            m_pDIStateDlg = new CDIStateDlg;
            m_pDIStateDlg->Create(CDIStateDlg::IDD, this);
        }
        m_pDIStateDlg->ShowWindow(SW_SHOWNOACTIVATE);
    }
    else
    {
        if (m_pDIStateDlg)
        {
            m_pDIStateDlg->ShowWindow(SW_HIDE);
        }
    }
}

void CManualSmvGooseSendDlg::_reloadNaviMenu(UINT nMenuID)
{
    if(!m_pNaviBar)
    {
        return;
    }

    ASSERT(-1 != nMenuID);

    m_nMenuID = nMenuID;
    m_menu.DestroyMenu();

    // 添加底部菜单
    m_menu.LoadMenu(m_nMenuID);

    m_menuMain.LoadMenu(&m_menu);	

    m_pNaviBar->SetCommandTarget(this);
    m_pNaviBar->SetNaviMenu(&m_menuMain);

    _resetMenu();
}

void CManualSmvGooseSendDlg::_loadNaviMenu()
{
    ASSERT(-1 != m_nMenuID);

    // 添加底部菜单
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);
    m_menu.DestroyMenu();


    // 调用虚函数修改菜单
    _modifyMenu();

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();

    if(m_pNaviBar)
    {
        // 设置菜单目标对象和数据对象
        m_pNaviBar->SetCommandTarget(this);
        m_pNaviBar->SetNaviMenu(&m_menuMain, TRUE);
    }
}


void  CManualSmvGooseSendDlg::_setSMVModify(BOOL bModify)
{
    if (m_bSmvModify != bModify)
    {
        m_bSmvModify = bModify;

        if (m_bSmvModify)
        {
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, TRUE);
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
        }
        m_pNaviBar->UpdateNaviMenu();
    }

}

void  CManualSmvGooseSendDlg::_setGooseModify(BOOL bModify)
{
    if (m_bGooseModify != bModify)
    {
        m_bGooseModify = bModify;

        if (m_bGooseModify)
        {
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, TRUE);
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->EnableMenuItem(4, FALSE);
        }
        m_pNaviBar->UpdateNaviMenu();
    }

}

///
/// @brief
///      判断SMV是否为空
///
/// @return
///      BOOL
///
BOOL CManualSmvGooseSendDlg::_isSmvEmpty()
{
    if (m_pConfigManualTest)
    {
        CSMVSendVirtualChannel* pVirChannel = m_pConfigManualTest->FirstSMVSendVirtualChannel();
        if (pVirChannel)
        {
            return FALSE;
        }
    }
    return TRUE;
}

///
/// @brief
///      判断goose是否为空
///
/// @return
///      BOOL
///
BOOL CManualSmvGooseSendDlg::_isGooseEmpty()
{
    return m_gooseCbMap.size() == 0;
}

void CManualSmvGooseSendDlg::OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pVirChannel == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;

    switch (pItemNotify->colIndex)
    {
    case MMS_COL_INDEX_CHAN:
        {
            pVirChannel->SetSelected(pItem->IsChecked());
            *pResult = TRUE;
        }
        break;
    case MMS_COL_INDEX_AMPL:
        {
            CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
            if (NULL == pParamConfigSMVSend)
            {
                return ;
            }

            CString strValue = pItem->GetValue();
            double dValue = 0.0f;
            bool bConvert = false;
            bConvert = DSM_Str2Double(strValue, dValue);
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
            _smvModifyEffImed();

            *pResult = TRUE;
        }
        break;
    case MMS_COL_INDEX_PHASE:
        {
            CString strValue = pItem->GetValue();
            double dValue = 0.0f;
            bool bConvert = false;
            bConvert = DSM_Str2Double(strValue, dValue);

            if (bConvert)
            {
                pVirChannel->SetPhaseAngle(dValue);
            }
            dValue = pVirChannel->GetPhaseAngle();
#if !SHENG_ZE
			dValue = AngleConversion(dValue);
#endif
            strValue.Format(_T("%.03lf"), dValue);
            pItem->SetValue(strValue);
            _smvModifyEffImed();
            *pResult = TRUE;

        }
        break;
    case MMS_COL_INDEX_FREQUENCE:
        {
            //
            //频率全部修改
            //
            CString strValue = pItem->GetValue();
            double dValue = 0.0f;
            bool bConvert = false;
            bConvert = DSM_Str2Double(strValue, dValue);

            if (bConvert)
            {
                for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
                {
                    pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
                    if (pVirChannel)
                    {
                        pVirChannel->SetFrequency(dValue);
                    }
                    dValue = pVirChannel->GetFrequency();
                    strValue.Format(_T("%.03lf"), dValue);

                    pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_FREQUENCE);
                    if (pItem)
                    {
                        pItem->SetValue(strValue);
                        m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_FREQUENCE);
                    }
                }
                _smvModifyEffImed();
                *pResult = TRUE;
            }
        }
        break;
    //case MMS_COL_INDEX_STEP:
    //    {
    //        if (m_pConfigManualTest->GetSMVChangeItem() == CHANGE_ITEM_AMPLITUDE)
    //        {
    //            CString strChanName = pVirChannel->GetName().c_str();
    //            if (strChanName.GetLength() > 0)
    //            {
    //                //改变幅值，步长均可编辑
    //                if (strChanName[0] == _T('U'))
    //                {
    //                    //电压
    //                    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    //                    if (NULL == pParamConfigSMVSend)
    //                    {
    //                        return ;
    //                    }
    //                    if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
    //                    {
    //                        //一次值
    //                        pVirChannel->SetStep(dValue*1000.0f);
    //                        dValue = pVirChannel->GetStep();
    //                        strValue.Format(_T("%.03lf"), dValue/1000.0f);
    //                    }
    //                    else
    //                    {
    //                        //二次值
    //                        pVirChannel->SetStep(dValue);
    //                        dValue = pVirChannel->GetStep();
    //                        strValue.Format(_T("%.03lf"), dValue);
    //                    }
    //                }
    //                else
    //                {
    //                    //电流 时间
    //                    pVirChannel->SetStep(dValue);
    //                    dValue = pVirChannel->GetStep();
    //                    strValue.Format(_T("%.03lf"), dValue);
    //                }
    //            }
    //        }
    //        else
    //        {
    //            pVirChannel->SetStep(dValue);
    //            dValue = pVirChannel->GetStep();
    //            strValue.Format(_T("%.03lf"), dValue);
    //        }
    //        pItem->SetValue(strValue);
    //        *pResult = TRUE;
    //    }
    //    break;
#ifndef  ZY_PRJ
	case MMS_COL_INDEX_PHASE_STEP:
		{
			CString strValue = pItem->GetValue();
			double dValue = 0.0f;
			bool bConvert = false;
			bConvert = DSM_Str2Double(strValue, dValue);

			if (bConvert)
			{
				pVirChannel->SetPhaseAngleStep(dValue);
			}
			dValue = pVirChannel->GetPhaseAngleStep();
			strValue.Format(_T("%.03lf"), dValue);
			pItem->SetValue(strValue);
			//_smvModifyEffImed();
			*pResult = TRUE;
		}
		break;
	case MMS_COL_INDEX_AMPL_STEP:
		{
			CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
			if (NULL == pParamConfigSMVSend)
			{
				return ;
			}

			CString strValue = pItem->GetValue();
			double dValue = 0.0f;
			bool bConvert = false;
			bConvert = DSM_Str2Double(strValue, dValue);
			CString strChanName = pVirChannel->GetName().c_str();
 
			if (strChanName.GetLength() > 0)
			{
				//改变幅值，步长均可编辑
				if (strChanName[0] == _T('U'))
				{
					//电压
					if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
					{
						//一次值
						pVirChannel->SetAmplitudeStep(dValue*1000.0f);
						dValue = pVirChannel->GetAmplitudeStep();
						strValue.Format(_T("%.03lf"), dValue/1000.0f);
					}
					else
					{
						//二次值
						pVirChannel->SetAmplitudeStep(dValue);
						dValue = pVirChannel->GetAmplitudeStep();
						strValue.Format(_T("%.03lf"), dValue);
					}
				}
				else
				{
					//电流 时间
					pVirChannel->SetAmplitudeStep(dValue);
					dValue = pVirChannel->GetAmplitudeStep();
					strValue.Format(_T("%.03lf"), dValue);
				}
			}
			pItem->SetValue(strValue);
			*pResult = TRUE;
		}
		break;
#endif
    }

}

void CManualSmvGooseSendDlg::OnStepOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
#ifdef ZY_PRJ    ///不再处理步长响应后的跳转界面
    int nColIndex = m_ctlSmvReport.GetFocusedColIndex();
    if (nColIndex == MMS_COL_INDEX_STEP)
    {
        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_STEP_SET_DLG);
        *pResult = TRUE;
        return;
    }
#else
	return;
#endif
}
void CManualSmvGooseSendDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        if (m_eShowReportType == SRTShowSmvReport)
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlSmvReport.GetInplaceEdit());
        }
        else
        {
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlGooseReport.GetInplaceEdit());
        }
		m_bIsEditing =  TRUE;
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
		m_bIsEditing = FALSE;
	}
}
void CManualSmvGooseSendDlg::OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CChannel* pChannel = (CChannel*)m_ctlGooseReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pChannel == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;

    CString strValue = pItem->GetValue();

    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_FLOAT:
        {
            //
            float fValue = 0.0f;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            if (DSM_Str2Float(strValue, fValue))
            {
                gooseDataVar.m_stFloat.datVal = fValue;
                pChannel->SetGooseSendValue(gooseDataVar);
            }
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%.03f"), gooseDataVar.m_stFloat.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT8:
        {
            //
            int nValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            nValue = _ttoi(strValue);
            gooseDataVar.m_stInt8.datVal = (INT8)nValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%d"), gooseDataVar.m_stInt8.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT16:
        {
            //
            int nValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            nValue = _ttoi(strValue);
            gooseDataVar.m_stInt16.datVal = (INT16)nValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%d"), gooseDataVar.m_stInt16.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT24:
        {
            //
            int nValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            nValue = _ttoi(strValue);
            gooseDataVar.m_stInt24.datVal = nValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%d"), gooseDataVar.m_stInt24.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_INT32:
        {
            //
            int nValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            nValue = _ttoi(strValue);
            gooseDataVar.m_stInt32.datVal = nValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%d"), gooseDataVar.m_stInt32.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT8:
        {
            //
            UINT uValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            uValue = _tcstoul(strValue, NULL, 10);
            gooseDataVar.m_stUInt8.datVal = (UINT8)uValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%u"), gooseDataVar.m_stUInt8.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT16:
        {
            //
            UINT uValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            uValue = _tcstoul(strValue, NULL, 10);
            gooseDataVar.m_stUInt16.datVal = (UINT16)uValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%u"), gooseDataVar.m_stUInt16.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT24:
        {
            //
            UINT uValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            uValue = _tcstoul(strValue, NULL, 10);
            gooseDataVar.m_stUInt24.datVal = uValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%u"), gooseDataVar.m_stUInt24.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_UINT32:
        {
            //
            UINT uValue = 0;
            GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
            uValue = _tcstoul(strValue, NULL, 10);
            gooseDataVar.m_stUInt32.datVal = uValue;
            pChannel->SetGooseSendValue(gooseDataVar);
            gooseDataVar = pChannel->GetGooseSendValue();
            strValue.Format(_T("%u"), gooseDataVar.m_stUInt32.datVal);
            pItem->SetValue(strValue);

            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}

void CManualSmvGooseSendDlg::OnGooseSendItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    CChannel* pChannel = (CChannel*)m_ctlGooseReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pChannel == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;
    if (pItem == NULL)
    {
        return;
    }

    GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            gooseDataVar.m_stBoolean.boolVal = (bool)pItemNotify->pConstraint->GetConstraintData();
            pChannel->SetGooseSendValue(gooseDataVar);
            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
        {
            eDPValueIndex eValueIndex;
            eValueIndex = (eDPValueIndex)pItemNotify->pConstraint->GetConstraintData();
            gooseDataVar.m_stBitString.SetString(strDPValue[eValueIndex]);
            pChannel->SetGooseSendValue(gooseDataVar);
            _gooseModifyEffImed();
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}


void CManualSmvGooseSendDlg::OnGooseSendInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify->pButton);

    CChannel* pChannel = (CChannel*)m_ctlGooseReport.GetRecordRowData(pItemNotify->rowIndex);
    if (pChannel == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;
    if (pItem == NULL)
    {
        return;
    }
    
    CString strValue = pItem->GetValue();

    GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_TIME:
        {
            //时间
            SYSTEMTIME sysTime = pChannel->GetGooseSendValue().m_stUtcTime.GetUtcTime();
            SYSTEMTIME localTime;
            DM_SystemTimeToTzSpecificLocalTime(&sysTime, &localTime);

            CRect rcItem = m_ctlGooseReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);
            CTimeEditCtrl_MS timeEditCtrl;
            timeEditCtrl.SetBkColor(ELT_T1_EDITBKCOLOR);
            timeEditCtrl.SetTextColor(ELT_T1_EDITTEXTCOLOR);

            static CWnd* pSrc = NULL;
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&timeEditCtrl);
            if (IDOK == timeEditCtrl.GoModal(&m_ctlGooseReport, rcItem, localTime))
            {
                DM_TzSpecificLocalTimeToSystemTime(&localTime, &sysTime);
                gooseDataVar.m_stUtcTime.SetUtcTime(sysTime);
                pChannel->SetGooseSendValue(gooseDataVar);
                CString strTime = _T("");
                CTimeEditCtrl_MS::SysTime2Str(localTime, strTime);
                pItem->SetValue(strTime);
                m_ctlGooseReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
                _gooseModifyEffImed();
            }
            CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
            *pResult = TRUE;
        }
        break;
    case CHANNEL_TYPE_GOOSE_QUALITY:
        {
            CRect rcItem = m_ctlGooseReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);
            CBitStrEditCtrl bitStrEditCtrl;
            bitStrEditCtrl.SetBkColor(ELT_T1_EDITBKCOLOR);
            bitStrEditCtrl.SetTextColor(ELT_T1_EDITTEXTCOLOR);
            bitStrEditCtrl.SetBorderColor(ELT_T1_EDITBORDERCOLOR);
            static CWnd* pSrc = NULL;
            pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&bitStrEditCtrl);
            if (IDOK == bitStrEditCtrl.GoModal(&m_ctlGooseReport, rcItem, strValue))
            {
                gooseDataVar.m_stBitString.SetString((LPCTSTR)strValue);
                pChannel->SetGooseSendValue(gooseDataVar);
                pItem->SetValue(gooseDataVar.m_stBitString.GetString().c_str());
                m_ctlGooseReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
                _gooseModifyEffImed();
            }
            CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}

void CManualSmvGooseSendDlg::OnGoosePointItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    int nRowIndex = m_ctlGooseReport.GetFocusedRowIndex();
    int nColIndex = m_ctlGooseReport.GetFocusedColIndex();
    if (nRowIndex <0 || nColIndex < 0)
    {
        return;
    }
    CChannel* pChannel = (CChannel*)m_ctlGooseReport.GetRecordRowData(nRowIndex);
    if (pChannel == NULL)
    {
        return;
    }

    GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();
    switch(pChannel->GetChannelType())
    {
    case CHANNEL_TYPE_GOOSE_POINT:
        {
            CELRecordItemText* pItem = (CELRecordItemText*)m_ctlGooseReport.GetRecordRow(nRowIndex)->GetItem(nColIndex);
            if (pItem == NULL)
            {
                return;
            }
            gooseDataVar.m_stBoolean.boolVal = !gooseDataVar.m_stBoolean.boolVal;
            if (gooseDataVar.m_stBoolean.boolVal)
            {
                pItem->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_ON));
            }
            else
            {
                pItem->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_POINT_OFF));
            }
            m_ctlGooseReport.RedrawRowItem(nRowIndex, nColIndex);
            pChannel->SetGooseSendValue(gooseDataVar);
            _gooseModifyEffImed();
            *pResult = TRUE;
            return;
        }
        break;
    default:
        break;
    }
}
void CManualSmvGooseSendDlg::OnNaviMenuParamSmvGooseSend(UINT nID)
{
    if (m_pConfigManualTest == NULL)
    {
        return;
    }
    switch (nID)
    {
    case  ID_MENU_MANUAL_SHOW_GOOSE_REP:
        {
            _showGooseReport();
        }
        break;
    case  ID_MENU_MANUAL_SEND_SMV:
        {

            RETAILMSG(OUTPUT_LOG_MSG, (L"CManualSmvGooseSendDlg::OnNaviMenuParamSmvGooseSend <F2>      \n"));
            //发送smv
            eStartMode startMode = m_pConfigManualTest->GetStartMode();

            if (startMode == START_MODE_APART)
            {
                if (m_dwSendStatus & SSSmv)
                {
                    //发送SMV->停发SMV
                    m_dwSendStatus &= ~SSSmv;
                    m_pDataLayer->StopManualTestSMV();
                    if (m_dwSendStatus == SSInit)
                    {
                        //smv goose 均停发
                        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_LIST_DLG);
                        return;
                    }
                }
                else
                {
                    //停发SMV->发送SMV
                    if (_isSmvEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_SMV_TIPS));
                        return;
                    }
                    m_dwSendStatus |= SSSmv;
                    m_pDataLayer->StartManualTestSMV(this->GetSafeHwnd());

                    if (!IsDIShowing())
                    {
                        ShowDI(TRUE);
                    }
                }
            }
            else if(startMode == START_MODE_TOGETHER)
            {
                if ( m_dwSendStatus & (SSSmv|SSSmvGoose))
                {
                    //发送全部->停止全部
                    m_dwSendStatus &= ~(SSSmv|SSSmvGoose);
                    m_pDataLayer->StopManualTestSMV();
                    m_pDataLayer->StopManualTestGoose();
                    if (m_dwSendStatus == SSInit)
                    {
                        //smv goose 均停发
                        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_LIST_DLG);
                        return;
                    }
                }
                else
                {

                    if (_isSmvEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_SMV_TIPS));
                        return;
                    }

                    if (_isGooseEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_GOOSE_TIPS));
                        return;
                    }
                    //停止全部->发送全部
                    m_dwSendStatus |= (SSSmv|SSSmvGoose);
                    m_pDataLayer->StartManualTestSMV(this->GetSafeHwnd());
                    m_pDataLayer->StartManualTestGoose(this->GetSafeHwnd());

                    if (!IsDIShowing())
                    {
                        ShowDI(TRUE);
                    }
                }
            }
            _resetMenu();
        }
        break;
    case  ID_MENU_MANUAL_SMV_ADD:
        {
            int nColSel = m_ctlSmvReport.GetFocusedColIndex();
            if (nColSel == MMS_COL_INDEX_AMPL ||
                nColSel == MMS_COL_INDEX_PHASE)
            {
                _smvAddSub(SCVCAdd);
                _smvModifyEffImed();
            }
        }
        break;
    case  ID_MENU_MANUAL_SMV_SUB:
        {
            int nColSel = m_ctlSmvReport.GetFocusedColIndex();
            if (nColSel == MMS_COL_INDEX_AMPL ||
                nColSel == MMS_COL_INDEX_PHASE)
            {
                _smvAddSub(SCVCSub);
                _smvModifyEffImed();
            }
        }
        break;
    case  ID_MENU_MANUAL_SMV_MODIFY_EFF:
        {
            if (m_dwSendStatus & SSSmv)
            {
                m_pDataLayer->UpdateManualTestSMV();
                _setSMVModify(FALSE);
            }
        }
        break;
    case  ID_MENU_MANUAL_GSE_MODIFY_EFF:
        {
            if (m_dwSendStatus & SSGoose)
            {
                m_pDataLayer->UpdateManualTestGoose();
                _setGooseModify(FALSE);
            }
        }
        break;

    case ID_MENU_MANUAL_EQUAL_AMPL:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->EqualAmplitude(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }
        }
        break;
    case ID_MENU_MANUAL_RATED_AMPL:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->RatedAmplitude(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }
        }
        break;
    case ID_MENU_MANUAL_THREE_PHASE_BALANCE:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->ThreePhaseBalance(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }

        }
        break;
    case ID_MENU_MANUAL_EQUAL_PHASE_ANGLE:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->EqualPhaseAngle(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }

        }
        break;
    case ID_MENU_MANUAL_PHASE_ANGLE_POS_SEQ:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->PhaseAnglePositiveSequence(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }
        }
        break;
    case ID_MENU_MANUAL_PHASE_ANGLE_REV_SEQ:
        {
            int nSelRow = m_ctlSmvReport.GetFocusedRowIndex();
            if (nSelRow < 0)
            {
                break;
            }
            CSMVSendVirtualChannel* pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(nSelRow);
            if (pVirChannel)
            {
                m_pConfigManualTest->PhaseAngleReverseSequence(pVirChannel);
                _updateSmvReport();
                _smvModifyEffImed();
            }
        }
        break;
    case  ID_MENU_MANUAL_STATUS_WORD_QUALITY:
        {
            CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

            if(pData && CManualSmvGooseSendDlg::IDD == pData->m_nPageID)
            {
                CManualSmvGooseSendDataExtra* pDataExtra = dynamic_cast<CManualSmvGooseSendDataExtra*>(pData->m_pPageDataExtra);
                if (pDataExtra == NULL)
                {
                    pData->m_pPageDataExtra = new CManualSmvGooseSendDataExtra(pData);
                    pDataExtra = dynamic_cast<CManualSmvGooseSendDataExtra*>(pData->m_pPageDataExtra);
                }
                if (pDataExtra != NULL)
                {
                    pDataExtra->m_uNextPageID = IDD_MANUAL_SMV_CTRL_LIST_DLG;
                }
                CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CTRL_LIST_DLG);
            }
        }
        break;
    case  ID_MENU_MANUAL_SMV_MAP:
        {
            CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_MAP_DLG);
        }
        break;
    case  ID_MENU_MANUAL_DI_MAP:
        {
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DI_MAP_DLG);
        }
        break;
    case  ID_MENU_MANUAL_SET:
        {
            CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SET_DLG);
        }
        break;
    case  ID_MENU_MANUAL_RESULT:
        {
            CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_LIST_DLG);
        }
        break;
    case  ID_MENU_MANUAL_SHOW_DI:
        {
            //显示开入映射
            if (IsDIShowing())
            {
                ShowDI(FALSE);
                m_pConfigManualTest->SetIsShowDI(FALSE);
            }
            else
            {
                ShowDI(TRUE);
                m_pConfigManualTest->SetIsShowDI(TRUE);
            }
            
        }
        break;
    case  ID_MENU_MANUAL_SHOW_SMV_REP:
        {
            _showSmvReport();
        }
        break;
    case  ID_MENU_MANUAL_GOOSE_SEND_GOOSE:
        {
            //发送goose
            //发送smv

            eStartMode startMode = m_pConfigManualTest->GetStartMode();

            if (startMode == START_MODE_APART)
            {
                if (m_dwSendStatus & SSGoose)
                {
                    //发送GOOSE->停发GOOSE
                    m_dwSendStatus &= ~SSGoose;
                    m_pDataLayer->StopManualTestGoose();
                    if (m_dwSendStatus == SSInit)
                    {
                        //smv goose 均停发
                        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_LIST_DLG);
                        return;
                    }
                }
                else
                {
                    if (_isGooseEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_GOOSE_TIPS));
                        return;
                    }
                    //停发GOOSE->发送GOOSE
                    m_dwSendStatus |= SSGoose;
                    m_pDataLayer->StartManualTestGoose(this->GetSafeHwnd());

                    if (m_pConfigManualTest->GetIsShowDI())
                    {
                        if (!IsDIShowing())
                        {
                            ShowDI(TRUE);
                        }
                    }
                }
            }
            else if(startMode == START_MODE_TOGETHER)
            {
                if ( m_dwSendStatus & (SSSmv|SSSmvGoose))
                {
                    //发送全部->停止全部
                    m_dwSendStatus &= ~(SSSmv|SSSmvGoose);
                    m_pDataLayer->StopManualTestSMV();
                    m_pDataLayer->StopManualTestGoose();
                    if (m_dwSendStatus == SSInit)
                    {
                        //smv goose 均停发
                        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_LIST_DLG);
                        return;
                    }
                }
                else
                {
                    if (_isSmvEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_SMV_TIPS));
                        return;
                    }

                    if (_isGooseEmpty())
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_NO_GOOSE_TIPS));
                        return;
                    }
                    //停止全部->发送全部
                    m_dwSendStatus |= (SSSmv|SSSmvGoose);
                    m_pDataLayer->StartManualTestSMV(this->GetSafeHwnd());
                    m_pDataLayer->StartManualTestGoose(this->GetSafeHwnd());

                    if (!IsDIShowing())
                    {
                        ShowDI(TRUE);
                    }
                }
            }
            _resetMenu();
        }
        break;
    case ID_MENU_MANUAL_GOOSE_CTRL:
        {
			int testi = 0;
        }
        break;
    case  ID_MENU_MANUAL_GOOSE_ALL_ON:
        {
            _modifyAllGooseValue(MGVTAllOn);
            _gooseModifyEffImed();
        }
        break;
    case  ID_MENU_MANUAL_GOOSE_ALL_OFF:
        {
            _modifyAllGooseValue(MGVTAllOff);
            _gooseModifyEffImed();
        }
        break;
    case ID_MENU_MANUAL_GOOSE_REVERT :
        {
            _modifyAllGooseValue(MGVTAllRevert);
            _gooseModifyEffImed();
        }
        break;
    }
}

void CManualSmvGooseSendDlg::OnUpdateNaviMenuParamSmvGooseSend(CCmdUI* pCmdUI)
{
    if (m_pConfigManualTest == NULL)
    {
        return;
    }
    switch(pCmdUI->m_nID)
    {
    case  ID_MENU_MANUAL_SMV_MODIFY_EFF:
        {
            ////
            ////手动生效下更新菜单，使能或禁能菜单
            ////
            eModifyEffectiveMode effMode = m_pConfigManualTest->GetModifyEffectiveMode();
            if (effMode == EFFECTIVE_MODE_MANUALLY)
            {
                if (m_dwSendStatus&SSSmv && m_bSmvModify)
                {
                    pCmdUI->Enable(TRUE);
                }
                else
                {
                    pCmdUI->Enable(FALSE);
                }
            }
            else
            {
                pCmdUI->Enable(FALSE);
            }
        }
        break;
    case  ID_MENU_MANUAL_GSE_MODIFY_EFF:
        {
            ////
            ////手动生效下更新菜单，使能或禁能菜单
            ////
            eModifyEffectiveMode effMode = m_pConfigManualTest->GetModifyEffectiveMode();
            if (effMode == EFFECTIVE_MODE_MANUALLY)
            {
                if (m_dwSendStatus&SSGoose  && m_bGooseModify)
                {
                    pCmdUI->Enable(TRUE);
                }
                else
                {
                    pCmdUI->Enable(FALSE);
                }
            }
            else
            {
                pCmdUI->Enable(FALSE);
            }
        }
        break;
    case  ID_MENU_MANUAL_SET:
    case  ID_MENU_MANUAL_RESULT:
        {
            if (m_dwSendStatus == SSInit)
            {
                pCmdUI->Enable(TRUE);
            }
            else
            {
                pCmdUI->Enable(FALSE);
            }
        }
        break;
    default:
        break;
    }
}
///
/// @brief
///      对通道中值做加法
///
/// @return
///      void    
///
void CManualSmvGooseSendDlg::_smvAddSub(MSSSmvChanValCalc nScvc)
{

    int nColSel = m_ctlSmvReport.GetFocusedColIndex();
    ASSERT(nColSel == MMS_COL_INDEX_AMPL || nColSel == MMS_COL_INDEX_PHASE);

    CSMVSendVirtualChannel* pVirChannel = NULL;
    CELRecordItemText* pItem = NULL;
    CString strContent = _T("");
    double dContent = 0.0f;

    switch(nColSel)
    {
    case MMS_COL_INDEX_AMPL:
        {
            //设置幅值
            CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
            if (NULL == pParamConfigSMVSend)
            {
                return ;
            }

            CELRecordItemText* pItem = NULL;
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                CELRecordItem* pItemSel = m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_CHAN);
                if (pItemSel->IsChecked())
                {
                    pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
                    if (pVirChannel != NULL)
                    {
                        dContent = pVirChannel->GetAmplitude();
                        if(nScvc == SCVCAdd)
                        {
                            dContent += pVirChannel->GetAmplitudeStep();
                        }
                        else if(nScvc == SCVCSub)
                        {
                            dContent -= pVirChannel->GetAmplitudeStep();
                        }
                        pVirChannel->SetAmplitude(dContent);
                        
                        //
                        pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_AMPL);
                        if (pItem)
                        {
                            CString strChanName = pVirChannel->GetName().c_str();
                            if (strChanName.GetLength() > 0)
                            {
                                if (strChanName[0] == _T('U'))
                                {
                                    //电压通道
                                    if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
                                    {
                                        //一次值
                                        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude()/1000.0f);
                                    }
                                    else
                                    {
                                        //二次值
                                        strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
                                    }
                                }
                                else
                                {
                                    //电流时间
                                    strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
                                }
                            }
                            pItem->SetValue(strContent);
                            m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_AMPL);
                        }
                    }
                }
            }
        }
        break;
    case MMS_COL_INDEX_PHASE:
        {
            CString strChanName = _T("");
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                CELRecordItem* pItemSel = m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_CHAN);
                if (pItemSel->IsChecked())
                {
                    pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
                    if (pVirChannel != NULL)
                    {
                        strChanName = pVirChannel->GetName().c_str();

                        if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
                        {
                            if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
                            {
                                //
                                //改变项为相角时，只能对交流的电压电流进行
                                //
                                dContent = pVirChannel->GetPhaseAngle();

                                if(nScvc == SCVCAdd)
                                {
                                    dContent += pVirChannel->GetPhaseAngleStep();
                                }
                                else if(nScvc == SCVCSub)
                                {
                                    dContent -= pVirChannel->GetPhaseAngleStep();
                                }
#if !SHENG_ZE
								dContent = AngleConversion(dContent);
#endif
                                pVirChannel->SetPhaseAngle(dContent);
                                pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_PHASE);
                                if (pItem)
                                {
                                    strContent.Format(_T("%.03lf"), pVirChannel->GetPhaseAngle());
                                    pItem->SetValue(strContent);
                                    m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_PHASE);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    //if (m_pConfigManualTest == NULL)
    //{
    //    return;
    //}
    //
    //CSMVSendVirtualChannel* pVirChannel = NULL;
    //CELRecordItemText* pItem = NULL;
    //CString strContent = _T("");
    //double dContent = 0.0f;
    //eSMVChangeItem chanItem = m_pConfigManualTest->GetSMVChangeItem();
    //switch(chanItem)
    //{
    //case CHANGE_ITEM_AMPLITUDE:
    //    {
    //        //设置幅值
    //        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    //        if (NULL == pParamConfigSMVSend)
    //        {
    //            return ;
    //        }

    //        for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
    //        {
    //            pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
    //            if (pVirChannel != NULL)
    //            {
    //                dContent = pVirChannel->GetAmplitude();
    //                if(nScvc == SCVCAdd)
    //                {
    //                    dContent += pVirChannel->GetStep();
    //                }
    //                else if(nScvc == SCVCSub)
    //                {
    //                    dContent -= pVirChannel->GetStep();
    //                }
    //                pVirChannel->SetAmplitude(dContent);
    //                
    //                //
    //                pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_AMPL);
    //                if (pItem)
    //                {
    //                    CString strChanName = pVirChannel->GetName().c_str();
    //                    if (strChanName.GetLength() > 0)
    //                    {
    //                        if (strChanName[0] == _T('U'))
    //                        {
    //                            //电压通道
    //                            if (pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
    //                            {
    //                                //一次值
    //                                strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude()/1000.0f);
    //                            }
    //                            else
    //                            {
    //                                //二次值
    //                                strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
    //                            }
    //                        }
    //                        else
    //                        {
    //                            //电流时间
    //                            strContent.Format(_T("%.03lf"), pVirChannel->GetAmplitude());
    //                        }
    //                    }
    //                    pItem->SetValue(strContent);
    //                    m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_AMPL);
    //                }
    //            }
    //        }
    //    }
    //    break;
    //case CHANGE_ITEM_PHASE_ANGLE:
    //    {
    //        CString strChanName = _T("");

    //        for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
    //        {
    //            pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
    //            if (pVirChannel != NULL)
    //            {
    //                strChanName = pVirChannel->GetName().c_str();

    //                if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    //                {
    //                    if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
    //                    {
    //                        //
    //                        //改变项为相角时，只能对交流的电压电流进行
    //                        //
    //                        dContent = pVirChannel->GetPhaseAngle();

    //                        if(nScvc == SCVCAdd)
    //                        {
    //                            dContent += pVirChannel->GetStep();
    //                        }
    //                        else if(nScvc == SCVCSub)
    //                        {
    //                            dContent -= pVirChannel->GetStep();
    //                        }

    //                        pVirChannel->SetPhaseAngle(dContent);
    //                        pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_PHASE);
    //                        if (pItem)
    //                        {
    //                            strContent.Format(_T("%.03lf"), pVirChannel->GetPhaseAngle());
    //                            pItem->SetValue(strContent);
    //                            m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_PHASE);
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //    break;
    //case CHANGE_ITEM_FREQUENCY:
    //    {
    //        CString strChanName = _T("");

    //        for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
    //        {
    //            pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(i);
    //            if (pVirChannel != NULL)
    //            {
    //                strChanName = pVirChannel->GetName().c_str();

    //                if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    //                {
    //                    if (pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
    //                    {
    //                        //
    //                        //改变项为频率时，只能对交流的电压电流进行
    //                        //
    //                        dContent = pVirChannel->GetFrequency();
    //                        if(nScvc == SCVCAdd)
    //                        {
    //                            dContent += pVirChannel->GetStep();
    //                        }
    //                        else if(nScvc == SCVCSub)
    //                        {
    //                            dContent -= pVirChannel->GetStep();
    //                        }
    //                        pVirChannel->SetFrequency(dContent);
    //                        pItem = (CELRecordItemText*)m_ctlSmvReport.GetRecordRow(i)->GetItem(MMS_COL_INDEX_FREQUENCE);
    //                        if (pItem)
    //                        {
    //                            strContent.Format(_T("%.03lf"), pVirChannel->GetFrequency());
    //                            pItem->SetValue(strContent);
    //                            m_ctlSmvReport.RedrawRowItem(i, MMS_COL_INDEX_FREQUENCE);
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //    }
    //    break;
    //default:
    //    break;
    //}
}

void CManualSmvGooseSendDlg::_nextCtrlBlock()
{

    if(m_pConfigGooseSend == NULL && m_pGooseCb == NULL)
    {
        return;
    }
    
    CGooseCb* pOldGooseCb = m_pGooseCb;

    m_pGooseCb = m_pConfigGooseSend->NextGocb(m_pGooseCb);
    if (m_pGooseCb == NULL)
    {
        m_pGooseCb = m_pConfigGooseSend->FirstGocb();
    }

    while( (m_pGooseCb != NULL ) && (m_pGooseCb != pOldGooseCb))
    {
        if (m_pGooseCb->IsSelected())
        {
            break;
        }

        m_pGooseCb = m_pConfigGooseSend->NextGocb(m_pGooseCb);
        if (m_pGooseCb == NULL)
        {
            m_pGooseCb = m_pConfigGooseSend->FirstGocb();
        }
    }
    
    if (m_pGooseCb != NULL && m_pGooseCb != pOldGooseCb)
    {
        _updateGooseReport();
        CString csCaption;
        if (m_pGooseCb)
        {
			csCaption.Format(L"%s->%s: 0x%04X", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION), m_pGooseCb->GetAPPID());
        }
        else
        {
            csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION));
        }
        CStatusMgr::SetTitle(csCaption);
    }
}

void CManualSmvGooseSendDlg::_smvModifyEffImed()
{
    if (m_dwSendStatus & SSSmv)
    {
        eModifyEffectiveMode effMode = m_pConfigManualTest->GetModifyEffectiveMode();
        if (effMode == EFFECTIVE_MODE_IMMEDIATE)
        {
            m_pDataLayer->UpdateManualTestSMV();
        }
        else
        {
            _setSMVModify(TRUE);


            m_pNaviBar->UpdateNaviMenu();
        }
    }
}

void CManualSmvGooseSendDlg::_gooseModifyEffImed()
{

    if (m_dwSendStatus & SSGoose)
    {
        eModifyEffectiveMode effMode = m_pConfigManualTest->GetModifyEffectiveMode();
        if (effMode == EFFECTIVE_MODE_IMMEDIATE)
        {
            m_pDataLayer->UpdateManualTestGoose();
        }
        else
        {
            _setGooseModify(TRUE);
            m_pNaviBar->UpdateNaviMenu();
        }
    }
}

void CManualSmvGooseSendDlg::_modifyAllGooseValue( MSSModifyGooseValType mgvModifyGooseValType )
{

    CChannel* pChannel = NULL;

    int nRowCount = m_ctlGooseReport.GetRecordRowCount();

    for (int i = 0; i < nRowCount; ++i)
    {
        pChannel = (CChannel*)m_ctlGooseReport.GetRecordRowData(i);
        if (pChannel == NULL)
        {
            continue;
        }

        GOOSE_DATA_VARIANT gooseDataVar = pChannel->GetGooseSendValue();

        switch(pChannel->GetChannelType())
        {
        case CHANNEL_TYPE_GOOSE_POINT:
            {
                switch(mgvModifyGooseValType)
                {
                case MGVTAllOn:
                    {
                        gooseDataVar.m_stBoolean.boolVal = true;
                    }
                    break;
                case MGVTAllOff:
                    {
                        gooseDataVar.m_stBoolean.boolVal = false;
                    }
                    break;
                case MGVTAllRevert:
                    {
                        gooseDataVar.m_stBoolean.boolVal = !gooseDataVar.m_stBoolean.boolVal;
                    }
                    break;
                default:
                    break;
                }

            }
            break;
        case CHANNEL_TYPE_GOOSE_DOUBLE_POINT:
            {
                switch(mgvModifyGooseValType)
                {
                case MGVTAllOn:
                    {
                        gooseDataVar.m_stBitString.SetString(strDPValue[DP_VALUE_ON]);
                    }
                    break;
                case MGVTAllOff:
                    {
                        gooseDataVar.m_stBitString.SetString(strDPValue[DP_VALUE_OFF]);
                    }
                    break;
                case MGVTAllRevert:
                    {
                        std::wstring strValue = gooseDataVar.m_stBitString.GetString();
                        for (int i =DP_VALUE_INIT; i <= DP_VALUE_BAD; ++i)
                        {
                            if (strDPValue[i] == strValue)
                            {
                                gooseDataVar.m_stBitString.SetString(strDPValue[DP_VALUE_BAD - i]);
                                break;
                            }
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case CHANNEL_TYPE_GOOSE_QUALITY:
            {

            }
            break;
        default:
            break;
        }
        pChannel->SetGooseSendValue(gooseDataVar);
    }
    _updateGooseReport();
}




//DWORD WINAPI ManualStopTestingProc(WPARAM wParam, LPARAM lParam, void* arg)
//{
//    CDialog* pDlg = (CDialog*)arg;
//    
//    pDlg->EndDialog(IDOK);
//
//    return 0;
//}

BOOL CManualSmvGooseSendDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
                if (m_dwSendStatus != SSInit)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_STOP_TESTING_TIPS), 3000);
                    return TRUE;
                }
                else
                {
                    return CBaseDlg::PreTranslateMessage(pMsg);
                }
            }
            break;
		case VK_F8:
			{
				_showContexMenu();
				return TRUE;
			}
            break;
        case DSM_KEY_HOME:
            {
                if (m_dwSendStatus != SSInit)
                {
                    //退出测试
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_MANUAL_STOP_TESTING_TIPS), 3000);
                    return TRUE;
                }
                else
                {
                    return CBaseDlg::PreTranslateMessage(pMsg);
                }
            }
            break;
        default:
            break;
        }
    }
    BOOL bRet = CBaseDlg::PreTranslateMessage(pMsg);
    
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
            {
                if (m_eShowReportType == SRTShowSmvReport && m_ctlSmvReport.GetSafeHwnd())
                {
                    int nSelCol = m_ctlSmvReport.GetFocusedColIndex();

                    switch(nSelCol)
                    {
                    case MMS_COL_INDEX_AMPL:
                        {
                            _updateMenuSmv(SFCAmpl);
                        }
                        break;
                    case MMS_COL_INDEX_PHASE:
                        {
                            _updateMenuSmv(SFCPhase);
                        }
                        break;
                    default:
                        {
                            _updateMenuSmv(SFCOther);
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



    return bRet;
}

void CManualSmvGooseSendDlg::_showContexMenu()
{
	/*CMenu menu ; 
	CNaviMenu realMenu;
	menu.LoadMenu(IDR_MENU_MANUAL_SMV_SEND_F6);
	realMenu.LoadMenu(&menu);
	menu.DestroyMenu();*/

	//正在编辑，不弹出右键，否则出错
	if (m_bIsEditing)
	{
		return;
	}

	CReportV* cCurrentReport;
	if (m_eShowReportType == SRTShowSmvReport)
	{
		cCurrentReport = &m_ctlSmvReport;
	}
	else if (m_eShowReportType == SRTShowGooseReport)
	{
		cCurrentReport = &m_ctlGooseReport;
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

void CManualSmvGooseSendDlg::OnDestroy()
{
    // TODO: 在此处添加消息处理程序代码
    if(m_pConfigGooseSend!= NULL)
    {
        m_pConfigGooseSend->SaveConfig();
    }
    if(m_pDIStateDlg)
    {
        ::SendMessage(m_pDIStateDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
    }

	KillTimer(2);

    CBaseDlg::OnDestroy();
}

void CManualSmvGooseSendDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    
    if (nIDEvent == 1)
    {
        m_pConfigGooseSend->SaveConfig();
    }


    CBaseDlg::OnTimer(nIDEvent);
}

void CManualSmvGooseSendDlg::OnPaint()
{

	__super::OnPaint();
}


LRESULT CManualSmvGooseSendDlg::OnDIChanged( WPARAM wParam, LPARAM lParam )
{
    if (m_pDIStateDlg && m_pDIStateDlg->GetSafeHwnd())
    {
        ::SendMessage(m_pDIStateDlg->GetSafeHwnd(), WM_DI_CHANGED, wParam, lParam);
    }
    return 0;
}

LRESULT CManualSmvGooseSendDlg::OnManualTestResult( WPARAM wParam, LPARAM lParam )
{
    enumErrorCode errCode = (enumErrorCode)wParam;
    CString strTips = _T("");
    switch(errCode)
    {
    case ERROR_SMV_TOO_MANY_CHANNEL_ASDU:
        {

            m_dwSendStatus &= ~SSSmv;
            strTips =  dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_TOO_MANY_CHANNEL_ASDU);
            CStatusMgr::SetTips(strTips);
        }
        break;
    case ERROR_GOOSE_TOO_MANY_CHANNEL:
        {
            m_dwSendStatus &= ~SSGoose;
            strTips =  dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_TOO_MANY_CHANNEL);
            CStatusMgr::SetTips(strTips);
        }
        break;
    default:
        break;
    }

    _resetMenu();
    return 0;
}

void CManualSmvGooseSendDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvGooseSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CManualSmvGooseSendDataExtra(pData);
    }

    CManualSmvGooseSendDataExtra* pDataExtra = dynamic_cast<CManualSmvGooseSendDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }
    
    pDataExtra->m_dwSendStatus = m_dwSendStatus;
    pDataExtra->m_eShowReportType = m_eShowReportType;
    if (m_eShowReportType == SRTShowSmvReport)
    {
        pDataExtra->m_nFocusRow = m_ctlSmvReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusCol = m_ctlSmvReport.GetFocusedColIndex();
        if (pDataExtra->m_nFocusRow >= 0)
        {
            pDataExtra->m_pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvReport.GetRecordRowData(pDataExtra->m_nFocusRow);
        }
    }
    else if (m_eShowReportType == SRTShowGooseReport)
    {
        pDataExtra->m_nFocusRow = m_ctlGooseReport.GetFocusedRowIndex();
        pDataExtra->m_nFocusCol = m_ctlGooseReport.GetFocusedColIndex();
    }
    pDataExtra->m_pGooseCb = m_pGooseCb;
    
    m_pConfigManualTest->SetIsShowDI(IsDIShowing());
    ShowDI(FALSE);
    m_pConfigManualTest->SaveConfig();
}

void CManualSmvGooseSendDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvGooseSendDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CManualSmvGooseSendDataExtra* pDataExtra = dynamic_cast<CManualSmvGooseSendDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        m_dwSendStatus = pDataExtra->m_dwSendStatus;
        m_pGooseCb = pDataExtra->m_pGooseCb;
        m_eShowReportType = pDataExtra->m_eShowReportType;
        if (m_eShowReportType == SRTShowSmvReport)
        {
            _showSmvReport();
            if (m_ctlSmvReport.GetSafeHwnd())
            {
                if (m_ctlSmvReport.GetRecordRowCount() > pDataExtra->m_nFocusRow && pDataExtra->m_nFocusRow >= 0)
                {
                    m_ctlSmvReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRow);
                }
                m_ctlSmvReport.SetFocusedColIndex(pDataExtra->m_nFocusCol);
                m_ctlSmvReport.EnsureVisible(pDataExtra->m_nFocusRow, FALSE);
            }
        }
        else if (m_eShowReportType == SRTShowGooseReport)
        {
            _showGooseReport();
            if (m_ctlGooseReport.GetSafeHwnd())
            {
                if (m_ctlGooseReport.GetRecordRowCount() > pDataExtra->m_nFocusRow && pDataExtra->m_nFocusRow >= 0)
                {
                    m_ctlGooseReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRow);
                }
                m_ctlGooseReport.SetFocusedColIndex(pDataExtra->m_nFocusCol);
                m_ctlGooseReport.EnsureVisible(pDataExtra->m_nFocusRow, FALSE);
            }
        }
        _resetMenu();
        //
        //如果是从SMV控制块列表（设置通道品质或状态字）返回，且正在发送SMV，则立即发送一次smv
        //
        if (pDataExtra->m_uNextPageID == IDD_MANUAL_SMV_CTRL_LIST_DLG)
        {
            if (m_dwSendStatus & SSSmv)
            {
                m_pDataLayer->UpdateManualTestSMV();
            }
        }
    }
}

void CManualSmvGooseSendDlg::OnNaviMenuGooseCtrlSel( UINT nID )
{
    if (m_pConfigManualTest == NULL)
    {
        return;
    }
    _highlightGooseF3(FALSE);
    //
    //变更控制块
    //
    CUINTGooseCbMap::iterator iter = m_gooseCbMap.find(nID);
    if (iter != m_gooseCbMap.end())
    {
        CGooseCb* pOldGooseCb = NULL;
        pOldGooseCb = m_pGooseCb;

        if (iter->second != NULL && iter->second != pOldGooseCb)
        {
            m_pGooseCb = iter->second;

            _updateGooseReport();
            CString csCaption;
            if (m_pGooseCb)
            {
				csCaption.Format(L"%s->%s: 0x%04X", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION), m_pGooseCb->GetAPPID());
            }
            else
            {
                csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION));
            }
            CStatusMgr::SetTitle(csCaption);
        }
    }
    //
    //变更控制块焦点
    //
    _highlightGooseF3();
}

#if !SHENG_ZE
double CManualSmvGooseSendDlg::AngleConversion(double Val)
{
	if (Val < 0.000001 && Val > -0.000001)
	{
		Val = 0;
	}
	if(Val > 0)
	{
		while(Val - 360 >= 360)
		{
			Val -= 360;
		}

		while (Val > 180)
		{
			Val = Val - 360;
		}
		if (Val < 0.000001 && Val > -0.000001)
		{
			Val = 0;
		}
	}

	if(Val < 0)
	{
		while(Val + 360 <= -360)
		{
			Val += 360;
		}
		while (Val < -180 )
		{
			Val = Val - (-360);
		}
		if (Val < 0.000001 && Val > -0.000001)
		{
			Val = 0;
		}
	}
	return Val;
};
#endif