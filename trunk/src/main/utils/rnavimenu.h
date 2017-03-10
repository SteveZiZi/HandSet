/**
 * \file
 *      rnavimenu.h
 *
 * \brief
 *      导航菜单资源类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/11/25
 */

#pragma once

#include "src/main/mgr/navibarmgr.h"
#include "src/utils/navicontrol/navimenubar/navitoolbar.h"

class CRNaviMenu
{
public:
    CRNaviMenu(CWnd* pNewTarget, UINT menuID)
        : m_hPreTarget(NULL)
        , m_pPreMenu(NULL)
    {
        CNaviToolBar* pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
        ASSERT(pNaviBar);

        m_hPreTarget = pNaviBar->GetCommandTarget()->GetSafeHwnd();
        m_pPreMenu   = pNaviBar->GetNaviMenu();

        CMenu menu;
        menu.LoadMenu(menuID);

        m_menuCurrent.LoadMenu(&menu);
        menu.DestroyMenu();

        pNaviBar->SetCommandTarget(pNewTarget);
        pNaviBar->SetNaviMenu(&m_menuCurrent);
    }

    ~CRNaviMenu()
    {
        CNaviToolBar* pBar = CNaviBarMgr::GetInstance()->GetNaviBar();

        if(IsWindow(m_hPreTarget))
        {
            pBar->SetCommandTarget(CWnd::FromHandle(m_hPreTarget));
            pBar->SetNaviMenu(m_pPreMenu);
        }
    }

private:
    HWND                    m_hPreTarget;           ///< 上一个菜单命令对象句柄
    CNaviMenu*              m_pPreMenu;             ///< 上一个菜单对象指针
    CNaviMenu               m_menuCurrent;          ///< 当前菜单对象
};

