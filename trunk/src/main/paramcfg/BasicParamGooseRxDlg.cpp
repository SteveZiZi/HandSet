// BasicParamGooseRxDlg.cpp : implementation file
//

#include "stdafx.h"

#include "BasicParamGooseRxDlg.h"

#include "src/main/common.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/popdlg/progressbardlg.h"
#include "src/main/eventproc.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/utils/utility.h"

#include "src/service/datalayer/dsmdatalayer.h"
#include "src/service/datalayer/config/goosecb.h"

#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/scl/SCLManager.h"
#include "src/service/datalayer/config/ParamConfigGooseRecv.h"


/////< 开入映射表
//#define DSM_BASIC_DI_MAP_DI_CHAN                100
//#define DSM_BASIC_DI_MAP_CTRL_BLOCK             200
//#define DSM_BASIC_DI_MAP_CHAN                   193
//
/////< 最大显示开入通道数
//#define GOOSE_REC_DI_MAP_MAX                    8

// CBasicParamGooseRxDlg dialog

IMPLEMENT_DYNAMIC(CBasicParamGooseRxDlg, CBaseDlg)

CBasicParamGooseRxDlg::CBasicParamGooseRxDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamGooseRxDlg::IDD, DSM_STR_BASIC_PARAM_MSG, pParent)
{
	m_nMenuID	 = IDR_MENU_MSG_PARAM_GOOSE_REC;
}

CBasicParamGooseRxDlg::~CBasicParamGooseRxDlg()
{
}

void CBasicParamGooseRxDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamGooseRxDlg, CBaseDlg)

	///<  gooose接收
	 ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_GOOSE_REC_RET, &CBasicParamGooseRxDlg::OnGooseRecItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_GOOSE_REC_RET,  &CBasicParamGooseRxDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_GOOSE_REC_RET,  &CBasicParamGooseRxDlg::OnInplaceEditAction)

	///< 菜单从F1-F6对应功能处理函数
	ON_COMMAND_RANGE(ID_MENU_PARAM_GOOSER_F1, ID_MENU_PARAM_GOOSER_F6, &CBasicParamGooseRxDlg::OnNaviMenuParamGooseRec)

	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBasicParamGooseTxDlg message handlers

BOOL CBasicParamGooseRxDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	///< 设置标题
	CString csCaption = L"";
	csCaption.Format(L"%s->%s",dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION));
	CStatusMgr::SetTitle(csCaption);

	_restorePage();

	SetTimer(1, 4800, NULL);


	return FALSE;
}


BOOL CBasicParamGooseRxDlg::PreTranslateMessage(MSG* pMsg)
{
	return CBaseDlg::PreTranslateMessage(pMsg);
}

///
/// @brief
///      恢复页面
///
/// @return
///      void    
///
void CBasicParamGooseRxDlg::_restorePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();


	if(!pData || CBasicParamGooseRxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageBack)
	{
		//从其他页面返回


		CBasicParamPageDataExtraGooseRx* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGooseRx*>(pData->m_pPageDataExtra);
		ASSERT(pDataExtra != NULL);
		if (pDataExtra == NULL)
		{
			return;
		}
		//显示对应报表
		if (pDataExtra->m_nMsgParamType == DSM_MSG_GOOSE_RX_PARAM)
		{
			_showGooseSettingReport();
		}

		if (m_ctlSetting.GetRecordRowCount() > pDataExtra->m_nRightReportFocus  && pDataExtra->m_nRightReportFocus >= 0 && m_ctlSetting.GetRecordRowCount() > 0)
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
///      保存页面
///
/// @return
///      void    
///
void CBasicParamGooseRxDlg::_SavePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CBasicParamGooseRxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();


	if (pData->m_pPageDataExtra == NULL)
	{
		pData->m_pPageDataExtra = new CBasicParamPageDataExtraGooseRx(pData);
	}

	CBasicParamPageDataExtraGooseRx* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGooseRx*>(pData->m_pPageDataExtra);
	ASSERT(pDataExtra != NULL);
	if (pDataExtra == NULL)
	{
		return;
	}

	//
	//恢复左右的选中行
	//
	pDataExtra->m_nMsgParamType = DSM_MSG_GOOSE_RX_PARAM;
	pDataExtra->m_nRightReportFocus = m_ctlSetting.GetFocusedRowIndex();
			
		


	int nRowSel = 0;
	//
	//选中的goose接收控制块，用于查看goose控制块信息
	//
	if (m_ctlSetting.GetSafeHwnd())
	{
		nRowSel = m_ctlSetting.GetFocusedRowIndex();
		if (nRowSel >= 0)
		{
			pDataExtra->m_pGooseRecSelectedCb = (CGooseCb*)m_ctlSetting.GetRecordRow(nRowSel)->GetItem(2)->GetItemData();
		}
		else
		{
			pDataExtra ->m_pGooseRecSelectedCb = NULL;
		}
	}
	else
	{
		pDataExtra ->m_pGooseRecSelectedCb = NULL;
	}


	//恢复表格控制块信息
	CParamConfigGOOSERecv* pParamConfigGooseRec= CParamConfigGOOSERecv::getInstance();
	if (NULL == pParamConfigGooseRec)
	{
		return ;
	}
	pParamConfigGooseRec->SaveConfig();

}

