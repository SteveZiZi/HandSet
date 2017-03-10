/// @file
///
/// @brief
///     从全站配置中添加goose对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///


#include "stdafx.h"
#include "src/main/resource.h"
#include "BasicParamDlg.h"
#include "src/main/paramcfg/common_cfg.h"
#include "BasicParamAddGooseFromSclDlg.h"
#include "src/service/datalayer/scl/SCLManager.h"
#include "src/service/datalayer/config/ParamConfigGooseSend.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"

#if !SU_FEI
#include "BasicParamGooseTxDlg.h"
#endif

// CAddGooseFromSclDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamAddGooseFromSclDlg, CBaseDlg)

CBasicParamAddGooseFromSclDlg::CBasicParamAddGooseFromSclDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamAddGooseFromSclDlg::IDD, DSM_STR_BASIC_PARAM_ADD_GOOSE_FROM_SCL_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_EMPTY;
#if SU_FEI
    m_nMsgParamType =  DSM_MSG_PARAM_GOOSES;
#else
	m_nMsgParamType =  DSM_MSG_GOOSE_PARAM;
#endif
}

CBasicParamAddGooseFromSclDlg::~CBasicParamAddGooseFromSclDlg()
{
}

void CBasicParamAddGooseFromSclDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamAddGooseFromSclDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCL_RET, &CBasicParamAddGooseFromSclDlg::OnGooseSelItemValueChanged)
END_MESSAGE_MAP()


// CAddGooseFromSclDlg 消息处理程序

BOOL CBasicParamAddGooseFromSclDlg::OnInitDialog()
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
    m_nMsgParamType = pDataExtra->m_nMsgParamType;


    _creatCtrlBlockReport();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CBasicParamAddGooseFromSclDlg::_creatCtrlBlockReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCL_RET);

    // 设置字体
    m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlCtrlBlockReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlCtrlBlockReport.PostInitTemplateStyle();
    // 插入列元素

    
    m_ctlCtrlBlockReport.InsertColumn(0, L"   " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_APPID),      EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_APPID);
    m_ctlCtrlBlockReport.InsertColumn(1, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT), EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHAN_COUNT);
    m_ctlCtrlBlockReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_DESC),  EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CTRL_DESC);
    m_ctlCtrlBlockReport.InsertColumn(3, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_PORT),       EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_PORT);
	m_ctlCtrlBlockReport.InsertColumn(4, _T(""),                                             EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHECKBOX);
    // 设置选择焦点样式
    m_ctlCtrlBlockReport.SetRowSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetRowFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetSubItemFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetFocusedColIndex(4);

#if !SU_FEI
	m_ctlCtrlBlockReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

#endif



    m_ctlCtrlBlockReport.SetFocus();
    m_ctlCtrlBlockReport.ShowWindow(SW_SHOW);

    CSCLManager* pSCLManager = CSCLManager::getInstance();
    if (NULL == pSCLManager)
    {
        return ;
    }

    //
    //初始化全站配置文件中的smv控制块
    //
    
    CParamConfigGOOSEBase* pParamConfigGooseBase = NULL;
    if (m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
    {
        pParamConfigGooseBase = CParamConfigGOOSESend::getInstance();
    }
   /* else
    {
        pParamConfigGooseBase = CParamConfigGOOSERecv::getInstance();
    }*/

    if (NULL == pParamConfigGooseBase)
    {
        return ;
    }
    //取消原来选中的控制块
    pSCLManager->UnSelectAllGooseCb();
    

    CGooseCb* pGooseCb = pParamConfigGooseBase->FirstGocb();
    while(pGooseCb)
    {
        pSCLManager->SelectGooseCb(pGooseCb->GetAPPID());
        pGooseCb = pParamConfigGooseBase->NextGocb(pGooseCb);
    }

    int i = 0;
    pGooseCb = pSCLManager->FirstGooseCb();
    while(pGooseCb)
    {
        if (m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
        {
            m_ctlCtrlBlockReport.InsertRecordRow(i, new CDsmSelGooseCtrlFromSclRecord(pGooseCb, eGooseSend));
        }
        /*else
        {
            m_ctlCtrlBlockReport.InsertRecordRow(i, new CDsmSelGooseCtrlFromSclRecord(pGooseCb, eGooseRec));
        }*/
        m_ctlCtrlBlockReport.SetRecordRowData(i, (DWORD_PTR)pGooseCb);
        ++i;
        pGooseCb = pSCLManager->NextGooseCb(pGooseCb);
    }

    m_ctlCtrlBlockReport.GetColumnExAttr(4)->GetEditOptions()->SetAllowEdit(TRUE);

    if (m_ctlCtrlBlockReport.GetRecordRowCount() > 0)
    {
        m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
    }
}

void CBasicParamAddGooseFromSclDlg::OnGooseSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return ;
    }

    CGooseCb* pGooseCb = (CGooseCb*) m_ctlCtrlBlockReport.GetRecordRowData(pItemNotify->rowIndex);

    ASSERT(pGooseCb);
    if (pGooseCb == NULL)
    {
        return;
    }

    if(pItemNotify->pItem->GetHasCheckbox())
    {
        //
        //设置是否选中控制块
        //
        pGooseCb->Selected(pItemNotify->pItem->IsChecked() == TRUE ? true : false);
    }
    *pResult = TRUE;
}

void CBasicParamAddGooseFromSclDlg::_SavePage()
{
    
    
    CParamConfigGOOSEBase* pParamConfigGooseBase = NULL;
    if (m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
    {
        pParamConfigGooseBase = CParamConfigGOOSESend::getInstance();
    }
    /*else
    {
        pParamConfigGooseBase = CParamConfigGOOSERecv::getInstance();
    }*/

    if (NULL == pParamConfigGooseBase)
    {
        return ;
    }
    
    CGooseCb * pGooseCb = NULL;
    for ( int i = 0; i  < m_ctlCtrlBlockReport.GetRecordRowCount(); ++i)
    {
        pGooseCb = (CGooseCb*)m_ctlCtrlBlockReport.GetRecordRowData(i);
        ASSERT(pGooseCb);
        if (pGooseCb != NULL)
        {
            if (pGooseCb->IsSelected())
            {
                pGooseCb->Selected(false);
                pParamConfigGooseBase->AddGocb(pGooseCb);
            }
            else
            {
                pParamConfigGooseBase->DeleteGocb(pGooseCb->GetAPPID());
            }
        }
    }

    //
    //保存数据
    //
    pParamConfigGooseBase->SaveConfig();

    //
    //清除原来选中的控制块信息
    //
    CSCLManager* pSCLManager = CSCLManager::getInstance();
    if (NULL == pSCLManager)
    {
        return ;
    }
    //取消原来选中的控制块
    pSCLManager->UnSelectAllIEC9_2Cb();
}

