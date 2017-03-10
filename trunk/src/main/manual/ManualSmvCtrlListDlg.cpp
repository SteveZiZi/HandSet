// ManualSmvCtrlListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ManualSmvCtrlListDlg.h"


#define DSM_MANUAL_SMV_WIDTH_CTRL_INDEX  129
#define DSM_MANUAL_SMV_WIDTH_CTRL_DESC   400

// CManualSmvCtrlListDlg 对话框

IMPLEMENT_DYNAMIC(CManualSmvCtrlListDlg, CBaseDlg)

CManualSmvCtrlListDlg::CManualSmvCtrlListDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualSmvCtrlListDlg::IDD,DSM_STR_MANUAL_SMV_CTRL_LIST, pParent)
    , m_pParamConfigSMVSend(NULL)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CManualSmvCtrlListDlg::~CManualSmvCtrlListDlg()
{
}

void CManualSmvCtrlListDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSmvCtrlListDlg, CBaseDlg)
    ON_NOTIFY(ELNM_OKCLICK, IDD_MANUAL_SMV_CTRL_RET, &CManualSmvCtrlListDlg::OnSmvCtrlOkClick)
END_MESSAGE_MAP()


// CManualSmvCtrlListDlg 消息处理程序

BOOL CManualSmvCtrlListDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (m_pParamConfigSMVSend == NULL)
    {
        return FALSE;
    }

    _createReport();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void  CManualSmvCtrlListDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvCtrlReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDD_MANUAL_SMV_CTRL_RET);

    // 设置字体
    m_ctlSmvCtrlReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvCtrlReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvCtrlReport.PostInitTemplateStyle();

    // 插入列元素

    m_ctlSmvCtrlReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_MANUAL_INDEX), EL_CENTER, DSM_MANUAL_SMV_WIDTH_CTRL_INDEX);
    m_ctlSmvCtrlReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CTRL_DESC), EL_LEFT, DSM_MANUAL_SMV_WIDTH_CTRL_DESC);


    // 设置选择焦点样式
    m_ctlSmvCtrlReport.SetRowSelectionVisible(TRUE);
    m_ctlSmvCtrlReport.SetRowFocusEnable(TRUE);
    m_ctlSmvCtrlReport.SetSubItemFocusEnable(TRUE);
    m_ctlSmvCtrlReport.SetFocus();


    int nIndex = 0;
    CString strIndex = _T("");
    CString strDesc = _T("");
    enumFrameType frameType = m_pParamConfigSMVSend->GetSMVType();
    switch(frameType)
    {
    case FRAME_TYPE_NET_9_2:
        {
            CIEC9_2Cb* pIEC92Cb = m_pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC92Cb)
            {
                if (pIEC92Cb->IsSelected())
                {
                    strIndex.Format(_T("%d"), nIndex + 1);
                    strDesc.Format(_T("0x%04X-%s"), pIEC92Cb->GetAPPID(), pIEC92Cb->GetDescription().c_str());
                    m_ctlSmvCtrlReport.InsertRecordRow( nIndex, new CDsmManualSmvCtrlListRecord(strIndex , strDesc));
                    m_ctlSmvCtrlReport.SetRecordRowData(nIndex, (DWORD_PTR)pIEC92Cb);
                    ++nIndex;
                }
                pIEC92Cb = m_pParamConfigSMVSend->NextIEC9_2Cb(pIEC92Cb);
            }
        }
        break;
    case FRAME_TYPE_NET_9_1:
        {
            CIEC9_1Cb* pIEC91Cb = m_pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC91Cb)
            {
                if (pIEC91Cb->IsSelected())
                {
                    strIndex.Format(_T("%d"), nIndex + 1);
                    strDesc.Format(_T("0x%04X-%s"), pIEC91Cb->GetAPPID(), pIEC91Cb->GetDescription().c_str());
                    m_ctlSmvCtrlReport.InsertRecordRow( nIndex, new CDsmManualSmvCtrlListRecord(strIndex , strDesc));
                    m_ctlSmvCtrlReport.SetRecordRowData(nIndex, (DWORD_PTR)pIEC91Cb);
                    ++nIndex;
                }
                pIEC91Cb = m_pParamConfigSMVSend->NextIEC9_1Cb(pIEC91Cb);
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            CFT3Cb* pFt3Cb = m_pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                if (pFt3Cb->IsSelected())
                {
                    strIndex.Format(_T("%d"), nIndex + 1);
                    strDesc.Format(_T("0x%04X-%s"), pFt3Cb->GetAPPID(), pFt3Cb->GetDescription().c_str());
                    m_ctlSmvCtrlReport.InsertRecordRow( nIndex, new CDsmManualSmvCtrlListRecord(strIndex , strDesc));
                    m_ctlSmvCtrlReport.SetRecordRowData(nIndex, (DWORD_PTR)pFt3Cb);
                    ++nIndex;
                }
                pFt3Cb = m_pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFT3ExpandCb* pFt3ExpCb = m_pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExpCb)
            {
                if (pFt3ExpCb->IsSelected())
                {
                    strIndex.Format(_T("%d"), nIndex + 1);
                    strDesc.Format(_T("0x%04X-%s"), pFt3ExpCb->GetAPPID(), pFt3ExpCb->GetDescription().c_str());
                    m_ctlSmvCtrlReport.InsertRecordRow( nIndex, new CDsmManualSmvCtrlListRecord(strIndex , strDesc));
                    m_ctlSmvCtrlReport.SetRecordRowData(nIndex, (DWORD_PTR)pFt3ExpCb);
                    ++nIndex;
                }
                pFt3ExpCb = m_pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpCb);
            }
        }
        break;
    default:
        break;
    }

    if (m_ctlSmvCtrlReport.GetRecordRowCount() > 0)
    {
        m_ctlSmvCtrlReport.SetFocusedSelectionRowIndex(0);
    }
}

