/**
 * \file
 *      SMVBaseDlg.h
 *
 * \brief
 *      smv页面基类头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */

#pragma once

#include "src/main/dlg/basedlg.h"

#include "src/service/datalayer/dsmdatalayer.h"

#define DSM_SMVBASEDLG      CSMVBaseDlg

#define DSM_SMV_TIMER_ID                0x81        ///< 定时器ID
#define DSM_SMV_TIMER_INTERVAL          400         ///< SMV业务定时器间隔

class CEventProc;
class CPageDataExtra;
class CDsmDataLayer;
class CSmvRecvConfig;
class CBaseDetectItem;


// CSMVBaseDlg 对话框

class CSMVBaseDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CSMVBaseDlg)

public:
	CSMVBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuF1(UINT nID);
    afx_msg void OnNaviMenuF3T6(UINT nID);
#if SU_FEI
    afx_msg void OnNaviMenuF6X(UINT nID);
#endif
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
    virtual void _beginTimer(UINT nTimeout = DSM_SMV_TIMER_INTERVAL);


    /**
     *      停止默认定时器
     */
    virtual void _stopTimer();

    /**
     *      更新标题
     */
    virtual void _updateCaption();

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \note
     *      子类选择控制块需要重载该方法做一些补充操作
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

    /**
     *      加载控制块到F2菜单中
     */
    virtual void _loadCtrlBlk();

    /**
     *      高亮选中的控制块菜单
     *
     *  \param[in]
     *      pSelCtrlBlk         需要高亮的控制块对象
     */
    void _highlightCtrlBlk(CBaseDetectItem* pSelCtrlBlk);

    /**
     *      初始化当前控制块
     */
    virtual void _initCurrentCtrlBlk();


private:

    /**
     *      高亮当前业务的菜单
     */
    void _highlightMenu();

    /**
     *      关联菜单ID与控制块（冲突覆盖）
     *
     *  \param[in]
     *      nCmdID          菜单命令ID
     *  \param[in]
     *      pCtrlBlk        菜单ID对应的控制块
     */
    void _setMapping(UINT nCmdID, CBaseDetectItem* pCtrlBlk);

    /**
     *      高亮指定索引的控制块菜单
     *
     *  \param[in]
     *      nIndex          高亮的控制块菜单索引号
     */
    inline void _highlightCtrlBlk(int nIndex);

    /**
     *      设置当前业务模式
     */
    void _setMode();



protected:

    typedef std::map<UINT, CBaseDetectItem*>        MapCtrlBlk;

    UINT                    m_nCaptionID;           ///< 标题
    UINT                    m_nTemplateID;          ///< SMV页面模板ID

    UINT_PTR                m_nTimerID;             ///< 刷新数据定时器ID

    CEventProc*             m_pEventProc;           ///< 页面管理器指针
    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针
    CSmvRecvConfig*         m_pSMVRecvConfig;       ///< smv接收配置指针
    CBaseDetectItem*        m_pCurrentCtrlBlk;      ///< 当前工作控制块

    int                     m_nHighLightCtrlBlk;    ///< 当前高亮控制块菜单ID与beginID的差值
    UINT                    m_nCtrlBlkLastMenuID;   ///< 最后一个控制块菜单ID
    MapCtrlBlk              m_mapCtrlBlk;           ///< 菜单命令ID映射控制块表
    CPageDataExtra*         m_pPageDataExtra;       ///< 页面状态数据

    bool                    m_bChannelChange;       ///< 指明控制块通道数发生变化
    bool                    m_bEnableTimer;         ///< 是否启用定时器
};
