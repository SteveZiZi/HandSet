/// @file
///
/// @brief
///     �ֶ�����DI�仯 ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "ManualResultDIActionDlg.h"
#include "ManualResultListDlg.h"
#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/send/StateTestResult.h"

// CManualResultDIActionDlg �Ի���

IMPLEMENT_DYNAMIC(CManualResultDIActionDlg, CBaseDlg)

CManualResultDIActionDlg::CManualResultDIActionDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualResultDIActionDlg::IDD, DSM_STR_MANUAL_TEST_RESULT_CAPTION, pParent)
    , m_pStateTestResult(NULL)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CManualResultDIActionDlg::~CManualResultDIActionDlg()
{
}

void CManualResultDIActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualResultDIActionDlg, CBaseDlg)
END_MESSAGE_MAP()


// CManualResultDIActionDlg ��Ϣ�������

BOOL CManualResultDIActionDlg::OnInitDialog()
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

    _createReport();

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

///
/// @brief
///      ��������������
/// 
/// @return
///      void    
///
void CManualResultDIActionDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlDIActionReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_RESULT_DI_ACTION_RET);

    // ��������
    m_ctlDIActionReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlDIActionReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlDIActionReport.PostInitTemplateStyle();
    // ������Ԫ��

    m_ctlDIActionReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_MANUAL_DI), EL_CENTER, DSM_MANUAL_DI_ACTION_WIDTH_DI);
    m_ctlDIActionReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION_COUNT), EL_LEFT, DSM_MANUAL_DI_ACTION_WIDTH_ACTION_COUNT);
    m_ctlDIActionReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION1), EL_CENTER, DSM_MANUAL_DI_ACTION_WIDTH_ACTION1);
    m_ctlDIActionReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION2), EL_CENTER, DSM_MANUAL_DI_ACTION_WIDTH_ACTION2);
    m_ctlDIActionReport.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_MANUAL_ACTION3), EL_CENTER, DSM_MANUAL_DI_ACTION_WIDTH_ACTION3);
   
    CChannelRecvValues* pChannelRecvValues = NULL;
    CString strDIName = _T("");
    for (int i = 0 ;  i < 8; ++i)
    {
        strDIName.Format(_T("DI%d"), i + 1);
        pChannelRecvValues = m_pStateTestResult->GetChannelRecvValues((LPCTSTR)strDIName);
        m_ctlDIActionReport.InsertRecordRow(i, new CDsmManualResultDIActionRecord(pChannelRecvValues));
    }


    // ����ѡ�񽹵���ʽ
    m_ctlDIActionReport.SetRowSelectionVisible(TRUE);
    m_ctlDIActionReport.SetRowFocusEnable(TRUE);
    m_ctlDIActionReport.SetFocus();
}
