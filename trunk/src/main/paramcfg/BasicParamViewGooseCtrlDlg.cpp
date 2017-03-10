/// @file
///
/// @brief
///     查看goose对话框 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#include "stdafx.h"
#include "BasicParamGooseRxDlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/paramcfg/common_cfg.h"
#include "BasicParamViewGooseCtrlDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/config/goosecb.h"
#include "src/utils/CommonControl/MacEditCtrl/MacEditCtrl.h"
#include "src/service/datalayer/config/ParamConfigGooseRecv.h"


//////////////////////////////////////////////////////////////////////////
// 查看GOOSE通道报表
//
#if SU_FEI
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_INDEX   50
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_DESC    290
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_TYPE    180
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_MAP     80
#else
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_INDEX   60
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_DESC    210
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_TYPE    179
#define DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_MAP     80

#endif


#define MSGPARAM_VIEW_GOOSE_CTRL_BLOCK_DESC       0         ///< 控制块描述
#define MSGPARAM_VIEW_GOOSE_REC_PORT              1         ///< 
#define MSGPARAM_VIEW_GOOSE_APPID                 2         ///< 
#define MSGPARAM_VIEW_GOOSE_MAC_ADDR              3         ///< 
#define MSGPARAM_VIEW_GOOSE_VLAN_ID               4         ///< 
#define MSGPARAM_VIEW_GOOSE_VLAN_PRIORITY         5         ///< 
#define MSGPARAM_VIEW_GOOSE_CFG_VERSION           6         ///< 
#define MSGPARAM_VIEW_GOOSE_TEST_FLAG             7         ///< 
#define MSGPARAM_VIEW_GOOSE_NEED_COMM             8         ///< 
#define MSGPARAM_VIEW_GOOSE_GOCB_REF              9         ///< 
#define MSGPARAM_VIEW_GOOSE_DATA_SET              10        ///< 
#define MSGPARAM_VIEW_GOOSE_GO_ID                 11        ///< 
#define MSGPARAM_VIEW_GOOSE_CHAN_COUNT            12        ///< 




// CBasicParamViewGooseCtrlDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamViewGooseCtrlDlg, CBaseDlg)

CBasicParamViewGooseCtrlDlg::CBasicParamViewGooseCtrlDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamViewGooseCtrlDlg::IDD, DSM_STR_BASIC_PARAM_GOOSE_CTRL_BLOCK_CAPTION, pParent)
    , m_pGooseCb(NULL)
    , m_nShowReportType(VGShowCtrlBlockReport)
{
    m_nMenuID = IDR_MENU_MSG_PARAM_GOOSE_CTRL_BLOCK;
}

CBasicParamViewGooseCtrlDlg::~CBasicParamViewGooseCtrlDlg()
{
}

void CBasicParamViewGooseCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamViewGooseCtrlDlg, CBaseDlg)
    ON_COMMAND(ID_MENU_PARAM_GOOSES_CTRL_F1, &CBasicParamViewGooseCtrlDlg::OnMenuParamViewGoosesCtrlF1)
#if !SU_FEI
	ON_COMMAND(ID_MENU_PARAM_GOOSES_CTRL_F2, &CBasicParamViewGooseCtrlDlg::OnMenuParamViewGoosesCtrlF2)
#endif
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_VIEW_GOOSE_CHAN_RET, &CBasicParamViewGooseCtrlDlg::OnViewChanItemValueChanged)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CBasicParamViewGooseCtrlDlg 消息处理程序


BOOL CBasicParamViewGooseCtrlDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化


    CPageData * pPageData = CEventProc::GetInstance()->GetPreviousPageData();
#if SU_FEI
    if (pPageData == NULL || 
        pPageData->m_nPageID != CBasicParamDlg::IDD || 
        pPageData->m_pPageDataExtra == NULL)
#else
	if (pPageData == NULL || 
		pPageData->m_nPageID != CBasicParamGooseRxDlg::IDD || 
		pPageData->m_pPageDataExtra == NULL)
#endif
    {
        //
        //参数配置进入（goose编辑或查看）
        //
        return FALSE;
    }
#if SU_FEI
    CBasicParamPageDataExtra* pDataExtra = dynamic_cast<CBasicParamPageDataExtra*>(pPageData->m_pPageDataExtra);
#else
	CBasicParamPageDataExtraGooseRx* pDataExtra = dynamic_cast<CBasicParamPageDataExtraGooseRx*>(pPageData->m_pPageDataExtra);
