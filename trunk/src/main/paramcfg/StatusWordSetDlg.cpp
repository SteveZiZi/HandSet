// EditStatusWordDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "StatusWordSetDlg.h"
#include "src/service/datalayer/config/ParamConfigSMVSend.h"
#include "src/main/paramcfg/EditSmvSendDlg.h"
#include "src/main/manual/manualsmvstatusworddlg.h"
#include "src/main/mgr/statusmgr.h"

// 编辑SMV控制块报表
//
#if SU_FEI
#define DSM_SWS_WIDTH_CAPTION           180
#define DSM_SWS_WIDTH_CONTENT           420
#else
#define DSM_SWS_WIDTH_CAPTION           180
#define DSM_SWS_WIDTH_CONTENT           313
#endif
// CStatusWordSetDlg 对话框

IMPLEMENT_DYNAMIC(CStatusWordSetDlg, CBaseDlg)

CStatusWordSetDlg::CStatusWordSetDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStatusWordSetDlg::IDD, DSM_STR_MSW_STATUS_WORD_SET1, pParent)
    , m_usWordValue(0)
    , m_pBaseCb(NULL)
    , m_eStatusWordType(SWTStatusWord1)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CStatusWordSetDlg::~CStatusWordSetDlg()
{

}

void CStatusWordSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStatusWordSetDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATUS_WORD_SET_RET, &CStatusWordSetDlg::OnStatusWordItemSelectChanged)

    ON_WM_TIMER()
END_MESSAGE_MAP()


// CStatusWordSetDlg 消息处理程序

BOOL CStatusWordSetDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();


    // TODO:  在此添加额外的初始化
    //
    //从状态列表中进入
    //
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL || 
        pPageData->m_pPageDataExtra == NULL)
    {
        //
        //参数配置进入（goose编辑或查看）
        //
        return FALSE;
    }
    
    if (pPageData->m_nPageID == CEditSmvSendDlg::IDD)
    {
        CEditSmvSendDataExtra* pDataExtra = dynamic_cast<CEditSmvSendDataExtra*>(pPageData->m_pPageDataExtra);
        if (pDataExtra == NULL || pDataExtra->m_pBaseCb == NULL)
        {
            return FALSE;
        }

        m_pBaseCb = pDataExtra->m_pBaseCb;

        //word类型
        if (pDataExtra->m_eStatusWordType == ESSStatusWord1)
        {
            m_eStatusWordType = SWTStatusWord1;
            CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_MSW_STATUS_WORD_SET1));
        }
        else if (pDataExtra->m_eStatusWordType == ESSStatusWord2)
        {
            m_eStatusWordType = SWTStatusWord2;
            CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_MSW_STATUS_WORD_SET2));
        }
        else
        {
            return FALSE;
        }
    }
    else if (pPageData->m_nPageID == CManualSmvStatusWordDlg::IDD)
    {
        CManualSmvStatusWordDataExtra* pDataExtra = dynamic_cast<CManualSmvStatusWordDataExtra*>(pPageData->m_pPageDataExtra);
        if (pDataExtra == NULL || pDataExtra->m_pBaseCb == NULL)
        {
            return FALSE;
        }

        m_pBaseCb = pDataExtra->m_pBaseCb;

        //word类型
        if (pDataExtra->m_eStatusWordType == MSSWStatusWord1)
        {
            m_eStatusWordType = SWTStatusWord1;
            CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_MSW_STATUS_WORD_SET1));
        }
        else if (pDataExtra->m_eStatusWordType == MSSWStatusWord2)
        {
            m_eStatusWordType = SWTStatusWord2;
            CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_MSW_STATUS_WORD_SET2));
        }
        else
        {
            return FALSE;
        }
    }


    //smv类型
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return FALSE;
    }

    m_nSmvSendType = pParamConfigSMVSend->GetSMVType();

    switch (m_nSmvSendType)
    {
    case FRAME_TYPE_NET_9_1:
        {
            CIEC9_1Cb* pIEC91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (pIEC91Cb == NULL)
            {
                return FALSE;
            }
            
            if (m_eStatusWordType == SWTStatusWord1)
            {
                m_usWordValue = pIEC91Cb->GetStatusWord1();
            }
            else
            {
                m_usWordValue = pIEC91Cb->GetStatusWord2();
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb == NULL)
            {
                return FALSE;
            }
            if (m_eStatusWordType == SWTStatusWord1)
            {
                m_usWordValue = pFt3Cb->GetStatusWord1();
            }
            else
            {
                m_usWordValue = pFt3Cb->GetStatusWord2();
            }

        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb == NULL)
            {
                return FALSE;
            }
            if (m_eStatusWordType == SWTStatusWord1)
            {
                m_usWordValue = pFt3ExpCb->GetStatusWord1();
            }
            else
            {
                m_usWordValue = pFt3ExpCb->GetStatusWord2();
            }
        }
        break;
    default:
        break;
    }

    _createReport();

    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CStatusWordSetDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStatusWordReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATUS_WORD_SET_RET);

    // 设置字体
    m_ctlStatusWordReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlStatusWordReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlStatusWordReport.InsertColumn(0, 
        _T(""),
        EL_LEFT,
        DSM_SWS_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlStatusWordReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_SWS_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlStatusWordReport.SetRowSelectionVisible(FALSE);
    m_ctlStatusWordReport.SetRowFocusEnable(TRUE);
    m_ctlStatusWordReport.SetSubItemFocusEnable(TRUE);
    m_ctlStatusWordReport.SetFreezeColumnsCount(1);
    m_ctlStatusWordReport.SetFreezeSelectionVisible(TRUE);
    m_ctlStatusWordReport.SetFocusedColIndex(1);
    m_ctlStatusWordReport.SetFocus();

    if (m_eStatusWordType == SWTStatusWord1)
    {
        //word1
        _initReportStatusWord1();
    }
    else
    {
        if (m_nSmvSendType == FRAME_TYPE_NET_9_1 || m_nSmvSendType == FRAME_TYPE_FT3_NANRUI)
        {
            _initReportStatusWord2_9_1orFT3();
        }
        else if (m_nSmvSendType == FRAME_TYPE_FT3_GUOWANG)
        {
            _initReportStatusWord2FT3Ext();
        }
    }
}

