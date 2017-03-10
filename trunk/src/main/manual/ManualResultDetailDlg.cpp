/// @file
///
/// @brief
///     �ֶ�ʵ������� ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "ManualResultDetailDlg.h"
#include "ManualResultListDlg.h"
#include "src/main/mgr/StatusMgr.h"



#define DSM_SMV_GSE_TIP_SHOW_TIME           100000    ///< Ĭ����ʾ��ʾ��ʱ��

//
//smv����
//
#define DSM_MANUAL_RD_SMV_COL_INDEX_CHAN           0
#define DSM_MANUAL_RD_SMV_COL_INDEX_AMPL           1
#define DSM_MANUAL_RD_SMV_COL_INDEX_PHASE          2
#define DSM_MANUAL_RD_SMV_COL_INDEX_FREQUENCE      3



#define DSM_MANUAL_RD_SMV_COL_COUNT           4
#define DSM_MANUAL_RD_SMV_FREEZE_COL_COUNT    1
//
//smv �б�
//
#define DSM_MANUAL_RD_SMV_WIDTH_CHAN          68
#define DSM_MANUAL_RD_SMV_WIDTH_AMPL          154
#define DSM_MANUAL_RD_SMV_WIDTH_PHASE         154
#define DSM_MANUAL_RD_SMV_WIDTH_FREQUENCE     154



#define DSM_MANUAL_RD_GOOSE_COL_COUNT         4
#define DSM_MANUAL_RD_GOOSE_FREEZE_COL_COUNT  3

//
//goose����
//
#define DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_INDEX  50
#define DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_DESC   230
#define DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_TYPE   80
#define DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_VALUE  169

//
//DI�ı䱨��
//
#define DSM_MANUAL_RD_DI_ACTION_WIDTH_DI            81
#define DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION_COUNT  80
#define DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION1       123
#define DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION2       123
#define DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION3       123

// CManualResultDetailDlg �Ի���

IMPLEMENT_DYNAMIC(CManualResultDetailDlg, CBaseDlg)

CManualResultDetailDlg::CManualResultDetailDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualResultDetailDlg::IDD, DSM_STR_MANUAL_RESULT_DETAIL_CAPTION, pParent)
    , m_pGooseCb(NULL)
    , m_pStateTestResult(NULL)
    , m_pDataLayer(NULL)
    , m_bShowAllData(TRUE)
    , m_nSmvGseFocusRowIndex(-1)
    , m_nSmvGseFocusColIndex(-1)
    , m_eShowReportType(MRDSmvReport)
{
    m_nMenuID = IDR_MENU_MANUAL_RESULT_DETAIL;
}

CManualResultDetailDlg::~CManualResultDetailDlg()
{
}

void CManualResultDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualResultDetailDlg, CBaseDlg)
    ON_COMMAND_RANGE(ID_MENU_MANUAL_DETAIL_F1,   ID_MENU_MANUAL_DETAIL_F6, &CManualResultDetailDlg::OnNaviMenuResultDetail)
    ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_MANUAL_DETAIL_F1,   ID_MENU_MANUAL_DETAIL_F6, &CManualResultDetailDlg::OnUpdateNaviMenuResultDetail)
    ON_COMMAND_RANGE(ID_MENU_MANUAL_DETAIL_CB_MIN,   ID_MENU_MANUAL_DETAIL_CB_MAX, &CManualResultDetailDlg::OnNaviMenuDetailCbSel)

END_MESSAGE_MAP()


// CManualResultDetailDlg ��Ϣ�������

