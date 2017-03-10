// SmvPolarityCfgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SmvPolarityCfgDlg.h"
#include "src/utils/utility/dsm_utility.h"

// ���� - set����
//
#if SU_FEI
#define DSM_SMV_POLARITY_CFG_WIDTH_CAPTION       180
#define DSM_SMV_POLARITY_CFG_WIDTH_CONTENT       420
#else
#define DSM_SMV_POLARITY_CFG_WIDTH_CAPTION       140
#define DSM_SMV_POLARITY_CFG_WIDTH_CONTENT       353
#endif

#define DSM_SMV_POLARITY_CFG_SILL_VALUE_INDEX        0
#define DSM_SMV_POLARITY_CFG_NULL_SHIFT_VALUE_INDEX  1


///
/// @brief
///      ���Բ�������
///
class CDsmSmvPolarityCfgRecord : public CELRecord
{
public:
    CDsmSmvPolarityCfgRecord(CString strCaption, CString strContent);
    virtual ~CDsmSmvPolarityCfgRecord(){}
};

CDsmSmvPolarityCfgRecord::CDsmSmvPolarityCfgRecord(CString strCaption, CString strContent)
{
    AddItem(new CELRecordItemText(strCaption));
    
    CELRecordItem* pItem = AddItem(new CELRecordItemText(strContent));
    if (pItem)
    {
        pItem->SetEditable(TRUE);
    }
}


// CSmvPolarityCfgDlg �Ի���

IMPLEMENT_DYNAMIC(CSmvPolarityCfgDlg, CBaseDlg)

CSmvPolarityCfgDlg::CSmvPolarityCfgDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CSmvPolarityCfgDlg::IDD,DSM_STR_POLARITY_CFG_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CSmvPolarityCfgDlg::~CSmvPolarityCfgDlg()
{
}

void CSmvPolarityCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvPolarityCfgDlg, CBaseDlg)
    ON_WM_TIMER()
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_POLARITY_CFG_RET, &CSmvPolarityCfgDlg::OnCfgItemValueChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_POLARITY_CFG_RET,  &CSmvPolarityCfgDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_POLARITY_CFG_RET,  &CSmvPolarityCfgDlg::OnInplaceEditAction)
END_MESSAGE_MAP()


// CSmvPolarityCfgDlg ��Ϣ�������

BOOL CSmvPolarityCfgDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    m_pDataLayer = CDsmDataLayer::getInstance();
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }

    _createCfgReport();

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CSmvPolarityCfgDlg::_createCfgReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height()/ ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCfgReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_POLARITY_CFG_RET);

    // ��������
    m_ctlCfgReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlCfgReport.PostInitTemplateStyle();
    // ������Ԫ��

    m_ctlCfgReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_SMV_POLARITY_CFG_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlCfgReport.InsertColumn(1,
        _T(""),
        EL_LEFT, 
        DSM_SMV_POLARITY_CFG_WIDTH_CONTENT);

    // ����ѡ�񽹵���ʽ
    m_ctlCfgReport.SetRowSelectionVisible(FALSE);
    m_ctlCfgReport.SetRowFocusEnable(TRUE);
    m_ctlCfgReport.SetSubItemFocusEnable(TRUE);
    m_ctlCfgReport.SetFreezeColumnsCount(1);
    m_ctlCfgReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCfgReport.SetFocusedColIndex(1);
    m_ctlCfgReport.SetFocus();
    
    CSmvRecvConfigPublic * pConfigPublic = m_pDataLayer->GetSmvConfigPublic();
    if (pConfigPublic == NULL)
    {
        return;
    }
    
    CString strValue = _T("");
    strValue.Format(_T("%.03f"), pConfigPublic->GetSillValue());

    //
    //�ż�ֵ
    //
    m_ctlCfgReport.InsertRecordRow(DSM_SMV_POLARITY_CFG_SILL_VALUE_INDEX,
        new CDsmSmvPolarityCfgRecord(dsm_stringMgr::Get(DSM_STR_POLARITY_CFG_SILL_VALUE), strValue));
    //
    //��Ưֵ
    //
    strValue.Format(_T("%.03f"), pConfigPublic->GetNullShiftValue());
    m_ctlCfgReport.InsertRecordRow(DSM_SMV_POLARITY_CFG_NULL_SHIFT_VALUE_INDEX,
        new CDsmSmvPolarityCfgRecord(dsm_stringMgr::Get(DSM_STR_POLARITY_CFG_NULL_SHIFT_VALUE), strValue));

    m_ctlCfgReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);

	
	m_ctlCfgReport.SetFocus();
	m_ctlCfgReport.SetFocusedColIndex(1);
	m_ctlCfgReport.SetFocusedSelectionRowIndex(0);
}

void CSmvPolarityCfgDlg::OnCfgItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return;
    }

    CSmvRecvConfigPublic * pConfigPublic = m_pDataLayer->GetSmvConfigPublic();
    if (pConfigPublic == NULL)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlCfgReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex));
    if (pItem == NULL)
    {
        return;
    }



    CString strValue = pItem->GetValue();

    switch(pItemNotify->rowIndex)
    {
    case DSM_SMV_POLARITY_CFG_SILL_VALUE_INDEX:
        {
            float fValue = 0.0f;
            if (DSM_Str2Float(strValue, fValue))
            {
                pConfigPublic->SetSillValue(fValue);
            }
            ;
            fValue = pConfigPublic->GetSillValue();
            strValue.Format(_T("%.03f"), fValue);
            pItem->SetValue(strValue);
            
        }
        break;
    case DSM_SMV_POLARITY_CFG_NULL_SHIFT_VALUE_INDEX:
        {

            float fValue = 0.0f;
            if (DSM_Str2Float(strValue, fValue))
            {
                pConfigPublic->SetNullShiftValue(fValue);
            }
            ;
            fValue = pConfigPublic->GetNullShiftValue();
            strValue.Format(_T("%.03f"), fValue);
            pItem->SetValue(strValue);
        }
        break;
    default:
        break;
    }
}

void CSmvPolarityCfgDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit����ʱ, ����༭�˵�
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlCfgReport.GetInplaceEdit());
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace�˳�ʱ���˳��༭�˵�
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}
