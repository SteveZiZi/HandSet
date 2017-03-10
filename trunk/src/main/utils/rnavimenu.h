/**
 * \file
 *      rnavimenu.h
 *
 * \brief
 *      �����˵���Դ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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
    HWND                    m_hPreTarget;           ///< ��һ���˵����������
    CNaviMenu*              m_pPreMenu;             ///< ��һ���˵�����ָ��
    CNaviMenu               m_menuCurrent;          ///< ��ǰ�˵�����
};