BOOL CManualResultDetailDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    CPageData* pData = CEventProc::GetInstance()->GetPreviousPageData();

    if(!pData || CManualResultListDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CManualResultListDataExtra(pData);
    }

    CManualResultListDataExtra* pDataExtra = dynamic_cast<CManualResultListDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return FALSE;
    }

    m_pStateTestResult = pDataExtra->m_pStateTestResult;
    if (m_pStateTestResult == NULL)
    {
        return FALSE;
    }

    m_pDataLayer = CDsmDataLayer::getInstance();
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }
    
    if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
    {
        m_eShowReportType = MRDSmvReport;
        _showSmvReport();
    }
    else if(m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
    {

        m_eShowReportType = MRDGooseReport;
        CGooseCb *pGooseCb = m_pStateTestResult->FirstGooseSendCb();
        m_pGooseCb = pGooseCb;

        int i = 0; 
        while(pGooseCb)
        {
            m_gooseCbMap.insert(std::pair<UINT, CGooseCb*>(ID_MENU_MANUAL_DETAIL_CB_MIN + i, pGooseCb));
            ++i;
            pGooseCb = m_pStateTestResult->NextGooseSendCb(pGooseCb);
        }

        _showGooseReport();
    }
    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


///
/// @brief
///      ����Smv����
/// 
/// @return
///      void    
///
void CManualResultDetailDlg::_createSmvReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_DETAIL_SMV_RET);

    // ��������
    m_ctlSmvReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlSmvReport.PostInitTemplateStyle();

    // ������Ԫ��

    m_ctlSmvReport.InsertColumn(DSM_MANUAL_RD_SMV_COL_INDEX_CHAN, L"  " + dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN), EL_CENTER, DSM_MANUAL_RD_SMV_WIDTH_CHAN);
    m_ctlSmvReport.InsertColumn(DSM_MANUAL_RD_SMV_COL_INDEX_AMPL, dsm_stringMgr::Get(DSM_STR_MANUAL_AMPL), EL_LEFT, DSM_MANUAL_RD_SMV_WIDTH_AMPL);
    m_ctlSmvReport.InsertColumn(DSM_MANUAL_RD_SMV_COL_INDEX_PHASE, dsm_stringMgr::Get(DSM_STR_MANUAL_PHASE), EL_LEFT, DSM_MANUAL_RD_SMV_WIDTH_PHASE);
    m_ctlSmvReport.InsertColumn(DSM_MANUAL_RD_SMV_COL_INDEX_FREQUENCE, dsm_stringMgr::Get(DSM_STR_MANUAL_FREQUENCE), EL_LEFT, DSM_MANUAL_RD_SMV_WIDTH_FREQUENCE);


    // ����ѡ�񽹵���ʽ
    m_ctlSmvReport.SetRowFocusEnable(TRUE);
    m_ctlSmvReport.SetRowSelectionVisible(FALSE);
    m_ctlSmvReport.SetSubItemFocusEnable(TRUE);
    m_ctlSmvReport.SetFocusedColIndex(1);
    m_ctlSmvReport.SetFreezeColumnsCount(DSM_MANUAL_RD_SMV_FREEZE_COL_COUNT);
    m_ctlSmvReport.SetFocus();

    if (m_ctlSmvReport.GetRecordRowCount() > 0)
    {
        m_ctlSmvReport.SetFocusedSelectionRowIndex(0);
    }

    _updateSmvReport();
}
///
/// @brief
///      ��ʾSmv����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_showSmvReport()
{
    if (m_ctlSmvReport.GetSafeHwnd())
    {
        _updateSmvReport();
    }
    else
    {
        _createSmvReport();
    }
    m_ctlSmvReport.ShowWindow(SW_SHOW);
    m_ctlSmvReport.SetFocus();

    if (m_ctlGooseReport.GetSafeHwnd())
    {
        m_ctlGooseReport.ShowWindow(SW_HIDE);
    }
    if (m_ctlDIActionReport.GetSafeHwnd())
    {
        m_ctlDIActionReport.ShowWindow(SW_HIDE);
    }

    m_eShowReportType = MRDSmvReport;

    _resetMenu();

    CString csCaption;
    csCaption.Format(L"%s->%s%d: %s",
        dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
        dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
        m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
        dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CAPTION));

    CStatusMgr::SetTitle(csCaption);
}

