/**
 * \file
 *      BaseDlg.cpp
 *
 * \brief
 *      dsm业务页面的基类实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */


// BaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BaseDlg.h"

#include "src/main/common.h"
#include "src/main/mgr/statusmgr.h"



// CBaseDlg 对话框

IMPLEMENT_DYNAMIC(CBaseDlg, CDialog)

CBaseDlg::CBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
    , m_nIDCaption(nIDCaption)
    , m_pNaviBar(NULL)
    , m_nMenuID(UINT_MAX)
{
}

CBaseDlg::~CBaseDlg()
{
    m_brBackground.DeleteObject();
}

void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDlg, CDialog)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CBaseDlg 消息处理程序

BOOL CBaseDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    // 传给导航菜单处理消息
    if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if(AfxGetMainWnd()->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}


BOOL CBaseDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_brBackground.CreateSolidBrush(DSM_COLOR_BK);

    CStatusMgr::SetTitle(dsm_stringMgr::Get(m_nIDCaption));

    _loadNaviMenu();

    m_pNaviBar->UpdateNaviMenu();

    MoveWindow(DSM_RCBODY);
    ShowWindow(SW_SHOW);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CBaseDlg::OnCtlColor(CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */)
{
    return m_brBackground;
}


BOOL CBaseDlg::_AllowExit()
{
    return TRUE;
}


void CBaseDlg::_loadNaviMenu()
{
    ASSERT(-1 != m_nMenuID);

    // 添加底部菜单
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);
    m_menu.DestroyMenu();

    // 调用虚函数修改菜单
    _modifyMenu();

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();

    if(m_pNaviBar)
    {
        // 设置菜单目标对象和数据对象
        m_pNaviBar->SetCommandTarget(this);
        m_pNaviBar->SetNaviMenu(&m_menuMain);
    }
}

/**
 *      重新加载菜单HMENU
 */
void CBaseDlg::_reloadNaviMenu(UINT nMenuID)
{
    if(!m_pNaviBar)
    {
        return;
    }

    ASSERT(-1 != nMenuID);

    m_nMenuID = nMenuID;

    if(m_menu.GetSafeHmenu())
    {
        m_menu.DestroyMenu();
    }

    // 添加底部菜单
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);

    // 设置菜单目标对象和数据对象
    m_pNaviBar->SetCommandTarget(this);
    m_pNaviBar->SetNaviMenu(&m_menuMain);
}

