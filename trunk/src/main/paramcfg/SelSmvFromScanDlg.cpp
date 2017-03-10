/// @file
///
/// @brief
///     从扫描列表中添加smv发送对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///


#include "stdafx.h"

#include "SelSmvFromScanDlg.h"
#include "src/main/paramcfg/common_cfg.h"
#include "src/main/mgr/StatusMgr.h"



// CAddSmvFromScanDlg 对话框

IMPLEMENT_DYNAMIC(CSelSmvFromScanDlg, CBaseDlg)

CSelSmvFromScanDlg::CSelSmvFromScanDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CSelSmvFromScanDlg::IDD, DSM_STR_SEL_SMV_FROM_SCAN_CAPTION, pParent)
{
    m_pConfigSmvSend = NULL;
    m_pDataLayer = NULL;
    m_nMenuID = IDR_MENU_EMPTY;
}

CSelSmvFromScanDlg::~CSelSmvFromScanDlg()
{
}

void CSelSmvFromScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelSmvFromScanDlg, CBaseDlg)
    ON_WM_TIMER()
END_MESSAGE_MAP()



BOOL CSelSmvFromScanDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    m_pDataLayer = CDsmDataLayer::getInstance();
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }

    m_pConfigSmvSend = CParamConfigSMVSend::getInstance();
    if (m_pConfigSmvSend == NULL)
    {
        return FALSE;
    }

    SetTimer(1, 300, NULL);

    CString strSmvType = _T("");
    switch(m_pConfigSmvSend->GetSMVType())
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        {
            m_pDataLayer->BeginDetect(NET_RECV_TYPE_SMV);
            strSmvType = _T("9-1/2");
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
    case FRAME_TYPE_FT3_GUOWANG:
        {
            m_pDataLayer->BeginDetect(FT3_RECV_TYPE_SMV);
            strSmvType = _T("FT3");
        }
        break;
    default:
        break;
    }
    
    _createReport();

    CString csCaption;
    csCaption.Format(_T("%s-%s"), 
        dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCAN_CAPTION),
        strSmvType);
    CStatusMgr::SetTitle(csCaption);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSelSmvFromScanDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SEL_SMV_FROM_SCAN_RET);

    // 设置字体
    m_ctlSmvReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlSmvReport.PostInitTemplateStyle();
    // 插入列元素

   
    m_ctlSmvReport.InsertColumn(0, L"   " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_APPID),      EL_CENTER, DSM_SEL_SMV_FROM_SCAN_WIDTH_APPID);
    m_ctlSmvReport.InsertColumn(1, L"  " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_CHAN_COUNT), EL_CENTER, DSM_SEL_SMV_FROM_SCAN_WIDTH_CHAN_COUNT);
    m_ctlSmvReport.InsertColumn(2, L"  " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_DESC),       EL_CENTER, DSM_SEL_SMV_FROM_SCAN_WIDTH_CTRL_DESC);
    m_ctlSmvReport.InsertColumn(3, L"  " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_PORT),       EL_CENTER, DSM_SEL_SMV_FROM_SCAN_WIDTH_PORT);
	m_ctlSmvReport.InsertColumn(4, _T(""),                                                  EL_CENTER, DSM_SEL_SMV_FROM_SCAN_WIDTH_CHECKBOX);

    // 设置选择焦点样式
    m_ctlSmvReport.SetRowSelectionVisible(TRUE);
    m_ctlSmvReport.SetRowFocusEnable(TRUE);
    m_ctlSmvReport.SetSubItemFocusEnable(TRUE);
    m_ctlSmvReport.SetFreezeSelectionVisible(TRUE);
    m_ctlSmvReport.SetFocusedColIndex(4);


#if !SU_FEI
	m_ctlSmvReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif

    m_ctlSmvReport.SetFocus();
    m_ctlSmvReport.ShowWindow(SW_SHOW);
}