#endif
    if (pDataExtra == NULL)
    {
        return FALSE;
    }
    //
    //增加对控制块信息的判断
    //
    m_pGooseCb = pDataExtra->m_pGooseRecSelectedCb;
    if (m_pGooseCb == NULL)
    {
        return FALSE;
    }

    _showCtrlBlockReport();

    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CBasicParamViewGooseCtrlDlg::_createCtrlBlockReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = rcReport.Height()/ ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + nRows * ELT_T1_ROWHEIGHT;

    m_ctlCtrlBlockReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_VIEW_GOOSE_CTRL_BLOCK_RET);

    // 设置字体
    m_ctlCtrlBlockReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlCtrlBlockReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlCtrlBlockReport.InsertColumn(0, 
        _T(""),
        EL_LEFT,
        DSM_BASIC_EDIT_GOOSE_WIDTH_CAPTION,
        FALSE,
        EL_CENTER,
        DSM_REPORT_COLOR_CAPTION_BK);
    m_ctlCtrlBlockReport.InsertColumn(1, 
        _T(""),
        EL_LEFT,
        DSM_BASIC_EDIT_GOOSE_WIDTH_CONTENT);


    // 设置选择焦点样式
    m_ctlCtrlBlockReport.SetRowSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetRowFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetSubItemFocusEnable(TRUE);
    m_ctlCtrlBlockReport.SetFreezeColumnsCount(1);
    m_ctlCtrlBlockReport.SetFreezeSelectionVisible(TRUE);
    m_ctlCtrlBlockReport.SetFocusedColIndex(1);
    m_ctlCtrlBlockReport.SetFocus();

#if !SU_FEI
	m_ctlCtrlBlockReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
    
    CString strTemp  = _T("");
    //
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_CTRL_BLOCK_DESC, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CTRL_BLOCK_DESC),
        m_pGooseCb->GetDescription().c_str()));

    //switch(m_pGooseCb->GetPortName())
    //{
    //case NET_PORT_LIGHT_A:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX1);
    //    }
    //    break;
    //case NET_PORT_LIGHT_B:
    //    {
    //        strTemp = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NET_PORT_RX2);
    //    }
    //    break;
    //default:
    //    strTemp = _T("");
    //    break;
    //}
    strTemp = dsm_stringMgr::Get(DSM_STR_EDIT_SMV_UNKNOWN);

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_REC_PORT, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_REC_PORT),
        strTemp));
    
    strTemp.Format(_T("%04X"), m_pGooseCb->GetAPPID());
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_APPID, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_APPID_HEX),
        strTemp));
    
    unsigned char ucMac[6];
    m_pGooseCb->GetDesMAC(ucMac, 6);
    CMacEditCtrl::MacAddr2Str(ucMac, strTemp);
    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_MAC_ADDR, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MAC_ADDR),
        strTemp));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_VLAN_ID, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VLAN_ID),
        m_pGooseCb->GetVlanID()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_VLAN_PRIORITY, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VLAN_PRIORITY),
        m_pGooseCb->GetVLanPriority()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_CFG_VERSION, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CFG_VERSION),
        m_pGooseCb->GetConfigRevision()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_TEST_FLAG, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_TEST_FLAG),
        _T("")));
    m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_VIEW_GOOSE_TEST_FLAG)->GetItem(1)->HasCheckbox(TRUE);
    m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_VIEW_GOOSE_TEST_FLAG)->GetItem(1)->SetChecked(m_pGooseCb->GetTestFlag());

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_NEED_COMM, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_NEED_COMM),
        _T("")));
    m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_VIEW_GOOSE_NEED_COMM)->GetItem(1)->HasCheckbox(TRUE);
    m_ctlCtrlBlockReport.GetRecordRow(MSGPARAM_VIEW_GOOSE_NEED_COMM)->GetItem(1)->SetChecked(m_pGooseCb->GetNeedComm());

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_GOCB_REF, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOCB_REF),
        m_pGooseCb->GetGocbRef().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_DATA_SET, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_DATA_SET),
        m_pGooseCb->GetDataSet().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_GO_ID, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GO_ID),
        m_pGooseCb->GetGoID().c_str()));

    m_ctlCtrlBlockReport.InsertRecordRow(MSGPARAM_VIEW_GOOSE_CHAN_COUNT, new CDsmGooseCtrlInfoRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_COUNT),
        m_pGooseCb->GetChannelCount()));

    m_ctlCtrlBlockReport.SetFocusedSelectionRowIndex(0);
}
void CBasicParamViewGooseCtrlDlg::_createChannelReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlChannelReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_BASIC_PARAM_VIEW_GOOSE_CHAN_RET);

    // 设置字体
    m_ctlChannelReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlChannelReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlChannelReport.PostInitTemplateStyle();

    // 插入列元素

    m_ctlChannelReport.InsertColumn(0, _T(""), EL_CENTER, DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_INDEX);
    m_ctlChannelReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_DESC), EL_CENTER, DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_DESC);
    m_ctlChannelReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_TYPE), EL_CENTER, DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_TYPE);
    m_ctlChannelReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_CHAN_MAP), EL_CENTER, DSM_BASIC_VIEW_GOOSE_WIDTH_CHAN_MAP);

    // 设置选择焦点样式
    m_ctlChannelReport.SetRowSelectionVisible(FALSE);
    m_ctlChannelReport.SetRowFocusEnable(TRUE);
    m_ctlChannelReport.SetSubItemFocusEnable(TRUE);
    m_ctlChannelReport.SetFreezeColumnsCount(3);
    m_ctlChannelReport.SetFreezeSelectionVisible(TRUE);
    m_ctlChannelReport.SetFocusedColIndex(3);
    m_ctlChannelReport.SetFocus();

