// BasicParamGooseTxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "src/main/common.h"

#include "BasicParamGooseTxDlg.h"

#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/popdlg/progressbardlg.h"
#include "src/main/eventproc.h"
#include "src/utils/utility/dsm_utility.h"
//#include "src/main/dlg/timescan1588dlg.h"
#include "src/main/utils/utility.h"

#include "src/service/datalayer/dsmdatalayer.h"
#include "src/service/datalayer/config/goosecb.h"

#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/scl/SCLManager.h"
#include "src/service/datalayer/config/ParamConfigGooseSend.h"


#define MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX			0           ///< �������T0
#define MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX			1           ///< ��С���T1
#define MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX   2			///< GOOSE����


#define MSGPARAM_TICK_SPACE_MIN                  500         ///< ���������Сֵ
#define MSGPARAM_TICK_SPACE_MAX                  65535       ///< ����������ֵ

#define MSGPARAM_MIN_SPACE_MIN                  1            ///< ��С�����Сֵ
#define MSGPARAM_MIN_SPACE_MAX                  65535        ///< ��С������ֵ



// CBasicParamGooseTxDlg dialog

IMPLEMENT_DYNAMIC(CBasicParamGooseTxDlg, CBaseDlg)

CBasicParamGooseTxDlg::CBasicParamGooseTxDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamGooseTxDlg::IDD, DSM_STR_BASIC_PARAM_MSG, pParent)
{
	m_nMenuID	 = IDR_MENU_MSG_PARAM_GOOSE_SEND;

}

CBasicParamGooseTxDlg::~CBasicParamGooseTxDlg()
{
}

void CBasicParamGooseTxDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamGooseTxDlg, CBaseDlg)

	///<  gooose����
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_GOOSE_SEND_RET, &CBasicParamGooseTxDlg::OnGooseSendItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_GOOSE_SEND_RET, &CBasicParamGooseTxDlg::OnGooseSendItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_GOOSE_SEND_RET,  &CBasicParamGooseTxDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_GOOSE_SEND_RET,  &CBasicParamGooseTxDlg::OnInplaceEditAction)
	
	///< �˵���F1-F6��Ӧ���ܴ�����
	ON_COMMAND_RANGE(ID_MENU_PARAM_GOOSES_F1, ID_MENU_PARAM_GOOSES_F6, &CBasicParamGooseTxDlg::OnNaviMenuParamGooseSend)

	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBasicParamGooseTxDlg message handlers

BOOL CBasicParamGooseTxDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	///< ���ñ���
	CString csCaption = L"";
	csCaption.Format(L"%s->%s",dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_S_CAPTION));
	CStatusMgr::SetTitle(csCaption);

	_restorePage();

	SetTimer(1, 4800, NULL);



	return FALSE;
}


BOOL CBasicParamGooseTxDlg::PreTranslateMessage(MSG* pMsg)
{
	return CBaseDlg::PreTranslateMessage(pMsg);
}

///
/// @brief
///      �ָ�ҳ��
///
/// @return
///      void    
///
void CBasicParamGooseTxDlg::_restorePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
	

	if(!pData || CBasicParamGooseTxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageBack)
	{
		//������ҳ�淵��


		CBasicParamPageDataExtraGoose* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGoose*>(pData->m_pPageDataExtra);
		ASSERT(pDataExtra != NULL);
		if (pDataExtra == NULL)
		{
			return;
		}
		//��ʾ��Ӧ����
		if (pDataExtra->m_nMsgParamType == DSM_MSG_GOOSE_PARAM)
		{
			_showGooseSettingReport();
		}

		if (m_ctlSetting.GetRecordRowCount() > pDataExtra->m_nRightReportFocus  && pDataExtra->m_nRightReportFocus >= 0)
		{
			m_ctlSetting.SetFocusedSelectionRowIndex(pDataExtra->m_nRightReportFocus);
			m_ctlSetting.EnsureVisible(pDataExtra->m_nRightReportFocus, FALSE);
		}

		m_ctlSetting.SetFocus();

	}
	else
	{
		_showGooseSettingReport();
	}
}

