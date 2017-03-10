/**
 * \file
 *      BaseDlg.h
 *
 * \brief
 *      dsm业务页面的基类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */


#pragma once

#include "src/main/resource.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/navicontrol/navimenubar/navitoolbar.h"

#define PARAM_CFG_SAVE_TIMER        4800         ///< 定时判断是否保存参数配置文件

#define DSM_BASEDLG                 CBaseDlg

// CBaseDlg 对话框

class CBaseDlg : public CDialog
{
	DECLARE_DYNAMIC(CBaseDlg)

public:
	CBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:

    /**
     *      退出当前窗口
     */
    virtual BOOL _AllowExit();

    /**
     *      是否允许被模态打断
     *
     *  \note
     *      外部窗口若想弹出模态时，
     *      需要询问当前的页面是否允许其弹出
     */
    virtual BOOL _AllowInterrup()
    {
        return TRUE;
    }

    /**
     *      保存页面
     */
    virtual void _SavePage() {}


protected:

    /**
     *      加载底部导航栏菜单
     */
    virtual void _loadNaviMenu();

    /**
     *      虚函数修改菜单HMENU，子类
     */
    virtual void _modifyMenu(){}

    /**
     *      重新加载菜单HMENU
     */
    virtual void _reloadNaviMenu(UINT nMenuID);

protected:
    UINT                    m_nIDCaption;           ///< 标题字符串

    CBrush                  m_brBackground;         ///< 背景画刷

    UINT                    m_nMenuID;              ///< 菜单ID
    CMenu                   m_menu;                 ///< （伪）菜单数据对象
    CNaviMenu               m_menuMain;             ///< （真）菜单数据对象
    CNaviToolBar*           m_pNaviBar;             ///< 菜单对象
};
