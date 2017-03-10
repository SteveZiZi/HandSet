// BasicSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BasicSettingDlg.h"

#include <algorithm>

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/main/dlg/pcapbrowserdlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"

#include "src/utils/commoncontrol/timeeditctrl/timeeditctrl.h"
#include "src/utils/commoncontrol/progressslider/progressslider.h"


#define	IDC_BASICCFG_SETTING_BASE		999		///<定义了基本设置的控件ID


#define DSM_BASICCFG_RPT_CAPTION                0
#define DSM_BASICCFG_RPT_VALUE                  1

#define DSM_BASICCFG_RPT_BRIGHTNESS             0       ///< 背光亮度
#define DSM_BASICCFG_RPT_BRIGHTTIME             1       ///< 背光时间
#define DSM_BASICCFG_RPT_TIMEDATE               2       ///< 日期时间
#define DSM_BASICCFG_RPT_TIMEZONE               3       ///< 时区设置
#define DSM_BASICCFG_RPT_TIMESYNC               4       ///< 时间同步

#define DSM_BASICFG_STR_TIME_FMT    L"%04d-%02d-%02d %02d:%02d:%02d"    ///< 时间格式化字符串

// CBasicSettingDlg dialog

IMPLEMENT_DYNAMIC(CBasicSettingDlg, CBaseDlg)

CBasicSettingDlg::CBasicSettingDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicSettingDlg::IDD, DSM_STR_BASIC_BASECAPTION, pParent)
	, m_csDefaultTZS(L"China Standard Time")
{
	m_nMenuID = IDR_MENU_NULL;
}

CBasicSettingDlg::~CBasicSettingDlg()
{
	std::vector<stTimeInfo*>::iterator it = m_vecTimezone.begin();
	std::vector<stTimeInfo*>::iterator itEnd = m_vecTimezone.end();

	for(; it != itEnd; it++)
	{
		delete *it;
	}

	m_vecTimezone.clear();
}

void CBasicSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicSettingDlg, CBaseDlg)
	ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASICCFG_SETTING_BASE, &CBasicSettingDlg::OnItemValueChangedBasic)
	ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASICCFG_SETTING_BASE, &CBasicSettingDlg::OnItemValueChangedBasic)
	ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_BASICCFG_SETTING_BASE, &CBasicSettingDlg::OnItemButtonClickBasic)
	ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASICCFG_SETTING_BASE, &CBasicSettingDlg::OnItemSelectChangeBasic)
	ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASICCFG_SETTING_BASE,  &CBasicSettingDlg::OnInplaceEditAction)
	ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASICCFG_SETTING_BASE,  &CBasicSettingDlg::OnInplaceEditAction)
#if !SHENG_ZE
	ON_MESSAGE(WM_COMCTRL_PROGRESSSLIDER_CHANGE, &CBasicSettingDlg::OnBrightnessChange)
#endif
END_MESSAGE_MAP()


// CBasicSettingDlg message handlers

BOOL CBasicSettingDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();

	// 加载时区信息列表
	_loadTimezone();

	_showBasicSettingReport();

	return FALSE;
}


/***
**	创建基本设置表格
**
**	#define  IDC_BASICCFG_SETTING_BASE	999
**
**/
void CBasicSettingDlg::_createBasicSettingReport()
{
	CRect rcReport(DSM_BASICCFG_RCRIGHT);
	DSM_ParentToChild(this, rcReport);

	m_ctlBasicSettingReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_BASICCFG_SETTING_BASE);

	// 设置字体
	m_ctlBasicSettingReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	// 设置横向报表样式
	m_ctlBasicSettingReport.PostInitTemplateStyle();

	// 插入列元素
	m_ctlBasicSettingReport.InsertColumn(0, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION - 70);   // 标题
	m_ctlBasicSettingReport.InsertColumn(1, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT + 70);   // 内容

	// 设置选择焦点样式
	m_ctlBasicSettingReport.SetRowSelectionVisible(FALSE);
	m_ctlBasicSettingReport.SetRowFocusEnable(TRUE);
	m_ctlBasicSettingReport.SetSubItemFocusEnable(TRUE);
	m_ctlBasicSettingReport.SetFreezeColumnsCount(1);
	m_ctlBasicSettingReport.SetFreezeSelectionVisible(TRUE);
	m_ctlBasicSettingReport.SetFocusedColIndex(1);