///
/// @brief
///      ����ҳ��
///
/// @return
///      void    
///
void CBasicParamGooseTxDlg::_SavePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CBasicParamGooseTxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();


	if (pData->m_pPageDataExtra == NULL)
	{
		pData->m_pPageDataExtra = new CBasicParamPageDataExtraGoose(pData);
	}

	CBasicParamPageDataExtraGoose* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGoose*>(pData->m_pPageDataExtra);
	ASSERT(pDataExtra != NULL);
	if (pDataExtra == NULL)
	{
		return;
	}

	//
	//�ָ����ҵ�ѡ����
	//
	
	pDataExtra->m_nMsgParamType = DSM_MSG_GOOSE_PARAM;
	pDataExtra->m_nRightReportFocus = m_ctlSetting.GetFocusedRowIndex();



	int nRowSel = 0;
	//
	//ѡ�е�goose���Ϳ��ƿ飬���ڱ༭goose���ƿ���Ϣ
	//
	if (m_ctlSetting.GetSafeHwnd())
	{
		nRowSel = m_ctlSetting.GetFocusedRowIndex();
		if (nRowSel >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
		{
			pDataExtra->m_pGooseSendSelectedCb = (CGooseCb*)m_ctlSetting.GetRecordRow(nRowSel)->GetItem(2)->GetItemData();
		}
		else
		{
			pDataExtra ->m_pGooseSendSelectedCb = NULL;
		}
	}
	else
	{
		pDataExtra ->m_pGooseSendSelectedCb = NULL;
	}



	//�ָ������ƿ���Ϣ
	CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
	if (NULL == pParamConfigGooseSend)
	{
		return ;
	}
	pParamConfigGooseSend->SaveConfig();

}

/*********************/
/* ����setting����*/
/**/
/**/
/**/
/****************/
void CBasicParamGooseTxDlg::_createGooseSettingReport()
{
	CRect rcReport(DSM_BASICCFG_RCRIGHT);
	DSM_ParentToChild(this, rcReport);

	m_ctlSetting.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_GOOSE_SEND_RET);

	// ��������
	m_ctlSetting.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlSetting.PostInitTemplateStyle();

	m_ctlSetting.InsertColumn(0, L"", EL_CENTER, DSM_REPORT_WIDTH_MARGIN);    // �߿�
	m_ctlSetting.InsertColumn(1, L"", EL_LEFT, 230);                          // ����
	m_ctlSetting.InsertColumn(2, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);     // ����

	// ����ѡ�񽹵���ʽ
	m_ctlSetting.SetRowSelectionVisible(FALSE);
	m_ctlSetting.SetRowFocusEnable(TRUE);
	m_ctlSetting.SetSubItemFocusEnable(TRUE);
	m_ctlSetting.SetFreezeColumnsCount(2);
	m_ctlSetting.SetFreezeSelectionVisible(TRUE);
	m_ctlSetting.SetFocusedColIndex(2);

#if !SU_FEI
	m_ctlSetting.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

#endif

	CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
	if (pGooseSend == NULL)
	{
		return;
	}
	// �������
	m_ctlSetting.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX, new CDsmBasicGooseSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TICK_SPACE),
		pGooseSend->GetHeartbeatInterval()));

	// �Ƶ��
	m_ctlSetting.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX, new CDsmBasicGooseSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MIN_SPACE),
		pGooseSend->GetMinimumInterval()));
	// goose����

	int i = 0;
	CString strCaption = _T("");
	CString strContent = _T("");

	CGooseCb* pGoosecb = NULL;
	pGoosecb = pGooseSend->FirstGocb();

	while (pGoosecb != NULL)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), i + 1);
		strContent.Format(_T("0x%04X-%s"), pGoosecb->GetAPPID(), pGoosecb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + i, new CDsmBasicGooseSendRecord(
			strCaption,
			strContent));
		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pGoosecb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pGoosecb);
		}
		pGoosecb = pGooseSend->NextGocb(pGoosecb);
		++i;
	}

	m_ctlSetting.SetFocusedSelectionRowIndex(0);
	m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

	for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
	{
		m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
	}
}

/*********************/
/* ��ʾsetting����*/
/**/
/**/
/**/
/****************/
void CBasicParamGooseTxDlg::_showGooseSettingReport()
{
	if (m_ctlSetting.GetSafeHwnd())
	{
		m_ctlSetting.ShowWindow(SW_SHOW);
	}
	else
	{
		_createGooseSettingReport();
	}
	m_ctlSetting.SetFocus();
	m_ctlSetting.SetFocusedSelectionRowIndex(0);
}

//�˳�ʱ�������ʱ��
void CBasicParamGooseTxDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������


	KillTimer(1);

}

