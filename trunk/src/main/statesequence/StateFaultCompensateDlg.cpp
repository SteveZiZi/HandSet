/// @file
///
/// @brief
///     零序补偿 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///


#include "stdafx.h"

#include "StateFaultCompensateDlg.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/MsgDlg.h"
#include "src/main/mgr/StatusMgr.h"


#define DSM_STATE_FAULT_COMPENSATE_WIDTH_CAPTION0            120
#define DSM_STATE_FAULT_COMPENSATE_WIDTH_CONTENT0            145
#define DSM_STATE_FAULT_COMPENSATE_WIDTH_CAPTION1            120
#define DSM_STATE_FAULT_COMPENSATE_WIDTH_CONTENT1            144

#define DSM_SFC_ROW0_INDEX                     0                       ///< 
#define DSM_SFC_ROW1_INDEX                     1                       ///< 
#define DSM_SFC_ROW0_COL1_MODEL_VALUE          1                       ///< 模值
#define DSM_SFC_ROW1_COL1_ANGLE                1                       ///< 辐角
#define DSM_SFC_ROW0_COL3_REAL                 3                       ///< 实部
#define DSM_SFC_ROW1_COL3_VIRTUAL              3                       ///< 虚部 



// CStateFaultCompensateDlg 对话框

IMPLEMENT_DYNAMIC(CStateFaultCompensateDlg, CBaseDlg)

CStateFaultCompensateDlg::CStateFaultCompensateDlg(CWnd* pParent /*=NULL*/)
: CBaseDlg(CStateFaultCompensateDlg::IDD, DSM_STR_STATE_FAULT_COMPENSATE_K1_CAPTION, pParent)
, m_eStateFaultSetType(eSFSTImpedanceZ1)
{
    m_nMenuID = IDR_MENU_STATE_COMPENSATE;
}

CStateFaultCompensateDlg::~CStateFaultCompensateDlg()
{
}

void CStateFaultCompensateDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateFaultCompensateDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_FAULT_COMPENSATE_RET, &CStateFaultCompensateDlg::OnItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_FAULT_COMPENSATE_RET,  &CStateFaultCompensateDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_FAULT_COMPENSATE_RET,  &CStateFaultCompensateDlg::OnInplaceEditAction)
    ON_COMMAND_RANGE(ID_MENU_STATE_COMPENSATE_F1,   ID_MENU_STATE_COMPENSATE_F6, &CStateFaultCompensateDlg::OnNaviMenuCompensate)


END_MESSAGE_MAP()


// CStateFaultCompensateDlg 消息处理程序

BOOL CStateFaultCompensateDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CPageData* pData = CEventProc::GetInstance()->GetPreviousPageData();
#if CHUN_HUA
    if(!pData || CStateFaultCalcDlg::IDD != pData->m_nPageID)
#else
	 if(!pData || CStateSetDlg::IDD != pData->m_nPageID)
#endif
    {
        return FALSE;
    }

#if CHUN_HUA
    CStateFaultCalcDataExtra* pDataExtra = dynamic_cast<CStateFaultCalcDataExtra*>(pData->m_pPageDataExtra);
#else
	CStateSetDataExtra* pDataExtra = dynamic_cast<CStateSetDataExtra*>(pData->m_pPageDataExtra);
#endif
	ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return FALSE;
    }
    m_eStateFaultSetType = pDataExtra->m_eStateFaultSetType;


    CParamConfigStateSequence* pConfigStateSequ = CParamConfigStateSequence::getInstance();
    if (pConfigStateSequ && pConfigStateSequ->GetFaultCalc())
    {
        if (m_eStateFaultSetType == eSFSTCompensateK1)
        {
            m_compensate = pConfigStateSequ->GetFaultCalc()->GetLineCompensateKl();
        }
        else if (m_eStateFaultSetType == eSFSTCompensateKs)
        {
            m_compensate = pConfigStateSequ->GetFaultCalc()->GetSystemCompensateKs();
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    _createReport();


    CString csCaption = _T("");

    if (m_eStateFaultSetType == eSFSTImpedanceZ1)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_IMPEDANCE_Z1_CAPTION);
    }
    else if (m_eStateFaultSetType == eSFSTImpedanceZs)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_IMPEDANCE_ZS_CAPTION);
    }

    CStatusMgr::SetTitle(csCaption);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}