///
/// @brief
///      ����Smv����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_updateSmvReport()
{

    //���α�λΪsmv��ĳ����仯
    ASSERT(m_pStateTestResult && m_pStateTestResult->GetTestType() == TEST_TYPE_SMV);
    
    //������еļ�¼��
    m_ctlSmvReport.DeleteAllRecordRow();

    //�ֶ�����������ù�����
    int nRowIndex = 0;
    
    //
    //smv��λǰ�Ĳ��Խ����
    //
    CStateTestResult* pPreSmvStateTestResult = m_pDataLayer->PrevManualTestSMVResult(m_pStateTestResult);

    //ͨ����λǰ��ͨ��,ͬ�����±�λǰͨ�����Ա�����λ��ĶԱ�
    CSMVSendVirtualChannel* pPreVirChannel = NULL;
    if (pPreSmvStateTestResult)
    {
        pPreVirChannel = pPreSmvStateTestResult->FirstSMVSendVirtualChannel();
    }
    //��ǰͨ����Ϣ
    CSMVSendVirtualChannel* pVirChannel = m_pStateTestResult->FirstSMVSendVirtualChannel();
    
    while(pVirChannel)
    {
        if (m_bShowAllData || 
            pVirChannel->IsAmplitudeChanged() ||
            pVirChannel->IsPhaseAngleChanged() ||
            pVirChannel->IsFrequencyChanged())
        {
            m_ctlSmvReport.InsertRecordRow(nRowIndex, new CDsmManualResultDetailSmvRecord(pVirChannel, pPreVirChannel));
            ++nRowIndex;
        }

        pVirChannel = m_pStateTestResult->NextSMVSendVirtualChannel(pVirChannel);
        if(pPreSmvStateTestResult)
        {
            pPreVirChannel = pPreSmvStateTestResult->NextSMVSendVirtualChannel(pPreVirChannel);
        }
    }
}

///
/// @brief
///      ����Goose���ñ���
/// 
/// @return
///      void    
///
void CManualResultDetailDlg::_createGooseReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlGooseReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_DETAIL_GOOSE_RET);

    // ��������
    m_ctlGooseReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlGooseReport.PostInitTemplateStyle();

    // ������Ԫ��

    m_ctlGooseReport.InsertColumn(0, _T(""), EL_CENTER, DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_INDEX);
    m_ctlGooseReport.InsertColumn(1, L"            " + dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_DESC), EL_CENTER, DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_DESC);
    m_ctlGooseReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_TYPE), EL_LEFT, DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_TYPE);
    m_ctlGooseReport.InsertColumn(3, L"          " + dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_VALUE), EL_CENTER, DSM_MANUAL_RD_GOOSE_WIDTH_CHAN_VALUE);


    // ����ѡ�񽹵���ʽ
    m_ctlGooseReport.SetRowFocusEnable(TRUE);
    m_ctlGooseReport.SetRowSelectionVisible(FALSE);
    m_ctlGooseReport.SetSubItemFocusEnable(TRUE);
    m_ctlGooseReport.SetFreezeColumnsCount(DSM_MANUAL_RD_GOOSE_FREEZE_COL_COUNT);
    m_ctlGooseReport.SetFocusedColIndex(3);
    m_ctlGooseReport.SetFocus();

    _updateGooseReport();
}

///
/// @brief
///      ��ʾgoose����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_showGooseReport()
{
    if (m_ctlGooseReport.GetSafeHwnd())
    {
        _updateGooseReport();
    }
    else
    {
        _createGooseReport();
    }
    m_ctlGooseReport.ShowWindow(SW_SHOW);
    m_ctlGooseReport.SetFocus();

    if (m_ctlSmvReport.GetSafeHwnd())
    {
        m_ctlSmvReport.ShowWindow(SW_HIDE);
    }
    if (m_ctlDIActionReport.GetSafeHwnd())
    {
        m_ctlDIActionReport.ShowWindow(SW_HIDE);
    }

    m_eShowReportType = MRDGooseReport;

    _resetMenu();
    _highlightGooseF2();

    CString csCaption = _T("");
    if (m_pGooseCb)
    {
        csCaption.Format(L"%s->%s%d: %s-0x%04X",
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
            m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
            dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION),
            m_pGooseCb->GetAPPID());
    }
    else
    {
        csCaption.Format(L"%s->%s%d: %s", 
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
            m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
            dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION));
    }
    CStatusMgr::SetTitle(csCaption);
}

