// ManualSmvStatusWordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ManualSmvStatusWordDlg.h"
#include "src/main/manual/ManualSmvCtrlListDlg.h"
#include "src/main/statesequence/statesetdlg.h"

#define DSM_STATUS_WORD_WIDTH_CAPTION            180
#define DSM_STATUS_WORD_WIDTH_CONTENT            349


//控制块ID
#define DSM_CHAN_STATEWORD_CTRL_MIN 33407
#define DSM_CHAN_STATEWORD_CTRL_MAX 33415


#define DSM_SW_STATUS_WORD1_ROW            0
#define DSM_SW_STATUS_WORD2_ROW            1


// CManualSmvStatusWordDlg 对话框

IMPLEMENT_DYNAMIC(CManualSmvStatusWordDlg, CBaseDlg)

CManualSmvStatusWordDlg::CManualSmvStatusWordDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualSmvStatusWordDlg::IDD, DSM_STR_MANUAL_STATUS_WORD, pParent)
    , m_pBaseCb(NULL)
    , m_pParamConfigSMVSend(NULL)
    , m_nSmvSendType(FRAME_TYPE_NET_9_1)
{
    m_nMenuID = IDR_MENU_MANUAL_SMV_STATUS_WORD;
}

CManualSmvStatusWordDlg::~CManualSmvStatusWordDlg()
{
}

void CManualSmvStatusWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSmvStatusWordDlg, CBaseDlg)
	ON_COMMAND_RANGE(ID_MUNU_MSSW_F1,   ID_MUNU_MSSW_F6, &CManualSmvStatusWordDlg::OnNaviMenuSmvStatusWord)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDD_MANUAL_STATUS_WORD_RET,  &CManualSmvStatusWordDlg::OnSmvStatusWordValueChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDD_MANUAL_STATUS_WORD_RET,  &CManualSmvStatusWordDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDD_MANUAL_STATUS_WORD_RET,  &CManualSmvStatusWordDlg::OnInplaceEditAction)
	ON_COMMAND_RANGE(DSM_CHAN_STATEWORD_CTRL_MIN,   DSM_CHAN_STATEWORD_CTRL_MAX, &CManualSmvStatusWordDlg::OnNaviMenuCtrlSel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualSmvStatusWordDlg 消息处理程序

BOOL CManualSmvStatusWordDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    m_pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (m_pParamConfigSMVSend == NULL)
    {
        return FALSE;
    }

    m_nSmvSendType = m_pParamConfigSMVSend->GetSMVType();
	_resetMenu();
    //
    //从SMV控制块列表中进入
    //
    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    if (pPageData == NULL) //|| 
      //  pPageData->m_nPageID != CManualSmvCtrlListDlg::IDD ||
	  //pPageData->m_nPageID != CStateSetDlg::IDD ||
        //pPageData->m_pPageDataExtra == NULL)
    {
        return FALSE;
    }
#ifdef ZY_PRJ
    CManualSmvCtrlListDataExtra* pDataExtra = dynamic_cast<CManualSmvCtrlListDataExtra*>(pPageData->m_pPageDataExtra);
    if (pDataExtra == NULL || pDataExtra->m_pBaseCb == NULL)
    {
        return FALSE;
    }
    m_pBaseCb = pDataExtra->m_pBaseCb;
    if (m_pBaseCb == NULL)
    {
        return FALSE;
    }

#else

	if ( NULL==m_ctrlCbMap[DSM_CHAN_STATEWORD_CTRL_MIN])
	{
		return FALSE;
	}
	m_pBaseCb =  m_ctrlCbMap[DSM_CHAN_STATEWORD_CTRL_MIN];
	if (m_pBaseCb == NULL)
	{
		return FALSE;
	}
#endif

    _createReport();
    _restorePage();
    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CManualSmvStatusWordDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height() / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStatusWordReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDD_MANUAL_STATUS_WORD_RET);

    // 设置字体
    m_ctlStatusWordReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlStatusWordReport.PostInitTemplateStyle();
    // 插入列元素
    m_ctlStatusWordReport.InsertColumn(0,
        _T(""),
        EL_LEFT,
        DSM_STATUS_WORD_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlStatusWordReport.InsertColumn(1,
        _T(""),
        EL_LEFT,
        DSM_STATUS_WORD_WIDTH_CONTENT);

    // 设置选择焦点样式
    m_ctlStatusWordReport.SetRowSelectionVisible(FALSE);
    m_ctlStatusWordReport.SetRowFocusEnable(TRUE);
    m_ctlStatusWordReport.SetSubItemFocusEnable(TRUE);
    m_ctlStatusWordReport.SetFreezeColumnsCount(1);
    m_ctlStatusWordReport.SetFreezeSelectionVisible(TRUE);
    m_ctlStatusWordReport.SetFocusedColIndex(1);
    m_ctlStatusWordReport.SetFocus();

    CString strCaption = _T("");
    CString strContent = _T("");

    switch(m_nSmvSendType)
    {
    case FRAME_TYPE_NET_9_1:
        { 
            CIEC9_1Cb* pIEC91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
            if (pIEC91Cb)
            {
                strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord1());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
                    strContent));

                strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord2());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
                    strContent));
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        { 
            CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
            if (pFt3Cb)
            {
                strContent.Format(_T("%04X"),  pFt3Cb->GetStatusWord1());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
                    strContent));

                strContent.Format(_T("%04X"),  pFt3Cb->GetStatusWord2());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
                    strContent));
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        { 
            CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
            if (pFt3ExpCb)
            {
                strContent.Format(_T("%04X"),  pFt3ExpCb->GetStatusWord1());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
                    strContent));

                strContent.Format(_T("%04X"),  pFt3ExpCb->GetStatusWord2());
                m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
                    new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
                    strContent));
            }
        }
        break;
    default:
        break;
    }
    

    m_ctlStatusWordReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);


    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvStatusWordDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageEnter)
    {
        m_ctlStatusWordReport.SetFocusedSelectionRowIndex(0);
    }
}