/*********************/
/* 创建setting报表*/
/**/
/**/
/**/
/****************/
void CBasicParamGooseRxDlg::_createGooseSettingReport()
{
	CRect rcReport(DSM_BASICCFG_RCRIGHT);
	DSM_ParentToChild(this, rcReport);

	m_ctlSetting.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_GOOSE_REC_RET);

	// 设置字体
	m_ctlSetting.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlSetting.PostInitTemplateStyle();

	m_ctlSetting.InsertColumn(0, L"", EL_CENTER, DSM_REPORT_WIDTH_MARGIN);    // 边框
	m_ctlSetting.InsertColumn(1, L"", EL_LEFT, 230);                          // 标题
	m_ctlSetting.InsertColumn(2, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);     // 内容

	// 设置选择焦点样式
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


	CParamConfigGOOSERecv* pGooseRec = CParamConfigGOOSERecv::getInstance();
	if (pGooseRec == NULL)
	{
		return;
	}

	// goose接收

	int i = 0;
	CString strCaption = _T("");
	CString strContent = _T("");


	CGooseCb* pGoosecb = NULL;
	pGoosecb = pGooseRec->FirstGocb();

	while (pGoosecb != NULL)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC), i + 1);
		strContent.Format(_T("0x%04X-%s"), pGoosecb->GetAPPID(), pGoosecb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow(i, new CDsmBasicGooseRecRecord( strCaption, strContent));
		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pGoosecb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pGoosecb);
		}
		pGoosecb = pGooseRec->NextGocb(pGoosecb);
		++i;
	}

	m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

	for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
	{
		m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
	}
	

	///< 添加标题
	CString csCaption;
	csCaption.Format(L"%s->%s",
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG),
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION));

	CStatusMgr::SetTitle(csCaption);

}

/*********************/
/* 显示setting报表*/
/**/
/**/
/**/
/****************/
void CBasicParamGooseRxDlg::_showGooseSettingReport()
{
	/*if (m_ctlDIMapReport.GetSafeHwnd())
	{
		m_ctlDIMapReport.ShowWindow(SW_HIDE);
	}*/
	if (m_ctlSetting.GetSafeHwnd())
	{
		m_ctlSetting.ShowWindow(SW_SHOW);
	}
	else
	{
		_createGooseSettingReport();
	}
	m_ctlSetting.SetFocus();	

	if (m_ctlSetting.GetRecordRowCount() > 0)
	{
		m_ctlSetting.SetFocusedSelectionRowIndex(0);
	}

	/*m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, _T(" "));
	m_pNaviBar->UpdateNaviMenu();*/
}

//退出时，清除定时器
void CBasicParamGooseRxDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	// TODO: 在此处添加消息处理程序代码


	KillTimer(1);

}

