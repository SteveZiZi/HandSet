/// @file
///
/// @brief
///     交直流设置实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#include "stdafx.h"
#include "BasicParamACDCDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/config/ParamConfigSMVSend.h"


//////////////////////////////////////////////////////////////////////////
// 交直流设置
//
#if SU_FEI
#define DSM_BASIC_ACDC_WIDTH_CHAN1               120
#define DSM_BASIC_ACDC_WIDTH_SET1                180
#define DSM_BASIC_ACDC_WIDTH_CHAN2               120
#define DSM_BASIC_ACDC_WIDTH_SET2                180
#else
#define DSM_BASIC_ACDC_WIDTH_CHAN1               90
#define DSM_BASIC_ACDC_WIDTH_SET1                176
#define DSM_BASIC_ACDC_WIDTH_CHAN2               90
#define DSM_BASIC_ACDC_WIDTH_SET2                157


#define DSM_VOL_REPORT_RECT              CRect(110, DSM_P_BODY_TOP, 375, DSM_P_BODY_BOTTOM)        ///< 左侧电压区域
#define DSM_CUR_REPORT_RECT				 CRect(375, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< 右侧电流区域
#define IDC_BASIC_PARAM_ACDC_RET_CUR	 1129
#endif

// CBasicParamACDCDlg 对话框

IMPLEMENT_DYNAMIC(CBasicParamACDCDlg, CBaseDlg)

CBasicParamACDCDlg::CBasicParamACDCDlg(CWnd* pParent /*=NULL*/)
: CBaseDlg(CBasicParamACDCDlg::IDD, DSM_STR_BASIC_PARAM_ACDC_CAPTION, pParent)
{
	m_nMenuID = IDR_MENU_ParamACDC;

}

CBasicParamACDCDlg::~CBasicParamACDCDlg()
{
}

void CBasicParamACDCDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicParamACDCDlg, CBaseDlg)
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_ACDC_RET, &CBasicParamACDCDlg::OnACDCItemValueChanged)
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASIC_PARAM_ACDC_RET_CUR, &CBasicParamACDCDlg::OnACDCItemCurValueChanged)
	ON_WM_TIMER()

#if !CHUN_HUA
	ON_COMMAND_RANGE(ID_PARAM_ACDC_F1, ID_PARAM_ACDC_F2, &CBasicParamACDCDlg::OnNaviMenuParamSmvSend)
#endif

END_MESSAGE_MAP()


// CBasicParamACDCDlg 消息处理程序


BOOL CBasicParamACDCDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);
	_createReport();
#if !SU_FEI
	_createReportCur();
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBasicParamACDCDlg::_createReportCur()
{
	CRect rcReport(DSM_CUR_REPORT_RECT);
	DSM_ParentToChild(this, rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlACDCReport_Cur.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_ACDC_RET_CUR);

	// 设置字体
	m_ctlACDCReport_Cur.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlACDCReport_Cur.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlACDCReport_Cur.PostInitTemplateStyle();

	// 插入列元素

	m_ctlACDCReport_Cur.InsertColumn(0, _T("     ") + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_CURRENT) , EL_CENTER, DSM_BASIC_ACDC_WIDTH_CHAN1);
	m_ctlACDCReport_Cur.InsertColumn(1, _T("     ") + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_SET), EL_CENTER,   DSM_BASIC_ACDC_WIDTH_SET1);
#if SU_FEI
	m_ctlACDCReport_Cur.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_CURRENT), EL_CENTER, DSM_BASIC_ACDC_WIDTH_CHAN2);
	m_ctlACDCReport_Cur.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_SET), EL_CENTER,   DSM_BASIC_ACDC_WIDTH_SET2);
#endif
	// 设置选择焦点样式
	//m_ctlACDCReport.SetSelectionRowHighlightColor(DSM_REPORT_COLOR_BK_LOW);
	//m_ctlACDCReport.SetFocusedSubItemHighlightColor(DSM_REPORT_COLOR_BK_ITEM_HIGH);
	//m_ctlACDCReport.SetSelectionRowLowlightColor(DSM_REPORT_COLOR_BK_LOW);

	m_ctlACDCReport_Cur.SetRowSelectionVisible(FALSE);
	m_ctlACDCReport_Cur.SetRowFocusEnable(TRUE);
	m_ctlACDCReport_Cur.SetSubItemFocusEnable(TRUE);
	m_ctlACDCReport_Cur.SetFocusedColIndex(1);
	//m_ctlACDCReport_Cur.SetFocus();

	m_ctlACDCReport_Cur.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));


	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}

	CString strACDC[8][1] = {
				_T("Ia1"),
				_T("Ib1"),
				_T("Ic1"),
				_T("Ix1"),
				_T("Ia2"),
				_T("Ib2"),
				_T("Ic2"),
				_T("Ix2"),
			};
	for (int i = 0; i < 8; ++i)
	{

		m_ctlACDCReport_Cur.InsertRecordRow(i, new CDsmBasicACDCSetRecord(
			strACDC[i][0],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][0])/*,
			strACDC[i][1],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][1])*/
			));
	}

	m_ctlACDCReport_Cur.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