///
/// @brief
///      ����Goose����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_updateGooseReport()
{
    //
    //����goose������Ϣ����ʾ��ǰ�������ָ��goose���ƿ��ͨ����Ϣ
    //
    ASSERT(m_pStateTestResult && m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE);
    
    if (m_pGooseCb == NULL)
    {
        return ;
    }

    if (m_ctlGooseReport.GetSafeHwnd() == NULL)
    {
        return;
    }

    m_ctlGooseReport.DeleteAllRecordRow();

    int       nRowIndex = 0;
    int       nChanIndex = 1;
    int       nSubChanIndex = 1;
    CChannel* pChannel = NULL;
    CChannel* pSubChannel = NULL;

    int       nGooseCbIndex = 0;
    CGooseCb* pPreGooseCb = NULL;
    CChannel* pPreChannel = NULL;
    CChannel* pPreSubChannel = NULL;

    //
    //�ҵ���ǰgoose���ƿ��Ӧ����һ״̬�������goose���ƿ�
    //
    CStateTestResult* pPreGooseStateTestResult = m_pDataLayer->PrevManualTestGooseResult(m_pStateTestResult);
    nGooseCbIndex = m_pStateTestResult->GetIndexByGooseCb(m_pGooseCb);
    if (nGooseCbIndex >= 0 && pPreGooseStateTestResult)
    {
        pPreGooseCb = pPreGooseStateTestResult->GetGooseCbByIndex(nGooseCbIndex);
    }
    
    //
    //�ҵ�ͨ����Ӧ����һ״̬������е�ͨ��
    //
    if (m_pGooseCb->IsSendValueChanged() && pPreGooseCb)
    {
        pPreChannel = pPreGooseCb->FirstChannel();
    }
    pChannel = m_pGooseCb->FirstChannel();
    while(pChannel)
    {
        //
        //ͨ��,��ͨ���ź���ͨ���ŷֱ����õ�0��1�� 
        //
        if (m_bShowAllData || (!m_bShowAllData && pChannel->IsSendValueChanged()))
        {
            m_ctlGooseReport.InsertRecordRow(nRowIndex, new CDsmManualResultDetailGooseRecord(pChannel, nChanIndex, 0, pPreChannel));
            m_ctlGooseReport.SetRecordRowData(nRowIndex, (DWORD_PTR)pChannel);
            ++nRowIndex;
        }

        if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
        {
            //
            //��ͨ��
            //
            nSubChanIndex = 1;
            //
            //�ҵ���ͨ������һ״̬������ж�Ӧͨ������ͨ��
            //
            if (m_pGooseCb->IsSendValueChanged() && pPreGooseCb)
            {
                pPreSubChannel = pPreChannel->FirstSubChannel();
            }
            pSubChannel = pChannel->FirstSubChannel();
            while(pSubChannel)
            {
                if (m_bShowAllData || (!m_bShowAllData && pSubChannel->IsSendValueChanged()))
                { 
                    m_ctlGooseReport.InsertRecordRow(nRowIndex, new CDsmManualResultDetailGooseRecord(pSubChannel, nChanIndex, nSubChanIndex, pPreSubChannel));
                    m_ctlGooseReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);

                    ++nRowIndex;
                }

                ++nSubChanIndex;
                pSubChannel = pChannel->NextSubChannel(pSubChannel);
                if (m_pGooseCb->IsSendValueChanged() && pPreGooseCb)
                {
                    pPreSubChannel = pPreChannel->NextSubChannel(pPreSubChannel);
                }
            }
        }

        ++nChanIndex;
        pChannel = m_pGooseCb->NextChannel(pChannel);
        if (m_pGooseCb->IsSendValueChanged() && pPreGooseCb)
        {
            pPreChannel = pPreGooseCb->NextChannel(pPreChannel);
        }
    }
    if (m_ctlGooseReport.GetRecordRowCount() > 0)
    {
        m_ctlGooseReport.SetFocusedSelectionRowIndex(0);
    }
}



void CManualResultDetailDlg::_highlightGooseF2( BOOL bHighLight/* = TRUE*/)
{

    if(m_pStateTestResult && m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
    {
        CUINTGooseCbMap::iterator iter;
        for (iter = m_gooseCbMap.begin(); iter != m_gooseCbMap.end(); ++iter)
        {
            if (m_pGooseCb == iter->second)
            {
                CNaviBarMgr::GetInstance()->HighlightMenuItem(1, iter->first - ID_MENU_MANUAL_DETAIL_CB_MIN, bHighLight);
                break;
            }
        }
    }
    ;
}

void CManualResultDetailDlg::_createDIActionReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlDIActionReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_RESULT_DI_ACTION_REP);

    // ��������
    m_ctlDIActionReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlDIActionReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlDIActionReport.PostInitTemplateStyle();
    // ������Ԫ��

    m_ctlDIActionReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_MANUAL_DI), EL_CENTER, DSM_MANUAL_RD_DI_ACTION_WIDTH_DI);
    m_ctlDIActionReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION_COUNT), EL_LEFT, DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION_COUNT);
    m_ctlDIActionReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION1), EL_CENTER, DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION1);
    m_ctlDIActionReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION2), EL_CENTER, DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION2);
    m_ctlDIActionReport.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION3), EL_CENTER, DSM_MANUAL_RD_DI_ACTION_WIDTH_ACTION3);
    
    // ����ѡ�񽹵���ʽ
    m_ctlDIActionReport.SetRowFocusEnable(TRUE);
    m_ctlDIActionReport.SetRowSelectionVisible(TRUE);
    m_ctlDIActionReport.SetFocus();
    _updateDIActionReport();
}