#if !SU_FEI
	m_ctlBasicSettingReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
	CELRecordItemEditOptions* pOptions      = NULL;
	CELRecordItemConstraint* pCon           = NULL;

	m_ctlBasicSettingReport.GetColumnExAttr(DSM_BASICCFG_RPT_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);

	// 背光亮度
	m_ctlBasicSettingReport.InsertRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS, 
		new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_BRIGHTNESS), 
		dsm_config::GetBrightness()));

	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();

	CELRecordItemText* pItemBrightness = dynamic_cast<CELRecordItemText*>(m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE));
	pItemBrightness->SetFormatString(L"%s%%");

	// 背光时间
	m_ctlBasicSettingReport.InsertRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_BRIGHTIME), dsm_config::GetBrightTime()));

	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
	pOptions = m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL);
	pOptions->AddComboButton();
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_1MIN), 60);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_2MIN), 120);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_5MIN), 300);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_10MIN), 600);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_30MIN), 1800);
	pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_NEVER), 864000);

	pCon = pOptions->FindConstraint(dsm_config::GetBrightTime());
	ASSERT(pCon);
	CELRecordItemText* pItemBrighttime = dynamic_cast<CELRecordItemText*>(m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE));

	pItemBrighttime->SetValue(pCon->GetConstraintCaption());

	SYSTEMTIME st;
	GetLocalTime(&st);
	CString csTime;
	csTime.Format(DSM_BASICFG_STR_TIME_FMT, 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	// 日期时间
	m_ctlBasicSettingReport.InsertRecordRow(DSM_BASICCFG_RPT_TIMEDATE, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIME), csTime));
	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();


	// 时区
	m_ctlBasicSettingReport.InsertRecordRow(DSM_BASICCFG_RPT_TIMEZONE, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIMEZONE), L""));

	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
	pOptions = m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL);
	pOptions->AddComboButton();

	TIME_ZONE_INFORMATION tzi;
	::ZeroMemory(&tzi, sizeof(tzi));
	GetTimeZoneInformation(&tzi);

	stTimeInfo* pTIInfo = NULL;
	std::vector<stTimeInfo*>::const_iterator it = m_vecTimezone.begin();
	std::vector<stTimeInfo*>::const_iterator itEnd = m_vecTimezone.end();

	bool bInited = false;
	CString csTimezoneDisp;

	// 生成时区列表
	for(; it != itEnd; it++)
	{
		pTIInfo = *it;
		pOptions->AddConstraint(pTIInfo->csDisp, (DWORD_PTR)pTIInfo);

		CString cs(pTIInfo->csDisp);
		CString cs1(pTIInfo->tzinfo.StandardName);
		CString cs2(tzi.StandardName);

		if(!bInited &&
			0 == _tcscmp(pTIInfo->csStd, tzi.StandardName) && 
			pTIInfo->tzinfo.Bias == tzi.Bias && 
			pTIInfo->tzinfo.StandardBias == tzi.StandardBias)

		{
			bInited = true;
			csTimezoneDisp = pTIInfo->csDisp;
		}
	}

	CELRecordItemText* pItemTimezone = dynamic_cast<CELRecordItemText*>(m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE));
	if(pItemTimezone)
	{
		pItemTimezone->SetValue(csTimezoneDisp);
	}

	bool bTimeSync = dsm_config::GetTimeAutoSync();

	// 同步方式
	CString csTimeSync = bTimeSync ? 
		dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO) :
	dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL);

	m_ctlBasicSettingReport.InsertRecordRow(DSM_BASICCFG_RPT_TIMESYNC, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC), csTimeSync));

	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->HasCheckbox(TRUE);
	m_ctlBasicSettingReport.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetChecked(bTimeSync ? TRUE : FALSE);


	m_ctlBasicSettingReport.SetFocusedSelectionRowIndex(0);


	///< 设置标题

	CString	csCaption;
#if SHENG_ZE
	csCaption.Format(L"%s - %s",
		dsm_stringMgr::Get(DSM_STR_BASIC_SYSTEMCFG),
		dsm_stringMgr::Get(DSM_STR_BASIC_BASECAPTION));
#else
	csCaption.Format(L"%s",
		dsm_stringMgr::Get(DSM_STR_BASIC_BASECAPTION));
#endif
	CStatusMgr::SetTitle(csCaption);
}

