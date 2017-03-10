//
/// @file
///
/// @brief
///     试验结果列表 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "ManualResultListDlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/service/datalayer/DsmDataLayer.h"

//////////////////////////////////////////////////////////////////////////
// 手动试验- 试验结果 变化列表
//
#define DSM_MANUAL_RES_LIST_WIDTH_INDEX        50
#if SHENG_ZE
#define DSM_MANUAL_RES_LIST_WIDTH_TIME         180
#else
#define DSM_MANUAL_RES_LIST_WIDTH_TIME         179
#endif 
#define DSM_MANUAL_RES_LIST_WIDTH_ACTION_ITEM  150
#define DSM_MANUAL_RES_LIST_WIDTH_ACTION       150


// CManualResultListDlg 对话框

IMPLEMENT_DYNAMIC(CManualResultListDlg, CBaseDlg)

CManualResultListDlg::CManualResultListDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualResultListDlg::IDD, DSM_STR_MANUAL_TEST_RESULT_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_MANUAL_RESULT_LIST;
}

CManualResultListDlg::~CManualResultListDlg()
{
}

void CManualResultListDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualResultListDlg, CBaseDlg)
    ON_NOTIFY(ELNM_OKCLICK, IDC_MANUAL_RESULT_LIST_RET, &CManualResultListDlg::OnStateOkClick)
    ON_COMMAND_RANGE(ID_MENU_MANUAL_RESULT_LIST_F1,   ID_MENU_MANUAL_RESULT_LIST_F6, &CManualResultListDlg::OnNaviMenuManualResultList)

END_MESSAGE_MAP()


// CManualResultListDlg 消息处理程序

BOOL CManualResultListDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化



    _createReport();
    _restorePage();
	

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


///
/// @brief
///      创建结果报表
/// 
/// @return
///      void    
///
void CManualResultListDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlResultReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_RESULT_LIST_RET);

    // 设置字体
    m_ctlResultReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlResultReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlResultReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlResultReport.InsertColumn(0, _T(""), EL_CENTER, DSM_MANUAL_RES_LIST_WIDTH_INDEX);
    m_ctlResultReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION_TIME), EL_LEFT, DSM_MANUAL_RES_LIST_WIDTH_TIME);
    m_ctlResultReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION_ITEM), EL_LEFT, DSM_MANUAL_RES_LIST_WIDTH_ACTION_ITEM);
    m_ctlResultReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_MANUAL_DI_ACTION), EL_LEFT, DSM_MANUAL_RES_LIST_WIDTH_ACTION);

    CDsmDataLayer* pDataLayer = CDsmDataLayer::getInstance();
    if (pDataLayer == NULL)
    {
        return;
    }

    int nResultCount = pDataLayer->GetManualTestResultCount();
    CStateTestResult* pTestResult = NULL;

    for (int i = 0; i < nResultCount; ++i)
    {
        pTestResult = pDataLayer->GetManualTestResult(i);
        m_ctlResultReport.InsertRecordRow(i, new CDsmManualResultListRecord(pTestResult, i +1));
        m_ctlResultReport.SetRecordRowData(i , (DWORD_PTR)pTestResult);
    }

    // 设置选择焦点样式
    m_ctlResultReport.SetRowSelectionVisible(TRUE);
    m_ctlResultReport.SetRowFocusEnable(TRUE);
    m_ctlResultReport.SetSubItemFocusEnable(FALSE);
    //m_ctlResultReport.SetFreezeColumnsCount(1);
    //m_ctlResultReport.SetFreezeSelectionVisible(TRUE);
    //m_ctlResultReport.SetFocusedColIndex(1);
    m_ctlResultReport.SetFocus();
}

void CManualResultListDlg::OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    UNREFERENCED_PARAMETER(pResult);
    if (m_ctlResultReport.GetFocusedRowIndex() >= 0)
    {
        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_DETAIL_DLG);
    }

    *pResult = TRUE;
}


void CManualResultListDlg::OnNaviMenuManualResultList(UINT nID)
{
    switch(nID)
    {
#if SHENG_ZE
    case ID_MENU_MANUAL_RESULT_LIST_F6:
#else
	case ID_MENU_MANUAL_RESULT_LIST_F1:
#endif
        {
            if (m_ctlResultReport.GetFocusedRowIndex() >= 0)
            {
                CEventProc::GetInstance()->ShowPage(IDD_MANUAL_RESULT_DETAIL_DLG);
            }
        }
        break;
    }
}
void CManualResultListDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualResultListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CManualResultListDataExtra(pData);
    }

    CManualResultListDataExtra* pDataExtra = dynamic_cast<CManualResultListDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }

    pDataExtra->m_nFocusRow = m_ctlResultReport.GetFocusedRowIndex();

    if (pDataExtra->m_nFocusRow >= 0)
    {
        pDataExtra->m_pStateTestResult = (CStateTestResult*)m_ctlResultReport.GetRecordRowData(pDataExtra->m_nFocusRow);
    }
    else
    {
        pDataExtra->m_pStateTestResult = NULL;
    }
}

void CManualResultListDlg::_restorePage()
{

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualResultListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CManualResultListDataExtra* pDataExtra = dynamic_cast<CManualResultListDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        if (m_ctlResultReport.GetRecordRowCount() > pDataExtra->m_nFocusRow && pDataExtra->m_nFocusRow >= 0)
        {
            m_ctlResultReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRow);
            m_ctlResultReport.EnsureVisible(pDataExtra->m_nFocusRow, FALSE);
        }
        m_ctlResultReport.SetFocus();
    }
    else
    {
        if (m_ctlResultReport.GetRecordRowCount() > 0)
        {
            m_ctlResultReport.SetFocusedSelectionRowIndex(0);
        }
    }
}