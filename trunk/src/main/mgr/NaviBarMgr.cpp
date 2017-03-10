/**
 * \file
 *      NaviBarMgr.cpp
 *
 * \brief
 *      导航栏管理器
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/13
 */

#include "StdAfx.h"
#include "NaviBarMgr.h"

#include "src/main/dsmdlg.h"
#include "src/main/mgr/statusmgr.h"

CNaviBarMgr::CNaviBarMgr(void)
    : m_pNaviBar(NULL)
    , m_pMenuLast(NULL)
    , m_pPreEditWnd(NULL)
{

}

CNaviBarMgr::~CNaviBarMgr(void)
{

}

void CNaviBarMgr::SetNaviBar( CNaviToolBar* pNaviBar )
{
    ASSERT(pNaviBar);

    m_pNaviBar = pNaviBar;
}

CNaviToolBar* CNaviBarMgr::GetNaviBar()
{
    return m_pNaviBar;
}

void CNaviBarMgr::HighlightMenuItem( int nSub, int nPos, BOOL bHighlight /*= TRUE*/ )
{
    CNaviMenu* pMenu = m_pNaviBar->GetNaviMenu();

    if(pMenu)
    {
        CNaviMenu* pSubMenu = pMenu->GetSubMenu(nSub);

        if(pSubMenu)
        {
            if(pSubMenu->GetMenuItemCount() >= nPos)
            {
                return pSubMenu->HighlightMenuItem(nPos, bHighlight);
            }
        }
    }

    ASSERT(FALSE);
}

CWnd* CNaviBarMgr::EnterInplaceEdit( CWnd* target, UINT uMenuID/* = IDR_MENU_INPLACEEDIT*/)
{
    CStatusMgr::SetShowInputMode(true);

    CMenu mm;
    mm.LoadMenu(uMenuID);

    m_menuInplaceEdit.LoadMenu(&mm);
    mm.DestroyMenu();

    m_pPreEditWnd = m_pNaviBar->GetCommandTarget();

    m_pMenuLast = m_pNaviBar->GetNaviMenu();
    m_pNaviBar->SetCommandTarget(target);
    m_pNaviBar->SetNaviMenu(&m_menuInplaceEdit);

    return m_pPreEditWnd;
}

void CNaviBarMgr::LeaveInplaceEdit(CWnd* target)
{
    CStatusMgr::SetShowInputMode(false);

    if (m_pMenuLast)
    {
        // 先set target，再set menu，set menu后会调用updatecmdui

        if(target)
        {
            m_pNaviBar->SetCommandTarget(target);
        }
        else
        {
            ASSERT(m_pPreEditWnd);
            m_pNaviBar->SetCommandTarget(m_pPreEditWnd);
        }

        m_pNaviBar->SetNaviMenu(m_pMenuLast);
        m_pMenuLast = NULL;
    }
}