//启动定时器  
void CBasicParamGooseRxDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CParamConfigGOOSERecv* pParamConfigGooseRec = CParamConfigGOOSERecv::getInstance();
		if (NULL == pParamConfigGooseRec)
		{
			return ;
		}
		pParamConfigGooseRec->SaveConfig();
	}

	CBaseDlg::OnTimer(nIDEvent);
}

void CBasicParamGooseRxDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	*pResult = FALSE;
	static CWnd* pSrc = NULL;

	ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

	// inplaceEdit弹出时, 切换菜单
	if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
	{
		pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlSetting.GetInplaceEdit());
		*pResult = TRUE;
	}
	// inplace退出时，切换菜单
	else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
	{
		CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
		*pResult = TRUE;
	}
}

void CBasicParamGooseRxDlg::OnGooseRecItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

	if (pItemNotify == NULL)
	{
		return ;
	}

	CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
	if (pConfigGooseRec == NULL)
	{
		return;
	}
	CELRecordItem* pItem = m_ctlSetting.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex);
	if (pItem && pItem->GetHasCheckbox())
	{
		CGooseCb* pGooseCb = (CGooseCb*)pItem->GetItemData();
		if (!pGooseCb)
		{
			return;
		}

		bool bCheck = pItem->IsChecked() ? true: false;
		if (bCheck)
		{
			//
			//判断已经选中多少个控制块
			//
			if (pConfigGooseRec->GetSelectedGocbCount() < MSGPARAM_GOOSE_REC_CTRL_SELECTED_MAX)
			{
				pGooseCb->Selected(bCheck);
			}
			else
			{
				pItem->SetChecked(FALSE);
				//MessageBox(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC_SEL_MAX), 
				//    dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_R_CAPTION),
				//    MB_ICONINFORMATION);

				CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC_SEL_MAX), 3000);

			}
		}
		else
		{
			pGooseCb->Selected(bCheck);
		}
	}
	*pResult = TRUE;

	
}

