/**
 * \file
 *      SMVMonitorDlg.h
 *
 * \brief
 *      监视页面
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */



#pragma once

#include "src/main/smv/smvbasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

// CSMVMonitorDlg 对话框

#if SU_FEI

class CSMVMonitorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVMonitorDlg)

public:
	CSMVMonitorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVMonitorDlg();

// 对话框数据
	enum { IDD = IDD_SMV_MONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF1();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu() {}

    /**
     *      禁止加载控制块到F1菜单中
     */
    void _loadCtrlBlk() {}

    /**
     *      更新标题
     */
    virtual void _updateCaption() {}

    /**
     *      初始化当前控制块
     */
    virtual void _initCurrentCtrlBlk() {}


private:
    /**
     *      创建通道状态报表
     */
    void _createReport();

    /**
     *      创建92列
     */
    void _insertColumn92();

    /**
     *      创建91与FT3列
     */
    void _insertColumnFT3();

    /**
     *      初次显示当前控制块通道状态信息
     */
    void _initReport();

    /**
     *      更新当前显示页面的信息
     */
    void _updateReport();

    /**
     *      更新底部状态信息
     */
    void _updateStatus(bool bNull = false);

    /**
     *      更新标题
     */
    void _updateCtrlBlkCaption();

    /**
     *      更新菜单
     */
    void _updateMenu();

    /**
     *      清空界面的数据显示，将数据项设置为 "--"
     */
    void _updateNullUI();

    /**
     *      检查通道数是否变化
     */
    bool _checkChannel();

private:
    /**
     *      页面模式,普通或是核相
     */
    enum  MODE_PAGE
    {
        PAGE_NORMAL,            ///< 普通页面（显示1个控制块）
        PAGE_DUALPHASE          ///< 核相监控页面（显示2个控制块）
    };

    /**
     *      当前显示控制块的标识
     */
    enum MODE_CTRL
    {
        CTRL_ONE,           ///< 第一个控制块
        CTRL_TWO            ///< 第二个控制块
    };



private:

    CString                     m_csMUSync;             ///< MU - 同步
    CString                     m_csMUNotSync;          ///< MU -不同步
    CString                     m_csMUNULL;             ///< MU -不同步
    CString                     m_csWarnGood;           ///< 故障报警 - 良好
    CString                     m_csWarnBad;            ///< 故障报警 - 维修
    CString                     m_csWarnNULL;           ///< 故障报警 - 维修
    CString                     m_csFixConn;            ///< 检修 - 接通
    CString                     m_csFixTest;            ///< 检修 - 试验
    CString                     m_csFixNULL;            ///< 检修 - 试验
    CString                     m_csWakeValid;          ///< 唤醒 - 数据有效
    CString                     m_csWakeInvalid;        ///< 唤醒 - 数据无效
    CString                     m_csWakeNULL;           ///< 唤醒 - 数据无效

    CRect                       m_rcStatus;             ///< 底部状态区域
    CRect                       m_rc92MU;               ///< 92MU同步区域
    CRect                       m_rcFT3MU;              ///< FT3MU同步区域
    CRect                       m_rcFT3Warn;            ///< FT3故障报警区域
    CRect                       m_rcFT3Wake;            ///< FT3唤醒区域
    CRect                       m_rcFT3Fix;             ///< FT3检修区域

    CBitmap                     m_panelStatus;          ///< 状态栏画布

    // 92           DSM_SMV_MONITOR_PROTO_92            0x01
    // 91 FT3       DSM_SMV_MONITOR_PROTO_91FT3         0x02
    // 未知    
    DWORD                       m_dwTag;                ///< 协议标志

    int                         m_nBegin;               ///< 显示页面第一行索引
    int                         m_nEnd;                 ///< 显示页面最后一行索引

    CDataStatusWatch            m_data;                 ///< 状态监视数据

    CReportV                    m_reportChannel;        ///< 通道状态报表
    CReportV                    m_reportChannel2;       ///< 通道状态报表2（核相时用）
    CReportV*                   m_pCurrentReport;       ///< 当前显示的报表

    CBaseDetectItem*            m_pCurCtrlBlk;          ///< 当前控制块
    CBaseDetectItem*            m_pCtrlBlk1;            ///< smv1
    CBaseDetectItem*            m_pCtrlBlk2;            ///< smv2

    MODE_PAGE                   m_ePage;                ///< 页面模式,普通或是核相
    MODE_CTRL                   m_modeCtrl;             ///< 当前显示的控制块标识
};

#endif