void CStateFaultCompensateDlg::_createReport()
{

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
   // int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + 2 * ELT_T1_ROWHEIGHT;//nRows * ELT_T1_ROWHEIGHT;

    m_ctlCompensateReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_FAULT_COMPENSATE_RET);

    // 设置字体
    m_ctlCompensateReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlCompensateReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlCompensateReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_COMPENSATE_WIDTH_CAPTION0,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlCompensateReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_COMPENSATE_WIDTH_CONTENT0);
    // 插入列元素
    m_ctlCompensateReport.InsertColumn(2,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_COMPENSATE_WIDTH_CAPTION1,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlCompensateReport.InsertColumn(3,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_COMPENSATE_WIDTH_CONTENT1);

    // 设置选择焦点样式
    m_ctlCompensateReport.SetRowSelectionVisible(FALSE);
    m_ctlCompensateReport.SetRowFocusEnable(TRUE);
    m_ctlCompensateReport.SetSubItemFocusEnable(TRUE);
    m_ctlCompensateReport.SetFreezeColumnsCount(1);
    m_ctlCompensateReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCompensateReport.SetFocusedColIndex(1);
    m_ctlCompensateReport.SetFocus();

    CString strCaption0 = _T("");
    CString strContent0 = _T("");
    CString strCaption1 = _T("");
    CString strContent1 = _T("");

    //模值
    strCaption0 = dsm_stringMgr::Get(DSM_STR_STATE_MODEL_VALUE);
    strContent0.Format(_T("%.03lf"), m_compensate.GetModelValue());
    //实部
    strCaption1 = dsm_stringMgr::Get(DSM_STR_STATE_REAL);
    strContent1.Format(_T("%.03lf"), m_compensate.GetReal());

    m_ctlCompensateReport.InsertRecordRow(DSM_SFC_ROW0_INDEX, new CDsmStateFaultCompensateRecord(strCaption0, strContent0, strCaption1, strContent1));

    //辐角
    strCaption0 = dsm_stringMgr::Get(DSM_STR_STATE_AMPL_ANGLE);
    strContent0.Format(_T("%.03lf"), m_compensate.GetAngle());
    //虚部
    strCaption1 = dsm_stringMgr::Get(DSM_STR_STATE_VIRTUAL);
    strContent1.Format(_T("%.03lf"), m_compensate.GetVirtual());
    m_ctlCompensateReport.InsertRecordRow(DSM_SFC_ROW1_INDEX, new CDsmStateFaultCompensateRecord(strCaption0, strContent0, strCaption1, strContent1));


    m_ctlCompensateReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlCompensateReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i <m_ctlCompensateReport.GetRecordRowCount(); i++)
    {
        m_ctlCompensateReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
        m_ctlCompensateReport.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
    }

    m_ctlCompensateReport.SetFocusedSelectionRowIndex(0);
    m_ctlCompensateReport.SetFocus();
    m_ctlCompensateReport.ShowWindow(SW_SHOW);
}


void  CStateFaultCompensateDlg::_updateReport()
{
    
    CELRecordItemText* pItem = NULL;
    CString strContent = _T("");

    //模值
    strContent.Format(_T("%.03lf"), m_compensate.GetModelValue());
    pItem = (CELRecordItemText*)m_ctlCompensateReport.GetRecordRow(DSM_SFC_ROW0_INDEX)->GetItem(DSM_SFC_ROW0_COL1_MODEL_VALUE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    //辐角
    strContent.Format(_T("%.03lf"), m_compensate.GetAngle());
    pItem = (CELRecordItemText*)m_ctlCompensateReport.GetRecordRow(DSM_SFC_ROW1_INDEX)->GetItem(DSM_SFC_ROW1_COL1_ANGLE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }
    //实部
    strContent.Format(_T("%.03lf"), m_compensate.GetReal());
    pItem = (CELRecordItemText*)m_ctlCompensateReport.GetRecordRow(DSM_SFC_ROW0_INDEX)->GetItem(DSM_SFC_ROW0_COL3_REAL);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    //虚部
    strContent.Format(_T("%.03lf"), m_compensate.GetVirtual());
    pItem = (CELRecordItemText*)m_ctlCompensateReport.GetRecordRow(DSM_SFC_ROW1_INDEX)->GetItem(DSM_SFC_ROW1_COL3_VIRTUAL);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }
    m_ctlCompensateReport.RedrawRowItems(0, m_ctlCompensateReport.GetRecordRowCount()-1);
}

