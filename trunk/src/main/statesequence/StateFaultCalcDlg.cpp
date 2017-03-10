/// @file
///
/// @brief
///     故障计算 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"

#include "StateFaultCalcDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/statesequence/statesetdlg.h"

#include "src/main/popdlg/msgbtndlg.h"


#define DSM_STATE_FAULT_CALC_WIDTH_CAPTION            180
#define DSM_STATE_FAULT_CALC_WIDTH_CONTENT            420

#define DSM_SFC_INDEX_FAULTTYPE                       0
#define DSM_SFC_INDEX_CALC_MODE_TYPE                  1
#define DSM_SFC_INDEX_DIRECTION                       2
#define DSM_SFC_INDEX_RATED_VOLTAGE                   3
#define DSM_SFC_INDEX_LOAD_CURRENT                    4
#define DSM_SFC_INDEX_LOAD_ANGLE                      5
#define DSM_SFC_INDEX_LINE_IMPEDANCE_Z1               6
#define DSM_SFC_INDEX_NEXT                            7


//
//故障设置
//
enum SFCRecordFuncType
{
    SFCUnknown = 0,
    SFCFaultType ,                                  ///< SMV类型
    SFCCalcModeType,
    SFCDirection,
    SFCRatedVoltage,
    SFCLoadCurrent,
    SFCLoadAngle,
    SFCLineImpedanceZ1,
    SFCLineCompensateK1,
    SFCSystemImpdanceZs,
    SFCSystemCompensateKs,
    SFCFaultVoltage,
    SFCFaultCurrent
};

// 提示区域
#if SU_FEI
#define DSM_STATE_FAULT_CALC_TIP     CRect(30, 20, 610, 90)
#else
#define DSM_STATE_FAULT_CALC_TIP     CRect(20, 20, 620, 50)
#endif
// CStateFaultCalcDlg 对话框

IMPLEMENT_DYNAMIC(CStateFaultCalcDlg, CBaseDlg)

CStateFaultCalcDlg::CStateFaultCalcDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateFaultCalcDlg::IDD, DSM_STR_STATE_FAULT_CALC_CAPTION, pParent)
    , m_pFaultCalc(NULL)
    , m_pConfigStateSequ(NULL)
    , m_pStatus(NULL)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CStateFaultCalcDlg::~CStateFaultCalcDlg()
{
}

void CStateFaultCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateFaultCalcDlg, CBaseDlg)
#if CHUN_HUA
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_STATE_FAULT_CALC_RET, &CStateFaultCalcDlg::OnItemSelectChanged)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_FAULT_CALC_RET, &CStateFaultCalcDlg::OnItemValueChanged)
    ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_FAULT_CALC_RET, &CStateFaultCalcDlg::OnItemOkClick)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_FAULT_CALC_RET,  &CStateFaultCalcDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_FAULT_CALC_RET,  &CStateFaultCalcDlg::OnInplaceEditAction)
    ON_WM_TIMER()
#endif
END_MESSAGE_MAP()


// CStateFaultCalcDlg 消息处理程序

BOOL CStateFaultCalcDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
#if CHUN_HUA
    CPageData* pData = CEventProc::GetInstance()->GetPreviousPageData();

    if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }


    CStateSetDataExtra* pDataExtra = dynamic_cast<CStateSetDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return FALSE;
    }
    m_pStatus = pDataExtra->m_pStatus;
    if (m_pStatus == NULL)
    {
        return FALSE;
    }

    m_pConfigStateSequ = CParamConfigStateSequence::getInstance();
    if (m_pConfigStateSequ)
    {
        m_pFaultCalc = m_pConfigStateSequ->GetFaultCalc();
    }
    if (m_pFaultCalc == NULL)
    {
        return FALSE;
    }

    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);
    _createReport();

    _restorePage();
