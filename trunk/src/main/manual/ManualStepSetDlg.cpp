/// @file
///
/// @brief
///     步长设置 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///
#include "stdafx.h"
#include "ManualStepSetDlg.h"
#include "src/main/manual/manualsmvgoosesend.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/mgr/StatusMgr.h"


#define DSM_STEP_WIDTH_CAPTION               180
#define DSM_STEP_WIDTH_CONTENT               420


#define DSM_MS_ROW_INDEX_AMPL_STEP           0
#define DSM_MS_ROW_INDEX_PHASE_ANGLE_STEP    1

// CManualStepSetDlg 对话框

IMPLEMENT_DYNAMIC(CManualStepSetDlg, CBaseDlg)

CManualStepSetDlg::CManualStepSetDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualStepSetDlg::IDD, DSM_STR_MANUAL_STEP_SET_CAPTION, pParent)
    , m_pVirChannel(NULL)
    , m_pParamConfigSMVSend(NULL)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CManualStepSetDlg::~CManualStepSetDlg()
{
}

void CManualStepSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualStepSetDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDD_MANUAL_STEP_RET, &CManualStepSetDlg::OnStepItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDD_MANUAL_STEP_RET,  &CManualStepSetDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDD_MANUAL_STEP_RET,  &CManualStepSetDlg::OnInplaceEditAction)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualStepSetDlg 消息处理程序

BOOL CManualStepSetDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //
    //从SMV控制块列表中进入
    //
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL || 
        pPageData->m_nPageID != CManualSmvGooseSendDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
    {
        return FALSE;
    }

    CManualSmvGooseSendDataExtra* pDataExtra = dynamic_cast<CManualSmvGooseSendDataExtra*>(pPageData->m_pPageDataExtra);
    if (pDataExtra == NULL || pDataExtra->m_pVirChannel == NULL)
    {
        return FALSE;
    }
    m_pVirChannel = pDataExtra->m_pVirChannel;


    m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == m_pParamConfigSMVSend)
    {
        return FALSE;
    }

    _createReport();
    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    CString strTitle = _T("");
    strTitle.Format(_T("%s - %s"), 
        dsm_stringMgr::Get(DSM_STR_MANUAL_STEP_SET_CAPTION),
        m_pVirChannel->GetName().c_str()
        );
    CStatusMgr::SetTitle(strTitle);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CManualStepSetDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStepReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDD_MANUAL_STEP_RET);

    // 设置字体
    m_ctlStepReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlStepReport.PostInitTemplateStyle();
    // 插入列元素
    m_ctlStepReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STEP_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlStepReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STEP_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlStepReport.SetRowSelectionVisible(FALSE);
    m_ctlStepReport.SetRowFocusEnable(TRUE);
    m_ctlStepReport.SetSubItemFocusEnable(TRUE);
    m_ctlStepReport.SetFreezeColumnsCount(1);
    m_ctlStepReport.SetFreezeSelectionVisible(TRUE);
    m_ctlStepReport.SetFocusedColIndex(1);
    m_ctlStepReport.SetFocus();
    //幅值步长
    CString strContent = _T("");
    CString strChanName = _T("");
    strChanName = m_pVirChannel->GetName().c_str();

    m_ctlStepReport.InsertRecordRow(DSM_MS_ROW_INDEX_AMPL_STEP,
        new CDsmManualStepRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_AMPL_STEP),
        _T("")));

    m_ctlStepReport.InsertRecordRow(DSM_MS_ROW_INDEX_PHASE_ANGLE_STEP,
        new CDsmManualStepRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_PHASE_ANGLE_STEP),
        _T("")));

    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlStepReport.GetRecordRow(DSM_MS_ROW_INDEX_AMPL_STEP)->GetItem(1);

    if (strChanName[0] == _T('U'))
    {
        //电压
        if (m_pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
        {
            strContent.Format(_T("%.03lf"), m_pVirChannel->GetAmplitudeStep()/1000.0f);
            pItem->SetFormatString(_T("%s kV"));
        }
        else
        {
            strContent.Format(_T("%.03lf"), m_pVirChannel->GetAmplitudeStep());
            pItem->SetFormatString(_T("%s V"));
        }
    }
    else if (strChanName[0] == _T('I'))
    {
        strContent.Format(_T("%.03lf"), m_pVirChannel->GetAmplitudeStep());
        pItem->SetFormatString(_T("%s A"));
    }
    else if (strChanName[0] == _T('t'))
    {
        strContent.Format(_T("%.03lf"), m_pVirChannel->GetAmplitudeStep());
        pItem->SetFormatString(_T("%s μs"));
    }
    pItem->SetValue(strContent);
    pItem->SetEditable(TRUE);

    //相角步长,只有交流类型可以改变
    pItem = (CELRecordItemText*)m_ctlStepReport.GetRecordRow(DSM_MS_ROW_INDEX_PHASE_ANGLE_STEP)->GetItem(1);
    if (strChanName[0] == _T('U') || strChanName[0] == _T('I'))
    {
        if (m_pVirChannel->GetACDCType() == CURRENT_TYPE_AC)
        {
            strContent.Format(_T("%.03lf"), m_pVirChannel->GetPhaseAngleStep());
            pItem->SetFormatString(_T("%s°"));
            pItem->SetEditable(TRUE);
        }
        else
        {
            strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
            pItem->SetEditable(FALSE);
        }
    }
    else
    {
        strContent = dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN);
        pItem->SetEditable(FALSE);
    }
    pItem->SetValue(strContent);

    m_ctlStepReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlStepReport.SetFocusedSelectionRowIndex(0);
}

