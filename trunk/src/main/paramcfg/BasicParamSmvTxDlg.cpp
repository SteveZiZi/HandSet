// E:\pragram code\codewc\src\main\paramcfg\BasicParamSmvTxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "src/main/common.h"

#include "BasicParamSmvTxDlg.h"




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
#include "src/service/datalayer/config/ParamConfigSMVSend.h"

#include "BasicParamACDCDlg.h"



// CBasicParamSmvTxDlg dialog



///<  SMV发送设置
enum SSRecordFuncType
{
	RFTUnknown = 0,
	RFTSmvType ,                                    ///< SMV类型
	RFTSampleValDis,                                ///< 采样值显示
	RFTSampleFreq,                                  ///< 采样频率
	RFTASDUCount,                                   ///< ASDU
	RFTACDC,                                        ///< 交直流
	RFTBaudRate,                                    ///< 波特率
	RFTSmvSend                                      ///< SMV发送
};
//
////
////smv发送
////
#define MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX       5        ///< SMV发送起始行(91)
#define MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX       5        ///< SMV发送起始行(92)
#define MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX      5        ///< SMV发送起始行(FT3)
#define MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX  5        ///< SMV发送起始行(FT3扩展)


IMPLEMENT_DYNAMIC(CBasicParamSmvTxDlg, CBaseDlg)

CBasicParamSmvTxDlg::CBasicParamSmvTxDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicParamSmvTxDlg::IDD, DSM_STR_BASIC_PARAM_MSG, pParent)
{
	m_nMenuID	= IDR_MENU_MSG_PARAM_SMV_SEND;
	
}

CBasicParamSmvTxDlg::~CBasicParamSmvTxDlg()
{
}

void CBasicParamSmvTxDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamSmvTxDlg, CBaseDlg)
	ON_COMMAND_RANGE(ID_MENU_PARAM_SMVS_F1, ID_MENU_PARAM_SMVS_F6, &CBasicParamSmvTxDlg::OnNaviMenuParamSmvSend)
	
	///<  SV设置参数
	ON_NOTIFY(ELNM_OKCLICK, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamSmvTxDlg::OnSmvSendOkClick)
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamSmvTxDlg::OnSmvSendItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamSmvTxDlg::OnSmvSendItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASIC_PARAM_SMV_SEND_RET, &CBasicParamSmvTxDlg::OnSmvSendItemSelectChanged)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASIC_PARAM_SMV_SEND_RET,  &CBasicParamSmvTxDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASIC_PARAM_SMV_SEND_RET,  &CBasicParamSmvTxDlg::OnInplaceEditAction)


	ON_WM_DESTROY()
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CBasicParamSmvTxDlg message handlers
BOOL CBasicParamSmvTxDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();
	
	///< 设置标题
	CString csCaption = L"";
	csCaption.Format(L"%s->%s",dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_MSG), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_CAPTION));
	CStatusMgr::SetTitle(csCaption);

	_restorePage();
	
	SetTimer(1, 4800, NULL);
	
	 return FALSE;
}

BOOL CBasicParamSmvTxDlg::PreTranslateMessage(MSG* pMsg)
{

	
	return CBaseDlg::PreTranslateMessage(pMsg);
}

//退出时，清除定时器
void CBasicParamSmvTxDlg::OnDestroy()
{
	CBaseDlg::OnDestroy();

	// TODO: 在此处添加消息处理程序代码


	KillTimer(1);


}

