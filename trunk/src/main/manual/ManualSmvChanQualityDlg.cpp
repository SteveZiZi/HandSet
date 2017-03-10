/// @file
///
/// @brief
///     控制块通道品质列表 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.10.18
///
/// 修改历史：
///
#include "stdafx.h"
#include "ManualSmvChanQualityDlg.h"
#include "src/main/manual/manualsmvctrllistdlg.h"


#define  DSM_CHAN_QUILATY_WIDTH_INDEX      50
#define  DSM_CHAN_QUILATY_WIDTH_CHAN_DESC  170
#define  DSM_CHAN_QUILATY_WIDTH_CHAN_TYPE  100
#define  DSM_CHAN_QUILATY_WIDTH_QUALITY    150

#if !CHUN_HUA
#define  DSM_CHAN_QUILATY_WIDTH_CHECKBOX    58
#endif

//控制块ID
#define DSM_CHAN_QUILATY_CTRL_MIN 33407
#define DSM_CHAN_QUILATY_CTRL_MAX 33410
// CManualSmvChanQualityDlg 对话框

IMPLEMENT_DYNAMIC(CManualSmvChanQualityDlg, CBaseDlg)

CManualSmvChanQualityDlg::CManualSmvChanQualityDlg(CWnd* pParent /*=NULL*/)
: CBaseDlg(CManualSmvChanQualityDlg::IDD, DSM_STR_MANUAL_CHAN_QUALITY,pParent)
, m_pIEC92Cb(NULL)
{
	m_nMenuID = IDR_MENU_MANUAL_SMV_CHAN_QUALITY;
}

CManualSmvChanQualityDlg::~CManualSmvChanQualityDlg()
{
	m_ctrlCbMap.clear();
}

void CManualSmvChanQualityDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSmvChanQualityDlg, CBaseDlg)
	ON_COMMAND_RANGE(ID_MUNU_MSCQ_F1,   ID_MUNU_MSCQ_F6, &CManualSmvChanQualityDlg::OnNaviMenuSmvChanQuality)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDD_MANUAL_CHAN_QUALITY_RET,  &CManualSmvChanQualityDlg::OnSmvQualityValueChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDD_MANUAL_CHAN_QUALITY_RET,  &CManualSmvChanQualityDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDD_MANUAL_CHAN_QUALITY_RET,  &CManualSmvChanQualityDlg::OnInplaceEditAction)
	ON_COMMAND_RANGE(DSM_CHAN_QUILATY_CTRL_MIN,   DSM_CHAN_QUILATY_CTRL_MAX, &CManualSmvChanQualityDlg::OnNaviMenuCtrlSel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualSmvChanQualityDlg 消息处理程序

BOOL CManualSmvChanQualityDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//
	//从状态列表中进入
	//
	_resetMenu();

	CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
	if (pPageData == NULL)// || 
		//  pPageData->m_nPageID != CManualSmvCtrlListDlg::IDD)// || 
		//  pPageData->m_pPageDataExtra == NULL)
	{
		return FALSE;
	}

	if ( NULL!=m_ctrlCbMap[DSM_CHAN_QUILATY_CTRL_MIN])
	{
		m_pIEC92Cb = dynamic_cast<CIEC9_2Cb*>( m_ctrlCbMap[DSM_CHAN_QUILATY_CTRL_MIN]);
		_createReport();
		_updateReport();
		_restorePage();	
		SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);
	}
	else
	{
		m_menuMain.ResetMenu();
		_showNoneCtrlTips();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CManualSmvChanQualityDlg::_createReport()
{

	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

	m_ctlChanQualityReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDD_MANUAL_CHAN_QUALITY_RET);

	// 设置字体
	m_ctlChanQualityReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlChanQualityReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlChanQualityReport.PostInitTemplateStyle();


	// 插入列元素
	m_ctlChanQualityReport.InsertColumn(0,
		_T(""),
		EL_LEFT,
		DSM_CHAN_QUILATY_WIDTH_INDEX);
	m_ctlChanQualityReport.InsertColumn(1,
		dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_DESC),
		EL_LEFT,
		DSM_CHAN_QUILATY_WIDTH_CHAN_DESC);
	m_ctlChanQualityReport.InsertColumn(2,
		dsm_stringMgr::Get(DSM_STR_MANUAL_TYPE),
		EL_LEFT,
		DSM_CHAN_QUILATY_WIDTH_CHAN_TYPE);
	m_ctlChanQualityReport.InsertColumn(3,
		dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN_QUALITY),
		EL_LEFT,
		DSM_CHAN_QUILATY_WIDTH_QUALITY);

