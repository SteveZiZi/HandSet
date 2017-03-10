/// @file
///
/// @brief
///     手动试验参数设置 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "ManualSetDlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/config/ParamconfigManualTest.h"

//////////////////////////////////////////////////////////////////////////
// 手动试验 - set报表
//
#define DSM_MANUAL_SET_WIDTH_CAPTION       180
#define DSM_MANUAL_SET_WIDTH_CONTENT       349


//#define MS_ITEM_SMV_CHANGE_ITEM           0
#define MS_ITEM_EFFECT_TYPE               0
#define MS_ITEM_START_TYPE                1


// CManualSetDlg 对话框

IMPLEMENT_DYNAMIC(CManualSetDlg, CBaseDlg)

CManualSetDlg::CManualSetDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualSetDlg::IDD, DSM_STR_MANUAL_SET_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CManualSetDlg::~CManualSetDlg()
{
}

void CManualSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSetDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_MANUAL_SET_RET, &CManualSetDlg::OnItemSelectChanged)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualSetDlg 消息处理程序

BOOL CManualSetDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    _createSetReport();

    CString csCaption;
    csCaption.Format(L"%s->%s", dsm_stringMgr::Get(DSM_STR_MANUAL_TEST_CAPTION), dsm_stringMgr::Get(DSM_STR_MANUAL_SET_CAPTION));
    CStatusMgr::SetTitle(csCaption);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CManualSetDlg::_createSetReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height()/ ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSetReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_MANUAL_SET_RET);

    // 设置字体
    m_ctlSetReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlSetReport.PostInitTemplateStyle();
    // 插入列元素

    m_ctlSetReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_MANUAL_SET_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlSetReport.InsertColumn(1,
        _T(""),
        EL_LEFT, 
        DSM_MANUAL_SET_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlSetReport.SetRowSelectionVisible(FALSE);
    m_ctlSetReport.SetRowFocusEnable(TRUE);
    m_ctlSetReport.SetSubItemFocusEnable(TRUE);
    m_ctlSetReport.SetFreezeColumnsCount(1);
    m_ctlSetReport.SetFreezeSelectionVisible(TRUE);
    m_ctlSetReport.SetFocusedColIndex(1);
    m_ctlSetReport.SetFocus();

    CParamconfigManualTest* pConfigManualTest = CParamconfigManualTest::getInstance();
    if (pConfigManualTest == NULL)
    {
        return;
    }

    //
    //SMV变化项
    //
    //m_ctlSetReport.InsertRecordRow(MS_ITEM_SMV_CHANGE_ITEM, 
    //    new CDsmManualSetRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_CHANGE_ITEM), _T("")));
    //CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetReport.GetRecordRow(MS_ITEM_SMV_CHANGE_ITEM)->GetItem(1);
    //pItem->GetEditOptions(NULL)->AddComboButton();
    //pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_AMPL), CHANGE_ITEM_AMPLITUDE);
    //pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_PHASE), CHANGE_ITEM_PHASE_ANGLE);
    ////pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_FREQUENCE), CHANGE_ITEM_FREQUENCY);
    //CELRecordItemConstraint* pItemCon = pItem->GetEditOptions(NULL)->FindConstraint(pConfigManualTest->GetSMVChangeItem());
    //if (pItemCon)
    //{
    //    pItem->SetValue(pItemCon->GetConstraintCaption());
    //}
    //else
    //{
    //    pItem->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN));
    //}

    //
    //生效方式
    //
    m_ctlSetReport.InsertRecordRow(MS_ITEM_EFFECT_TYPE,
        new CDsmManualSetRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_EFFECT_TYPE), _T("")));
    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetReport.GetRecordRow(MS_ITEM_EFFECT_TYPE)->GetItem(1);
    pItem->GetEditOptions(NULL)->AddComboButton();
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_EFFECT_IMMED), EFFECTIVE_MODE_IMMEDIATE);
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_EFFECT_MANUAL), EFFECTIVE_MODE_MANUALLY);
    CELRecordItemConstraint* pItemCon = pItem->GetEditOptions(NULL)->FindConstraint(pConfigManualTest->GetModifyEffectiveMode());
    if (pItemCon)
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }
    else
    {
        pItem->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN));
    }

    //
    //开始方式
    //
    m_ctlSetReport.InsertRecordRow(MS_ITEM_START_TYPE, 
        new CDsmManualSetRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_START_TYPE), _T("")));

    pItem = (CELRecordItemText*)m_ctlSetReport.GetRecordRow(MS_ITEM_START_TYPE)->GetItem(1);
    pItem->GetEditOptions(NULL)->AddComboButton();
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_START_APART), START_MODE_APART);
    pItem->GetEditOptions(NULL)->AddConstraint(dsm_stringMgr::Get(DSM_STR_MANUAL_START_TOGETHER), START_MODE_TOGETHER);
    pItemCon = pItem->GetEditOptions(NULL)->FindConstraint(pConfigManualTest->GetStartMode());
    if (pItemCon)
    {
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }
    else
    {
        pItem->SetValue(dsm_stringMgr::Get(DSM_STR_MANUAL_UNKNOWN));
    }

    m_ctlSetReport.SetFocusedSelectionRowIndex(0);
    m_ctlSetReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i <m_ctlSetReport.GetRecordRowCount(); i++)
    {
        m_ctlSetReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
    }
}

void CManualSetDlg::OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify)
    {
        return;
    }

    CParamconfigManualTest* pConfigManualTest = CParamconfigManualTest::getInstance();
    if (pConfigManualTest == NULL)
    {
        return;
    }

    switch(pItemNotify->rowIndex)
    {
    //case MS_ITEM_SMV_CHANGE_ITEM:
    //    {
    //        pConfigManualTest->SetSMVChangeItem((eSMVChangeItem)pItemNotify->pConstraint->GetConstraintData());
    //        *pResult = TRUE;
    //    }
    //    break;
    case MS_ITEM_EFFECT_TYPE:
        {
            pConfigManualTest->SetModifyEffectiveMode((eModifyEffectiveMode)pItemNotify->pConstraint->GetConstraintData());
            *pResult = TRUE;
        }
        break;
    case MS_ITEM_START_TYPE:
        {
            pConfigManualTest->SetStartMode((eStartMode)pItemNotify->pConstraint->GetConstraintData());
            *pResult = TRUE;
        }
        break;
    default:
        *pResult = FALSE;
        break;
    }
}
BOOL CManualSetDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    CParamconfigManualTest* pConfigManualTest = CParamconfigManualTest::getInstance();
    if (pConfigManualTest != NULL)
    {
        pConfigManualTest->SaveConfig();
    }
    return CBaseDlg::DestroyWindow();
}

void CManualSetDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        CParamconfigManualTest* pConfigManualTest = CParamconfigManualTest::getInstance();
        if (pConfigManualTest != NULL)
        {
            pConfigManualTest->SaveConfig();
        }
    }
    CBaseDlg::OnTimer(nIDEvent);
}