//	m_ctlACDCReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
	for (int i = 0; i < m_ctlACDCReport_Cur.GetRecordRowCount(); ++i)
	{
		m_ctlACDCReport_Cur.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
	//	m_ctlACDCReport.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
	}
}

void CBasicParamACDCDlg::_createReport()
{
	CRect rcReport(DSM_VOL_REPORT_RECT);
	DSM_ParentToChild(this, rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
	int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

	m_ctlACDCReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_BASIC_PARAM_ACDC_RET);

	// 设置字体
	m_ctlACDCReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlACDCReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_ctlACDCReport.PostInitTemplateStyle();

	// 插入列元素

	m_ctlACDCReport.InsertColumn(0, _T("     ") + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_VOLT) , EL_CENTER, DSM_BASIC_ACDC_WIDTH_CHAN1);
	m_ctlACDCReport.InsertColumn(1, _T("     ") + dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_SET), EL_CENTER,   DSM_BASIC_ACDC_WIDTH_SET1);
#if SU_FEI
	m_ctlACDCReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_CURRENT), EL_CENTER, DSM_BASIC_ACDC_WIDTH_CHAN2);
	m_ctlACDCReport.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_SET), EL_CENTER,   DSM_BASIC_ACDC_WIDTH_SET2);
#endif
	// 设置选择焦点样式
	//m_ctlACDCReport.SetSelectionRowHighlightColor(DSM_REPORT_COLOR_BK_LOW);
	//m_ctlACDCReport.SetFocusedSubItemHighlightColor(DSM_REPORT_COLOR_BK_ITEM_HIGH);
	//m_ctlACDCReport.SetSelectionRowLowlightColor(DSM_REPORT_COLOR_BK_LOW);

	m_ctlACDCReport.SetRowSelectionVisible(FALSE);
	m_ctlACDCReport.SetRowFocusEnable(TRUE);
	m_ctlACDCReport.SetSubItemFocusEnable(TRUE);
	m_ctlACDCReport.SetFocusedColIndex(1);
	m_ctlACDCReport.SetFocus();


	m_ctlACDCReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));


	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
#if SU_FEI
	CString strACDC[8][2] = {
		_T("Ua1"), _T("Ia1"),
		_T("Ub1"), _T("Ib1"),
		_T("Uc1"), _T("Ic1"),
		_T("Ux1"), _T("Ix1"),
		_T("Ua2"), _T("Ia2"),
		_T("Ub2"), _T("Ib2"),
		_T("Uc2"), _T("Ic2"),
		_T("Ux2"), _T("Ix2"),
	};
	for (int i = 0; i < 8; ++i)
	{

		m_ctlACDCReport.InsertRecordRow(i, new CDsmBasicACDCSetRecord(
			strACDC[i][0],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][0]),
			strACDC[i][1],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][1])
			));
	}
#else
	CString strACDC[8][1] = {
				_T("Ua1"),
				_T("Ub1"),
				_T("Uc1"),
				_T("Ux1"),
				_T("Ua2"),
				_T("Ub2"),
				_T("Uc2"),
				_T("Ux2"),
			};
	for (int i = 0; i < 8; ++i)
	{

		m_ctlACDCReport.InsertRecordRow(i, new CDsmBasicACDCSetRecord(
			strACDC[i][0],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][0])/*,
			strACDC[i][1],
			pParamConfigSMVSend->GetCurrentType((LPCTSTR)strACDC[i][1])*/
			));
	}
#endif
	m_ctlACDCReport.GetColumnExAttr(1)->GetEditOptions()->SetAllowEdit(TRUE);
//	m_ctlACDCReport.GetColumnExAttr(3)->GetEditOptions()->SetAllowEdit(TRUE);
	for (int i = 0; i < m_ctlACDCReport.GetRecordRowCount(); ++i)
	{
		m_ctlACDCReport.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
	//	m_ctlACDCReport.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
	}

	m_ctlACDCReport.SetFocusedSelectionRowIndex(0);
}


