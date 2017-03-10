/// @file
///
/// @brief
///     故障计算 阻抗 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"

#include "StateFaultImpedanceDlg.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/MsgDlg.h"
#include "src/main/mgr/StatusMgr.h"

#define DSM_STATE_FAULT_IMPEDANCE_WIDTH_CAPTION0            120
#define DSM_STATE_FAULT_IMPEDANCE_WIDTH_CONTENT0            145
#define DSM_STATE_FAULT_IMPEDANCE_WIDTH_CAPTION1            120
#define DSM_STATE_FAULT_IMPEDANCE_WIDTH_CONTENT1            144

#define DSM_SFC_ROW0_INDEX                                  0                      ///< 
#define DSM_SFC_ROW1_INDEX                                  1                      ///< 
#define DSM_SFC_ROW0_COL1_IMPEDANCE                         1                      ///< 阻抗
#define DSM_SFC_ROW0_COL3_RESISTANCE                        3                      ///< 阻抗角
#define DSM_SFC_ROW1_COL1_ANGLE                             1                      ///< 电阻
#define DSM_SFC_ROW1_COL3_REACTANCE                         3                      ///< 电抗 

// CStateFaultImpedanceDlg 对话框

IMPLEMENT_DYNAMIC(CStateFaultImpedanceDlg, CBaseDlg)

CStateFaultImpedanceDlg::CStateFaultImpedanceDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateFaultImpedanceDlg::IDD, DSM_STR_STATE_FAULT_IMPEDANCE_Z1_CAPTION, pParent)
    , m_eStateFaultSetType(eSFSTImpedanceZ1)
{
    m_nMenuID = IDR_MENU_STATE_IMPEDANCE;
}

CStateFaultImpedanceDlg::~CStateFaultImpedanceDlg()
{
}

void CStateFaultImpedanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateFaultImpedanceDlg, CBaseDlg)

    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_STATE_FAULT_IMPEDANCE_RET, &CStateFaultImpedanceDlg::OnItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_STATE_FAULT_IMPEDANCE_RET,  &CStateFaultImpedanceDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_STATE_FAULT_IMPEDANCE_RET,  &CStateFaultImpedanceDlg::OnInplaceEditAction)
    ON_COMMAND_RANGE(ID_MENU_STATE_IMPEDANCE_F1,   ID_MENU_STATE_IMPEDANCE_F6, &CStateFaultImpedanceDlg::OnNaviMenuCompensate)

END_MESSAGE_MAP()


// CStateFaultImpedanceDlg 消息处理程序

BOOL CStateFaultImpedanceDlg::OnInitDialog()
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
#endif    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return FALSE;
    }
    m_eStateFaultSetType = pDataExtra->m_eStateFaultSetType;

    CParamConfigStateSequence* pConfigStateSequ = CParamConfigStateSequence::getInstance();
    if (pConfigStateSequ && pConfigStateSequ->GetFaultCalc())
    {
        if (m_eStateFaultSetType == eSFSTImpedanceZ1)
        {
            m_impedance = pConfigStateSequ->GetFaultCalc()->GetLineImpedanceZl();
        }
        else if (m_eStateFaultSetType == eSFSTImpedanceZs)
        {
            m_impedance = pConfigStateSequ->GetFaultCalc()->GetSystemImpedanceZs();
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
    
    if (m_eStateFaultSetType == eSFSTCompensateK1)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_COMPENSATE_K1_CAPTION);
    }
    else if (m_eStateFaultSetType == eSFSTCompensateKs)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_STATE_FAULT_COMPENSATE_KS_CAPTION);
    }

    CStatusMgr::SetTitle(csCaption);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}



