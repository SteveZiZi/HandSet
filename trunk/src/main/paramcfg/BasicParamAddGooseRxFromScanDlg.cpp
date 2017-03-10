/// @file
///
/// @brief
///     ��ɨ���б������goose�Ի��� ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "BasicParamDlg.h"
#include "BasicParamAddGooseRxFromScanDlg.h"
#include "src/service/datalayer/scl/SCLManager.h"

#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/config/ParamConfigGooseSend.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"


#include "BasicParamGooseRxDlg.h"


// CAddGooseFromScanDlg �Ի���

IMPLEMENT_DYNAMIC(CBasicParamAddGooseRxFromScanDlg, CBaseDlg)

CBasicParamAddGooseRxFromScanDlg::CBasicParamAddGooseRxFromScanDlg(CWnd* pParent /*=NULL*/)
: CBaseDlg(CBasicParamAddGooseRxFromScanDlg::IDD, DSM_STR_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_CAPTION, pParent)
{
	m_pDataLayer = NULL;
	m_nMenuID = IDR_MENU_EMPTY;
	m_nMsgParamType =  DSM_MSG_GOOSE_RX_PARAM;
}

CBasicParamAddGooseRxFromScanDlg::~CBasicParamAddGooseRxFromScanDlg()
{
}

void CBasicParamAddGooseRxFromScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamAddGooseRxFromScanDlg, CBaseDlg)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAddGooseFromScanDlg ��Ϣ�������

BOOL CBasicParamAddGooseRxFromScanDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_pDataLayer = CDsmDataLayer::getInstance();
	if (m_pDataLayer == NULL)
	{
		return FALSE;
	}

	CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();

	if (pPageData == NULL || 
		pPageData->m_nPageID != CBasicParamGooseRxDlg::IDD || 
		pPageData->m_pPageDataExtra == NULL)

	{
		return FALSE;
	}

	CBasicParamPageDataExtraGooseRx* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGooseRx*>(pPageData->m_pPageDataExtra);

	if (pDataExtra == NULL)
	{
		return FALSE;
	}

	//
	//����goose���ͺ�goose���ղ�ͬ����ʼ��ѡ�е�goose���ƿ�
	//
	m_nMsgParamType = pDataExtra->m_nMsgParamType;

	SetTimer(1, 300, NULL);

	m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);

	_creatCtrlBlockReport();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CBasicParamAddGooseRxFromScanDlg::_creatCtrlBlockReport()
{
	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCAN_RET);

	// ��������
	m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlCtrlBlockReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlCtrlBlockReport.PostInitTemplateStyle();
	// ������Ԫ��

	
	m_ctlCtrlBlockReport.InsertColumn(0, L"   " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_APPID),      EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_APPID);
	m_ctlCtrlBlockReport.InsertColumn(1, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT), EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHAN_COUNT);
	m_ctlCtrlBlockReport.InsertColumn(2, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_DESC),  EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CTRL_DESC);
	m_ctlCtrlBlockReport.InsertColumn(3, L"  " + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_PORT),       EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_PORT);
	m_ctlCtrlBlockReport.InsertColumn(4, _T(""),                                             EL_CENTER, DSM_BASIC_GOOSE_ADD_WIDTH_CHECKBOX);
	// ����ѡ�񽹵���ʽ
	m_ctlCtrlBlockReport.SetRowSelectionVisible(TRUE);
	m_ctlCtrlBlockReport.SetRowFocusEnable(TRUE);
	m_ctlCtrlBlockReport.SetSubItemFocusEnable(TRUE);
	m_ctlCtrlBlockReport.SetFreezeSelectionVisible(TRUE);
	m_ctlCtrlBlockReport.SetFocusedColIndex(4);

	m_ctlCtrlBlockReport.SetFocus();
	m_ctlCtrlBlockReport.ShowWindow(SW_SHOW);

#if !SU_FEI
	m_ctlCtrlBlockReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

#endif


	//��ʼ������
	//
	_updateDetectGooseReport();

}

void CBasicParamAddGooseRxFromScanDlg::_updateDetectGooseReport()
{
	if (!m_ctlCtrlBlockReport.GetSafeHwnd())
	{
		return;
	}

	int nRowCount = m_ctlCtrlBlockReport.GetRecordRowCount();
	int nDetectCount = m_pDataLayer->GetNetDetectItemsCount();
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

			if (m_nMsgParamType == DSM_MSG_GOOSE_RX_PARAM)
			{
				m_ctlCtrlBlockReport.InsertRecordRow(i, new CDsmSelGooseCtrlFromScanRecord(pNetItem, eGooseRec));
			}
			
			m_ctlCtrlBlockReport.SetRecordRowData(i, (DWORD_PTR)pNetItem);

			if(i == 0)
			{
				m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
			}
		}


	}
}

void CBasicParamAddGooseRxFromScanDlg::_SavePage()
{
	CParamConfigGOOSEBase* pParamConfigGooseBase = NULL;
	if (m_nMsgParamType == DSM_MSG_GOOSE_RX_PARAM)
	{
		pParamConfigGooseBase = CParamConfigGOOSERecv::getInstance();
	}

	CNetDetectItem* pNetDetectItem = NULL;
	CELRecordItemText* pItem = NULL;
	for (int i = 0; i < m_ctlCtrlBlockReport.GetRecordRowCount(); ++i)
	{
		pNetDetectItem = (CNetDetectItem*) m_ctlCtrlBlockReport.GetRecordRowData(i);
		pItem = dynamic_cast<CELRecordItemText*>(m_ctlCtrlBlockReport.GetRecordRow(i)->GetItem(4));
		if (pNetDetectItem != NULL && pItem != NULL)
		{
			if (pItem->GetHasCheckbox() && pItem->IsChecked())
			{
				pParamConfigGooseBase->AddGocb(m_pDataLayer->GetDetectItemFrame(pNetDetectItem));
			}
		}
	}

	//
	//��������
	//
	pParamConfigGooseBase->SaveConfig();
}

BOOL CBasicParamAddGooseRxFromScanDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

	KillTimer(1);
	return CBaseDlg::DestroyWindow();
}

void CBasicParamAddGooseRxFromScanDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
	{
		_updateDetectGooseReport();
	}


	CBaseDlg::OnTimer(nIDEvent);
}