#if !CHUN_HUA
	m_ctlChanQualityReport.InsertColumn(4,
		L" ",
		EL_LEFT,
		DSM_CHAN_QUILATY_WIDTH_CHECKBOX);
#endif

	// 设置选择焦点样式
	m_ctlChanQualityReport.SetRowSelectionVisible(FALSE);
	m_ctlChanQualityReport.SetRowFocusEnable(TRUE);
	m_ctlChanQualityReport.SetSubItemFocusEnable(TRUE);
	m_ctlChanQualityReport.SetFreezeColumnsCount(3);
	m_ctlChanQualityReport.SetFreezeSelectionVisible(TRUE);
	m_ctlChanQualityReport.SetFocusedColIndex(3);
	m_ctlChanQualityReport.SetFocus();

#if !CHUN_HUA
	m_ctlChanQualityReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif

	//m_ctlChanQualityReport.GetColumnExAttr(0)->GetEditOptions()->SetAllowEdit(TRUE);
	//m_ctlChanQualityReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(FALSE);
	//m_ctlChanQualityReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(FALSE);
	//m_ctlChanQualityReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);


	int nIndex = 0;


	CChannel* pChannel = m_pIEC92Cb->FirstChannel();
	while(pChannel)
	{
		m_ctlChanQualityReport.InsertRecordRow(nIndex, new CDsmManualSmvChanQualityRecord(pChannel, nIndex+1));
		m_ctlChanQualityReport.SetRecordRowData(nIndex, (DWORD_PTR)pChannel);
		
		pChannel = m_pIEC92Cb->NextChannel(pChannel);
		++nIndex;
	}

	

	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CManualSmvChanQualityDlg::IDD != pData->m_nPageID)
	{
		return; 
	}

	if(pData->m_eAction == PageEnter)
	{
		if (m_ctlChanQualityReport.GetRecordRowCount() > 0)
		{
			m_ctlChanQualityReport.SetFocusedSelectionRowIndex(0);
		}
	}
	m_ctlChanQualityReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
	_resetMenu();
}