#if !SU_FEI
	m_ctlChannelReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif


    int nRowIndex = 0;
    int nIndex = 1;
    int nSubIndex = 1;
    CString strIndex = _T("");
    CString strTemp = _T("");
    CChannel* pChannel = NULL;
    CChannel* pSubChannel = NULL;

    pChannel = m_pGooseCb->FirstChannel();
    while(pChannel)
    {
        //
        //通道,将通道号和子通道号分别设置到0、1列 
        //
        m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseRecChannelRecord(pChannel, nIndex));
        m_ctlChannelReport.SetRecordRowData(nRowIndex, (DWORD_PTR)pChannel);
        ++nRowIndex;

        if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
        {
            //
            //子通道
            //
            nSubIndex = 1;
            pSubChannel = pChannel->FirstSubChannel();
            while(pSubChannel)
            {
                m_ctlChannelReport.InsertRecordRow(nRowIndex, new CDsmEditGooseRecChannelRecord(pSubChannel, nIndex, nSubIndex));
                m_ctlChannelReport.SetRecordRowData(nRowIndex,  (DWORD_PTR)pSubChannel);
                ++nRowIndex;
                ++nSubIndex;
                pSubChannel = pChannel->NextSubChannel(pSubChannel);
            }
        }
        ++nIndex;
        pChannel = m_pGooseCb->NextChannel(pChannel);
    }
    
    if (m_ctlChannelReport.GetRecordRowCount() > 0)
    {
        m_ctlChannelReport.SetFocusedSelectionRowIndex(0);
    }
    m_ctlChannelReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
}

/**
 *      显示控制块报表
 */
void CBasicParamViewGooseCtrlDlg::_showCtrlBlockReport()
{
    if(m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        m_ctlCtrlBlockReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createCtrlBlockReport();
    }
    
    m_nShowReportType = VGShowCtrlBlockReport;

    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_HIDE);
    }
#if SU_FEI
    m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL), NULL, NULL);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_SEL), NULL, NULL);
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_UNSEL), NULL, NULL);

#endif
    m_pNaviBar->UpdateNaviMenu();

    m_ctlCtrlBlockReport.SetFocus();
    CString csCaption;
    csCaption.Format(L"%s", dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_GOOSE_CTRL_BLOCK_CAPTION));
    CStatusMgr::SetTitle(csCaption);

}

/**
 *      显示通道报表
 */
void CBasicParamViewGooseCtrlDlg::_showChannelReport()
{
    if(m_ctlChannelReport.GetSafeHwnd())
    {
        m_ctlChannelReport.ShowWindow(SW_SHOW);
    }
    else
    {
        _createChannelReport();
    }

    m_nShowReportType = VGShowChannelReport;
    if(m_ctlCtrlBlockReport.GetSafeHwnd())
    {
        m_ctlCtrlBlockReport.ShowWindow(SW_HIDE);
    }
    m_ctlChannelReport.SetFocus();
#if SU_FEI
    m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN), NULL, NULL);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CTRL_UNSEL), NULL, NULL);
	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_CTRLCHAN_CHAN_SEL), NULL, NULL);
#endif
    m_pNaviBar->UpdateNaviMenu();

    CString csCaption;
    csCaption.Format(_T("%s-0x%04X"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_VIEW_CHAN_CAPTION), m_pGooseCb->GetAPPID());
    CStatusMgr::SetTitle(csCaption);
}

void CBasicParamViewGooseCtrlDlg::OnViewChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    ASSERT(pItem);
    
    CChannel* pChannel = (CChannel*)m_ctlChannelReport.GetRecordRowData(pItemNotify->rowIndex);
    ASSERT(pChannel);
    if (pChannel != NULL)
    {
        if (pItem->GetHasCheckbox())
        {
            pChannel->Selected(pItem->IsChecked() == TRUE ? true : false);
        }
    }
    *pResult = TRUE;
}