void CBasicSettingDlg::_loadTimezone()
{
	HKEY hKey;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Time Zones", 0, 0, &hKey))
	{
		ASSERT(FALSE);
		return ;
	}

	DWORD dwDefaultType             = REG_NONE;
	DWORD dwDefaultLen              = MAX_PATH;
	wchar_t szDefaultTZS[MAX_PATH]  = {0};

	// 获取默认时区项名
	if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Default", NULL, &dwDefaultType, (LPBYTE)szDefaultTZS, &dwDefaultLen) && REG_SZ == dwDefaultType)
	{
		m_csDefaultTZS = szDefaultTZS;
	}

	DWORD dwIndex               = 0;
	DWORD dwLen                 = MAX_PATH;
	wchar_t szKeyName[MAX_PATH] = {0};

	HKEY hSubKey;
	DWORD dwSubLen;
	DWORD dwSubType;
	wchar_t szDlt[MAX_PATH] = {0};
	wchar_t szStd[MAX_PATH] = {0};
	wchar_t szDisplayValue[MAX_PATH] = {0};
	TIME_ZONE_INFORMATION tziSub;

	stTimeInfo* pInfo = NULL;

	// 遍历所有有效时区
	while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex, szKeyName, &dwLen, NULL, NULL, NULL, NULL))
	{
		dwIndex++;
		dwLen = MAX_PATH;

		// 打开指定时区
		if(ERROR_SUCCESS != RegOpenKeyEx(hKey, szKeyName, 0, 0, &hSubKey))
		{
			continue;
		}

		dwSubLen = sizeof(tziSub);
		ZeroMemory(&tziSub, dwSubLen);

		// 获取TIME_ZONE_INFORMATION二进制值
		if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"TZI", NULL, &dwSubType, (LPBYTE)&tziSub, &dwSubLen) || 
			REG_BINARY != dwSubType)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		dwSubLen = MAX_PATH;

		// 获取Display值，界面显示值
		if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Display", NULL, &dwSubType, (LPBYTE)szDisplayValue, &dwSubLen) || 
			REG_SZ != dwSubType)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		dwSubLen = MAX_PATH;

		// 获取Dlt值
		if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Dlt", NULL, &dwSubType, (LPBYTE)szDlt, &dwSubLen) || 
			REG_SZ != dwSubType)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		dwSubLen = MAX_PATH;

		// 获取Std值
		if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Std", NULL, &dwSubType, (LPBYTE)szStd, &dwSubLen) || 
			REG_SZ != dwSubType)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		RegCloseKey(hSubKey);

		pInfo               = new stTimeInfo;
		pInfo->csDisp       = szDisplayValue;
		pInfo->csDlt        = szDlt;
		pInfo->csStd        = szStd;
		pInfo->tzinfo       = tziSub;
		pInfo->csKeyName    = szKeyName;

		m_vecTimezone.push_back(pInfo);
	}

	RegCloseKey(hKey);

	_adjustTimezone();
}

void CBasicSettingDlg::_adjustTimezone()
{
	_qsort(0, m_vecTimezone.size() - 1);
}

void CBasicSettingDlg::_qsort( int nleft, int nRight )
{
	if(nleft >= nRight)
	{
		return;
	}

	int nIndex = _partition(nleft, nRight);

	_qsort(nleft, nIndex -1);
	_qsort(nIndex + 1, nRight);
}

int CBasicSettingDlg::_partition( int nLeft, int nRight )
{
	int nIndex  = nLeft;

	stTimeInfo *pPivot = m_vecTimezone[nIndex];

	std::swap(m_vecTimezone[nIndex], m_vecTimezone[nRight]);

	for(int i = nLeft; i < nRight; i++)
	{
		if(m_vecTimezone[i]->tzinfo.Bias < pPivot->tzinfo.Bias)
		{
			std::swap(m_vecTimezone[nIndex++], m_vecTimezone[i]);
		}
	}

	std::swap(m_vecTimezone[nRight], m_vecTimezone[nIndex]);

	return nIndex;
}
/***
**	创建基本设置表格
**
**
**
**/
void CBasicSettingDlg::_showBasicSettingReport()
{
	if (m_ctlBasicSettingReport.GetSafeHwnd())
	{
		m_ctlBasicSettingReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createBasicSettingReport();
	}

	m_ctlBasicSettingReport.SetFocus();
}

BOOL CBasicSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	return CBaseDlg::PreTranslateMessage(pMsg);
}


void CBasicSettingDlg::OnItemButtonClickBasic(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

	if(pItemNotify)
	{
		switch(pItemNotify->rowIndex)
		{
			// 设置背光亮度
		case DSM_BASICCFG_RPT_BRIGHTNESS:
			_modifyBrightness(pItemNotify, pResult);
			break;

			// 设置时间
		case DSM_BASICCFG_RPT_TIMEDATE:
			_modifyTime(pItemNotify, pResult);
			break;

		default:
			return;
		}
	}
}

