// BasicAboutDlg.cpp : implementation file
//

#include "stdafx.h"
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
#include "BasicAboutDlg.h"

#if !SHENG_ZE
#include "src/main/utils/rnavimenu.h"
#include "src/main/utils/utility.h"
#include "src/utils/SliderBoxControl/SliderBox.h"
#include "src/utils/NaviControl/NaviMenuBar/NaviToolBar.h"
#endif

// CBasicAboutDlg dialog

IMPLEMENT_DYNAMIC(CBasicAboutDlg, CBaseDlg)

CBasicAboutDlg::CBasicAboutDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicAboutDlg::IDD, DSM_STR_BASIC_ABOUTCAPTION, pParent)
{
	m_nMenuID = IDR_MENU_ABOUT;
} 

CBasicAboutDlg::~CBasicAboutDlg()
{
}

void CBasicAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicAboutDlg, CBaseDlg)
END_MESSAGE_MAP()


// CBasicAboutDlg message handlers
BOOL CBasicAboutDlg::OnInitDialog()
{
	CBaseDlg::OnInitDialog();
    _createAboutReport();
    _showAboutReport();
	return FALSE;
}

/*
 *  创建关于表格
 */
void CBasicAboutDlg::_createAboutReport()
{
	CRect rcReport(DSM_BASICCFG_RCRIGHT);
	DSM_ParentToChild(this, rcReport);

	DWORD dwStyle = (DSM_WINSTYLE_REPORT_DEFAULT) & ~WS_TABSTOP;
	m_ctlAboutReport.Create(dwStyle, rcReport, this, IDC_BASICCFG_RET_ABOUT);

	// 设置字体
	m_ctlAboutReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	// 设置报表样式
	m_ctlAboutReport.PostInitTemplateStyle();
	m_ctlAboutReport.SetRowFocusEnable(FALSE);

	CString csSoftVer;
	csSoftVer.Format(L"%s.%s", dsm_config::GetSoftwareVersion(), dsm_config::GetSoftwareBuild());

	// 从配置加载信息
	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_PRODUCT_NAME, CReportAbout::COL_DESC, dsm_config::GetProductName());

	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_HW_VER, CReportAbout::COL_DESC, dsm_config::GetHardwareVersion());
	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_SYS_VER, CReportAbout::COL_DESC, dsm_config::GetSystemVersion());

	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_SW_VER, CReportAbout::COL_DESC, csSoftVer);

	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_COMPANY_NAME, CReportAbout::COL_DESC, dsm_config::GetCompanyName());
	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_COMPANY_ADDR, CReportAbout::COL_DESC, dsm_config::GetCompanyAddress());
	m_ctlAboutReport.SetAboutItemText(CReportAbout::ROW_SUPPORT, CReportAbout::COL_DESC, dsm_config::GetTelephoneSupport());
}
/*
 *  显示关于表格
 */
void CBasicAboutDlg::_showAboutReport()
{
    m_ctlAboutReport.ShowWindow(SW_SHOW);
}