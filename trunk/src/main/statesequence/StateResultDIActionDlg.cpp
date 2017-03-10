/// @file
///
/// @brief
///     状态序列测试结果DI变位列表 实现文件
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
#include "StateResultDIActionDlg.h"
#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/send/StateTestResult.h"

#define DSM_STATE_DI_ACTION_WIDTH_DI            79
#define DSM_STATE_DI_ACTION_WIDTH_ACTION_COUNT  90
#define DSM_STATE_DI_ACTION_WIDTH_ACTION1       120
#define DSM_STATE_DI_ACTION_WIDTH_ACTION2       120
#define DSM_STATE_DI_ACTION_WIDTH_ACTION3       120

// CStateResultDIActionDlg 对话框

IMPLEMENT_DYNAMIC(CStateResultDIActionDlg, CBaseDlg)

CStateResultDIActionDlg::CStateResultDIActionDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateResultDIActionDlg::IDD, DSM_STR_STATE_RESULT_CAPTION, pParent)
    , m_pStateTestResult(NULL)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CStateResultDIActionDlg::~CStateResultDIActionDlg()
{
}

void CStateResultDIActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateResultDIActionDlg, CBaseDlg)
END_MESSAGE_MAP()


BOOL CStateResultDIActionDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CPageData* pData = CEventProc::GetInstance()->GetPreviousPageData();

    if(!pData || CStateResultListDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CStateResultListDataExtra(pData);
    }

    CStateResultListDataExtra* pDataExtra = dynamic_cast<CStateResultListDataExtra*>(pData->m_pPageDataExtra);
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

    _createReport();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

///
/// @brief
///      创建开入量报表
/// 
/// @return
///      void    
///
void CStateResultDIActionDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlDIActionReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_RESULT_DI_ACTION_REP);

    // 设置字体
    m_ctlDIActionReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlDIActionReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlDIActionReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlDIActionReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_MANUAL_DI), EL_CENTER, DSM_STATE_DI_ACTION_WIDTH_DI);
    m_ctlDIActionReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION_COUNT), EL_LEFT, DSM_STATE_DI_ACTION_WIDTH_ACTION_COUNT);
    m_ctlDIActionReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION1), EL_CENTER, DSM_STATE_DI_ACTION_WIDTH_ACTION1);
    m_ctlDIActionReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION2), EL_CENTER, DSM_STATE_DI_ACTION_WIDTH_ACTION2);
    m_ctlDIActionReport.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION3), EL_CENTER, DSM_STATE_DI_ACTION_WIDTH_ACTION3);
        // 设置选择焦点样式
    m_ctlDIActionReport.SetRowSelectionVisible(TRUE);
    m_ctlDIActionReport.SetRowFocusEnable(TRUE);
    m_ctlDIActionReport.SetFocus();

    CChannelRecvValues* pChannelRecvValues= NULL;
    CString strDIName = _T("");
    for (int i = 0 ;  i < 8; ++i)
    {
        strDIName.Format(_T("DI%d"), i + 1);
        pChannelRecvValues = m_pStateTestResult->GetChannelRecvValues((LPCTSTR)strDIName);
        m_ctlDIActionReport.InsertRecordRow(i, new CDsmStateResultDIActionRecord(pChannelRecvValues));
    }
    m_ctlDIActionReport.SetFocusedSelectionRowIndex(0);

}