void CStateFaultImpedanceDlg::_createReport()
{

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
   // int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + 2 * ELT_T1_ROWHEIGHT;//nRows * ELT_T1_ROWHEIGHT;

    m_ctlImpedanceReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_FAULT_IMPEDANCE_RET);

    // 设置字体
    m_ctlImpedanceReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlImpedanceReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlImpedanceReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_IMPEDANCE_WIDTH_CAPTION0,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlImpedanceReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_IMPEDANCE_WIDTH_CONTENT0);
    m_ctlImpedanceReport.InsertColumn(2,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_IMPEDANCE_WIDTH_CAPTION1,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlImpedanceReport.InsertColumn(3,
        _T(""),
        EL_LEFT,
        DSM_STATE_FAULT_IMPEDANCE_WIDTH_CONTENT1);

    // 设置选择焦点样式
    m_ctlImpedanceReport.SetRowSelectionVisible(FALSE);
    m_ctlImpedanceReport.SetRowFocusEnable(TRUE);
    m_ctlImpedanceReport.SetSubItemFocusEnable(TRUE);
    m_ctlImpedanceReport.SetFreezeColumnsCount(1);
    m_ctlImpedanceReport.SetFreezeSelectionVisible(TRUE);
    m_ctlImpedanceReport.SetFocusedColIndex(1);
    m_ctlImpedanceReport.SetFocus();
    
    CString strCaption0 = _T("");
    CString strContent0 = _T("");
    CString strCaption1 = _T("");
    CString strContent1 = _T("");

    //阻抗
    strCaption0 = dsm_stringMgr::Get(DSM_STR_STATE_IMPEDANCE);
    strContent0.Format(_T("%.03lf"), m_impedance.GetImpedance());
    //电阻
    strCaption1 = dsm_stringMgr::Get(DSM_STR_STATE_RESISTANCE);
    strContent1.Format(_T("%.03lf"), m_impedance.GetResistance());
    m_ctlImpedanceReport.InsertRecordRow(DSM_SFC_ROW0_INDEX, new CDsmStateFaultImpedanceRecord(strCaption0, strContent0, strCaption1, strContent1));

    //阻抗角
    strCaption0 = dsm_stringMgr::Get(DSM_STR_STATE_IMPEDANCE_ANGLE);
    strContent0.Format(_T("%.03lf"), m_impedance.GetAngle());
    //电抗
    strCaption1 = dsm_stringMgr::Get(DSM_STR_STATE_REACTANCE);
    strContent1.Format(_T("%.03lf"), m_impedance.GetReactance());
    m_ctlImpedanceReport.InsertRecordRow(DSM_SFC_ROW1_INDEX, new CDsmStateFaultImpedanceRecord(strCaption0, strContent0, strCaption1, strContent1));

    m_ctlImpedanceReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlImpedanceReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i <m_ctlImpedanceReport.GetRecordRowCount(); i++)
    {
        m_ctlImpedanceReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
        m_ctlImpedanceReport.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
    }

    m_ctlImpedanceReport.SetFocusedSelectionRowIndex(0);
    m_ctlImpedanceReport.SetFocus();
    m_ctlImpedanceReport.ShowWindow(SW_SHOW);
}