void CStatusWordSetDlg::_initReportStatusWord1()
{

    int nRepInitInfo[16][3] =
    {
        {DSM_STR_MSW_NEED_MAINTAIN, DSM_STR_MSW_NORMAL, DSM_STR_MSW_WARNING},
        {DSM_STR_MSW_LLNO_MODE, DSM_STR_MSW_CONNECT, DSM_STR_MSW_TEST},
        {DSM_STR_MSW_WAKE_UP_INDI, DSM_STR_MSW_CONNET_DATA_EFF, DSM_STR_MSW_WAKE_TIME_DATA_INEFF},
        {DSM_STR_MSW_MU_SYN, DSM_STR_MSW_USE_INTER_ALGORITHM, DSM_STR_MSW_NO_USE_INTER_ALGORITHM},

        {DSM_STR_MSW_MU_SYN_FLAG, DSM_STR_MSW_SAMPLE_SYN, DSM_STR_MSW_SAMPLE_NO_SYN},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CUR_SENSOR_TYPE, DSM_STR_MSW_CUR_VALUE, DSM_STR_MSW_CUR_DERIVATIVE},
        {DSM_STR_MSW_RANGE_FLAG, DSM_STR_MSW_PROTECT_CUR_FACTOR_01CF, DSM_STR_MSW_PROTECT_CUR_FACTOR_00E7},
        {DSM_STR_MSW_RESERVE, DSM_STR_MSW_RESERVE_0, DSM_STR_MSW_RESERVE_1},
        {DSM_STR_MSW_RESERVE, DSM_STR_MSW_RESERVE_0, DSM_STR_MSW_RESERVE_1},
    };

    CString csBit = _T("");
    CString csCaption = _T("");
    CString csContent = _T("");
    CString csTemp = _T("");
    CString csChannel = _T("");

    csBit = dsm_stringMgr::Get(DSM_STR_MSW_BIT);
    csTemp = dsm_stringMgr::Get(DSM_STR_MSW_CHANNEL);    //通道%d无效标志
    
    for (int i = 0; i < 16; ++i)
    {
        if ( i >= 5 && i <= 11)
        {
            csChannel.Format(csTemp, i - 4);
            csCaption.Format(_T("%s%d %s"), csBit, i, csChannel);
        }
        else 
        {
            csCaption.Format(_T("%s%d %s"), csBit, i, dsm_stringMgr::Get(nRepInitInfo[i][0]));
        }
        m_ctlStatusWordReport.InsertRecordRow(i, new CDsmStatusWordSetRecord(csCaption, _T("")));

        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1);
        CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
        pOptions->AddComboButton();
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][1]), 0);
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][2]), 1);
        CELRecordItemConstraint* pItemCon = pOptions->FindConstraint((m_usWordValue>>i) & 0x0001);
        if (pItemCon)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }

    m_ctlStatusWordReport.SetFocusedSelectionRowIndex(0);
    m_ctlStatusWordReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for (int i = 0; i < m_ctlStatusWordReport.GetRecordRowCount(); ++i)
    {
        m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }

}