void CManualSmvStatusWordDlg::OnNaviMenuSmvStatusWord(UINT nID)
{
    switch (nID)
    {
	case  ID_MUNU_MSSW_F1:
		{

		}
    case  ID_MUNU_MSSW_F6:
        {
            int nRow = m_ctlStatusWordReport.GetFocusedRowIndex();
            if (nRow >= 0)
            {
                CEventProc::GetInstance()->ShowPage(IDD_STATUS_WORD_SET_DLG);
            }
        }
        break;
    }
}
void CManualSmvStatusWordDlg::OnSmvStatusWordValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;

    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if(!pItemNotify || !m_pBaseCb)
    {
        return;
    }

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlStatusWordReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(1));
    if (pItem == NULL)
    {
        return;
    }
    CString strValue = pItem->GetValue();
    int nValue = 0;

    switch(pItemNotify->rowIndex)
    {
    case DSM_SW_STATUS_WORD1_ROW:
        {
            //状态字1
            nValue = _tcstol(strValue, NULL, 16);

            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        p91Cb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), p91Cb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        pFt3Cb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3Cb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        pFt3ExtCb->SetStatusWord1((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3ExtCb->GetStatusWord1());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    case DSM_SW_STATUS_WORD2_ROW:
        {
            //状态字2
            nValue = _tcstol(strValue, NULL, 16);

            switch(m_nSmvSendType)
            {
            case FRAME_TYPE_NET_9_1:
                {
                    CIEC9_1Cb* p91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
                    if (p91Cb != NULL)
                    {
                        p91Cb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), p91Cb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_NANRUI:
                {
                    CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
                    if (pFt3Cb != NULL)
                    {
                        pFt3Cb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3Cb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            case FRAME_TYPE_FT3_GUOWANG:
                {
                    CFT3ExpandCb* pFt3ExtCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
                    if (pFt3ExtCb != NULL)
                    {
                        pFt3ExtCb->SetStatusWord2((unsigned short)nValue);
                        strValue.Format(_T("%04X"), pFt3ExtCb->GetStatusWord2());
                        pItem->SetValue(strValue);
                        *pResult = TRUE;
                    }
                }
                break;
            }
        }
        break;
    default:
        break;
    }
}


void CManualSmvStatusWordDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    *pResult = TRUE;
    static CWnd* pSrc = NULL;
    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlStatusWordReport.GetInplaceEdit());
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
    }
}
void CManualSmvStatusWordDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvStatusWordDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CManualSmvStatusWordDataExtra(pData);
    }

    CManualSmvStatusWordDataExtra* pDataExtra = dynamic_cast<CManualSmvStatusWordDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }

    pDataExtra->m_pBaseCb = m_pBaseCb;
    pDataExtra->m_nFocusRowIndex = m_ctlStatusWordReport.GetFocusedRowIndex();
    if (pDataExtra->m_nFocusRowIndex == DSM_SW_STATUS_WORD1_ROW)
    {
        pDataExtra->m_eStatusWordType = MSSWStatusWord1;
    }
    else if(pDataExtra->m_nFocusRowIndex == DSM_SW_STATUS_WORD2_ROW)
    {
        pDataExtra->m_eStatusWordType = MSSWStatusWord2;
    }
    else
    {
        pDataExtra->m_eStatusWordType = MSSWStatusWordUnknown;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    pParamConfigSMVSend->SaveConfig();
}

void CManualSmvStatusWordDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CManualSmvStatusWordDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回

        CManualSmvStatusWordDataExtra* pDataExtra = dynamic_cast<CManualSmvStatusWordDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }
        m_pBaseCb = pDataExtra->m_pBaseCb;

        if (pDataExtra->m_nFocusRowIndex >= 0 && pDataExtra->m_nFocusRowIndex < m_ctlStatusWordReport.GetRecordRowCount())
        {
            m_ctlStatusWordReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
        }
    }
}



void CManualSmvStatusWordDlg::OnTimer(UINT_PTR nIDEvent)
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

void CManualSmvStatusWordDlg::_resetMenu()
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
	ASSERT(m_nMenuID == IDR_MENU_MANUAL_SMV_STATUS_WORD);

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
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_STATEWORD_CTRL_MIN + nIndex, pIEC92Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_STATEWORD_CTRL_MIN+nIndex,strDesc);
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
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_STATEWORD_CTRL_MIN + nIndex, pIEC91Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_STATEWORD_CTRL_MIN+nIndex,strDesc);
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
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_STATEWORD_CTRL_MIN + nIndex, pFt3Cb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_STATEWORD_CTRL_MIN+nIndex,strDesc);
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
						m_ctrlCbMap.insert(std::pair<UINT, CBaseCb*>(DSM_CHAN_STATEWORD_CTRL_MIN + nIndex, pFt3ExpCb));
						menu.AppendMenu(MF_STRING,DSM_CHAN_STATEWORD_CTRL_MIN+nIndex,strDesc);
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
}

void CManualSmvStatusWordDlg::OnNaviMenuCtrlSel( UINT nID )
{
	CUINTCtrlCbMap::iterator iter = m_ctrlCbMap.find(nID);
	if (iter != m_ctrlCbMap.end())
	{
		CBaseCb* pOldCb = NULL;
		pOldCb = m_pBaseCb;
		if (iter->second != NULL && iter->second != pOldCb)
		{
			m_pBaseCb = iter->second;
			//m_pIEC92Cb = dynamic_cast<CIEC9_2Cb*>(m_pBaseCb) ;
			_updateReport();
		}
	}
}

void CManualSmvStatusWordDlg::_updateReport()
{
	if (!m_ctlStatusWordReport.GetSafeHwnd())
	{
		return;
	}

	m_ctlStatusWordReport.DeleteAllRecordRow();

	CString strCaption = _T("");
	CString strContent = _T("");

	switch(m_nSmvSendType)
	{
	case FRAME_TYPE_NET_9_1:
		{ 
			CIEC9_1Cb* pIEC91Cb = dynamic_cast<CIEC9_1Cb*>(m_pBaseCb);
			if (pIEC91Cb)
			{
				strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord1());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
					strContent));

				strContent.Format(_T("%04X"),  pIEC91Cb->GetStatusWord2());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
					strContent));
			}
		}
		break;
	case FRAME_TYPE_FT3_NANRUI:
		{ 
			CFT3Cb* pFt3Cb = dynamic_cast<CFT3Cb*>(m_pBaseCb);
			if (pFt3Cb)
			{
				strContent.Format(_T("%04X"),  pFt3Cb->GetStatusWord1());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
					strContent));

				strContent.Format(_T("%04X"),  pFt3Cb->GetStatusWord2());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
					strContent));
			}
		}
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		{ 
			CFT3ExpandCb* pFt3ExpCb = dynamic_cast<CFT3ExpandCb*>(m_pBaseCb);
			if (pFt3ExpCb)
			{
				strContent.Format(_T("%04X"),  pFt3ExpCb->GetStatusWord1());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD1_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD1),
					strContent));

				strContent.Format(_T("%04X"),  pFt3ExpCb->GetStatusWord2());
				m_ctlStatusWordReport.InsertRecordRow(DSM_SW_STATUS_WORD2_ROW,
					new CDsmManualSmvStatusWordRecord(dsm_stringMgr::Get(DSM_STR_MANUAL_STATUS_WORD2),
					strContent));
			}
		}
		break;
	default:
		break;
	}


	m_ctlStatusWordReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);


	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CManualSmvStatusWordDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageEnter)
	{
		m_ctlStatusWordReport.SetFocusedSelectionRowIndex(0);
	}
}