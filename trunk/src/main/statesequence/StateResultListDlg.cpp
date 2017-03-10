/// @file
///
/// @brief
///     状态序列测试结果列表 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "StateResultListDlg.h"
#include "src/service/datalayer/DsmDataLayer.h"
//////////////////////////////////////////////////////////////////////////
//状态序列-实验结果 状态切换
//
#define DSM_STATE_RESULT_LIST_WIDTH_STATE      70
#define DSM_STATE_RESULT_LIST_WIDTH_TIME       160
#define DSM_STATE_RESULT_LIST_WIDTH_DI_ACTION  299


// CStateResultListDlg 对话框

IMPLEMENT_DYNAMIC(CStateResultListDlg, CBaseDlg)

CStateResultListDlg::CStateResultListDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateResultListDlg::IDD, DSM_STR_STATE_DI_ACTION_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_STATE_RESULT_LIST;
}

CStateResultListDlg::~CStateResultListDlg()
{
}

void CStateResultListDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateResultListDlg, CBaseDlg)
    ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_RESULT_LIST_REP, &CStateResultListDlg::OnStateOkClick)
    ON_COMMAND_RANGE(ID_MENU_STATE_RESULT_LIST_F1,   ID_MENU_STATE_RESULT_LIST_F6, &CStateResultListDlg::OnNaviMenuStateResultList)

END_MESSAGE_MAP()


// CStateResultListDlg 消息处理程序

BOOL CStateResultListDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    _createReport();
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateResultListDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }

    if(pData->m_eAction == PageBack)
    {
        _restorePage();
    }
    else
    {

        if (m_ctlResultReport.GetRecordRowCount() > 0)
        {
            m_ctlResultReport.SetFocusedSelectionRowIndex(0);
        }
    }
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
void CStateResultListDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlResultReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_RESULT_LIST_REP);

    // 设置字体
    m_ctlResultReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlResultReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlResultReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlResultReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_STATE_RESULT_STATE), EL_CENTER, DSM_STATE_RESULT_LIST_WIDTH_STATE);
    m_ctlResultReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_STATE_RESULT_START_TIME), EL_CENTER, DSM_STATE_RESULT_LIST_WIDTH_TIME);
    m_ctlResultReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_STATE_RESULT_DI_ACTION), EL_CENTER, DSM_STATE_RESULT_LIST_WIDTH_DI_ACTION);
    // 设置选择焦点样式
    m_ctlResultReport.SetRowSelectionVisible(TRUE);
    m_ctlResultReport.SetRowFocusEnable(TRUE);
    m_ctlResultReport.SetFreezeSelectionVisible(TRUE);
    m_ctlResultReport.SetFocus();


    CDsmDataLayer* pDataLayer = CDsmDataLayer::getInstance();
    if (pDataLayer == NULL)
    {
        return;
    }
    
    int nResultCount = pDataLayer->GetStateSequenceResultCount();
    CStateTestResult* pTestResult = NULL;

    for (int i = 0; i < nResultCount; ++i)
    {
        pTestResult = pDataLayer->GetStateSequenceResult(i);
        if (pTestResult && pTestResult->IsTested())
        {
            m_ctlResultReport.InsertRecordRow(i, new CDsmStateResultListRecord(pTestResult, i +1));
            m_ctlResultReport.SetRecordRowData(i , (DWORD_PTR)pTestResult);
        }
        else
        {
            break;
        }
    }
    

}

void CStateResultListDlg::OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    UNREFERENCED_PARAMETER(pResult);
    
    if (m_ctlResultReport.GetFocusedRowIndex() >= 0)
    {
        CEventProc::GetInstance()->ShowPage(IDD_STATE_RESULT_DI_ACTION_DLG);
    }

    *pResult = TRUE;
}
void CStateResultListDlg::OnNaviMenuStateResultList(UINT nID)
{
    switch(nID)
    {
#if SHENG_ZE
    case ID_MENU_STATE_RESULT_LIST_F6:
#else
	case ID_MENU_STATE_RESULT_LIST_F1:
#endif
        {
            if (m_ctlResultReport.GetFocusedRowIndex() >= 0)
            {
                CEventProc::GetInstance()->ShowPage(IDD_STATE_RESULT_DI_ACTION_DLG);
            }
        }
        break;
    }
}
void CStateResultListDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateResultListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CStateResultListDataExtra(pData);
    }

    CStateResultListDataExtra* pDataExtra = dynamic_cast<CStateResultListDataExtra*>(pData->m_pPageDataExtra);
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

void CStateResultListDlg::_restorePage()
{

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateResultListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CStateResultListDataExtra* pDataExtra = dynamic_cast<CStateResultListDataExtra*>(pData->m_pPageDataExtra);
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
}