void CManualSmvChanQualityDlg::_resetMenu()
{
	CParamConfigSMVSend* m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (m_pParamConfigSMVSend == NULL)
	{
		return ;
	}
	enumFrameType frameType = m_pParamConfigSMVSend->GetSMVType();

	//
	//修改F1
	//
	ASSERT(m_nMenuID == IDR_MENU_MANUAL_SMV_CHAN_QUALITY);

	CNaviMenu* pSubMenu = m_pNaviBar->GetNaviMenu()->GetSubMenu(0);
	if (!pSubMenu)
	{
		//
		//已经加载过控制块，不再重新加载
		//
		CMenu  menu;
		VERIFY(menu.CreatePopupMenu());
		int nIndex = 0;
		CString strIndex = _T("");
		CString strDesc = _T("");

		switch(frameType)
		{
		case FRAME_TYPE_NET_9_2:
			{
				CIEC9_2Cb* pIEC92Cb = m_pParamConfigSMVSend->FirstIEC9_2Cb();
				while(pIEC92Cb)
				{
					if (pIEC92Cb->IsSelected())
					{
						strIndex.Format(_T("%d"), nIndex + 1);
						strDesc.Format(_T("0x%04X-%s"), pIEC92Cb->GetAPPID(), pIEC92Cb->GetDescription().c_str());		
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_QUILATY_CTRL_MIN + nIndex, pIEC92Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_QUILATY_CTRL_MIN+nIndex,strDesc);
						++nIndex;

					}
					pIEC92Cb = m_pParamConfigSMVSend->NextIEC9_2Cb(pIEC92Cb);
				}
			}
			break;
		case FRAME_TYPE_NET_9_1:
			{
				CIEC9_1Cb* pIEC91Cb = m_pParamConfigSMVSend->FirstIEC9_1Cb();
				while(pIEC91Cb)
				{
					if (pIEC91Cb->IsSelected())
					{
						strIndex.Format(_T("%d"), nIndex + 1);
						strDesc.Format(_T("0x%04X-%s"), pIEC91Cb->GetAPPID(), pIEC91Cb->GetDescription().c_str());
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_QUILATY_CTRL_MIN + nIndex, pIEC91Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_QUILATY_CTRL_MIN+nIndex,strDesc);
						++nIndex;
					}
					pIEC91Cb = m_pParamConfigSMVSend->NextIEC9_1Cb(pIEC91Cb);
				}
			}
			break;
		case FRAME_TYPE_FT3_NANRUI:
			{
				CFT3Cb* pFt3Cb = m_pParamConfigSMVSend->FirstFt3Cb();
				while(pFt3Cb)
				{
					if (pFt3Cb->IsSelected())
					{
						strIndex.Format(_T("%d"), nIndex + 1);
						strDesc.Format(_T("0x%04X-%s"), pFt3Cb->GetAPPID(), pFt3Cb->GetDescription().c_str());
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_QUILATY_CTRL_MIN + nIndex, pFt3Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_QUILATY_CTRL_MIN+nIndex,strDesc);
						++nIndex;
					}
					pFt3Cb = m_pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
				}
			}
			break;
		case FRAME_TYPE_FT3_GUOWANG:
			{
				CFT3ExpandCb* pFt3ExpCb = m_pParamConfigSMVSend->FirstFt3ExpandCb();
				while(pFt3ExpCb)
				{
					if (pFt3ExpCb->IsSelected())
					{
						strIndex.Format(_T("%d"), nIndex + 1);
						strDesc.Format(_T("0x%04X-%s"), pFt3ExpCb->GetAPPID(), pFt3ExpCb->GetDescription().c_str());
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_QUILATY_CTRL_MIN + nIndex, pFt3ExpCb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_QUILATY_CTRL_MIN+nIndex,strDesc);
						++nIndex;
					}
					pFt3ExpCb = m_pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpCb);
				}
			}
			break;
		default:
			break;
		}
		/*menu.AppendMenu(MF_STRING, iter->first, strDesc);*/

		pSubMenu = new CNaviMenu;
		pSubMenu->LoadMenu(&menu);
		menu.DestroyMenu();
		m_pNaviBar->GetNaviMenu()->SetMenuSubItem(0, pSubMenu);

	}

	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

}


void CManualSmvChanQualityDlg::OnSmvQualityValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
	if(!pItemNotify || !m_pIEC92Cb)
	{
		return;
	}

	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlChanQualityReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(3));
	if (pItem == NULL)
	{
		return;
	}
	CString strValue = pItem->GetValue();
	int nValue = 0;
	nValue = _tcstol(strValue, NULL, 16);

	CChannel* pChannel = (CChannel*) m_ctlChanQualityReport.GetRecordRowData(pItemNotify->rowIndex);
	if (pChannel)
	{
		pChannel->SetQuality(nValue);
		strValue.Format(_T("%04X"), pChannel->GetQuality());
		pItem->SetValue(strValue);
		*pResult = TRUE;
	}

}

void CManualSmvChanQualityDlg::OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	*pResult = TRUE;
	static CWnd* pSrc = NULL;
	ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

	if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
	{
		// inplaceEdit弹出时, 进入编辑菜单
		pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlChanQualityReport.GetInplaceEdit());
	}
	else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
	{
		// inplace退出时，退出编辑菜单
		CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
	}
}