//������ʱ��  
void CBasicParamGooseTxDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CParamConfigGOOSESend* pParamConfigGooseSend = CParamConfigGOOSESend::getInstance();
		if (NULL == pParamConfigGooseSend)
		{
			return ;
		}
		pParamConfigGooseSend->SaveConfig();
	}

}

void CBasicParamGooseTxDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	*pResult = FALSE;
	static CWnd* pSrc = NULL;

	ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

	// inplaceEdit����ʱ, �л��˵�
	if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
	{
		pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlSetting.GetInplaceEdit());
		*pResult = TRUE;
	}
	// inplace�˳�ʱ���л��˵�
	else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
	{
		CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
		*pResult = TRUE;
	}
}

void CBasicParamGooseTxDlg::OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

	CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
	if (pGooseSend == NULL)
	{
		return;
	}

	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
	ASSERT(pItem);

	switch(pItemNotify->rowIndex)
	{
		// �������
	case MSGPARAM_BASIC_GOOSES_ITEM_TICK_SPACE_ROW_INDEX:
		{
			CString strValue = pItem->GetValue();
			int nTickSpace = _ttoi(strValue);
			if (nTickSpace >= MSGPARAM_TICK_SPACE_MIN && nTickSpace <= MSGPARAM_TICK_SPACE_MAX)
			{
				pGooseSend->SetHeartbeatInterval(nTickSpace);
			}
			strValue.Format(_T("%d"),  pGooseSend->GetHeartbeatInterval());
			pItem->SetValue(strValue);

			*pResult = TRUE;
		}
		break;
		// �������
	case MSGPARAM_BASIC_GOOSES_ITEM_MIN_SPACE_ROW_INDEX:
		{

			CString strValue = pItem->GetValue();
			int nMinSpace = _ttoi(strValue);
			if (nMinSpace >= MSGPARAM_MIN_SPACE_MIN && nMinSpace <= MSGPARAM_MIN_SPACE_MAX)
			{
				pGooseSend->SetMinimumInterval(nMinSpace);
			}
			strValue.Format(_T("%d"), pGooseSend->GetMinimumInterval());
			pItem->SetValue(strValue);
			*pResult = TRUE;
		}
		break;
	default:
		{
			//goose����

			if(pItem->GetHasCheckbox())
			{
				CGooseCb* pGooseCb = (CGooseCb*)pItem->GetItemData();
				ASSERT(pGooseCb);
				if (pGooseCb != NULL)
				{
					bool bAuto = pItem->IsChecked() ? true: false;

					if (bAuto)
					{
						//
						//�ж��Ѿ�ѡ�ж��ٸ����ƿ�
						//
						CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
						if (pGooseSend == NULL)
						{
							return;
						}
						int nGooseCbSel = pGooseSend->GetSelectedGocbCount();
						if (nGooseCbSel < MSGPARAM_GOOSE_SEND_CTRL_SELECTED_MAX)
						{
							pGooseCb->Selected(bAuto);
						}
						else
						{
							pItem->SetChecked(FALSE);
							//MessageBox(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND_SEL_MAX), 
							//    dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND),
							//    MB_ICONINFORMATION);

							CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND_SEL_MAX), 3000);
						}
					}
					else
					{
						pGooseCb->Selected(bAuto);
					}
				}
			}
			*pResult = TRUE;
		}
		break;
	}

	*pResult = TRUE;
}