void CSelSmvFromScanDlg::_updateDetectSmvReport()
{
    if (!m_ctlSmvReport.GetSafeHwnd())
    {
        return;
    }

    int nRowCount = m_ctlSmvReport.GetRecordRowCount();
    
    int nDetectCount = 0;

    enumFrameType smvType = m_pConfigSmvSend->GetSMVType();

    if (smvType == FRAME_TYPE_NET_9_1 || smvType == FRAME_TYPE_NET_9_2)
    {
        nDetectCount = m_pDataLayer->GetNetDetectItemsCount();
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
                m_ctlSmvReport.InsertRecordRow(i, new CDsmSelSmvCtrlFromScanRecord(pNetItem));
                m_ctlSmvReport.SetRecordRowData(i, (DWORD_PTR)pNetItem);
                if (i == 0)
                {
                    m_ctlSmvReport.SetFocusedSelectionRowIndex(0);
                }
            }
        }
    }
    else 
    {
        nDetectCount = m_pDataLayer->GetFt3DetectItemsCount();
        if (nRowCount < nDetectCount)
        {
            CFt3DetectItem* pFt3Item = NULL;
            for (int i = nRowCount; i < nDetectCount; ++i)
            {
                pFt3Item = m_pDataLayer->GetFt3DetectItem(i);
                if (pFt3Item == NULL)
                {
                    break;
                }
                m_ctlSmvReport.InsertRecordRow(i, new CDsmSelSmvCtrlFromScanRecord(pFt3Item));
                m_ctlSmvReport.SetRecordRowData(i, (DWORD_PTR)pFt3Item);
                if (i == 0)
                {
                    m_ctlSmvReport.SetFocusedSelectionRowIndex(0);
                }
            }
        }
    }
}

void CSelSmvFromScanDlg::_SavePage()
{
    
    ASSERT(m_pConfigSmvSend);

    CNetDetectItem* pNetDetectItem = NULL;
    CELRecordItemText* pItem = NULL;
    switch(m_pConfigSmvSend->GetSMVType())
    {
    case FRAME_TYPE_NET_9_1:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pNetDetectItem = (CNetDetectItem*) m_ctlSmvReport.GetRecordRowData(i);
                pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(i)->GetItem(4));
                if (pNetDetectItem != NULL && pItem != NULL)
                {
                    if (pItem->GetHasCheckbox() && pItem->IsChecked())
                    {
                        m_pConfigSmvSend->AddIEC9_1Cb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
                    }
                }
            }
        }
        break;
    case FRAME_TYPE_NET_9_2:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pNetDetectItem = (CNetDetectItem*) m_ctlSmvReport.GetRecordRowData(i);
                pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(i)->GetItem(4));
                if (pNetDetectItem != NULL && pItem != NULL)
                {
                    if (pItem->GetHasCheckbox() && pItem->IsChecked())
                    {
                        m_pConfigSmvSend->AddIEC9_2Cb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
                    }
                }
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pNetDetectItem = (CNetDetectItem*) m_ctlSmvReport.GetRecordRowData(i);
                pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(i)->GetItem(4));
                if (pNetDetectItem != NULL && pItem != NULL)
                {
                    if (pItem->GetHasCheckbox() && pItem->IsChecked())
                    {
                        m_pConfigSmvSend->AddFt3Cb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
                    }
                }
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pNetDetectItem = (CNetDetectItem*) m_ctlSmvReport.GetRecordRowData(i);
                pItem = dynamic_cast<CELRecordItemText*>(m_ctlSmvReport.GetRecordRow(i)->GetItem(4));
                if (pNetDetectItem != NULL && pItem != NULL)
                {
                    if (pItem->GetHasCheckbox() && pItem->IsChecked())
                    {
                        m_pConfigSmvSend->AddFt3ExpandCb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
                    }
                }
            }
        }
        break;
    default:
        break;
    }


    //
    //保存数据
    //
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();
}

void CSelSmvFromScanDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    
    if (nIDEvent == 1)
    {
        _updateDetectSmvReport();
    }

    CBaseDlg::OnTimer(nIDEvent);
}

BOOL CSelSmvFromScanDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    KillTimer(1);
    return CBaseDlg::DestroyWindow();
}