void CManualSmvChanQualityDlg::_updateChanQualityValue()
{

	CELRecordItemText* pItem = NULL;
	CString strQuality = _T("");
	CChannel* pChannel = NULL;
	int nRowCount = m_ctlChanQualityReport.GetRecordRowCount();
	for (int i = 0; i < nRowCount; ++i)
	{
		pItem = (CELRecordItemText*)m_ctlChanQualityReport.GetRecordRow(i)->GetItem(3);
		pChannel = (CChannel*)m_ctlChanQualityReport.GetRecordRowData(i);
		if (pItem && pChannel)
		{
			strQuality.Format(_T("%04X"), pChannel->GetQuality());
			pItem->SetValue(strQuality);
		}
	}
	m_ctlChanQualityReport.RedrawRowItems(0, nRowCount-1);
}
void CManualSmvChanQualityDlg::OnNaviMenuSmvChanQuality(UINT nID)
{
	switch (nID)
	{
	case  ID_MUNU_MSCQ_F4:
		{
			m_pIEC92Cb->SetAllChannelsQuality(QUALITY_OPERATE_CLEAR);
			_updateChanQualityValue();
		}
		break;
	case  ID_MUNU_MSCQ_F2:
		{
			m_pIEC92Cb->SetAllChannelsQuality(QUALITY_OPERATE_TEST);
			_updateChanQualityValue();
		}
		break;
	case  ID_MUNU_MSCQ_F3:
		{
			m_pIEC92Cb->SetAllChannelsQuality(QUALITY_OPERATE_RUN);
			_updateChanQualityValue();
		}
		break;
	case  ID_MUNU_MSCQ_F1:
		{

		}
		break;
	case  ID_MUNU_MSCQ_F5:
		{
			int nRow = m_ctlChanQualityReport.GetFocusedRowIndex();
			if (nRow >= 0)
			{
				CEventProc::GetInstance()->ShowPage(IDD_MANUAL_QUALITY_SET_DLG);
			}
		}
		break;
#if !CHUN_HUA
	case  ID_MUNU_MSCQ_F6:
		{
			int nRow = m_ctlChanQualityReport.GetFocusedRowIndex();
			if (nRow >= 0)
			{
				BOOL isCheck = m_ctlChanQualityReport.GetRecordRow(nRow)->GetItem(0)->IsChecked();
				m_ctlChanQualityReport.GetRecordRow(nRow)->GetItem(0)->SetChecked(!isCheck);
				m_ctlChanQualityReport.RedrawItems(nRow,nRow);
			}
		}
		break;
#endif
	}
}

void CManualSmvChanQualityDlg::_SavePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CManualSmvChanQualityDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if (pData->m_pPageDataExtra == NULL)
	{
		pData->m_pPageDataExtra = new CManualSmvChanQualityDataExtra(pData);
	}

	CManualSmvChanQualityDataExtra* pDataExtra = dynamic_cast<CManualSmvChanQualityDataExtra*>(pData->m_pPageDataExtra);
	ASSERT(pDataExtra != NULL);
	if (pDataExtra == NULL)
	{
		return;
	}

	pDataExtra->m_nFocusRow = m_ctlChanQualityReport.GetFocusedRowIndex();
	pDataExtra->m_pBaseCb = m_pIEC92Cb;

	if (pDataExtra->m_nFocusRow >= 0)
	{
		pDataExtra->m_pChannel = (CChannel*)m_ctlChanQualityReport.GetRecordRowData(pDataExtra->m_nFocusRow);
	}

	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

#if !CHUN_HUA
	pDataExtra->m_pChannelList.RemoveAll();
	pDataExtra->m_nSelectIndex.RemoveAll();
	for (int i = 0; i < m_ctlChanQualityReport.GetRecordRowCount(); i++)
	{
		if (m_ctlChanQualityReport.GetRecordRow(i)->GetItem(4)->IsChecked())
		{
			pDataExtra->m_pChannelList.AddTail((CChannel*)m_ctlChanQualityReport.GetRecordRowData(i));
			pDataExtra->m_nSelectIndex.AddTail(i);
		}
	}
#endif

	pParamConfigSMVSend->SaveConfig();

}