#endif
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}
#if CHUN_HUA
void CStateFaultCalcDlg::_createReport()
{

    if (m_pFaultCalc == NULL)
    {
        return;
    }
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlFaultCalcReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_FAULT_CALC_RET);

    // 设置字体
    m_ctlFaultCalcReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlFaultCalcReport.PostInitTemplateStyle();
    // 插入列元素
    m_ctlFaultCalcReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_CALC_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlFaultCalcReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_CALC_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlFaultCalcReport.SetRowSelectionVisible(FALSE);
    m_ctlFaultCalcReport.SetRowFocusEnable(TRUE);
    m_ctlFaultCalcReport.SetSubItemFocusEnable(TRUE);
    m_ctlFaultCalcReport.SetFreezeColumnsCount(1);
    m_ctlFaultCalcReport.SetFreezeSelectionVisible(TRUE);
    m_ctlFaultCalcReport.SetFocusedColIndex(1);
    m_ctlFaultCalcReport.SetFocus();

    CString strCaption = _T("");
    CString strContent = _T("");
    CELRecordItemText* pItem = NULL;
    //故障类型
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_TYPE);
    SCCalc::SCType faultType = m_pFaultCalc->GetFaultType();
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_FAULTTYPE, new CDsmStateFaultCalcRecord(strCaption, _T("")));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_FAULTTYPE, 0, SFCFaultType);
    CELRecordItemEditOptions* pOptions = m_ctlFaultCalcReport.GetRecordRow(0)->GetItem(1)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_A_GROUND), SCCalc::A_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_B_GROUND), SCCalc::B_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_C_GROUND), SCCalc::C_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_AB_SHORT_CIRCUIT), SCCalc::AB_SHORT_CIRCUIT);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_BC_SHORT_CIRCUIT), SCCalc::BC_SHORT_CIRCUIT);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CA_SHORT_CIRCUIT), SCCalc::CA_SHORT_CIRCUIT);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_AB_GROUND), SCCalc::AB_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_BC_GROUND), SCCalc::BC_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CA_GROUND), SCCalc::CA_GROUND);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_ABC_SHORT_CIRCUIT), SCCalc::ABC_SHORT_CIRCUIT);
    //pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SINGLE_PHASE_SYSTEM), SCCalc::SINGLE_PHASE_SYSTEM);
    
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(faultType);
    if (pItemCon)
    {
        pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(0)->GetItem(1);
        if (pItem)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }
    
    //计算模型
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_CALC_MODE_TYPE);
    SCCalc::CalcModelType calcModeType = m_pFaultCalc->GetCalcModelType();
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_CALC_MODE_TYPE, new CDsmStateFaultCalcRecord(strCaption, _T("")));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_CALC_MODE_TYPE, 0, SFCCalcModeType);
    pOptions = m_ctlFaultCalcReport.GetRecordRow(1)->GetItem(1)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_CURRENT_FIXED), SCCalc::CURRENT_FIXED);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_VOLTAGE_FIXED), SCCalc::VOLTAGE_FIXED);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_FIXED), SCCalc::SYSTEM_IMPEDANCE_FIXED);

    pItemCon = pOptions->FindConstraint(calcModeType);
    if (pItemCon)
    {
        pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(1)->GetItem(1);
        if (pItem)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }

    //方向
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_DIRECTION);
    SCCalc::DirectionType directionType = m_pFaultCalc->GetDirection();
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_DIRECTION, new CDsmStateFaultCalcRecord(strCaption, _T("")));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_DIRECTION, 0, SFCDirection);
    pOptions = m_ctlFaultCalcReport.GetRecordRow(2)->GetItem(1)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_POSITIVE), SCCalc::POSITIVE);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_STATE_NEGATIVE), SCCalc::NEGATIVE);

    pItemCon = pOptions->FindConstraint(directionType);
    if (pItemCon)
    {
        pItem = (CELRecordItemText*)m_ctlFaultCalcReport.GetRecordRow(2)->GetItem(1);
        if (pItem)
        {
            pItem->SetValue(pItemCon->GetConstraintCaption());
        }
    }
    
    //额定电压
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_RATED_VOLTAGE);
    strContent.Format(_T("%.03lf"), m_pFaultCalc->GetRatedVoltage());
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_RATED_VOLTAGE, new CDsmStateFaultCalcRecord(strCaption, strContent));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_RATED_VOLTAGE, 0, SFCRatedVoltage);
    
    //负荷电流
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LOAD_CURRENT);
    strContent.Format(_T("%.03lf"), m_pFaultCalc->GetLoadCurrent());
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LOAD_CURRENT, new CDsmStateFaultCalcRecord(strCaption, strContent));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LOAD_CURRENT, 0, SFCLoadCurrent);
    
    //负荷功角
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LOAD_ANGLE);
    strContent.Format(_T("%.03lf"), m_pFaultCalc->GetLoadAngle());
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LOAD_ANGLE, new CDsmStateFaultCalcRecord(strCaption, strContent));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LOAD_ANGLE, 0, SFCLoadAngle);


    //线路阻抗z1
    strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LINE_IMPEDANCE_Z1);
    CImpedance impdance = m_pFaultCalc->GetLineImpedanceZl();

    strContent.Format(_T("%.03lf∠%.03lf°"), impdance.GetImpedance(), impdance.GetAngle());
    m_ctlFaultCalcReport.InsertRecordRow(DSM_SFC_INDEX_LINE_IMPEDANCE_Z1, new CDsmStateFaultCalcRecord(strCaption, strContent));
    m_ctlFaultCalcReport.SetRowItemData(DSM_SFC_INDEX_LINE_IMPEDANCE_Z1, 0, SFCLineImpedanceZ1);

    _resetPartReport();

    m_ctlFaultCalcReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i <m_ctlFaultCalcReport.GetRecordRowCount(); i++)
    {
        m_ctlFaultCalcReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateFaultCalcDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageEnter)
    {
        m_ctlFaultCalcReport.SetFocusedSelectionRowIndex(0);
    }
    m_ctlFaultCalcReport.SetFocus();
    m_ctlFaultCalcReport.ShowWindow(SW_SHOW);

}