void CBasicParamViewGooseCtrlDlg::OnMenuParamViewGoosesCtrlF1()
{
#if SU_FEI
    if(m_nShowReportType == VGShowCtrlBlockReport)
    {
        m_nShowReportType = VGShowChannelReport;
        _showChannelReport();
    }
    else
    {
        m_nShowReportType = VGShowCtrlBlockReport;
        _showCtrlBlockReport();
    }
#else
	
		m_nShowReportType = VGShowCtrlBlockReport;
		_showCtrlBlockReport();
	
#endif
}

#if !SU_FEI
void CBasicParamViewGooseCtrlDlg::OnMenuParamViewGoosesCtrlF2()
{
		m_nShowReportType = VGShowChannelReport;
		_showChannelReport();
}

BOOL CBasicParamViewGooseCtrlDlg::PreTranslateMessage(MSG* pMsg)
{

	BOOL nRet = CBaseDlg::PreTranslateMessage(pMsg);


	CChannel* pChannel = NULL;

	if (WM_KEYDOWN == pMsg->message)
	{
		switch(pMsg->wParam)
		{
		case VK_UP:
			{			
				if (m_nShowReportType == VGShowChannelReport)
				{
					if (m_ctlChannelReport.GetFocusedColIndex() == 3)
					{
						int index = m_ctlChannelReport.GetSelectionRowIndex();


						pChannel = m_pGooseCb->GetChannelByIndex(index);

						if (pChannel != NULL)
						{

							if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
							{
								//判断选中项是否可以编辑


								if (index < 0)
								{
									index = m_ctlChannelReport.GetRecordRowCount();
								}
								while(!m_ctlChannelReport.GetRecordRow(index)->GetItem(3)->IsEditable())
								{
									index--;
									if (index <= 0)
									{
										index = m_ctlChannelReport.GetRecordRowCount();
									}
								}
								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);

								break;


							}
							/*else
							{
								pChannel = m_pGooseCb->PrevChannel(pChannel);
								index--;

								while(pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_POINT &&
									pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
								{
									if (pChannel == m_pGooseCb->FirstChannel())
									{
										pChannel = m_pGooseCb->LastChannel();
										index = m_ctlChannelReport.GetRecordRowCount();
									}
									pChannel = m_pGooseCb->PrevChannel(pChannel);
									index--;

								}

								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);
							}		*/			

						}
						else
						{
							break;
						}

					}
				}
			}
			break;
		case VK_DOWN:
			{			
				if (m_nShowReportType == VGShowChannelReport)
				{
					if (m_ctlChannelReport.GetFocusedColIndex() == 3)
					{
						int index = m_ctlChannelReport.GetSelectionRowIndex();

						pChannel = m_pGooseCb->GetChannelByIndex(index);

						if (pChannel != NULL)
						{

							if (pChannel->GetChannelType() == CHANNEL_TYPE_GOOSE_STRUCT)
							{
								//判断选中项是否可以编辑


								if (index > m_ctlChannelReport.GetRecordRowCount())
								{
									index = 0;
								}
								while(!m_ctlChannelReport.GetRecordRow(index)->GetItem(3)->IsEditable())
								{
									index++;
									if (index >= m_ctlChannelReport.GetRecordRowCount())
									{
										index = 0;
									}
								}
								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);

								break;


							}
							/*else
							{
								pChannel = m_pGooseCb->NextChannel(pChannel);
								index++;

								while(pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_POINT &&
									pChannel->GetChannelType() != CHANNEL_TYPE_GOOSE_DOUBLE_POINT)
								{
									if (pChannel == m_pGooseCb->LastChannel())
									{
										pChannel = m_pGooseCb->FirstChannel();
										index = m_ctlChannelReport.GetRecordRowCount();
									}
									pChannel = m_pGooseCb->NextChannel(pChannel);
									index++;

								}

								m_ctlChannelReport.SetFocusedSelectionRowIndex(index);
							}		*/			

						}
						else
						{
							break;
						}

					}
				}
			}
			break;
		default:
			break;
		}
	}

	return nRet;
}

#endif

void CBasicParamViewGooseCtrlDlg::_SavePage()
{ 
    CParamConfigGOOSERecv* pParamConfigGooseRec= CParamConfigGOOSERecv::getInstance();
    if (NULL == pParamConfigGooseRec)
    {
        return ;
    }
    pParamConfigGooseRec->SaveConfig();
}

BOOL CBasicParamViewGooseCtrlDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    KillTimer(1);
    return CBaseDlg::DestroyWindow();
}

void CBasicParamViewGooseCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        CParamConfigGOOSERecv* pParamConfigGooseRec= CParamConfigGOOSERecv::getInstance();
        if (NULL == pParamConfigGooseRec)
        {
            return ;
        }
        pParamConfigGooseRec->SaveConfig();
    }
    CBaseDlg::OnTimer(nIDEvent);
}