//启动定时器  
void CBasicParamSmvTxDlg::OnTimer(UINT_PTR nIDEvent)
{
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
///
/// @brief
///      响应从F1到F6功能键
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::OnNaviMenuParamSmvSend(UINT nID)
{
	switch(nID)
	{

	case ID_MENU_PARAM_SMVS_F1:
		{
			//全站配置添加smv
			CEventProc::GetInstance()->ShowPage(IDD_SEL_SMV_FROM_SCL_DLG);

		}	
		break;
	case ID_MENU_PARAM_SMVS_F2:
		{
			//从扫描中添加SMV
			CEventProc::GetInstance()->ShowPage(IDD_SEL_SMV_FROM_SCAN_DLG);
		}
		break;

	case ID_MENU_PARAM_SMVS_F3:
		{
			//_showSettingReport(); 
			//手动添加
			CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
			if (NULL == pParamConfigSMVSend)
			{
				return ;
			}


			//手动添加一条记录
			int nRowIndex = m_ctlSetting.GetRecordRowCount();
			//send索引
			int nIndex = 0;   

			CBaseCb* pBaseCb = NULL;
			switch (pParamConfigSMVSend->GetSMVType())
			{
			case FRAME_TYPE_NET_9_1:
				{
					nIndex = nRowIndex - MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX + 1;
					pBaseCb = pParamConfigSMVSend->AddIEC9_1Cb();
				}
				break;
			case FRAME_TYPE_NET_9_2:
				{
					nIndex = nRowIndex - MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX + 1;
					pBaseCb = pParamConfigSMVSend->AddIEC9_2Cb();
				}
				break;
			case FRAME_TYPE_FT3_NANRUI:
				{
					nIndex = nRowIndex - MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX + 1;
					pBaseCb = pParamConfigSMVSend->AddFt3Cb();
				}
				break;
			case FRAME_TYPE_FT3_GUOWANG:
				{
					nIndex = nRowIndex - MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX + 1;
					pBaseCb = pParamConfigSMVSend->AddFt3ExpandCb();
				}
				break;
			default:
				break;
			}
			ASSERT(pBaseCb);
			if (pBaseCb == NULL)
			{
				return;
			}

			CString strCaption = _T("");
			CString strContent = _T("");
			strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), nIndex);
			strContent.Format(_T("0x%04X-%s"), pBaseCb->GetAPPID(), pBaseCb->GetDescription().c_str());

			m_ctlSetting.InsertRecordRow(nRowIndex, new CDsmBasicSmvSendRecord(strCaption , strContent, RFTSmvSend));
			m_ctlSetting.SetFocusedSelectionRowIndex(nRowIndex);
			m_ctlSetting.EnsureVisible(nRowIndex, FALSE);

			CELRecordItem * pItem = m_ctlSetting.GetRecordRow(nRowIndex)->GetItem(2);
			if (pItem != NULL)
			{
				pItem->HasCheckbox(TRUE);
				pItem->SetChecked(pBaseCb->IsSelected());
				pItem->SetItemData((DWORD_PTR)pBaseCb);
			}

		
		}
		break;

	case ID_MENU_PARAM_SMVS_F4:
		{

			//编辑控制块
			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}
			//编辑
			int nRowSel = m_ctlSetting.GetFocusedRowIndex();
			if (nRowSel >= 0)
			{
				SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(nRowSel, 0);
				if (ssRft == RFTSmvSend)
				{
					CEventProc::GetInstance()->ShowPage(IDD_EDIT_SMV_SEND_DLG);
				}
			}
		}
		break;

	case ID_MENU_PARAM_SMVS_F5:
		{
			
			//删除控制块
			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}

			m_ctlSetting.SetRedraw(FALSE);

			int nRowSel = m_ctlSetting.GetFocusedRowIndex();
			if (nRowSel >= 0)
			{
				SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(nRowSel, 0);
				if (ssRft == RFTSmvSend)
				{
					CBaseCb* pBaseCb = (CBaseCb*) m_ctlSetting.GetRowItemData(nRowSel, 2);
					ASSERT(pBaseCb);
					if (pBaseCb != NULL)
					{

						CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
						if (NULL == pParamConfigSMVSend)
						{
							m_ctlSetting.SetRedraw(TRUE);
							return ;
						}
						switch (pParamConfigSMVSend->GetSMVType())
						{
						case FRAME_TYPE_NET_9_1:
							{
								pParamConfigSMVSend->DeleteIEC9_1Cb((CIEC9_1Cb*)pBaseCb);
							}
							break;
						case FRAME_TYPE_NET_9_2:
							{
								pParamConfigSMVSend->DeleteIEC9_2Cb((CIEC9_2Cb*)pBaseCb);
							}
							break;
						case FRAME_TYPE_FT3_NANRUI:
							{
								pParamConfigSMVSend->DeleteFt3Cb((CFT3Cb*)pBaseCb);
							}
							break;
						case FRAME_TYPE_FT3_GUOWANG:
							{
								pParamConfigSMVSend->DeleteFt3ExpandCb((CFT3ExpandCb*)pBaseCb);
							}
							break;
						default:
							break;
						}
					}
					m_ctlSetting.DeleteRecordRow(nRowSel);

					CString strCaption = _T(""); 
					CELRecordItemText* pItemCaption = NULL;
					int nIndex = 1;
					for (int i = 0; i < m_ctlSetting.GetRecordRowCount(); ++i)
					{
						ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(i, 0);
						if (ssRft == RFTSmvSend)
						{
							strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), nIndex);
							pItemCaption = (CELRecordItemText*) m_ctlSetting.GetRecordRow(i)->GetItem(1);
							pItemCaption->SetValue(strCaption);
							m_ctlSetting.RedrawRowItem(i, 1);
							++nIndex;
						}
					}
				}
			}
			m_ctlSetting.SetRedraw(TRUE);
		}		
		break;

	case ID_MENU_PARAM_SMVS_F6:
		{
			//清空控制块
			if (GetFocus()->GetSafeHwnd() != m_ctlSetting.GetSafeHwnd())
			{
				return;
			}

			CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
			if (NULL == pParamConfigSMVSend)
			{
				return ;
			}
			switch(pParamConfigSMVSend->GetSMVType())
			{
			case FRAME_TYPE_NET_9_1:
				{
					while(MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX < m_ctlSetting.GetRecordRowCount())
					{
						m_ctlSetting.DeleteRecordRow(MSGPARAM_BASIC_91_SMVS_START_ROW_INDEX);
					}
					pParamConfigSMVSend->ClearAllIEC9_1Cb();
				}
				break;
			case FRAME_TYPE_NET_9_2:
				{
					while(MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX < m_ctlSetting.GetRecordRowCount())
					{
						m_ctlSetting.DeleteRecordRow(MSGPARAM_BASIC_92_SMVS_START_ROW_INDEX);
					}
					pParamConfigSMVSend->ClearAllIEC9_2Cb();
				}
				break;
			case FRAME_TYPE_FT3_NANRUI:
				{
					while(MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX < m_ctlSetting.GetRecordRowCount())
					{
						m_ctlSetting.DeleteRecordRow(MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX);
					}
					pParamConfigSMVSend->ClearAllFt3Cb();
				}
				break;
			case FRAME_TYPE_FT3_GUOWANG:
				{
					while(MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX < m_ctlSetting.GetRecordRowCount())
					{
						m_ctlSetting.DeleteRecordRow(MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX);
					}
					pParamConfigSMVSend->ClearAllFt3ExpandCb();
				}
				break;
			default:
				break;
			}

		}	
		break;

	default:
		break;
	}
}