void CStatusWordSetDlg::_initReportStatusWord2_9_1orFT3()
{
    //12通道
    int nRepInitInfo[16][3] =
    {
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_RESERVE, DSM_STR_MSW_RESERVE_0, DSM_STR_MSW_RESERVE_1},
        {DSM_STR_MSW_RESERVE, DSM_STR_MSW_RESERVE_0, DSM_STR_MSW_RESERVE_1},
        {DSM_STR_MSW_RESERVE, DSM_STR_MSW_RESERVE_0, DSM_STR_MSW_RESERVE_1},

        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},

        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
    };

    CString csBit = _T("");
    CString csCaption = _T("");
    CString csContent = _T("");
    CString csTemp = _T("");
    CString csChannel = _T("");

    csBit = dsm_stringMgr::Get(DSM_STR_MSW_BIT);
    csTemp = dsm_stringMgr::Get(DSM_STR_MSW_CHANNEL);    //通道%d无效标志

    for (int i = 0; i < 16; ++i)
    {
        if (i < 5)
        {
            csChannel.Format(csTemp, i + 8);
            csCaption.Format(_T("%s%d %s"), csBit, i, csChannel);
        }
        else 
        {
            csCaption.Format(_T("%s%d %s"), csBit, i, dsm_stringMgr::Get(nRepInitInfo[i][0]));
        }
        m_ctlStatusWordReport.InsertRecordRow(i, new CDsmStatusWordSetRecord(csCaption, _T("")));

        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1);
        CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
        pOptions->AddComboButton();
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][1]), 0);
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][2]), 1);
        CELRecordItemConstraint* pItemCon = pOptions->FindConstraint((m_usWordValue>>i) & 0x0001);
        if (pItemCon)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }

    m_ctlStatusWordReport.SetFocusedSelectionRowIndex(0);
    m_ctlStatusWordReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for (int i = 0; i < m_ctlStatusWordReport.GetRecordRowCount(); ++i)
    {
        m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
    m_ctlStatusWordReport.SetFocusedSelectionRowIndex(0);
}

void CStatusWordSetDlg::_initReportStatusWord2FT3Ext()
{
    //22通道

    int nRepInitInfo[16][3] =
    {
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},

        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CHANNEL, DSM_STR_MSW_DATA_EFFECTIVE, DSM_STR_MSW_DATA_INEFFECTIVE},
        {DSM_STR_MSW_CUSTOM, DSM_STR_MSW_CUSTOM_0, DSM_STR_MSW_CUSTOM_1},
    };

    CString csBit = _T("");
    CString csCaption = _T("");
    CString csContent = _T("");
    CString csTemp = _T("");
    CString csChannel = _T("");

    csBit = dsm_stringMgr::Get(DSM_STR_MSW_BIT);
    csTemp = dsm_stringMgr::Get(DSM_STR_MSW_CHANNEL);    //通道%d无效标志

    for (int i = 0; i < 16; ++i)
    {
        if (i < 15)
        {
            csChannel.Format(csTemp, i + 8);
            csCaption.Format(_T("%s%d %s"), csBit, i, csChannel);
        }
        else 
        {
            csCaption.Format(_T("%s%d %s"), csBit, i, dsm_stringMgr::Get(nRepInitInfo[i][0]));
        }
        m_ctlStatusWordReport.InsertRecordRow(i, new CDsmStatusWordSetRecord(csCaption, _T("")));

        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1);
        CELRecordItemEditOptions* pOptions = pItem->GetEditOptions(NULL);
        pOptions->AddComboButton();
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][1]), 0);
        pOptions->AddConstraint(dsm_stringMgr::Get(nRepInitInfo[i][2]), 1);
        CELRecordItemConstraint* pItemCon = pOptions->FindConstraint((m_usWordValue>>i) & 0x0001);
        if (pItemCon)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }

    m_ctlStatusWordReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for (int i = 0; i < m_ctlStatusWordReport.GetRecordRowCount(); ++i)
    {
        m_ctlStatusWordReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }

}

void CStatusWordSetDlg::_updateStatusWord()
{

    switch (m_nSmvSendType)
    {
    case FRAME_TYPE_NET_9_1:
        {
            CIEC9_1Cb* pIEC91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (pIEC91Cb == NULL)
            {
                return;
            }

            if (m_eStatusWordType == SWTStatusWord1)
            {
                pIEC91Cb->SetStatusWord1(m_usWordValue);
            }
            else
            {
                pIEC91Cb->SetStatusWord2(m_usWordValue);
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb == NULL)
            {
                return;
            }
            if (m_eStatusWordType == SWTStatusWord1)
            {
                pFt3Cb->SetStatusWord1(m_usWordValue);
            }
            else
            {
                pFt3Cb->SetStatusWord2(m_usWordValue);
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb == NULL)
            {
                return ;
            }
            if (m_eStatusWordType == SWTStatusWord1)
            {
                pFt3ExpCb->SetStatusWord1(m_usWordValue);
            }
            else
            {
                pFt3ExpCb->SetStatusWord2(m_usWordValue);
            }
        }
        break;
    default:
        break;
    }
}


void CStatusWordSetDlg::OnStatusWordItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }
    
    //
    //报表中每个item对应相应的BIT
    //
    int nRowSel = m_ctlStatusWordReport.GetFocusedRowIndex();
    DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();
    
    if (nRowSel < 16)
    {
        m_usWordValue = (m_usWordValue & ~(0x0001 << nRowSel)) | (dwValue << nRowSel);
        _updateStatusWord();
    }
}

void CStatusWordSetDlg::_SavePage()
{
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();
}
void CStatusWordSetDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        _SavePage();
    }
    CBaseDlg::OnTimer(nIDEvent);
}
