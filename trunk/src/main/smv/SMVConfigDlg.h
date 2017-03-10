/**
 * \file
 *      SMVConfigDlg.h
 *
 * \brief
 *      配置页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/main/control/menuex.h"

#include "src/utils/ReportControl/ListCtrlEL/ListCtrlEL.h"


class CSmvRecvChannelInfo;



// CSMVConfigDlg 对话框

class CSMVConfigDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVConfigDlg)

public:
	CSMVConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVConfigDlg();

// 对话框数据
	enum { IDD = IDD_SMV_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
#if SU_FEI
    afx_msg void OnMenuChanged();
#else	
	afx_msg void OnMenuF3();
	afx_msg void OnMenuF6();
#endif
    afx_msg void OnMenuF1();
    afx_msg void OnMenuF2();
    afx_msg void OnItemSelectChangedBase(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemSelectChangedChannel(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemValueChangedChannel(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult);
	

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
    /**
     *      修改菜单HMENU
     */
#if SU_FEI
    virtual void _modifyMenu() {}
#else
	virtual void _modifyMenu();
#endif

   /**
     *      禁止加载控制块到F1菜单中
     */
    void _loadCtrlBlk() {}

    /**
     *      初始化当前控制块
     */
    virtual void _initCurrentCtrlBlk() {}

private:

    /**
     *      创建左侧菜单
     */
#if SU_FEI
	void _createLeftMenu();
#endif
    /**
     *      创建报表
     *
     *  \param[in]
     *      report          待创建报表
     *  \param[in]
     *      nID             待创建报表的ID
     */
    void _createReport(CReportH& report, UINT nID);

    /**
     *      初始化基本报表
     */
    void _initBaseReport();

    /**
     *      初始化通道报表
     */
    void _initChannelReport();

    /**
     *      进入基本设置
     */
    void _enterBase();

    /**
     *      进入通道设置
     */
    void _enterChannel();

    /**
     *      根据当前控制块更新smv基本设置报表数据
     */
    void _updateBase();

    /**
     *      根据当前控制块更新smv通道设置数据
     */
    void _updateChannel();

    /**
     *     判断报表控件是否允许交出焦点
     */
    bool _allowKillFocus();

    /**
     *      修改通道类型数据
     */
    void _changedChannelType(ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult);

    /**
     *      修改通道相别数据
     */
    void _changedChannelPhase(ELNM_INPLACELIST_ARGS* pNotifyStruct, LRESULT* pResult);

    /**
     *      更新控制块标题
     */
    void _updateCtrlBlkCaption();

    /**
     *      更新底部菜单显示
     */

    void _updateMenu();

    /**
     *      更新F2菜单标题
     */

    void _updateMenuF2();

    /**
     *      更改通道的模板
     *
     *  \param[in]
     *      nMenuID     命令ID
     *  \return
     *      bool        更改成功返回true，失败返回false
     */
    bool _changeTemplate(UINT nMenuID);


private:

    /**
     *      当前设置控制块的模式
     */
    enum MODE_PAGE
    {
        PAGE_NORMAL,            ///< 普通通道设置模式
        PAGE_DUALPHASE          ///< 核相通道设置模式
    };

    /**
     *      当前显示控制块的标识
     */
    enum MODE_CTRL
    {
        CTRL_ONE,           ///< 第一个控制块
        CTRL_TWO            ///< 第二个控制块
    };

#if !SU_FEI
	enum SMV_MODE_SETTING
	{
		SETTING_BASIC,		///< 基本设置
		SETTING_CHANNEL		///< 通道设置
	};
#endif

private:

#if SU_FEI
    CMenuEx             m_menuNavi;             ///< 左侧导航菜单
#endif
#if !SU_FEI
	SMV_MODE_SETTING	m_modeSetting;			///< 当前显示报表模式
#endif

    CReportH*           m_pCurrentReport;       ///< 当前显示的报表

    CReportH            m_reportBase;           ///< 基本设置报表
    CReportH            m_reportChannel;        ///< 通道设置报表
    CReportH            m_reportChannel2;       ///< 核相页面的第二个控制通道设置报表

    MODE_PAGE           m_modePage;             ///< 当前设置控制块的模式
    MODE_CTRL           m_modeCtrl;             ///< 当前显示的控制块标识

    CBaseDetectItem*    m_pCurCtrlBlk;          ///< 当前控制块    m_pCurrentCtrlBlk功能移交给m_pCurCtrlBlk
    CBaseDetectItem*    m_pCtrlBlk1;            ///< smv1控制块
    CBaseDetectItem*    m_pCtrlBlk2;            ///< smv2控制块

    CNaviMenu*          m_pF6SubTemplate;       ///< 模板子菜单
    UINT                m_nBeginID;             ///< 子菜单命令起始ID
    UINT                m_nEndID;               ///< 子菜单命令结束ID

    typedef std::map<UINT, enumSmvRecvConfigTemplateType>   mapTemplate;
    typedef std::map<UINT, UINT> mapTemplateCap;

    mapTemplate         m_mapTemplate;          ///< 菜单ID与模板ID映射表
    mapTemplateCap      m_mapTemplateCaption;   ///< 模板名映射表
};