void CManualSmvChanQualityDlg::_restorePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CManualSmvChanQualityDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageBack)
	{
		//从其他页面返回
		CManualSmvChanQualityDataExtra* pDataExtra = dynamic_cast<CManualSmvChanQualityDataExtra*>(pData->m_pPageDataExtra);
		ASSERT(pDataExtra != NULL);
		if (pDataExtra == NULL)
		{
			return;
		}

		if(pDataExtra->m_pBaseCb != NULL)
		{
			m_pIEC92Cb =dynamic_cast<CIEC9_2Cb*>( pDataExtra->m_pBaseCb);
			_updateReport();
		}
		if (m_ctlChanQualityReport.GetRecordRowCount() > 0)
		{
			if (pDataExtra->m_nFocusRow >= 0 && pDataExtra->m_nFocusRow < m_ctlChanQualityReport.GetRecordRowCount())
			{
				m_ctlChanQualityReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRow);
			}
			else
			{
				m_ctlChanQualityReport.SetFocusedSelectionRowIndex(0);
			}
		}
#if !CHUN_HUA
		if (pDataExtra->m_nSelectIndex.GetCount()>0)
		{		
			
			int index = pDataExtra->m_nSelectIndex.GetHead();
			POSITION psn = pDataExtra->m_nSelectIndex.GetHeadPosition();
			for (int j= 0; j<pDataExtra->m_nSelectIndex.GetCount();j++)
			{
				index = pDataExtra->m_nSelectIndex.GetNext(psn);
				if (index>=0 && index<m_ctlChanQualityReport.GetRecordRowCount() )
				{			
					m_ctlChanQualityReport.GetRecordRow(index)->GetItem(4)->SetChecked(TRUE);
					
				}
				
			}
		}
#endif	

	}
}

void CManualSmvChanQualityDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (nIDEvent == 1)
	{
		CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
		if (NULL == pParamConfigSMVSend)
		{
			return ;
		}
		pParamConfigSMVSend->SaveConfig();
	}
	CBaseDlg::OnTimer(nIDEvent);
}

void CManualSmvChanQualityDlg::OnNaviMenuCtrlSel(UINT nID)
{
	CUINTCtrlCbMap::iterator iter = m_ctrlCbMap.find(nID);
	if (iter != m_ctrlCbMap.end())
	{
		CBaseCb* pOldCb = NULL;
		pOldCb = m_pBaseCb;
		if (iter->second != NULL && iter->second != pOldCb)
		{
			m_pBaseCb = iter->second;
			m_pIEC92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb) ;
			_updateReport();
		}
	}
}

void CManualSmvChanQualityDlg::_updateReport()
{
	if (!m_ctlChanQualityReport.GetSafeHwnd())
	{
		return;
	}

	m_ctlChanQualityReport.DeleteAllRecordRow();

	int nIndex = 0;
	CChannel* pChannel = m_pIEC92Cb->FirstChannel();
	while(pChannel)
	{
		m_ctlChanQualityReport.InsertRecordRow(nIndex, new CDsmManualSmvChanQualityRecord(pChannel, nIndex+1));
		m_ctlChanQualityReport.SetRecordRowData(nIndex, (DWORD_PTR)pChannel);
		pChannel = m_pIEC92Cb->NextChannel(pChannel);
		++nIndex;
	}

	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CManualSmvChanQualityDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageEnter)
	{
		if (m_ctlChanQualityReport.GetRecordRowCount() > 0)
		{
			m_ctlChanQualityReport.SetFocusedSelectionRowIndex(0);
		}
	}
	m_ctlChanQualityReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
}

void CManualSmvChanQualityDlg::_showNoneCtrlTips()
{
	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

	m_ctlChanQualityReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDD_MANUAL_CHAN_QUALITY_RET);

	// 设置字体
	m_ctlChanQualityReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_ctlChanQualityReport.PostInitTemplateStyle();


	// 插入列元素

	m_ctlChanQualityReport.InsertColumn(0,
		dsm_stringMgr::Get(DSM_STR_MANUAL_TYPE),
		EL_CENTER,
		DSM_CHAN_QUILATY_WIDTH_CHAN_TYPE);

	// 设置选择焦点样式
	m_ctlChanQualityReport.SetRowSelectionVisible(FALSE);
	m_ctlChanQualityReport.SetFreezeColumnsCount(0);
	m_ctlChanQualityReport.SetFreezeSelectionVisible(TRUE);
}