void CBasicSettingDlg::_modifyTime( ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult )
{
	*pResult = TRUE;

	CTimeEditCtrl ctrlTime;
	CRect rcItem = m_ctlBasicSettingReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

	ctrlTime.SetBkColor(DSM_REPORT_COLOR_BK);
	ctrlTime.SetTextColor(DSM_REPORT_COLOR_TEXT);
	ctrlTime.SetBorderColor(LBT_FB_BORDERCOLOR);

	SYSTEMTIME st;
	GetLocalTime(&st);

	CWnd* pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&ctrlTime);

	// 弹出时间设置控件
	if(IDOK == ctrlTime.GoModal(&m_ctlBasicSettingReport, rcItem, st))
	{
		CString csTime;
		csTime.Format(DSM_BASICFG_STR_TIME_FMT, 
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
		pItem->SetValue(csTime);

		SetLocalTime(&st);

		// 立刻同步FPGA时间
		::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_LOCAL_TO_FPGA, 0);

		m_ctlBasicSettingReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
	}

	CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
}
void CBasicSettingDlg::_modifyBrightness( ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult )
{
	*pResult = TRUE;

	CProgressSlider ctrlSlider;
	CRect rcItem = m_ctlBasicSettingReport.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

	ctrlSlider._SetVal(dsm_config::GetBrightness());
	ctrlSlider._SetRange(0, 100);
	ctrlSlider._SetRangeOp(10, 100);
	ctrlSlider._SetDisplayFmt(L"%d%%");
	ctrlSlider._SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	ctrlSlider._SetProgressColor(LBT_FB2_BKCOLOR, LBT_FB_HLCOLOR, LBT_FB_TEXTCOLOR);

	// 弹出亮度设置进度条
	if (IDOK == ctrlSlider._GoModal(rcItem, &m_ctlBasicSettingReport))
	{
		int nVal = ctrlSlider._GetVal();

		CStatusMgr::SetBrightness((BYTE)nVal);

		CString csBrightness;
		csBrightness.Format(L"%d", nVal);

		CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
		pItem->SetValue(csBrightness);
		m_ctlBasicSettingReport.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
	}
}


void CBasicSettingDlg::OnItemValueChangedBasic(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	*pResult = TRUE;
	ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
	ASSERT(pItem);

	switch(pItemNotify->rowIndex)
	{
		// 设置时间同步方式
	case DSM_BASICCFG_RPT_TIMESYNC:
		if(pItem->GetHasCheckbox())
		{
			bool bAuto = pItem->IsChecked() ? true : false;

			dsm_config::SetTimeSync(bAuto);
			pItem->SetValue(bAuto ? dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO): dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL));

			if(bAuto)
			{
				// 立即同步本地时间
				::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_FPGA_TO_LOCAL, 0);
			}
		}

		break;
	default:
		break;
	}
}

void CBasicSettingDlg::OnItemSelectChangeBasic(NMHDR* pNotifyStruct, LRESULT *pResult)
{
	ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

	if(!pItemNotify || ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
	{
		return;
	}

	*pResult = TRUE;

	switch(pItemNotify->rowIndex)
	{
		// 设置背光时间
	case DSM_BASICCFG_RPT_BRIGHTTIME:
		CStatusMgr::SetBrightTime(pItemNotify->pConstraint->GetConstraintData());
		break;

		// 设置时区
	case DSM_BASICCFG_RPT_TIMEZONE:
		{
			TIME_ZONE_INFORMATION tzi;
			::ZeroMemory(&tzi, sizeof(tzi));
			GetTimeZoneInformation(&tzi);

			stTimeInfo* pTI = (stTimeInfo*)pItemNotify->pConstraint->GetConstraintData();

			TIME_ZONE_INFORMATION tziNew = pTI->tzinfo;

			_tcsncpy(tziNew.DaylightName, pTI->csDlt, 32);
			_tcsncpy(tziNew.StandardName, pTI->csStd, 32);

			SYSTEMTIME st1;
			GetSystemTime(&st1);

			if(SetTimeZoneInformation(&tziNew))
			{
				SetSystemTime(&st1);
			}
			else
			{
				SetTimeZoneInformation(&tzi);
			}

			RegFlushKey(HKEY_LOCAL_MACHINE);
		}
		break;

	default:
		break;
	}
}

void CBasicSettingDlg::OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	*pResult = FALSE;
	static CWnd* pSrc = NULL;

	ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

	// inplaceEdit弹出时, 切换菜单
	if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
	{
		pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_ctlBasicSettingReport.GetInplaceEdit());
		*pResult = TRUE;
	}
	// inplace退出时，切换菜单
	else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
	{
		CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
		*pResult = TRUE;
	}
}


void CBasicSettingDlg::_modifyMenu()
{
#if SHENG_ZE
	m_menuMain.SetMenuItemCaption(0, L"基本设置");
#else
	m_menuMain.SetMenuItemBitmaps(0,dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_BASICSETTING_SEL), NULL, NULL);
#endif
}

#if !SHENG_ZE
LRESULT CBasicSettingDlg::OnBrightnessChange( WPARAM wParam, LPARAM /* lParam */ )
{
	CStatusMgr::SetBrightness((BYTE)wParam);

	CString csBrightness;
	csBrightness.Format(L"%d", wParam);

	CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE));
	pItem->SetValue(csBrightness);

	return 1;
}
#endif