/*********************/
/* 创建setting报表*/
/**/
/**/
/**/
/****************/
void CBasicParamSmvTxDlg::_createSettingReport()
{
	CRect rcReport(DSM_BASICCFG_RCRIGHT);
	DSM_ParentToChild(this, rcReport);

	m_ctlSetting.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_SMV_SEND_RET);

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
	//
	//根据SMV类型调用
	//
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
	enumFrameType nSmvSendType = pParamConfigSMVSend->GetSMVType();

	// SMV类型
	m_ctlSetting.InsertRecordRow(0, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_TYPE),
		_T(""), RFTSmvType));

	CELRecordItemEditOptions* pOption  = m_ctlSetting.GetRecordRow(0)->GetItem(2)->GetEditOptions(NULL);
	pOption->AddComboButton();
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC61850_92), FRAME_TYPE_NET_9_2);
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC61850_91), FRAME_TYPE_NET_9_1);
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC60044_FT3), FRAME_TYPE_FT3_NANRUI);
	pOption->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_IEC60044_FT3_EXT), FRAME_TYPE_FT3_GUOWANG);

	CELRecordItemConstraint * pItemCon = pOption->FindConstraint(nSmvSendType);
	if (pItemCon != NULL)
	{
		CELRecordItemText* pItemType =  dynamic_cast<CELRecordItemText*>(m_ctlSetting.GetRecordRow(0 )->GetItem(2));
		ASSERT(pItemType);
		pItemType->SetValue(pItemCon->GetConstraintCaption());
	}

	switch(nSmvSendType)
	{
	case FRAME_TYPE_NET_9_1:
		{
			_refreshSmvReportWithIEC6185091();
		}
		break;
	case FRAME_TYPE_NET_9_2:
		{
			_refreshSmvReportWithIEC6185092();
		}
		break;
	case FRAME_TYPE_FT3_NANRUI:
		{
			_refreshSmvReportWithIEC60044FT3();
		}
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		{
			_refreshSmvReportWithIEC60044FT3Ext();
		}
		break;
	default:
		break;
	}
	
	
	
	
}


/*********************/
/* 显示setting报表*/
/**/
/**/
/**/
/****************/
void CBasicParamSmvTxDlg::_showSettingReport()
{
	if (m_ctlSetting.GetSafeHwnd())
	{
		m_ctlSetting.ShowWindow(SW_SHOW);
	}
	else
	{
		_createSettingReport();
	}
	m_ctlSetting.SetFocus();
	m_ctlSetting.SetFocusedSelectionRowIndex(0);

}
///
/// @brief
///      构造交直流电显示
///
/// @return
///      void    
///
CString CBasicParamSmvTxDlg::_getACDCFormatString()
{
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return _T("");
	}

	CString strACDC[16] = {_T("Ua1"),_T("Ub1"), _T("Uc1"),_T("Ux1"), _T("Ua2"),_T("Ub2"), _T("Uc2"), _T("Ux2"),
		_T("Ia1"), _T("Ib1"), _T("Ic1"), _T("Ix1"), _T("Ia2"), _T("Ib2"), _T("Ic2"), _T("Ix2")
	};
	CString strFormat = _T("");
	int nACCount = 0;
	int nDCCount = 0;

	strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_CUR_CHAN);
	for (int i = 0; i < 16; ++i)
	{
		if (pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i]) == CURRENT_TYPE_DC)
		{
			if (nDCCount > 0)
			{
				strFormat += _T(",");
			}
			strFormat += strACDC[i];
			++nDCCount;
		}
		else
		{
			++nACCount;
		}
	}

	if (nACCount == 16)
	{
		strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_ALL_ACUR);
	}
	else if(nDCCount == 16)
	{
		strFormat = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_ALL_DCUR);
	}
	return strFormat;
}

