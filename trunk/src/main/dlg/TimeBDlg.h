/**
 * \file
 *      TimeBDlg.h
 *
 * \brief
 *      对时业务的头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/6/13
 */

#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

#define DSM_TIMETICK_TIMER_ID               0x84        ///< 定时器ID
#define DSM_TIMETICK_TIMER_INTERVAL         400         ///< 300ms定时器间隔
#if !SHENG_ZE
#define OPTICAL_STATUS(n) ((n)&0x3FF)
#endif
class CRNaviMenu;
class CDsmDataLayer;

// CTimeBDlg 对话框

class CTimeBDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CTimeBDlg)

public:
	CTimeBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTimeBDlg();

// 对话框数据
	enum { IDD = IDD_TIME_B_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnMenuF1();
    afx_msg void OnMenuF2();
    afx_msg void OnMenuF6();
    afx_msg void OnDestroy();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSelectClock(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

private:

    /**
     *      创建B码对时报表
     */
    void _createReportB();

    /**
     *      创建1588对时报表
     */
    void _createReport1588();

    /**
     *      创建1588时钟信息报表
     */
    void _createReportTime();

    /**
     *      切换对时方案报表
     */
    void _switchReport();

    /**
     *      更新菜单以及标题显示
     */
    void _updateDisp();

    
    /**
     *      启动默认定时器
     */
    void _beginTimer(UINT nTimeout = DSM_TIMETICK_TIMER_INTERVAL);

    /**
     *      停止默认定时器
     */
    void _stopTimer();

    /**
     *      更新界面数据
     */
    void _updateData();

    /**
     *      刷新B码数据
     */
    void _updateIRIGB();

    /**
     *      刷新1588探测时钟源数据
     */
    void _update1588Scan();

    /**
     *      刷新1588详细时间信息
     */
    void _update1588Detail();


private:

    enum TimeMode
    {
        TIME_B,
        TIME_1588,
        TIME_1588_DETAIL
    };

private:

    CReportV                m_reportB;          ///< B码报表
    CReportV                m_report1588;       ///< 1588报表
    CReportV                m_reportTime;       ///< 1588时钟信息

    UINT_PTR                m_nTimerID;         ///< 刷新数据定时器ID
    TimeMode                m_eTimeMode;        ///< 当前时间对时模式
    CDsmDataLayer*          m_pDataLayer;       ///< 数据层指针
    CDataTimeSyncIRIGB      m_dataIRIGB;        ///< B码数据
    CRNaviMenu*             m_pRNaviMenu;       ///< 菜单管理对象

    CString                 m_csDetailMac;      ///< 1588详细时间信息时钟源mac地址
    CString                 m_csDetailID;       ///< 1588详细时间信息时钟源ID
    CString                 m_csPort;           ///< 1588时钟数据相关的端口号
#if !SHENG_ZE
	bool					m_BtimeFlag;        ///< B码授时标志
#endif
};