void  CStateFaultImpedanceDlg::_updateReport()
{

    CELRecordItemText* pItem = NULL;
    CString strContent = _T("");

    //阻抗
    strContent.Format(_T("%.03lf"), m_impedance.GetImpedance());
    pItem = (CELRecordItemText*)m_ctlImpedanceReport.GetRecordRow(DSM_SFC_ROW0_INDEX)->GetItem(DSM_SFC_ROW0_COL1_IMPEDANCE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    //阻抗角
    strContent.Format(_T("%.03lf"), m_impedance.GetAngle());
    pItem = (CELRecordItemText*)m_ctlImpedanceReport.GetRecordRow(DSM_SFC_ROW1_INDEX)->GetItem(DSM_SFC_ROW1_COL1_ANGLE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    //电阻
    strContent.Format(_T("%.03lf"), m_impedance.GetResistance());
    pItem = (CELRecordItemText*)m_ctlImpedanceReport.GetRecordRow(DSM_SFC_ROW0_INDEX)->GetItem(DSM_SFC_ROW0_COL3_RESISTANCE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    //电抗
    strContent.Format(_T("%.03lf"), m_impedance.GetReactance());
    pItem = (CELRecordItemText*)m_ctlImpedanceReport.GetRecordRow(DSM_SFC_ROW1_INDEX)->GetItem(DSM_SFC_ROW1_COL3_REACTANCE);
    if (pItem)
    {
        pItem->SetValue(strContent);
    }

    m_ctlImpedanceReport.SetRedraw(TRUE);
    m_ctlImpedanceReport.RedrawRowItems(0, m_ctlImpedanceReport.GetRecordRowCount()-1);
}

void CStateFaultImpedanceDlg::OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
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
            if (pItemNotify->colIndex == DSM_SFC_ROW0_COL1_IMPEDANCE)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_impedance.SetImpedance(fValue);
                }
                strValue.Format(_T("%.03lf"), m_impedance.GetImpedance());
                pItem->SetValue(strValue);
            }
            else if (pItemNotify->colIndex == DSM_SFC_ROW0_COL3_RESISTANCE)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_impedance.SetResistance(fValue);
                }
                strValue.Format(_T("%.03lf"), m_impedance.GetResistance());
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
                    m_impedance.SetAngle(fValue);
                }
                strValue.Format(_T("%.03lf"), m_impedance.GetAngle());
                pItem->SetValue(strValue);
            }
            else if (pItemNotify->colIndex == DSM_SFC_ROW1_COL3_REACTANCE)
            {
                if (DSM_Str2Float(strValue, fValue))
                {
                    m_impedance.SetReactance(fValue);
                }
                strValue.Format(_T("%.03lf"), m_impedance.GetReactance());
                pItem->SetValue(strValue);
            }
            _updateReport();
            *pResult = TRUE;
        }
        break;
    }
}


void CStateFaultImpedanceDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    UNREFERENCED_PARAMETER(pResult);

    *pResult = FALSE;
    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    // inplaceEdit弹出时, 切换菜单
    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlImpedanceReport.GetInplaceEdit());
        *pResult = TRUE;
    }
    // inplace退出时，切换菜单
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
        *pResult = TRUE;
    }
}

void CStateFaultImpedanceDlg::OnNaviMenuCompensate(UINT nID)
{
    switch(nID)
    {
#if SHENG_ZE
    case ID_MENU_STATE_IMPEDANCE_F5:
#else
	case ID_MENU_STATE_IMPEDANCE_F1:
#endif
        {
            //取消
            CStatusMgr::SetTips(_T(""), 100);
            CEventProc::GetInstance()->OnKeyEsc();
        }
        break;
#if SHENG_ZE
    case ID_MENU_STATE_IMPEDANCE_F6:
#else
	case ID_MENU_STATE_IMPEDANCE_F2:
#endif
        {
            //确定
            CParamConfigStateSequence* pConfigStateSequ = CParamConfigStateSequence::getInstance();
            if (pConfigStateSequ && pConfigStateSequ->GetFaultCalc())
            {
                if (m_eStateFaultSetType == eSFSTImpedanceZ1)
                {
                    pConfigStateSequ->GetFaultCalc()->SetLineImpedanceZl(m_impedance);
                }
                else 
                {
                    pConfigStateSequ->GetFaultCalc()->SetSystemImpedanceZs(m_impedance);
                }
            }
            CStatusMgr::SetTips(_T(""), 100);
            CEventProc::GetInstance()->OnKeyEsc();
        }
        break;
    }
}

BOOL CStateFaultImpedanceDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类


    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
                if (m_eStateFaultSetType == eSFSTImpedanceZ1)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_CONFIRM_SET_IMPEDANCE_Z1), 3000);
                }
                else 
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_CONFIRM_SET_IMPEDANCE_Zs), 3000);
                }
                return TRUE;
            }
            break;
        }
    }

    return CBaseDlg::PreTranslateMessage(pMsg);
}