///
/// @brief
///      创建SMV发送9_1参数报表
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_refreshSmvReportWithIEC6185091()
{
	if(!m_ctlSetting.GetSafeHwnd())
	{
		return;
	}

	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

	m_ctlSetting.SetRedraw(FALSE);

	//
	//首先删除索引1及后面的记录项
	//
	while(m_ctlSetting.GetRecordRowCount() > 1)
	{
		m_ctlSetting.DeleteRecordRow(1);
	}

	// 采样值显示
	m_ctlSetting.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
		_T(""), RFTSampleValDis));
	CELRecordItemEditOptions* pOptions  = m_ctlSetting.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
	CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
	if (pItemCon)
	{
		CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(1)->GetItem(2);
		pItem->SetValue(pItemCon->GetConstraintCaption());
	}

	// 采样频率
	m_ctlSetting.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
		_T(""), RFTSampleFreq));
	pOptions  = m_ctlSetting.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
	pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
	if (pItemCon)
	{
		CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(2)->GetItem(2);
		pItem->SetValue(pItemCon->GetConstraintCaption());
	}

	// ASDU数目
	m_ctlSetting.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ASDU_COUNT),
		pParamConfigSMVSend->GetASDUCount(),RFTASDUCount));


	// 交直流设置
	m_ctlSetting.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
		_getACDCFormatString(), RFTACDC));

	//
	//添加SMV发送
	//
	int i = 1;
	CString strCaption = _T("");
	CString strContent = _T("");

	CIEC9_1Cb* pIEC91Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
	while(pIEC91Cb)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
		strContent.Format(_T("0x%04X-%s"), pIEC91Cb->GetAPPID(), pIEC91Cb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
			strCaption , strContent, RFTSmvSend));

		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(4 + i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pIEC91Cb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pIEC91Cb);
		}

		++i;
		pIEC91Cb = pParamConfigSMVSend->NextIEC9_1Cb(pIEC91Cb);
	}


	m_ctlSetting.SetFocusedSelectionRowIndex(0);
	m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
	for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
	{
		m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
	}
	m_ctlSetting.SetRedraw(TRUE);
}

///
/// @brief
///      创建SMV发送参数9_2报表
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_refreshSmvReportWithIEC6185092()
{
	if(!m_ctlSetting.GetSafeHwnd())
	{
		return;
	}

	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

	m_ctlSetting.SetRedraw(FALSE);
	//
	//首先删除索引1及后面的记录项
	//
	while(m_ctlSetting.GetRecordRowCount() > 1)
	{
		m_ctlSetting.DeleteRecordRow(1);
	}

	// 采样值显示
	m_ctlSetting.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
		_T(""), RFTSampleValDis));
	CELRecordItemEditOptions* pOptions  = m_ctlSetting.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
	CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
	if (pItemCon)
	{
		CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(1)->GetItem(2);
		pItem->SetValue(pItemCon->GetConstraintCaption());
	}

	// 采样频率
	m_ctlSetting.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
		_T(""), RFTSampleFreq));
	pOptions  = m_ctlSetting.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
	pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
	if (pItemCon)
	{
		CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(2)->GetItem(2);
		pItem->SetValue(pItemCon->GetConstraintCaption());
	}

	// ASDU数目
	m_ctlSetting.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ASDU_COUNT),
		pParamConfigSMVSend->GetASDUCount(),RFTASDUCount));


	// 交直流设置
	m_ctlSetting.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
		dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
		_getACDCFormatString(), RFTACDC));

	//
	//添加SMV发送
	//
	int i = 1;
	CString strCaption = _T("");
	CString strContent = _T("");

	CIEC9_2Cb* pIEC92Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
	while(pIEC92Cb)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
		strContent.Format(_T("0x%04X-%s"), pIEC92Cb->GetAPPID(), pIEC92Cb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
			strCaption , strContent, RFTSmvSend));

		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(4 + i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pIEC92Cb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pIEC92Cb);
		}

		++i;
		pIEC92Cb = pParamConfigSMVSend->NextIEC9_2Cb(pIEC92Cb);
	}


	m_ctlSetting.SetFocusedSelectionRowIndex(0);
	m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
	for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
	{
		m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
	}

	m_ctlSetting.SetRedraw(TRUE);
}