void CStateFaultCompensateDlg::OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
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
    float fValue = 0.0f;

    switch (pItemNotify->rowIndex)
    {
    case DSM_SFC_ROW0_INDEX:
        {
            if (pItemNotify->colIndex == DSM_SFC_ROW0_COL1_MODEL_VALUE)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_compensate.SetModelValue(fValue);
                }
                strValue.Format(_T("%.03lf"), m_compensate.GetModelValue());
                pItem->SetValue(strValue);
            }
            else if (pItemNotify->colIndex == DSM_SFC_ROW0_COL3_REAL)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_compensate.SetReal(fValue);
                }
                strValue.Format(_T("%.03lf"), m_compensate.GetReal());
                pItem->SetValue(strValue);
            }
            _updateReport();
            *pResult = TRUE;
        }
        break;
    case DSM_SFC_ROW1_INDEX:
        {
            if (pItemNotify->colIndex == DSM_SFC_ROW1_COL1_ANGLE)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_compensate.SetAngle(fValue);
                }
                strValue.Format(_T("%.03lf"), m_compensate.GetAngle());
                pItem->SetValue(strValue);
            }
            else if(pItemNotify->colIndex == DSM_SFC_ROW1_COL3_VIRTUAL)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_compensate.SetVirtual(fValue);
                }
                strValue.Format(_T("%.03lf"), m_compensate.GetVirtual());
                pItem->SetValue(strValue);
            }
            _updateReport();
            *pResult = TRUE;
        }
        break;
    }
}


void CStateFaultCompensateDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit弹出时, 切换菜单
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlCompensateReport.GetInplaceEdit());
        *pResult = TRUE;
    }
    // inplace退出时，切换菜单
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}

void CStateFaultCompensateDlg::OnNaviMenuCompensate(UINT nID)
{
    switch(nID)
    {
#if SHENG_ZE
    case ID_MENU_STATE_COMPENSATE_F5:
#else
	case ID_MENU_STATE_COMPENSATE_F1:
#endif
        {
            CStatusMgr::SetTips(_T(""), 100);
            CEventProc::GetInstance()->OnKeyEsc();
        }
        break;
#if SHENG_ZE
    case ID_MENU_STATE_COMPENSATE_F6:
#else
	case ID_MENU_STATE_COMPENSATE_F2:
#endif
        {
            CParamConfigStateSequence* pConfigStateSequ = CParamConfigStateSequence::getInstance();
            if (pConfigStateSequ && pConfigStateSequ->GetFaultCalc())
            {
                if (m_eStateFaultSetType == eSFSTCompensateK1)
                {
                    pConfigStateSequ->GetFaultCalc()->SetLineCompensateKl(m_compensate);
                }
                else if (m_eStateFaultSetType == eSFSTCompensateKs)
                {
                    pConfigStateSequ->GetFaultCalc()->SetSystemCompensateKs(m_compensate);
                }
            }
            CStatusMgr::SetTips(_T(""), 100);
            CEventProc::GetInstance()->OnKeyEsc();
        }
        break;
    }
}


BOOL CStateFaultCompensateDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    

    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
                if (m_eStateFaultSetType == eSFSTCompensateK1)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_CONFIRM_SET_COMPENSATE_K1), 3000);
                }
                else 
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_CONFIRM_SET_COMPENSATE_Ks), 3000);
                }
                return TRUE;
            }
            break;
        }
    }

    return CBaseDlg::PreTranslateMessage(pMsg);
}