void CStateFaultCalcDlg::_resetPartReport()
{

    CString strCaption = _T("");
    CString strContent = _T("");

    m_ctlFaultCalcReport.SetRedraw(FALSE);
    
    while(m_ctlFaultCalcReport.GetRecordRowCount() > DSM_SFC_INDEX_NEXT)
    {
        m_ctlFaultCalcReport.DeleteRecordRow(DSM_SFC_INDEX_NEXT);
    }

    int nNextIndex = DSM_SFC_INDEX_NEXT;
    SCCalc::SCType faultType = m_pFaultCalc->GetFaultType();
    SCCalc::CalcModelType calcModeType = m_pFaultCalc->GetCalcModelType();
    
    if (faultType == SCCalc::A_GROUND  ||
        faultType == SCCalc::B_GROUND  ||
        faultType == SCCalc::C_GROUND  ||
        faultType == SCCalc::AB_GROUND ||
        faultType == SCCalc::BC_GROUND ||
        faultType == SCCalc::CA_GROUND )
    {

        //有线路零序补偿系数K1
        strCaption = dsm_stringMgr::Get(DSM_STR_STATE_LINE_COMPENSATE_K1);
        CCompensate compensate = m_pFaultCalc->GetLineCompensateKl();
        strContent.Format(_T("%.03lf∠%.03lf"), compensate.GetModelValue(), compensate.GetAngle());
        m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
        m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCLineCompensateK1);
        ++nNextIndex;

        switch (calcModeType)
        {
        case SCCalc::SYSTEM_IMPEDANCE_FIXED:
            {
                //Zs
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_ZS);
                CImpedance impedance = m_pFaultCalc->GetSystemImpedanceZs();
                strContent.Format(_T("%.03lf∠%.03lf"), impedance.GetImpedance(), impedance.GetAngle());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemImpdanceZs);
                ++nNextIndex;

                //Ks
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_COMPENSTATE_KS);
                CCompensate compensate = m_pFaultCalc->GetSystemCompensateKs();
                strContent.Format(_T("%.03lf∠%.03lf"), compensate.GetModelValue(), compensate.GetAngle());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemCompensateKs);

            }
            break;
        case SCCalc::VOLTAGE_FIXED:
            {
                //故障电压
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_VOLTAGE);
                strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultVoltage);
            }
            break;
        case SCCalc::CURRENT_FIXED:
            {
                //故障电流
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_CURRENT);
                strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultCurrent);
            }
            break;
        }
    
    }
    else
    {
        switch (calcModeType)
        {
        case SCCalc::SYSTEM_IMPEDANCE_FIXED:
            {
                //Zs
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_SYSTEM_IMPEDANCE_ZS);
                CImpedance impedance = m_pFaultCalc->GetSystemImpedanceZs();
                strContent.Format(_T("%.03lf∠%.03lf"), impedance.GetImpedance(), impedance.GetAngle());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCSystemImpdanceZs);
                ++nNextIndex;
            }
            break;
        case SCCalc::VOLTAGE_FIXED:
            {
                //故障电压
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_VOLTAGE);
                strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultVoltage);
            }
            break;
        case SCCalc::CURRENT_FIXED:
            {
                //故障电流
                strCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_CURRENT);
                strContent.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
                m_ctlFaultCalcReport.InsertRecordRow(nNextIndex, new CDsmStateFaultCalcRecord(strCaption, strContent));
                m_ctlFaultCalcReport.SetRowItemData(nNextIndex, 0, SFCFaultCurrent);
            }
            break;
        }
    }
    for (int i = DSM_SFC_INDEX_NEXT; i < m_ctlFaultCalcReport.GetRecordRowCount(); ++i)
    {
        m_ctlFaultCalcReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
    m_ctlFaultCalcReport.SetRedraw(TRUE);
}