void CBasicParamGooseRxDlg::OnNaviMenuParamGooseRec(UINT nID)
{
	switch(nID)
	{
	case ID_MENU_PARAM_GOOSER_F1:
		{
			///< 从SCL文件导入
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCL_DLG);
		}
		break;
	case ID_MENU_PARAM_GOOSER_F2:
		{
			///< 从监控导入
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCAN_DLG);
		}		
		break;
	case ID_MENU_PARAM_GOOSER_F3:
		{
			///< 编辑

			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}
			//查看通道
			if (m_ctlSetting.GetFocusedRowIndex() >= 0 )
			{
				CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_VIEW_GOOSE_CTRL_DLG);
			}
		}
		break;
	case ID_MENU_PARAM_GOOSER_F4:
		{
			///< 删除
			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}

			m_ctlSetting.SetRedraw(FALSE);
			int nRowSel = m_ctlSetting.GetFocusedRowIndex();
			//删除
			if (nRowSel >= 0)
			{
				CGooseCb * pGooseCb = (CGooseCb*)m_ctlSetting.GetRowItemData(nRowSel, 2);
				if (pGooseCb)
				{
					CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
					if (pConfigGooseRec == NULL)
					{
						m_ctlSetting.SetRedraw(TRUE);
						return;
					}
					pConfigGooseRec->DeleteGocb(pGooseCb);
				}
				m_ctlSetting.DeleteRecordRow(nRowSel);
				//
				//更新caption
				//
				CString strCaption = _T("");
				for (int i = 0; i < m_ctlSetting.GetRecordRowCount(); ++i)
				{
					strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_REC), i + 1);
					CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(i)->GetItem(1);
					if (pItem)
					{
						pItem->SetValue(strCaption);
					}
					m_ctlSetting.RedrawRowItem(i, 1);
				}
			}
			m_ctlSetting.SetRedraw(TRUE);

		}		
		break;
	
	case ID_MENU_PARAM_GOOSER_F5:
		{
			///< 开入映射
			//_showDiMapReport();			
			 
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DI_MAP_DLG);
		}
		break;
	case ID_MENU_PARAM_GOOSER_F6:
		{
			//必须在开入映射创建表格后，并且是焦点的时才能删除映射
			//
			//if (!m_ctlDIMapReport.GetSafeHwnd())
			//{
			//	return;
			//}

			//if (GetFocus()->GetSafeHwnd() == m_ctlDIMapReport.GetSafeHwnd())
			//{
			//	//删除映射
			//	CString strCtrlBlockDesc = _T("");
			//	CString strChanDesc = _T("");
			//	CELRecordItemText* pItem = NULL;
			//	for (int i = 0; i < GOOSE_REC_DI_MAP_MAX; ++i)
			//	{
			//		pItem = (CELRecordItemText*)m_ctlDIMapReport.GetRecordRow(i)->GetItem(1);
			//		if (pItem)
			//		{
			//			pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
			//			m_ctlDIMapReport.RedrawRowItem(i, 1);
			//		}

			//		pItem = (CELRecordItemText*)m_ctlDIMapReport.GetRecordRow(i)->GetItem(2);
			//		if (pItem)
			//		{
			//			pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAP_NULL));
			//			m_ctlDIMapReport.RedrawRowItem(i, 2);
			//		}
			//	}

			//	//
			//	//清除映射
			//	//
			//	CParamConfigGOOSERecv* pConfigGooseRec = CParamConfigGOOSERecv::getInstance();
			//	if (pConfigGooseRec == NULL)
			//	{
			//		return;
			//	}
			//	pConfigGooseRec->ClearGOOSERecvChannelMap();	

			//}
		

		}
		break;
	}
}
/*
///
/// @brief
///      创建开入映射报表
///
/// @return
///      void    
///
void CBasicParamGooseRxDlg::_createDiMapReport()
{
	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlDIMapReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_CTRL_BLOCK_FROM_SCAN_RET);

	// 设置字体
	m_ctlDIMapReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlDIMapReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlDIMapReport.PostInitTemplateStyle();
	// 插入列元素

	m_ctlDIMapReport.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DI_CHAN),      EL_CENTER, DSM_BASIC_DI_MAP_DI_CHAN);
	m_ctlDIMapReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_DESC),    EL_CENTER, DSM_BASIC_DI_MAP_CTRL_BLOCK);
	m_ctlDIMapReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN),         EL_CENTER, DSM_BASIC_DI_MAP_CHAN);

	// 设置选择焦点样式
	m_ctlDIMapReport.SetRowSelectionVisible(TRUE);
	m_ctlDIMapReport.SetRowFocusEnable(TRUE);
	m_ctlDIMapReport.SetSubItemFocusEnable(TRUE);
	m_ctlDIMapReport.SetFreezeSelectionVisible(TRUE);
	m_ctlDIMapReport.SetFocusedColIndex(0);

	m_ctlDIMapReport.SetFocus();
	m_ctlDIMapReport.ShowWindow(SW_SHOW);

	////
	////插入映射信息
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

	for (int i = 0; i < GOOSE_REC_DI_MAP_MAX; ++i)
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

				//描述
				pBaseCb = pChannel->GetParentBaseCb();
				if (pBaseCb)
				{
					strCtrlBlockDesc.Format(_T("0x%04X-%s"), pBaseCb->GetAPPID(), pBaseCb->GetDescription().c_str());
				}
				//通道
				if (pParentChannel)
				{
					//子通道
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
	
}
///
/// @brief
///      显示开入映射报表
///
/// @return
///      void    
///
void CBasicParamGooseRxDlg::_showDiMapReport()
{
	if (m_ctlSetting.GetSafeHwnd())
	{
		m_ctlSetting.ShowWindow(SW_HIDE);
	}
	if (m_ctlDIMapReport.GetSafeHwnd())
	{
		m_ctlDIMapReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createDiMapReport();
	}
	m_ctlDIMapReport.SetFocusedSelectionRowIndex(0);

	///< 显示删除映射
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, _T("删除映射"));
	m_pNaviBar->UpdateNaviMenu();

}
*/