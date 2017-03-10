/**
 * \file
 *      NaviBarMgr.h
 *
 * \brief
 *      导航栏管理器
 *
 *  \note
 *      业务窗口通过该管理器设置自己的导航栏
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/13
 */

#pragma once

#include "src/utils/navicontrol/navimenubar/navitoolbar.h"
#include "src/main/resource.h"

/**
 *      导航栏管理器
 */
class CNaviBarMgr
{
public:
    /**
     *      获取静态实例对象
     */
    static CNaviBarMgr* GetInstance()
    {
        static CNaviBarMgr m_sInst;

        return &m_sInst;
    }

    /**
     *      绑定导航栏
     *
     *  \param[in]
     *      pNaviBar            待关联的导航栏指针
     *  \note
     *      主窗口初始化时先绑定导航栏，再使用
     */
    void SetNaviBar(CNaviToolBar* pNaviBar);

    /**
     *      获取导航栏
     *
     *  \return
     *      CNaviToolBar*       返回导航栏
     */
    CNaviToolBar* GetNaviBar();

    /**
     *      设置是否高亮某项
     *
     *  \param[in]
     *      nSub                子项序号
     *  \param[in]
     *      nPos                次子项序号
     *  \param[in]
     *      bHighlight          高亮标志
     */
    void HighlightMenuItem(int nSub, int nPos, BOOL bHighlight = TRUE);

    /**
     *      进入inpalceedit编辑菜单
     */
    CWnd* EnterInplaceEdit(CWnd* target, UINT uMenuID = IDR_MENU_INPLACEEDIT);

    /**
     *      离开inplceedit编辑菜单
     */
    void LeaveInplaceEdit(CWnd* target = NULL);

private:
    CNaviBarMgr(void);

    ~CNaviBarMgr(void);

private:
    CNaviToolBar*               m_pNaviBar;             ///< 主窗口的导航栏菜单

    CNaviMenu                   m_menuInplaceEdit;      ///< 编辑菜单
    CNaviMenu*                  m_pMenuLast;            ///< 上一个目标窗口关联的菜单(inplace edit)
    CWnd*                       m_pPreEditWnd;          ///< 编辑前窗口
};
