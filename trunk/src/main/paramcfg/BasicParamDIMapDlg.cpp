/// @file
///
/// @brief
///     ����ӳ���Ի��� ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "BasicParamDIMapDlg.h"
#include "BasicParamDlg.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"


#include "BasicParamGooseRxDlg.h"


//////////////////////////////////////////////////////////////////////////
// ����ӳ��
//
#if SU_FEI
#define DSM_BASIC_DI_MAP_DI_CHAN                100
#define DSM_BASIC_DI_MAP_CTRL_BLOCK             250
#define DSM_BASIC_DI_MAP_CHAN                   250


#define GOOSE_REC_DI_MAP_MAX                    8
#else

///< ����ӳ���
#define DSM_BASIC_DI_MAP_DI_CHAN                100
#define DSM_BASIC_DI_MAP_CTRL_BLOCK             219
#define DSM_BASIC_DI_MAP_CHAN                   210

///< �����ʾ����ͨ����
#define GOOSE_REC_DI_MAP_MAX                    8

#endif


// CBasicParamDIMapDlg �Ի���

IMPLEMENT_DYNAMIC(CBasicParamDIMapDlg, CBaseDlg)

CBasicParamDIMapDlg::CBasicParamDIMapDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamDIMapDlg::IDD, DSM_STR_BASIC_PARAM_DI_MAP_CAPTION,  pParent)
    , m_uPrevDlgID(0)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CBasicParamDIMapDlg::~CBasicParamDIMapDlg()
{
}

void CBasicParamDIMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamDIMapDlg, CBaseDlg)
    ON_COMMAND(ID_MENU_PARAM_DI_MAP_F1, &CBasicParamDIMapDlg::OnMenuParamDiMapF1)
END_MESSAGE_MAP()


// CBasicParamDIMapDlg ��Ϣ�������

BOOL CBasicParamDIMapDlg::OnInitDialog()
{
    //
    //����Ǵ��ֶ����ý���ӳ�䣬�޸Ĳ˵�
    //
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL)
    {
        return FALSE;
    }

    m_uPrevDlgID = pPageData->m_nPageID;
#if SU_FEI
    if (m_uPrevDlgID == CBasicParamDlg::IDD)
#else
	if (m_uPrevDlgID == CBasicParamGooseRxDlg::IDD)
#endif
    {
        m_nMenuID = IDR_MENU_MSG_PARAM_DI_MAP;
    }


    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _creatDIMapReport();

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

/**
 *      ��������ӳ�䱨��
 */
void CBasicParamDIMapDlg::_creatDIMapReport()
{

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlDIMapReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCAN_RET);

    // ��������
    m_ctlDIMapReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlDIMapReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlDIMapReport.PostInitTemplateStyle();
    // ������Ԫ��

    m_ctlDIMapReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DI_CHAN),      EL_CENTER, DSM_BASIC_DI_MAP_DI_CHAN);
    m_ctlDIMapReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_DESC),    EL_CENTER, DSM_BASIC_DI_MAP_CTRL_BLOCK);
    m_ctlDIMapReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN),         EL_LEFT, DSM_BASIC_DI_MAP_CHAN);

    // ����ѡ�񽹵���ʽ
    m_ctlDIMapReport.SetRowSelectionVisible(TRUE);
    m_ctlDIMapReport.SetRowFocusEnable(TRUE);
    m_ctlDIMapReport.SetSubItemFocusEnable(TRUE);
    m_ctlDIMapReport.SetFreezeSelectionVisible(TRUE);
    m_ctlDIMapReport.SetFocusedColIndex(0);

    m_ctlDIMapReport.SetFocus();
    m_ctlDIMapReport.ShowWindow(SW_SHOW);
    
    ////
    ////����ӳ����Ϣ
    ////
    CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
    if (pConfigGooseRec == NULL)
    {
        return;
    }
    pConfigGooseRec->RebuildGOOSERecvChannelMap();

    CString strCaption = _T("");
    CString strCtrlBlockDesc = _T("");
    CString strChanDesc = _T("");
    
    CGOOSERecvVirtualChannel* pVirChannel = NULL;
    CChannel* pChannel = NULL;
    CChannel* pParentChannel = NULL;
    CBaseCb* pBaseCb = NULL;

    for (int i = 0; i < 8; ++i)
    {
        pVirChannel = pConfigGooseRec->GetVirtualChannelByName(strGOOSERecvVCName[i]);
        if (pVirChannel)
        {
            pChannel = pVirChannel->GetChannel();
            if (pChannel)
            {
                pParentChannel = pChannel->GetParentChannel();


                //DI
                strCaption = strGOOSERecvVCName[i].c_str();

                //����
                pBaseCb = pChannel->GetParentBaseCb();
                if (pBaseCb)
                {
                    strCtrlBlockDesc.Format(_T("0x%04X-%s"), pBaseCb->GetAPPID(), pBaseCb->GetDescription().c_str());
                }
                //ͨ��
                if (pParentChannel)
                {
                    //��ͨ��
                    strChanDesc.Format(_T("%d.%d-%s"), pParentChannel->GetIndex()+1, pChannel->GetIndex() + 1, pChannel->GetDescription().c_str());
                }
                else
                {
                    strChanDesc.Format(_T("%d-%s"), pChannel->GetIndex() + 1, pChannel->GetDescription().c_str());
                }
                m_ctlDIMapReport.InsertRecordRow(i, new CDsmViewGooseRecMapRecord(strCaption, strCtrlBlockDesc, strChanDesc));
            }
            else
            {
                strCaption = strGOOSERecvVCName[i].c_str();
                strCtrlBlockDesc = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL);
                strChanDesc = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL);
                m_ctlDIMapReport.InsertRecordRow(i, new CDsmViewGooseRecMapRecord(strCaption, strCtrlBlockDesc, strChanDesc));
            }
        }
        else
        {
            ASSERT(FALSE);
        }
    }
    m_ctlDIMapReport.SetFocusedSelectionRowIndex(0);
}

void CBasicParamDIMapDlg::OnMenuParamDiMapF1()
{
    // TODO: �ڴ���������������
    //
    //���ӳ��
    //
    CString strCtrlBlockDesc = _T("");
    CString strChanDesc = _T("");
    CELRecordItemText* pItem = NULL;
    for (int i = 0; i < GOOSE_REC_DI_MAP_MAX; ++i)
    {
        pItem = (CELRecordItemText*)m_ctlDIMapReport.GetRecordRow(i)->GetItem(1);
        if (pItem)
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
            m_ctlDIMapReport.RedrawRowItem(i, 1);
        }

        pItem = (CELRecordItemText*)m_ctlDIMapReport.GetRecordRow(i)->GetItem(2);
        if (pItem)
        {
            pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
            m_ctlDIMapReport.RedrawRowItem(i, 2);
        }
    }
    
    //
    //���ӳ��
    //
    CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
    if (pConfigGooseRec == NULL)
    {
        return;
    }
    pConfigGooseRec->ClearGOOSERecvChannelMap();
  
}
