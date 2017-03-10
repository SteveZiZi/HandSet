/**
 * \file
 *      MainDlg.cpp
 *
 * \brief
 *      主页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */

// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MainDlg.h"

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



// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CBaseDlg)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CMainDlg::IDD, DSM_STR_MAIN_CAPTION, pParent)
    , m_pPageDataExtra(NULL)
    , m_csSelectPcapFile(L"")
{
    m_nMenuID = IDR_MENU_MAIN;
	
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CBaseDlg)
    ON_WM_PAINT()
    ON_COMMAND(ID_MENU_MAIN_F1, &CMainDlg::OnNaviMenuF1)
	ON_COMMAND(ID_MENU_MAIN_F2, &CMainDlg::OnNaviMenuF2)
	ON_COMMAND(ID_MENU_MAIN_F3, &CMainDlg::OnNaviMenuF3)
	ON_COMMAND(ID_MENU_MAIN_F4, &CMainDlg::OnNaviMenuF4)
	ON_COMMAND(ID_MENU_MAIN_F5, &CMainDlg::OnNaviMenuF5)
    ON_COMMAND(ID_MENU_MAIN_F6, &CMainDlg::OnNaviMenuF6)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    bool bInited = false;
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    // 新进页面，创建页面数据
    if(pData->m_eAction == PageEnter)
    {
        if(!pData->m_pPageDataExtra)
        {
            m_pPageDataExtra = new CMainPageDataExtra(pData);
            pData->m_pPageDataExtra = m_pPageDataExtra;
        }
        else
        {
            bInited = true;
        }
    }

    // 返回页面，取出旧的页面数据
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtra = dynamic_cast<CMainPageDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtra);
    }

    CDsmDataLayer::getInstance()->SetCurrentMode(DSM_MODE_MAIN_PAGE);

    _restorePage();


    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CMainDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CBaseDlg::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);
    //dc.FillSolidRect(rcClient, DSM_REPORT_COLOR_BK);

    CDC dcMemBmp;
    dcMemBmp.CreateCompatibleDC(&dc);
    HGDIOBJ hOriObj  = dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_BKGROUND));
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMemBmp, 0, 0, SRCCOPY);
    dcMemBmp.SelectObject(hOriObj);
    dcMemBmp.DeleteDC();

}

void CMainDlg::OnNaviMenuF1()
{
	CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
}
//报文发送
void CMainDlg::OnNaviMenuF2()
{
	CEventProc::GetInstance()->ShowPage(IDD_MAIN_SEND_DLG);
}
//辅助测试
void CMainDlg::OnNaviMenuF3()
{
	CEventProc::GetInstance()->ShowPage(IDD_LP_LIGHT_POWER_DLG);
}
//IEC设置
void CMainDlg::OnNaviMenuF4()
{
	CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DLG);
}
//基本设置
void CMainDlg::OnNaviMenuF5()
{
	CEventProc::GetInstance()->ShowPage(IDD_BASIC_SETTING_DLG);
}

void CMainDlg::OnNaviMenuF6()
{
    CEventProc::GetInstance()->ShowPage(IDD_BASIC_ABOUT_DLG);
}

BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

  //  UNREFERENCED_PARAMETER(pDC);

	return TRUE;

}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    return CBaseDlg::PreTranslateMessage(pMsg);
}


void CMainDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CMainDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    // 恢复之前的焦点按钮
    if(pData->m_eAction == PageBack)
    {

        CWnd* pWnd          = GetDlgItem(pData->m_nCtrlID); 
        m_csSelectPcapFile  = m_pPageDataExtra->GetPcapFile();

        this->SetFocus();
    }
    else
    {
       this->SetFocus();
    }
}

void CMainDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CMainDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    // 保存当前焦点按钮的ID
    pData->m_nCtrlID    = GetFocus()->GetDlgCtrlID();
    m_pPageDataExtra->SetPcapFile(m_csSelectPcapFile);

}

HBRUSH CMainDlg::OnCtlColor( CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */ )
{
    return (HBRUSH)::GetStockObject(NULL_BRUSH);
}