void CManualSmvCtrlListDlg::OnSmvCtrlOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);

    int nRowIndex = m_ctlSmvCtrlReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }
    CBaseCb* pBaseCb = (CBaseCb*)m_ctlSmvCtrlReport.GetRecordRowData(nRowIndex);

    //进入状态字或通信品质编辑页面

    enumFrameType eFrameType = m_pParamConfigSMVSend->GetSMVType();
    if (eFrameType == FRAME_TYPE_NET_9_1 ||
        eFrameType == FRAME_TYPE_FT3_NANRUI ||
        eFrameType == FRAME_TYPE_FT3_GUOWANG )
    {
        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_STATUS_WORD_DLG);
    }
    else if (eFrameType == FRAME_TYPE_NET_9_2)
    {
        CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_QUALITY_DLG);
    }
    *pResult = TRUE;
}

void CManualSmvCtrlListDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvCtrlListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CManualSmvCtrlListDataExtra(pData);
    }

    CManualSmvCtrlListDataExtra* pDataExtra = dynamic_cast<CManualSmvCtrlListDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }
    
    pDataExtra->m_nFocusRow = m_ctlSmvCtrlReport.GetFocusedRowIndex();
    if (pDataExtra->m_nFocusRow >= 0)
    {
        pDataExtra->m_pBaseCb = (CBaseCb*)m_ctlSmvCtrlReport.GetRecordRowData(pDataExtra->m_nFocusRow);
    }


}

void CManualSmvCtrlListDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvCtrlListDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CManualSmvCtrlListDataExtra* pDataExtra = dynamic_cast<CManualSmvCtrlListDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        
        if (m_ctlSmvCtrlReport.GetRecordRowCount() > 0)
        {
            if (pDataExtra->m_nFocusRow >= 0 && pDataExtra->m_nFocusRow < m_ctlSmvCtrlReport.GetRecordRowCount())
            {
                m_ctlSmvCtrlReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRow);
            }
            else
            {
                m_ctlSmvCtrlReport.SetFocusedSelectionRowIndex(0);
            }
        }
    }
}