///
/// @brief
///      ��ʾDIAction����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_showDIActionReport()
{
    if (m_ctlDIActionReport.GetSafeHwnd())
    {
        _updateDIActionReport();
    }
    else
    {
        _createDIActionReport();
    }
    m_ctlDIActionReport.ShowWindow(SW_SHOW);
    m_ctlDIActionReport.SetFocus();

    if (m_ctlSmvReport.GetSafeHwnd())
    {
        m_ctlSmvReport.ShowWindow(SW_HIDE);
    }
    if (m_ctlGooseReport.GetSafeHwnd())
    {
        m_ctlGooseReport.ShowWindow(SW_HIDE);
    }

    m_eShowReportType = MRDDIActionReport;
    
    _resetMenu();
    m_pNaviBar->UpdateNaviMenu();
}

///
/// @brief
///      ����DIAction����
///
/// @return
///      void    
///
void CManualResultDetailDlg::_updateDIActionReport()
{
    m_ctlDIActionReport.DeleteAllRecordRow();

    if (m_pStateTestResult)
    {
        CChannelRecvValues* pChannelRecvValues = NULL;
        CString strDIName = _T("");
        for (int i = 0 ;  i < 8; ++i)
        {
            strDIName.Format(_T("DI%d"), i + 1);
            pChannelRecvValues = m_pStateTestResult->GetChannelRecvValues((LPCTSTR)strDIName);
            m_ctlDIActionReport.InsertRecordRow(i, new CDsmManualResultDIActionRecord(pChannelRecvValues));
        }
        m_ctlDIActionReport.SetFocusedSelectionRowIndex(0);
    }

    CString csCaption = _T("");
    csCaption.Format(L"%s->%s%d: %s", 
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
            dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
            m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
            dsm_stringMgr::Get(DSM_STR_MANUAL_DI_CHANGE));
    CStatusMgr::SetTitle(csCaption);
}

void CManualResultDetailDlg::_loadNaviMenu()
{
    ASSERT(-1 != m_nMenuID);

    // ��ӵײ��˵�
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);
    m_menu.DestroyMenu();

    // �����麯���޸Ĳ˵�
    _modifyMenu();

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();

    if(m_pNaviBar)
    {
        // ���ò˵�Ŀ���������ݶ���
        m_pNaviBar->SetCommandTarget(this);
        m_pNaviBar->SetNaviMenu(&m_menuMain, TRUE);
    }
}
void CManualResultDetailDlg::_resetMenu()
{
    //
    //���ò˵�������SMV��GOOSE֮���л�ʱ�����Ը��Ĳ˵�
    //

    //F1
    if (m_eShowReportType != MRDDIActionReport)
    {
        if (m_bShowAllData)
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_ALLCHANGE_ALL), NULL, NULL);
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_ALLCHANGE_CHANGE), NULL, NULL);
        }
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(0, _T(""));
    }
    else
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, NULL);
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(0, _T(""));
    }

    //F2
    if (m_eShowReportType != MRDGooseReport)
    {
        //
        //���ÿ��ƿ�ѡ��
        //
        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, NULL);
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_EMPTY));
		m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, NULL, NULL, NULL);
    }
    else
    {  
        //
        //���ÿ��ƿ�ѡ��
        //
        m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, NULL);
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(1, dsm_stringMgr::Get(DSM_STR_MANUAL_CTRL_BLOCK));
		m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
        if (m_gooseCbMap.size() > 0)
        {
            //
            //���¼��ؿ��ƿ�
            //
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
            CNaviMenu* pSubMenu  = new CNaviMenu;
            pSubMenu->LoadMenu(&menu);
            menu.DestroyMenu();
            m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, pSubMenu);
        }
        _highlightGooseF2();
    }

    //
    //F4
    //
    
    if (m_eShowReportType != MRDDIActionReport)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_DI_CHANGE));
    }
    else
    {
        if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_SMV));
        }
        else
        {
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_GSE));
        }
    }

    m_pNaviBar->UpdateNaviMenu();
}