void CBasicParamACDCDlg::OnACDCItemCurValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);


	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

	if (pItemNotify == NULL)
	{
		return ;
	}
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
	ASSERT(pItemNotify->colIndex >= 1);


	CString strACDC = ((CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex-1))->GetValue();
	CELRecordItemText * pItem = (CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex);
	if (pItem == NULL)
	{
		return;
	}

	CString strCurType = _T("");

	if (pItem->IsChecked())
	{
		strCurType = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC);
		pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_DC);
	}
	else
	{
		strCurType = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC);
		pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_AC);
	}
	pItem->SetValue(strCurType);
	m_ctlACDCReport_Cur.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
	*pResult = TRUE;
}

void CBasicParamACDCDlg::OnACDCItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);


	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;

	if (pItemNotify == NULL)
	{
		return ;
	}
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
	ASSERT(pItemNotify->colIndex >= 1);


	CString strACDC = ((CELRecordItemText*)m_ctlACDCReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex-1))->GetValue();
	CELRecordItemText * pItem = (CELRecordItemText*)m_ctlACDCReport.GetRecordRow(pItemNotify->rowIndex)->GetItem(pItemNotify->colIndex);
	if (pItem == NULL)
	{
		return;
	}

	CString strCurType = _T("");

	if (pItem->IsChecked())
	{
		strCurType = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC);
		pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_DC);
	}
	else
	{
		strCurType = dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC);
		pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_AC);
	}
	pItem->SetValue(strCurType);
	m_ctlACDCReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
	*pResult = TRUE;
}


void CBasicParamACDCDlg::_SavePage()
{
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	if (NULL == pParamConfigSMVSend)
	{
		return ;
	}
	pParamConfigSMVSend->SaveConfig();
}

BOOL CBasicParamACDCDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	KillTimer(1);
	return CBaseDlg::DestroyWindow();
}


BOOL CBasicParamACDCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{

		switch (pMsg->wParam)
		{
		case VK_LEFT:
			{
				//
				//左右移动焦点到交直流设置
				//
#if SU_FEI
				int nFocusColIndex = m_ctlACDCReport.GetFocusedColIndex();
				if (nFocusColIndex == 3)
				{
					m_ctlACDCReport.SetFocusedColIndex(1);
					m_ctlACDCReport.RedrawRowItem(m_ctlACDCReport.GetFocusedRowIndex(), 1);
					m_ctlACDCReport.RedrawRowItem(m_ctlACDCReport.GetFocusedRowIndex(), 3);
				}
#else
			/*	GetDlgItem(IDC_BASIC_PARAM_ACDC_RET_CUR)->PostMessage(WM_KILLFOCUS, 0, 0);
				GetDlgItem(IDC_BASIC_PARAM_ACDC_RET_CUR)->Invalidate(TRUE);
				
				m_ctlACDCReport.SetFocus();
				m_ctlACDCReport.SetRowFocusEnable(TRUE);
				m_ctlACDCReport.SetFocusedSelectionRowIndex(m_ctlACDCReport_Cur.GetFocusedColIndex());
				GetDlgItem(IDC_BASIC_PARAM_ACDC_RET)->Invalidate(TRUE);*/
				
				if (m_ctlACDCReport_Cur.IsWindowEnabled())
				{
					m_ctlACDCReport.EnableWindow(TRUE);

					GetDlgItem(IDC_BASIC_PARAM_ACDC_RET_CUR)->PostMessage(WM_KILLFOCUS, 0, 0);
					m_ctlACDCReport.SetFocus();

					m_ctlACDCReport.SetRowFocusEnable(TRUE);					

					m_ctlACDCReport_Cur.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
						dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

					m_ctlACDCReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
						dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
					
					m_ctlACDCReport.RedrawRowItem(m_ctlACDCReport_Cur.GetFocusedRowIndex(), 1);
					m_ctlACDCReport.SetFocusedSelectionRowIndex(m_ctlACDCReport_Cur.GetFocusedRowIndex());

					m_ctlACDCReport_Cur.SetRowFocusEnable(FALSE);

					m_ctlACDCReport_Cur.EnableWindow(FALSE);
				}		
#endif
				return TRUE;
			}
			break;
		case VK_RIGHT:
			{
				//
				//左右移动焦点到交直流设置
				//
#if SU_FEI
				int nFocusColIndex = m_ctlACDCReport.GetFocusedColIndex();
				if (nFocusColIndex == 1)
				{
					m_ctlACDCReport.SetFocusedColIndex(3);
					m_ctlACDCReport.RedrawRowItem(m_ctlACDCReport.GetFocusedRowIndex(), 1);
					m_ctlACDCReport.RedrawRowItem(m_ctlACDCReport.GetFocusedRowIndex(), 3);
				}
#else
				/*GetDlgItem(IDC_BASIC_PARAM_ACDC_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
				GetDlgItem(IDC_BASIC_PARAM_ACDC_RET)->Invalidate(TRUE);

				m_ctlACDCReport_Cur.SetFocus();
				m_ctlACDCReport_Cur.SetRowFocusEnable(TRUE);
				m_ctlACDCReport_Cur.SetFocusedSelectionRowIndex(m_ctlACDCReport.GetFocusedColIndex());
				GetDlgItem(IDC_BASIC_PARAM_ACDC_RET_CUR)->Invalidate(TRUE);*/

				if (m_ctlACDCReport.IsWindowEnabled())
				{
					m_ctlACDCReport_Cur.EnableWindow(TRUE);
					
					GetDlgItem(IDC_BASIC_PARAM_ACDC_RET)->PostMessage(WM_KILLFOCUS, 0, 0);
					m_ctlACDCReport_Cur.SetFocus();

					m_ctlACDCReport_Cur.SetRowFocusEnable(TRUE);	

					m_ctlACDCReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
						dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL));

					m_ctlACDCReport_Cur.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
						dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));

					m_ctlACDCReport_Cur.RedrawRowItem(m_ctlACDCReport.GetFocusedRowIndex(), 1);
					m_ctlACDCReport_Cur.SetFocusedSelectionRowIndex(m_ctlACDCReport.GetFocusedRowIndex());

					m_ctlACDCReport.SetRowFocusEnable(FALSE);					
					m_ctlACDCReport.EnableWindow(FALSE);

				}