///
/// @brief
///      创建SMV发送参数ft3报表
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_refreshSmvReportWithIEC60044FT3()
{
    if(!m_ctlSetting.GetSafeHwnd())
    {
        return;
    }

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSetting.SetRedraw(FALSE);
    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSetting.GetRecordRowCount() > 1)
    {
        m_ctlSetting.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSetting.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSetting.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSetting.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSetting.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 交直流设置
    m_ctlSetting.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));

    // 波特率设置
    m_ctlSetting.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE),
        _T(""), RFTBaudRate));
    pOptions  = m_ctlSetting.GetRecordRow(4)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_2_5MBPS), FT3_BAUD_RATE_2P5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_5MBPS), FT3_BAUD_RATE_5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_10MBPS), FT3_BAUD_RATE_10M);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetBaudRate());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(4)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

	//
	//添加SMV发送
	//
	int i = 1;
	CString strCaption = _T("");
	CString strContent = _T("");

	CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
	while(pFt3Cb)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
		strContent.Format(_T("0x%04X-%s"), pFt3Cb->GetAPPID(), pFt3Cb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
			strCaption , strContent, RFTSmvSend));

		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(4 + i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pFt3Cb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pFt3Cb);
		}

		++i;
		pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
	}


    m_ctlSetting.SetFocusedSelectionRowIndex(0);
    m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
    {
        m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }

    m_ctlSetting.SetRedraw(TRUE);
}
///
/// @brief
///      创建SMV发送参数ft3ext报表
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_refreshSmvReportWithIEC60044FT3Ext()
{
    if(!m_ctlSetting.GetSafeHwnd())
    {
        return;
    }
    
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }

    m_ctlSetting.SetRedraw(FALSE);
    //
    //首先删除索引1及后面的记录项
    //
    while(m_ctlSetting.GetRecordRowCount() > 1)
    {
        m_ctlSetting.DeleteRecordRow(1);
    }

    // 采样值显示
    m_ctlSetting.InsertRecordRow(1, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_DIS),
        _T(""), RFTSampleValDis));
    CELRecordItemEditOptions* pOptions  = m_ctlSetting.GetRecordRow(1)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_ONCE), SMV_VALUE_TYPE_PRIMARY);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_VAL_TWICE), SMV_VALUE_TYPE_SECONDARY);
    CELRecordItemConstraint* pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetSVDisplay());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(1)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 采样频率
    m_ctlSetting.InsertRecordRow(2, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ),
        _T(""), RFTSampleFreq));
    pOptions  = m_ctlSetting.GetRecordRow(2)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_4K), FT3_SAMP_RATE_4K);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SAMPLE_FREQ_12_8K), FT3_SAMP_RATE_12P8K);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetFrequency());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(2)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

    // 交直流设置
    m_ctlSetting.InsertRecordRow(3, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC),
        _getACDCFormatString(), RFTACDC));

    // 波特率设置
    m_ctlSetting.InsertRecordRow(4, new CDsmBasicSmvSendRecord(
        dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE),
        _T(""), RFTBaudRate));
    pOptions  = m_ctlSetting.GetRecordRow(4)->GetItem(2)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_5MBPS), FT3_BAUD_RATE_5M);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_BAUD_RATE_10MBPS), FT3_BAUD_RATE_10M);
    pItemCon = pOptions->FindConstraint(pParamConfigSMVSend->GetBaudRate());
    if (pItemCon)
    {
        CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(4)->GetItem(2);
        pItem->SetValue(pItemCon->GetConstraintCaption());
    }

	//
	//添加SMV发送
	//
	int i = 1;
	CString strCaption = _T("");
	CString strContent = _T("");

	CFT3ExpandCb* pFt3ExpandCb = pParamConfigSMVSend->FirstFt3ExpandCb();
	while(pFt3ExpandCb)
	{
		strCaption.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_SMV_SEND), i);
		strContent.Format(_T("0x%04X-%s"), pFt3ExpandCb->GetAPPID(), pFt3ExpandCb->GetDescription().c_str());

		m_ctlSetting.InsertRecordRow( 4+i, new CDsmBasicSmvSendRecord(
			strCaption , strContent, RFTSmvSend));

		CELRecordItem * pItem = m_ctlSetting.GetRecordRow(4 + i)->GetItem(2);
		if (pItem != NULL)
		{
			pItem->HasCheckbox(TRUE);
			pItem->SetChecked(pFt3ExpandCb->IsSelected());
			pItem->SetItemData((DWORD_PTR)pFt3ExpandCb);
		}

		++i;
		pFt3ExpandCb = pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExpandCb);
	}


    m_ctlSetting.SetFocusedSelectionRowIndex(0);
    m_ctlSetting.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);

    for(int i = 0; i < m_ctlSetting.GetItemCount(); i++)
    {
        m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
    }

    m_ctlSetting.SetRedraw(TRUE);
}


///
/// @brief
///      恢复页面
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_restorePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CBasicParamSmvTxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	if(pData->m_eAction == PageBack)
	{
		//从其他页面返回


		CBasicParamPageDataExtraSmv* pDataExtra = dynamic_cast<CBasicParamPageDataExtraSmv*>(pData->m_pPageDataExtra);
		ASSERT(pDataExtra != NULL);
		if (pDataExtra == NULL)
		{
			return;
		}
		//显示对应报表
		if (pDataExtra->m_nMsgParamType == DSM_MSG_SMV_PARAM)
		{
			_showSettingReport();
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
		_showSettingReport();
	}
}