void CManualResultDetailDlg::_SavePage()
{
    _updateTips(TRUE, _T(""));
    __super::_SavePage();
}

void CManualResultDetailDlg::_updateTips(BOOL bAssign, CString strTips)
{
    if(bAssign)
    {   
        //
        //��ʾָ����tips
        //
        if(strTips.GetLength() == 0)
        {
            CStatusMgr::SetTips(strTips, 500);
        }
        else
        {
            CStatusMgr::SetTips(strTips, DSM_SMV_GSE_TIP_SHOW_TIME);
        }
    }
    else
    {
        //
        //������ݵ�ǰ���ѡ�е�Item��ʾ
        //
        int nRowIndex = -1;
        int nColIndex = -1;
        if (::IsWindowVisible(m_ctlSmvReport))
        {
            nRowIndex = m_ctlSmvReport.GetFocusedRowIndex();
            nColIndex = m_ctlSmvReport.GetFocusedColIndex();
            if (nRowIndex >= 0 && nColIndex >= 0)
            {
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(nRowIndex)->GetItem(nColIndex));
                if (pItem)
                {
                    CStatusMgr::SetTips(pItem->GetValue(), DSM_SMV_GSE_TIP_SHOW_TIME);
                }
            }
            else
            {
                CStatusMgr::SetTips(_T(""), 500);
            }
        }
        else if (::IsWindowVisible(m_ctlGooseReport))
        {       
            nRowIndex = m_ctlGooseReport.GetFocusedRowIndex();
            nColIndex = m_ctlGooseReport.GetFocusedColIndex();
            if (nRowIndex >= 0 && nColIndex >= 0)
            {
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlGooseReport.GetRecordRow(nRowIndex)->GetItem(nColIndex));
                if (pItem)
                {
                    CStatusMgr::SetTips(pItem->GetValue(), DSM_SMV_GSE_TIP_SHOW_TIME);
                }
            }
            else
            {
                CStatusMgr::SetTips(_T(""), 500);
            }
        }
        else
        {
            CStatusMgr::SetTips(_T(""), 500);
        }
    }
}
void CManualResultDetailDlg::OnNaviMenuResultDetail( UINT nID )
{
    if (m_pStateTestResult == NULL)
    {
        return;
    }
    
    switch (nID)
    {
    case ID_MENU_MANUAL_DETAIL_F1:
        {
            //
            //��ʾ ȫ��|�仯��
            //
            if (::IsWindowVisible(m_ctlGooseReport.GetSafeHwnd()) ||
                ::IsWindowVisible(m_ctlSmvReport.GetSafeHwnd()))
            {
                //
                //�ж��Ƿ���ȫ���ͱ仯��֮����л�
                //
                m_bShowAllData = !m_bShowAllData;

                if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                {
                    _showSmvReport();
                }
                else if(m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
                {
                    _showGooseReport();
                }

                _updateTips();
            }
        }
        break;
    case ID_MENU_MANUAL_DETAIL_F2:
        {
            //
            //���ƿ�
            //
        }
        break;
    //case ID_MENU_MANUAL_DETAIL_F3:
    //    {

    //    }
    //    break;
    case ID_MENU_MANUAL_DETAIL_F4:
        {
            //
            // ��ʾDI�仯
            //
            if (m_eShowReportType == MRDDIActionReport)
            {            
                if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                {
                    _showSmvReport();
                }
                else if(m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
                {
                    _showGooseReport();
                }
            }
            else
            {
                _showDIActionReport();
            }
            _updateTips();
        }
        break;
    case ID_MENU_MANUAL_DETAIL_F5:
        {
            //
            //��һ״̬
            //
            CStateTestResult* pStateTestResult = m_pDataLayer->PrevManualTestResult(m_pStateTestResult);
            if (pStateTestResult != NULL)
            {
                //����ǵ�һ��״̬�������������Ϊ��һ��״̬
                m_pStateTestResult = pStateTestResult;
                if (m_eShowReportType != MRDDIActionReport)
                {
                    if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                    {
                        _showSmvReport();
                    }
                    else if(m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
                    {
                        //
                        //��գ�����Ϊ��״̬�Ŀ��ƿ�
                        //
                        m_gooseCbMap.clear();
                        CGooseCb *pGooseCb = m_pStateTestResult->FirstGooseSendCb();
                        m_pGooseCb = pGooseCb;

                        int i = 0; 
                        while(pGooseCb)
                        {
                            m_gooseCbMap.insert(std::pair<UINT, CGooseCb*>(ID_MENU_MANUAL_DETAIL_CB_MIN + i, pGooseCb));
                            ++i;
                            pGooseCb = m_pStateTestResult->NextGooseSendCb(pGooseCb);
                        }
                        _showGooseReport();
                    }
                }
                else
                {
                    //��ʾDI��λ
                    _updateDIActionReport();

                    if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_SMV));
                    }
                    else
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_GSE));
                    }
                    m_pNaviBar->UpdateNaviMenu();
                }
            }
            _updateTips();
        }
        break;
    case ID_MENU_MANUAL_DETAIL_F6:
        {
            //
            //��һ״̬
            //
            CStateTestResult* pStateTestResult = m_pDataLayer->NextManualTestResult(m_pStateTestResult);
            if (pStateTestResult != NULL)
            {
                //��������һ��״̬�������������Ϊ��һ��״̬
                m_pStateTestResult = pStateTestResult;
                if (m_eShowReportType != MRDDIActionReport)
                {
                    if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                    {
                        _showSmvReport();
                    }
                    else if(m_pStateTestResult->GetTestType() == TEST_TYPE_GOOSE)
                    {
                        //
                        //��գ�����Ϊ��״̬�Ŀ��ƿ�
                        //
                        m_gooseCbMap.clear();
                        CGooseCb *pGooseCb = m_pStateTestResult->FirstGooseSendCb();
                        m_pGooseCb = pGooseCb;

                        int i = 0; 
                        while(pGooseCb)
                        {
                            m_gooseCbMap.insert(std::pair<UINT, CGooseCb*>(ID_MENU_MANUAL_DETAIL_CB_MIN + i, pGooseCb));
                            ++i;
                            pGooseCb = m_pStateTestResult->NextGooseSendCb(pGooseCb);
                        }

                        _showGooseReport();
                        _highlightGooseF2();
                    }
                }
                else
                {
                    //��ʾDI��λ
                    _updateDIActionReport();

                    if (m_pStateTestResult->GetTestType() == TEST_TYPE_SMV)
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_SMV));
                    }
                    else
                    {
                        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(3, dsm_stringMgr::Get(DSM_STR_MANUAL_F1_GSE));
                    }
                    m_pNaviBar->UpdateNaviMenu();

                }
            }
            _updateTips();
        }
        break;
    default:
        break;
    }
}