void CStateFaultCalcDlg::OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }
    
    SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(0)->GetItemData();


    switch(sfcType)
    {
    case SFCFaultType:
        {
            m_pFaultCalc->SetFaultType((SCCalc::SCType)pItemNotify->pConstraint->GetConstraintData());
            _resetPartReport();
            *pResult = TRUE;
        }
        break;
    case SFCCalcModeType:
        {
            m_pFaultCalc->SetCalcModelType((SCCalc::CalcModelType)pItemNotify->pConstraint->GetConstraintData());
            _resetPartReport();
            *pResult = TRUE;
        }
        break;
    case SFCDirection:
        {
            m_pFaultCalc->SetDirection((SCCalc::DirectionType)pItemNotify->pConstraint->GetConstraintData());
            *pResult = TRUE;
        }
        break;
    }
}

void CStateFaultCalcDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit弹出时, 切换菜单
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlFaultCalcReport.GetInplaceEdit());
        *pResult = TRUE;
    }
    // inplace退出时，切换菜单
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}
void CStateFaultCalcDlg::OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }
    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    if (pItem == NULL)
    {
        return;
    }

    CString strValue = pItem->GetValue();
    double dValue = 0.0f;

    SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(0)->GetItemData();

    switch(sfcType)
    {
    case SFCRatedVoltage:
        {
            if (DSM_Str2Double(strValue, dValue))
            {
                m_pFaultCalc->SetRatedVoltage(dValue);
            }
            strValue.Format(_T("%.03lf"), m_pFaultCalc->GetRatedVoltage());
            pItem->SetValue(strValue);

            *pResult = TRUE;
        }
        break;
    case SFCLoadCurrent:
        {
            if (DSM_Str2Double(strValue, dValue))
            {
                m_pFaultCalc->SetLoadCurrent(dValue);
            }
            strValue.Format(_T("%.03lf"), m_pFaultCalc->GetLoadCurrent());
            pItem->SetValue(strValue);

            *pResult = TRUE;

        }
        break;
    case SFCLoadAngle:
        {
            if (DSM_Str2Double(strValue, dValue))
            {
                m_pFaultCalc->SetLoadAngle(dValue);
            }
            strValue.Format(_T("%.03lf"), m_pFaultCalc->GetLoadAngle());
            pItem->SetValue(strValue);

            *pResult = TRUE;

        }
        break;
    case SFCFaultCurrent:
        {
            if (DSM_Str2Double(strValue, dValue))
            {
                m_pFaultCalc->SetFaultCurrent(dValue);
            }
            strValue.Format(_T("%.03lf"), m_pFaultCalc->GetFaultCurrent());
            pItem->SetValue(strValue);

            *pResult = TRUE;
        }
        break;
    case SFCFaultVoltage:
        {
            if (DSM_Str2Double(strValue, dValue))
            {
                m_pFaultCalc->SetFaultVoltage(dValue);
            }
            strValue.Format(_T("%.03lf"), m_pFaultCalc->GetFaultVoltage());
            pItem->SetValue(strValue);

            *pResult = TRUE;
        }
        break;
    }
}


void CStateFaultCalcDlg::OnItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{

    UNREFERENCED_PARAMETER(pNotifyStruct);
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    int nRowIndex = m_ctlFaultCalcReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }

    SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(nRowIndex)->GetItem(0)->GetItemData();

    switch (sfcType)
    {
    case SFCLineImpedanceZ1:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_IMPEDANCE_DLG);
            *pResult = TRUE;
        }
        break;
    case SFCLineCompensateK1:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_COMPENSATE_DLG);
            *pResult = TRUE;
        }
        break;
    case SFCSystemImpdanceZs:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_IMPEDANCE_DLG);
            *pResult = TRUE;
        }
        break;
    case SFCSystemCompensateKs:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_FAULT_COMPENSATE_DLG);
            *pResult = TRUE;
        }
        break;
    }
}


void CStateFaultCalcDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateFaultCalcDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CStateFaultCalcDataExtra(pData);
    }

    CStateFaultCalcDataExtra* pDataExtra = dynamic_cast<CStateFaultCalcDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }

    pDataExtra->m_nFocusRowIndex = m_ctlFaultCalcReport.GetFocusedRowIndex();

    if (pDataExtra->m_nFocusRowIndex >= 0)
    {

        SFCRecordFuncType sfcType = (SFCRecordFuncType)m_ctlFaultCalcReport.GetRecordRow(pDataExtra->m_nFocusRowIndex)->GetItem(0)->GetItemData();
        switch (sfcType)
        {
        case SFCLineImpedanceZ1:
            {
                pDataExtra->m_eStateFaultSetType = eSFSTImpedanceZ1;
            }
            break;
        case SFCLineCompensateK1:
            {
                pDataExtra->m_eStateFaultSetType = eSFSTCompensateK1;
            }
            break;
        case SFCSystemImpdanceZs:
            {
                pDataExtra->m_eStateFaultSetType = eSFSTImpedanceZs;
            }
            break;
        case SFCSystemCompensateKs:
            {
                pDataExtra->m_eStateFaultSetType = eSFSTCompensateKs;
            }
            break;
        default:
            {
                pDataExtra->m_eStateFaultSetType = eSFSTOther;
            }
        }
    }

    //
    //保存参数配置
    //
    
    m_pConfigStateSequ->SaveConfig();
}

void CStateFaultCalcDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateFaultCalcDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CStateFaultCalcDataExtra* pDataExtra = dynamic_cast<CStateFaultCalcDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }

        if (pDataExtra->m_nFocusRowIndex >= 0 && pDataExtra->m_nFocusRowIndex < m_ctlFaultCalcReport.GetRecordRowCount())
        {
            m_ctlFaultCalcReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            m_ctlFaultCalcReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
    }
}
void CStateFaultCalcDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if (nIDEvent == 1)
    {
        m_pConfigStateSequ->SaveConfig();
    }

    CBaseDlg::OnTimer(nIDEvent);
}


DWORD WINAPI DsmFaultCalcConfirm(WPARAM wParam , LPARAM/* lParam*/ , void* arg)
{
    CDialog* pDlg = (CDialog*)arg;
    
    UINT nMenuID = LOWORD(wParam);
    
    pDlg->EndDialog(nMenuID);

    return 0;
}

BOOL CStateFaultCalcDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    
    if (pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
#if SU_FEI
                CMsgDlg msgdlg(DSM_STR_STATE_CONFIRM_FAULT_CALC, 0, this);
#else
				CMsgBtnDlg msgdlg(DSM_STR_STATE_CONFIRM_FAULT_CALC, 0, this);
#endif
                msgdlg._SetTextRC(DSM_STATE_FAULT_CALC_TIP);
                msgdlg._SetMenu(IDR_MENU_STATE_FAULT_CALC_CONFIRM);
                msgdlg._SetBitmapDisp(false);
                msgdlg._SetCommandRange(ID_MENU_STATE_FAULT_CALC_CONFIRM_F4, ID_MENU_STATE_FAULT_CALC_CONFIRM_F6, DsmFaultCalcConfirm, (void*)&msgdlg);
                INT_PTR nRet = msgdlg.DoModal();
                msgdlg._ResetMenu();
                switch(nRet)
                {
                case ID_MENU_STATE_FAULT_CALC_CONFIRM_F4:
                    {
                        m_pConfigStateSequ->UseFaultCalc(m_pStatus, 1);
                    }
                    break;
                case ID_MENU_STATE_FAULT_CALC_CONFIRM_F5:
                    {
                        m_pConfigStateSequ->UseFaultCalc(m_pStatus, 2);
                    }
                    break;
#if !SU_FEI
				case 2:
					{
						m_pConfigStateSequ->UseFaultCalc(m_pStatus, 1);
					}
					break;
				case 3:
					{
						m_pConfigStateSequ->UseFaultCalc(m_pStatus, 2);
					}
					break;
#endif
                default:
                    break;
                }
                CEventProc::GetInstance()->OnKeyEsc();
                return TRUE;
            }
        }
    }


    return CBaseDlg::PreTranslateMessage(pMsg);
}


#endif