void CManualStepSetDlg::OnStepItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = (CELRecordItemText*)pItemNotify->pItem;

    CString strValue = pItem->GetValue();

    double dValue = 0.0f;

    bool bConvert = false;
    bConvert = DSM_Str2Double(strValue, dValue);

    switch (pItemNotify->rowIndex)
    {
    case DSM_MS_ROW_INDEX_AMPL_STEP:
        {
            CString strChanName = m_pVirChannel->GetName().c_str();
            if (strChanName.GetLength() > 0)
            {
                //改变幅值，步长均可编辑
                if (strChanName[0] == _T('U'))
                {
                    //电压
                    if (m_pParamConfigSMVSend->GetSVDisplay() == SMV_VALUE_TYPE_PRIMARY)
                    {
                        //一次值
                        m_pVirChannel->SetAmplitudeStep(dValue*1000.0f);
                        dValue = m_pVirChannel->GetAmplitudeStep();
                        strValue.Format(_T("%.03lf"), dValue/1000.0f);
                    }
                    else
                    {
                        //二次值
                        m_pVirChannel->SetAmplitudeStep(dValue);
                        dValue = m_pVirChannel->GetAmplitudeStep();
                        strValue.Format(_T("%.03lf"), dValue);
                    }
                }
                else
                {
                    //电流 时间
                    m_pVirChannel->SetAmplitudeStep(dValue);
                    dValue = m_pVirChannel->GetAmplitudeStep();
                    strValue.Format(_T("%.03lf"), dValue);
                }
            }
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    case DSM_MS_ROW_INDEX_PHASE_ANGLE_STEP:
        {
            m_pVirChannel->SetPhaseAngleStep(dValue);
            dValue = m_pVirChannel->GetPhaseAngleStep();
            strValue.Format(_T("%.03lf"), dValue);
            pItem->SetValue(strValue);
            *pResult = TRUE;
        }
        break;
    default:
        break;
    }
}
void CManualStepSetDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单

        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlStepReport.GetInplaceEdit());
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}
void CManualStepSetDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (nIDEvent == 1)
    {
        m_pParamConfigSMVSend->SaveConfig();
    }
    CBaseDlg::OnTimer(nIDEvent);
}