void CManualResultDetailDlg::OnUpdateNaviMenuResultDetail( CCmdUI* pCmdUI )
{
    if (m_pStateTestResult == NULL)
    {
        return;
    }

    switch(pCmdUI->m_nID)
    {
    case ID_MENU_MANUAL_DETAIL_F5:
        {
            CStateTestResult* pStateTestResult = m_pDataLayer->PrevManualTestResult(m_pStateTestResult);
            if (pStateTestResult != NULL)
            {
                pCmdUI->Enable(TRUE);
            }
            else
            {
                pCmdUI->Enable(FALSE);
            }
        }
        break;
    case ID_MENU_MANUAL_DETAIL_F6:
        {
            CStateTestResult* pStateTestResult = m_pDataLayer->NextManualTestResult(m_pStateTestResult);
            if (pStateTestResult != NULL)
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
void CManualResultDetailDlg::OnNaviMenuDetailCbSel( UINT nID )
{
    if (m_pStateTestResult == NULL)
    {
        return;
    }
    _highlightGooseF2(FALSE);
    CUINTGooseCbMap::iterator iter = m_gooseCbMap.find(nID);
    if (iter != m_gooseCbMap.end())
    {
        CGooseCb* pOldGooseCb = NULL;

        m_pGooseCb = iter->second;

        if (m_pGooseCb != NULL && m_pGooseCb != pOldGooseCb)
        {
            _updateGooseReport();

            CString csCaption = _T("");
            if (m_pGooseCb)
            {
                csCaption.Format(L"%s->%s%d: %s-0x%04X",
                    dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
                    dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
                    m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
                    dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION),
                    m_pGooseCb->GetAPPID());
            }
            else
            {
                csCaption.Format(L"%s->%s%d: %s", 
                    dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_DETAIL_CAPTION),
                    dsm_stringMgr::Get(DSM_STR_MANUAL_RESULT_CHANGE),
                    m_pDataLayer->GetManualTestResultIndex(m_pStateTestResult) + 1,
                    dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_CAPTION));
            }
            CStatusMgr::SetTitle(csCaption);
        }
    }
    _updateTips();
    _highlightGooseF2();
}
BOOL CManualResultDetailDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
            {
                if (::IsWindowVisible(m_ctlSmvReport))
                {
                    //
                    //SMV����
                    //
                    int nRowIndex = m_ctlSmvReport.GetFocusedRowIndex();
                    int nColIndex = m_ctlSmvReport.GetFocusedColIndex();
                    if (nRowIndex < 0)
                    {
                        return TRUE;
                    }
                    switch(pMsg->wParam)
                    {
                    case VK_LEFT:
                        {
                            if (nColIndex > DSM_MANUAL_RD_SMV_FREEZE_COL_COUNT)
                            {
                                --nColIndex;
                            } 
                        }
                        break;
                    case VK_RIGHT:
                        {
                            if (nColIndex < DSM_MANUAL_RD_SMV_COL_COUNT - 1)
                            {
                                ++nColIndex;
                            }
                        }
                        break;
                    case VK_UP:
                        {
                            if (nRowIndex == 0)
                            {
                                //��0�У��л������һ��
                                nRowIndex = m_ctlSmvReport.GetRecordRowCount()-1;
                            }
                            else if(nRowIndex != -1)
                            {
                                //ѡ����
                                --nRowIndex;
                            }
                        }
                        break;
                    case VK_DOWN:
                        {
                            if (nRowIndex == m_ctlSmvReport.GetRecordRowCount() - 1)
                            {
                                nRowIndex = 0;
                            }
                            else
                            {
                                ++nRowIndex;
                            }
                        }
                        break;
                    }
                    if (nRowIndex >= 0 && nColIndex >= 0)
                    {
                        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(nRowIndex)->GetItem(nColIndex));
                        if (pItem)
                        {
                            _updateTips(TRUE, pItem->GetValue());
                        }
                    }
                }
                else if (::IsWindowVisible(m_ctlGooseReport))
                {
                    //
                    //SMV����
                    //
                    int nRowIndex = m_ctlGooseReport.GetFocusedRowIndex();
                    int nColIndex = m_ctlGooseReport.GetFocusedColIndex();
                    if (nRowIndex < 0)
                    {
                        return TRUE;
                    }
                    switch(pMsg->wParam)
                    {
                    case VK_LEFT:
                        {
                            if (nColIndex > DSM_MANUAL_RD_GOOSE_FREEZE_COL_COUNT)
                            {
                                --nColIndex;
                            } 
                        }
                        break;
                    case VK_RIGHT:
                        {
                            if (nColIndex < DSM_MANUAL_RD_GOOSE_COL_COUNT - 1)
                            {
                                ++nColIndex;
                            }
                        }
                        break;
                    case VK_UP:
                        {
                            if (nRowIndex == 0)
                            {
                                //��0�У��л������һ��
                                nRowIndex = m_ctlGooseReport.GetRecordRowCount()-1;
                            }
                            else if(nRowIndex != -1)
                            {
                                //ѡ����
                                --nRowIndex;
                            }
                        }
                        break;
                    case VK_DOWN:
                        {
                            if (nRowIndex == m_ctlGooseReport.GetRecordRowCount() - 1)
                            {
                                nRowIndex = 0;
                            }
                            else
                            {
                                ++nRowIndex;
                            }
                        }
                        break;
                    }
                    if (nRowIndex >= 0 && nColIndex >= 0)
                    {
                        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlGooseReport.GetRecordRow(nRowIndex)->GetItem(nColIndex));
                        if (pItem)
                        {
                            _updateTips(TRUE, pItem->GetValue());
                        }
                    }
                }
            }
            break;
        case VK_ESCAPE:
            {
                CStatusMgr::SetTips(_T(""), 500);
            }
        }
    }
    return CBaseDlg::PreTranslateMessage(pMsg);
}
