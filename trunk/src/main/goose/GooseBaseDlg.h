/**
 * \file
 *      GooseBaseDlg.h
 *
 * \brief
 *      Goose页面基类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/8
 */

#pragma once

#include "src/main/dlg/basedlg.h"

#include "src/service/datalayer/dsmdatalayer.h"


#define DSM_GOOSEASEDLG      CGooseBaseDlg

#define DSM_GOOSE_TIMER_ID              0x82        ///< 定时器ID


class CEventProc;
class CPageDataExtra;


// CGooseBaseDlg 对话框

class CGooseBaseDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CGooseBaseDlg)

public:
	CGooseBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuF1(UINT nID);
    afx_msg void OnNaviMenuF1T6(UINT nID);
    afx_msg void OnDestroy();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


protected:
    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

    /**
     *      启动默认定时器
     */
    virtual void _beginTimer(UINT nTimeout = DSM_GOOSE_TIMER_INTERVAL);


    /**
     *      停止默认定时器
     */
    virtual void _stopTimer();

    /**
     *      更新标题
     */
    virtual void _updateCaption();

    /**
     *      加载控制块到F1菜单中
     */
    virtual void _loadCtrlBlk();

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \note
     *      子类选择控制块需要重载该方法做一些补充操作
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      高亮菜单
     */
    void _highlightMenu();

    /**
     *      关联菜单ID与控制块（冲突覆盖）
     */
    void _setMapping(UINT nCmdID, CBaseDetectItem* pCtrlBlk);

    /**
     *      获取指定控制的描述信息
     */
    CString _getDesc(CNetDetectItem* pCtrlBlk);

protected:

    typedef std::map<UINT, CBaseDetectItem*>        MapCtrlBlk;

    UINT                    m_nCaptionID;           ///< 标题
    UINT                    m_nTemplateID;          ///< GOOSE页面模板ID
    UINT_PTR                m_nTimerID;             ///< 刷新数据定时器ID

    CEventProc*             m_pEventProc;           ///< 页面管理器指针

    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针
    CBaseDetectItem*        m_pCurrentCtrlBlk;      ///< 当前工作控制块

    UINT                    m_nHighLightCtrlBlk;    ///< 当前高亮控制块菜单ID
    UINT                    m_nCtrlBlkLastMenuID;   ///< 最后一个控制块菜单ID
    MapCtrlBlk              m_mapCtrlBlk;           ///< 菜单命令ID映射控制块表
    CPageDataExtra*         m_pPageDataExtra;       ///< 页面状态数据
};