///
/// @brief
///      保存页面
///
/// @return
///      void    
///
void CBasicParamSmvTxDlg::_SavePage()
{
	CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

	if(!pData || CBasicParamSmvTxDlg::IDD != pData->m_nPageID)
	{
		return;
	}

	pData->m_nCtrlID = GetFocus()->GetDlgCtrlID();

	
	if (pData->m_pPageDataExtra == NULL)
	{
		pData->m_pPageDataExtra = new CBasicParamPageDataExtraSmv(pData);
	}

	CBasicParamPageDataExtraSmv* pDataExtra = dynamic_cast<CBasicParamPageDataExtraSmv*>(pData->m_pPageDataExtra);
	ASSERT(pDataExtra != NULL);
	if (pDataExtra == NULL)
	{
		return;
	}

	//
	
	pDataExtra->m_nMsgParamType = DSM_MSG_SMV_PARAM;
	pDataExtra->m_nRightReportFocus = m_ctlSetting.GetFocusedRowIndex();
	
	int nRowSel = 0;
	//
	//选中的SMV发送控制块，用于编辑SMV发送
	//
	if(m_ctlSetting.GetSafeHwnd())
	{
		nRowSel = m_ctlSetting.GetFocusedRowIndex();
		if (nRowSel >=0)
		{
			SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(nRowSel, 0);
			if (ssRft == RFTSmvSend)
			{
				pDataExtra->m_pSmvSendSelectedCb = (CBaseCb*) m_ctlSetting.GetRowItemData(nRowSel, 2);
			}
			else
			{
				pDataExtra->m_pSmvSendSelectedCb = NULL;
			}
		}
	}
	else
	{
		pDataExtra->m_pSmvSendSelectedCb = NULL;
	}



	//恢复表格控制块信息
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
	
	
		
	pParamConfigSMVSend->SaveConfig();

}


void CBasicParamSmvTxDlg::OnSmvSendOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNotifyStruct);
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	int nRowIndex = m_ctlSetting.GetFocusedRowIndex();
	if (nRowIndex < 0)
	{
		return;
	}

	SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(nRowIndex, 0);

	switch (ssRft)
	{
	case RFTACDC:
		{
			CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_ACDC_DLG);
			*pResult = TRUE;
		}
		break;
	} 
}


void CBasicParamSmvTxDlg::OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
	if (pItemNotify == NULL)
	{
		return;
	}

	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

	SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(pItemNotify->rowIndex, 0);

	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
	if (pItem == NULL)
	{
		return;
	}

	switch(ssRft)
	{
	case RFTASDUCount:
		{
			CString strValue = pItem->GetValue();
			int nValue = _ttoi(strValue);
			pParamConfigSMVSend->SetASDUCount(nValue);
			strValue.Format(_T("%d"), pParamConfigSMVSend->GetASDUCount());
			pItem->SetValue(strValue);
			*pResult = TRUE;
		}
		break;
	case RFTSmvSend:
		{
			if(pItem->GetHasCheckbox())
			{
				CBaseCb* pBaseCb = (CBaseCb*)pItem->GetItemData();
				if (pBaseCb != NULL)
				{
					bool bCheck = pItem->IsChecked() ? true: false;

					if (bCheck)
					{
						//
						//判断已经选中多少个控制块
						//
						enumFrameType smvType = pParamConfigSMVSend->GetSMVType();
						switch(smvType)
						{
						case FRAME_TYPE_NET_9_1:
							{

								FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();

								if (sampRate == FT3_SAMP_RATE_4K)
								{
									if (pParamConfigSMVSend->GetSelectedIEC9_1CbCount() < MSGPARAM_SMV_SEND_91_4K_SELECTED_MAX)
									{
										pBaseCb->Selected(bCheck);
									}
									else 
									{
										pItem->SetChecked(FALSE);
										CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_91_4K_CTRL_SEL_MAX), 3000);
									}
								}
								else if (sampRate == FT3_SAMP_RATE_12P8K)
								{
									if (pParamConfigSMVSend->GetSelectedIEC9_1CbCount() < MSGPARAM_SMV_SEND_91_12P8K_SELECTED_MAX)
									{
										pBaseCb->Selected(bCheck);
									}
									else 
									{
										pItem->SetChecked(FALSE);
										CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_91_128K_CTRL_SEL_MAX), 3000);
									}
								}
							}
							break;
						case FRAME_TYPE_NET_9_2:
							{
								FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();

								if (sampRate == FT3_SAMP_RATE_4K)
								{
									if (pParamConfigSMVSend->GetSelectedIEC9_2CbCount() < MSGPARAM_SMV_SEND_92_4K_SELECTED_MAX)
									{
										pBaseCb->Selected(bCheck);
									}
									else 
									{
										pItem->SetChecked(FALSE);
										CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_4K_CTRL_SEL_MAX), 3000);
									}
								}
								else if (sampRate == FT3_SAMP_RATE_12P8K)
								{
									if (pParamConfigSMVSend->GetSelectedIEC9_2CbCount() < MSGPARAM_SMV_SEND_92_12P8K_SELECTED_MAX)
									{
										pBaseCb->Selected(bCheck);
									}
									else 
									{
										pItem->SetChecked(FALSE);
										CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_92_128K_CTRL_SEL_MAX), 3000);
									}
								}
							}
							break;
						case FRAME_TYPE_FT3_NANRUI:
							{
								if (pParamConfigSMVSend->GetSelectedFt3CbCount() > 0)
								{
									//只允许选中一个
									for(int i = MSGPARAM_BASIC_FT3_SMVS_START_ROW_INDEX; i < m_ctlSetting.GetRecordRowCount(); ++i)
									{
										CBaseCb* pFt3Cb = (CBaseCb*)m_ctlSetting.GetRowItemData(i, 2);
										if (pFt3Cb != pBaseCb && pFt3Cb->IsSelected())
										{
											pFt3Cb->Selected(false);
											m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetChecked(FALSE);
											m_ctlSetting.RedrawRowItem(i, 2);
										}
									}
								}
								pBaseCb->Selected(bCheck);
							}
							break;
						case FRAME_TYPE_FT3_GUOWANG:
							{ 
								if (pParamConfigSMVSend->GetSelectedFt3ExpandCbCount() > 0)
								{
									//只允许选中一个
									for(int i = MSGPARAM_BASIC_FT3_EXT_SMVS_START_ROW_INDEX; i < m_ctlSetting.GetRecordRowCount(); ++i)
									{
										CBaseCb* pFt3ExtCb = (CBaseCb*)m_ctlSetting.GetRowItemData(i, 2);
										if (pFt3ExtCb != pBaseCb && pFt3ExtCb->IsSelected())
										{
											pFt3ExtCb->Selected(false);
											m_ctlSetting.GetRecordRow(i)->GetItem(2)->SetChecked(FALSE);
											m_ctlSetting.RedrawRowItem(i, 2);
										}
									}
								}
								pBaseCb->Selected(bCheck);
							}
						}
					}
					else
					{
						pBaseCb->Selected(bCheck);
						CStatusMgr::SetTips(_T(""), 100);
					}
				}
			}
			*pResult = TRUE;
		}

	}
}