#endif
				return TRUE;
			}
		default:
			break;
		}

	}
	return CBaseDlg::PreTranslateMessage(pMsg);
}

void CBasicParamACDCDlg::OnTimer(UINT_PTR nIDEvent)
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

#if !CHUN_HUA
void CBasicParamACDCDlg::OnNaviMenuParamSmvSend(UINT nID)
{
	CELRecordItemText * pItem =NULL;
	CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
	uint32_t nRowCount = m_ctlACDCReport.GetRecordRowCount();
	CString strACDC;
	switch(nID)
	{
		case ID_PARAM_ACDC_F1: //全直流
			for (int i =0; i< nRowCount; i++)
			{
				pItem = (CELRecordItemText*)m_ctlACDCReport.GetRecordRow(i)->GetItem(1);
				pItem->SetChecked(TRUE);


				strACDC = ((CELRecordItemText*)m_ctlACDCReport.GetRecordRow(i)->GetItem(0))->GetValue();

				pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_DC);

				pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC));
				m_ctlACDCReport.RedrawRowItem(i, 1);

				pItem = (CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(i)->GetItem(1);
				pItem->SetChecked(TRUE);

				strACDC = ((CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(i)->GetItem(0))->GetValue();
				pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_DC);

				pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_DC));
				m_ctlACDCReport_Cur.RedrawRowItem(i, 1);
			}
			
			break;
		case ID_PARAM_ACDC_F2: //全交流
			for (int i =0; i< nRowCount; i++)
			{
				pItem = (CELRecordItemText*)m_ctlACDCReport.GetRecordRow(i)->GetItem(1);
				pItem->SetChecked(FALSE);


				strACDC = ((CELRecordItemText*)m_ctlACDCReport.GetRecordRow(i)->GetItem(0))->GetValue();

				pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_AC);

				pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC));
				m_ctlACDCReport.RedrawRowItem(i, 1);

				pItem = (CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(i)->GetItem(1);
				pItem->SetChecked(FALSE);

				strACDC = ((CELRecordItemText*)m_ctlACDCReport_Cur.GetRecordRow(i)->GetItem(0))->GetValue();
				pParamConfigSMVSend->SetCurrentType((LPCTSTR)strACDC, CURRENT_TYPE_AC);

				pItem->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_ACDC_AC));
				m_ctlACDCReport_Cur.RedrawRowItem(i, 1);
			}
			break;
		default:
			break;
	}
}
#endif
