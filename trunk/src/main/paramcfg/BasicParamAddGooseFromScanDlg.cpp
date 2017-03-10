/// @file
///
/// @brief
///     从扫描列表中添加goose对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#include "stdafx.h"
#include "BasicParamDlg.h"
#include "BasicParamAddGooseFromScanDlg.h"
#include "src/service/datalayer/scl/SCLManager.h"

#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/config/ParamConfigGooseSend.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"

#if !SU_FEI
#include "BasicParamGooseTxDlg.h"

#endif
// CAddGooseFromScanDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamAddGooseFromScanDlg, CBaseDlg)

CBasicParamAddGooseFromScanDlg::CBasicParamAddGooseFromScanDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamAddGooseFromScanDlg::IDD, DSM_STR_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_CAPTION, pParent)
{
    m_pDataLayer = NULL;
    m_nMenuID = IDR_MENU_EMPTY;
#if SU_FEI
    m_nMsgParamType =  DSM_MSG_PARAM_GOOSES;
#else
	m_nMsgParamType =  DSM_MSG_GOOSE_PARAM;
#endif
}

CBasicParamAddGooseFromScanDlg::~CBasicParamAddGooseFromScanDlg()
{
}

void CBasicParamAddGooseFromScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamAddGooseFromScanDlg, CBaseDlg)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CAddGooseFromScanDlg 消息处理程序

BOOL CBasicParamAddGooseFromScanDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_pDataLayer = CDsmDataLayer::getInstance();
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }

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
        return FALSE;
    }
#if SU_FEI
    CBasicParamPageDataExtra* pDataExtra = dynamic_cast<CBasicParamPageDataExtra*>(pPageData->m_pPageDataExtra);
#else
	CBasicParamPageDataExtraGoose* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGoose*>(pPageData->m_pPageDataExtra);
#endif
    if (pDataExtra == NULL)
    {
        return FALSE;
    }
    
    //
    //根据goose发送和goose接收不同，初始化选中的goose控制块
    //
    m_nMsgParamType = pDataExtra->m_nMsgParamType;
    
    SetTimer(1, 300, NULL);

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);
    
    _creatCtrlBlockReport();


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CBasicParamAddGooseFromScanDlg::_creatCtrlBlockReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCAN_RET);

    // 设置字体
    m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlCtrlBlockReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlCtrlBlockReport.PostInitTemplateStyle();
    // 插入列元素

    
    m_ctlCtrlBlockReport.InsertColumn(0, L"   " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_APPID),      EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_APPID);
    m_ctlCtrlBlockReport.InsertColumn(1, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT), EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHAN_COUNT);
    m_ctlCtrlBlockReport.InsertColumn(2, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_DESC),  EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CTRL_DESC);
    m_ctlCtrlBlockReport.InsertColumn(3, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_PORT),       EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_PORT);
	m_ctlCtrlBlockReport.InsertColumn(4, _T(""),                                             EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHECKBOX);
    // 设置选择焦点样式
    m_ctlCtrlBlockReport.SetRowSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetRowFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetSubItemFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetFocusedColIndex(4);

    m_ctlCtrlBlockReport.SetFocus();
    m_ctlCtrlBlockReport.ShowWindow(SW_SHOW);
#if !SU_FEI
	m_ctlCtrlBlockReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

#endif

    //
    //初始化报表
    //
    _updateDetectGooseReport();

}

void CBasicParamAddGooseFromScanDlg::_updateDetectGooseReport()
{
    if (!m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        return;
    }

    int nRowCount = m_ctlCtrlBlockReport.GetRecordRowCount();
    int nDetectCount = m_pDataLayer->GetNetDetectItemsCount();
    if (nRowCount < nDetectCount)
    {
        CNetDetectItem*  pNetItem = NULL;
        for (int i = nRowCount; i < nDetectCount; ++i)
        {
            pNetItem = m_pDataLayer->GetNetDetectItem(i);
            if (pNetItem == NULL)
            {
                break;
            }

            if (m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
            {
                m_ctlCtrlBlockReport.InsertRecordRow(i, new CDsmSelGooseCtrlFromScanRecord(pNetItem, eGooseSend));
            }
            else
            {
                m_ctlCtrlBlockReport.InsertRecordRow(i, new CDsmSelGooseCtrlFromScanRecord(pNetItem, eGooseRec));
            }
            m_ctlCtrlBlockReport.SetRecordRowData(i, (DWORD_PTR)pNetItem);
            
            if(i == 0)
            {
                m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
            }
        }


    }
}

void CBasicParamAddGooseFromScanDlg::_SavePage()
{
    CParamConfigGOOSEBase* pParamConfigGooseBase = NULL;
    if (m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
    {
        pParamConfigGooseBase = CParamConfigGOOSESend::getInstance();
    }
    else
    {
        pParamConfigGooseBase = CParamConfigGOOSERecv::getInstance();
    }

    CNetDetectItem* pNetDetectItem = NULL;
    CELRecordItemText* pItem = NULL;
    for (int i = 0; i < m_ctlCtrlBlockReport.GetRecordRowCount(); ++i)
    {
        pNetDetectItem = (CNetDetectItem*) m_ctlCtrlBlockReport.GetRecordRowData(i);
        pItem = dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(i)->GetItem(4));
        if (pNetDetectItem != NULL && pItem != NULL)
        {
            if (pItem->GetHasCheckbox() && pItem->IsChecked())
            {
                pParamConfigGooseBase->AddGocb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
            }
        }
    }

    //
    //保存数据
    //
    pParamConfigGooseBase->SaveConfig();
}

BOOL CBasicParamAddGooseFromScanDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    KillTimer(1);
    return CBaseDlg::DestroyWindow();
}

void CBasicParamAddGooseFromScanDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        _updateDetectGooseReport();
    }


    CBaseDlg::OnTimer(nIDEvent);
}
