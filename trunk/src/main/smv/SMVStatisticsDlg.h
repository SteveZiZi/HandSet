/**
 * \file
 *      SMVStatisticsDlg.h
 *
 * \brief
 *      报文统计与离散度页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/7/11
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/utils/electriccontrol/horhistogramctrl/horhistogramctrl.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

#define STATISTICS_PILLARS          6           ///< 柱形个数


// CSMVStatisticsDlg 对话框

class CSMVStatisticsDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVStatisticsDlg)

public:
	CSMVStatisticsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVStatisticsDlg();

// 对话框数据
	enum { IDD = IDD_SMV_STATISTICS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
    //virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();

#if !SU_FEI
	
	afx_msg void OnMenuF4();
	afx_msg void OnMenuF5();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
#endif 
    //virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

public:

    /**
     *      保存页面
     */
    virtual void _SavePage();

protected:

    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  /param[in]
     *      nCmdID      控制块关联的菜单命令ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:


    /**
     *      创建报表
     */
    void _createReport();

    /**
     *      创建柱状图
     */
    void _createHorHistogram();

    /**
     *      更新报文统计信息
     */
    void _update();

    /**
     *      重设离散度标题
     */
    void _resetPillarTitle();

//     /**
//      *      设置采样率菜单
//      */
//     void _setSampleRateMenu();
// 
//     /**
//      *      设置92采样率
//      */
//     void _setSampleRate92(UINT nCmdID);

#if !SU_FEI
	///< 设置当前显示模式
	enum	SMV_STATISTICS_MODE 
	{
		REPORT_MODE,		///< 报文统计模式
		STATISTICS_MODE,	///< 离散度模式
		MONITOR_MODE		///< 监控模式
	};


	  /**
     *      创建通道状态报表
     */
    void _createMonitorReport();

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
    void _initMonitorReport();
    /**
     *      更新底部状态信息
     */
    void _updateStatus(bool bNull = false);

    /**
     *      更新底部状态信息
     */
    void _updateMonitorStatus(bool bNull = false);
    /**
     *      清空界面的数据显示，将数据项设置为 "--"
     */
    void _updateNullUI();

    /**
     *      检查通道数是否变化
     */
    bool _checkChannel();

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

	CDataStatusWatch            m_monitordata;                 ///< 状态监视数据
	CReportV                    m_reportMonitor;       ///< 当前显示的报表
#endif




private:

    CRect                   m_rcTop1;               ///< 报表标题
    CRect                   m_rcTop2;               ///< 离散图标题

    CReportV                m_reportStatistics;     ///< 报文统计数据报表

    float                   m_fdata[STATISTICS_PILLARS];
    MSG_DISP_PERIOD         m_etypes[STATISTICS_PILLARS];

    CHorHistogramCtrl       m_ctrlHistogram;        ///< 横向柱状图
    CDataFrameStatistics    m_data;                 ///< 报文统计数据
    CString                 m_csSampleInter;        ///< 采样间隔描述
    CString                 m_csSample;             ///< 采样率
#if !SU_FEI
	
	SMV_STATISTICS_MODE		m_modeCtr;				///< 当前显示模式

#endif
};