void CBasicParamGooseTxDlg::OnNaviMenuParamGooseSend(UINT nID)
{
	switch(nID)
	{
	case ID_MENU_PARAM_GOOSES_F1:
		{
			//ȫվ����
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCL_DLG);
		}
		break;
	case ID_MENU_PARAM_GOOSES_F2:
		{
			//��ɨ�������goose
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_DLG);
		}		
		break;
	case ID_MENU_PARAM_GOOSES_F3:
		{
			//_showGooseSettingReport();


			m_ctlSetting.SetRedraw(FALSE);
			CParamConfigGOOSESend* pGooseSend = CParamConfigGOOSESend::getInstance();
			if (pGooseSend == NULL)
			{
				return;
			}

#if !CHUN_HUA
			//�ֶ���ӵĿ��ƿ���Ŀ������20��
			if (pGooseSend->GetGocbCount() > 19)
			{
				CStatusMgr::SetTips(L"�ֶ���ӵĿ��ƿ���Ŀ���ܳ���20��", 3000);
				return;
			}

#endif

			CGooseCb* pGooseCb = pGooseSend->AddGocb();
			if (pGooseCb == NULL)
			{
				//���ʧ��;
				return;
			}
#if !CHUN_HUA
			//����Ĭ��ͨ��Ϊ6���������ǰ����GOOSE���ƿ飬��ô����ǻ�Ĭ��ͨ�������һ�����ƿ�Ŀ�����
			int channelCount= 6- pGooseCb->GetChannelCount();
			
			for (int i= 0; i<channelCount;i++)
			{
				pGooseCb->AddChannel();
			}	
#else
			//Ĭ�����һ��ͨ��
			if(pGooseCb->GetChannelCount() == 0)
			{
				pGooseCb->AddChannel();
			}
#endif
			int nRowCount = m_ctlSetting.GetRecordRowCount();
			ASSERT(nRowCount >=2 );
			CString strCaption = _T("");
			CString strContent = _T("");

			strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), nRowCount - MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX + 1);
			strContent.Format(_T("0x%04X-%s"), pGooseCb->GetAPPID(), pGooseCb->GetDescription().c_str());
			m_ctlSetting.InsertRecordRow(nRowCount, new CDsmBasicGooseSendRecord(strCaption, strContent));
			m_ctlSetting.SetFocusedSelectionRowIndex(nRowCount);
			m_ctlSetting.EnsureVisible(nRowCount, FALSE);
			CELRecordItem * pItem = m_ctlSetting.GetRecordRow(nRowCount)->GetItem(2);
			if (pItem != NULL)
			{
				pItem->HasCheckbox(TRUE);
				pItem->SetChecked(pGooseCb->IsSelected());
				pItem->SetItemData((DWORD_PTR)pGooseCb);
				pItem->SetEditable(TRUE);
			}

			m_ctlSetting.SetRedraw(TRUE);
		}
		break;
	case ID_MENU_PARAM_GOOSES_F4:
		{
			///< �༭

			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}
			int nFocusRow = m_ctlSetting.GetFocusedRowIndex();
			//�༭ͨ��
			if (nFocusRow >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
			{
				CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_EDIT_GOOSE_CTRL_DLG);
			}

		}		
		break;
	case ID_MENU_PARAM_GOOSES_F5:
		{			

			///< ɾ��

			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}
			m_ctlSetting.SetRedraw(FALSE);
			//ɾ��goose����
			int nFocusRow = m_ctlSetting.GetFocusedRowIndex();

			if (nFocusRow >= MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
			{
				CGooseCb* pGooseCb = (CGooseCb*)m_ctlSetting.GetRecordRow(nFocusRow)->GetItem(2)->GetItemData();
				m_ctlSetting.DeleteRecordRow(nFocusRow);

				CParamConfigGOOSESend* pConfigGooseSend = CParamConfigGOOSESend::getInstance();
				if (pConfigGooseSend == NULL)
				{
					m_ctlSetting.SetRedraw(TRUE);
					return;
				}
				pConfigGooseSend->DeleteGocb(pGooseCb);
				//
				//����caption
				//
				for (int i = MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX; i < m_ctlSetting.GetRecordRowCount(); ++i)
				{
					CString strCaption = _T("");
					CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(i)->GetItem(1);
					if (pItem)
					{
						strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_SEND), i - 1);
						pItem->SetValue(strCaption);
						m_ctlSetting.RedrawRowItem(i, 1);
					}
				}
			}
			m_ctlSetting.SetRedraw(TRUE);
		}
		break;

	case ID_MENU_PARAM_GOOSES_F6:
		{

			//���goose����

			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}

			m_ctlSetting.SetRedraw(FALSE);

			CParamConfigGOOSESend* pConfigGooseSend = CParamConfigGOOSESend::getInstance();
			if (pConfigGooseSend == NULL)
			{
				return;
			}
			CGooseCb* pGooseCb = NULL;
			while (m_ctlSetting.GetRecordRowCount() > MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)
			{
				pGooseCb = (CGooseCb*)m_ctlSetting.GetRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX)->GetItem(2)->GetItemData();
				m_ctlSetting.DeleteRecordRow(MSGPARAM_BASIC_GOOSES_ITEM_GOOSE_SEND_START_ROW_INDEX);
				pConfigGooseSend->DeleteGocb(pGooseCb);
			}

			m_ctlSetting.SetRedraw(TRUE);
		}
		break;
	}
}