void CBasicParamSmvTxDlg::OnSmvSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);
	*pResult = FALSE;

	ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

	if(!pItemNotify)
	{
		return;
	}

	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

	SSRecordFuncType ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(pItemNotify->rowIndex, 0);
	DWORD dwValue = pItemNotify->pConstraint->GetConstraintData();

	switch(ssRft)
	{
		// 通道类型
	case RFTSmvType:
		{
			pParamConfigSMVSend->SetSMVType((enumFrameType)dwValue);
			switch(dwValue)
			{
			case FRAME_TYPE_NET_9_1:
				{
					_refreshSmvReportWithIEC6185091();
				}
				break;
			case FRAME_TYPE_NET_9_2:
				{
					_refreshSmvReportWithIEC6185092();
				}
				break;
			case FRAME_TYPE_FT3_NANRUI:
				{
					_refreshSmvReportWithIEC60044FT3();
				}
				break;
			case FRAME_TYPE_FT3_GUOWANG:
				{
					_refreshSmvReportWithIEC60044FT3Ext();
				}
				break;
			default:
				break;
			}
			*pResult = TRUE;
		}
		break;
	case RFTSampleValDis:
		{
			//采样值显示
			pParamConfigSMVSend->SetSVDisplay((enumSmvValueType)dwValue);
			*pResult = TRUE;
		}
		break;

	case RFTSampleFreq:
		{
			//采样频率
			pParamConfigSMVSend->SetFrequency((FT3_SAMP_RATE)dwValue);
			switch(pParamConfigSMVSend->GetSMVType())
			{
			case FRAME_TYPE_NET_9_1:
				{
					CELRecordItem* pItem = NULL;
					CIEC9_1Cb* pIEC91Cb = NULL;
					SSRecordFuncType ssRft = RFTUnknown;
					for (int i = 0; i < m_ctlSetting.GetRecordRowCount(); ++i)
					{
						ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(i, 0);
						if (ssRft == RFTSmvSend)
						{
							pItem = m_ctlSetting.GetRecordRow(i)->GetItem(2);
							if (pItem)
							{
								pIEC91Cb = (CIEC9_1Cb*)pItem->GetItemData();
								pItem->SetChecked(pIEC91Cb->IsSelected());
								m_ctlSetting.RedrawRowItem(i, 2);
							}
						}
#if !SHENG_ZE
						// 当ASDU值为5，采样频率由4K变到12.8K的时候自动变为4
						else if (ssRft == RFTASDUCount)
						{
							pItem = m_ctlSetting.GetRecordRow(i)->GetItem(2);
							if (pItem)
							{
								unsigned int numASDU = pParamConfigSMVSend->GetASDUCount();
								FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();
								if(FT3_SAMP_RATE_12P8K == sampRate && 5 == numASDU)
								{
									CString strValue;
									int nValue = 4;
									pParamConfigSMVSend->SetASDUCount(nValue);
									strValue.Format(_T("%d"), nValue);

									CELRecordItemText* ppItem = dynamic_cast<CELRecordItemText*>(pItem);
									ppItem->SetValue(strValue);
									m_ctlSetting.RedrawRowItem(i, 2);
								}
							}
						}
#endif
					}
				}
				break;
			case FRAME_TYPE_NET_9_2:
				{
					CELRecordItem* pItem = NULL;
					CIEC9_2Cb* pIEC92Cb = NULL;
					SSRecordFuncType ssRft = RFTUnknown;
					for (int i = 0; i < m_ctlSetting.GetRecordRowCount(); ++i)
					{
						ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(i, 0);
						if (ssRft == RFTSmvSend)
						{
							pItem = m_ctlSetting.GetRecordRow(i)->GetItem(2);
							if (pItem)
							{
								pIEC92Cb = (CIEC9_2Cb*)pItem->GetItemData();
								pItem->SetChecked(pIEC92Cb->IsSelected());
								m_ctlSetting.RedrawRowItem(i, 2);
							}
						}
#if !SHENG_ZE
						// 当ASDU值为5，采样频率由4K变到12.8K的时候自动变为4
						else if (ssRft == RFTASDUCount)
						{
							pItem = m_ctlSetting.GetRecordRow(i)->GetItem(2);
							if (pItem)
							{
								unsigned int numASDU = pParamConfigSMVSend->GetASDUCount();
								FT3_SAMP_RATE sampRate = pParamConfigSMVSend->GetFrequency();
								if(FT3_SAMP_RATE_12P8K == sampRate && 5 == numASDU)
								{
									CString strValue;
									int nValue = 4;
									pParamConfigSMVSend->SetASDUCount(nValue);
									strValue.Format(_T("%d"), nValue);

									CELRecordItemText* ppItem = dynamic_cast<CELRecordItemText*>(pItem);
									ppItem->SetValue(strValue);
									m_ctlSetting.RedrawRowItem(i, 2);
								}
							}
						}
#endif
					}
				}
				break;
			case FRAME_TYPE_FT3_NANRUI:
				//{
				//    if (FT3_SAMP_RATE_12P8K == dwValue)
				//    {
				//        pParamConfigSMVSend->SetBaudRate(FT3_BAUD_RATE_10M);

				//        SSRecordFuncType ssRft = RFTUnknown;
				//        
				//        for (int i = 0; i < m_ctlSmvSendReport.GetRecordRowCount(); ++i)
				//        {
				//            ssRft = (SSRecordFuncType)m_ctlSmvSendReport.GetRowItemData(i, 0);
				//            if (ssRft == RFTBaudRate)
				//            {
				//                CELRecordItemConstraint* pItemCon  = m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2)->GetEditOptions(NULL)->FindConstraint(pParamConfigSMVSend->GetBaudRate());
				//                if (pItemCon)
				//                {
				//                    CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSmvSendReport.GetRecordRow(i)->GetItem(2);
				//                    pItem->SetValue(pItemCon->GetConstraintCaption());
				//                    m_ctlSmvSendReport.RedrawRowItem(i, 2);
				//                }
				//                break;
				//            }
				//        }
				//    }
				//}
				//break;
			case FRAME_TYPE_FT3_GUOWANG:
				{
					if (FT3_SAMP_RATE_12P8K == dwValue)
					{
						pParamConfigSMVSend->SetBaudRate(FT3_BAUD_RATE_10M);

						SSRecordFuncType ssRft = RFTUnknown;

						for (int i = 0; i < m_ctlSetting.GetRecordRowCount(); ++i)
						{
							ssRft = (SSRecordFuncType)m_ctlSetting.GetRowItemData(i, 0);
							if (ssRft == RFTBaudRate)
							{
								CELRecordItemConstraint* pItemCon  = m_ctlSetting.GetRecordRow(i)->GetItem(2)->GetEditOptions(NULL)->FindConstraint(pParamConfigSMVSend->GetBaudRate());
								if (pItemCon)
								{
									CELRecordItemText* pItem = (CELRecordItemText*)m_ctlSetting.GetRecordRow(i)->GetItem(2);
									pItem->SetValue(pItemCon->GetConstraintCaption());
									m_ctlSetting.RedrawRowItem(i, 2);
								}
								break;
							}
						}
					}
				}
				break;
			}
			*pResult = TRUE;
		}
		break;
		
	case RFTBaudRate:
		{
			//波特率
			pParamConfigSMVSend->SetBaudRate((FT3_BAUD_RATE)dwValue);
			*pResult = TRUE;
		}
		break;
	default:
		break;
	}
}

void CBasicParamSmvTxDlg::OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult)
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