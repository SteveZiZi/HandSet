/**
 * \file
 *      TimeScan1588Dlg.h
 *
 * \brief
 *      1588时钟源探测页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/10/9
 */


#pragma once

#include "src/main/common.h"
#include "src/main/resource.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

class CDsmDataLayer;
class CNaviMenu;
class CNaviToolBar;

// CTimeScan1588Dlg 对话框

class CTimeScan1588Dlg : public CDialog
{
	DECLARE_DYNAMIC(CTimeScan1588Dlg)

public:
	CTimeScan1588Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTimeScan1588Dlg();

// 对话框数据
	enum { IDD = IDD_SCAN_1588 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMenuF6();

private:
    /**
     *      创建探测1588时钟源报表
     */
    void _createReport();

    /**
     *      重新探测
     */
    void _rescan();

    /**
     *      清空选择
     */
    void _clearSelect();

    /**
     *      清理数据
     */
    void _clearRes();

    /**
     *      插入探测结果数据
     */
    void _insertRecord();


private:
    UINT                    m_nScanTimerID;         ///< 定时刷新探测结果的定时器ID
    HWND                    m_pHwndPrev;            ///< 之前target
    CNaviMenu*              m_pMenuPrev;            ///< m_pWndPrev关联的menu
    CNaviToolBar*           m_pNaviBar;             ///< 导航菜单栏对象

    CReportV                m_report1588;           ///< 探测1588时钟源结果报表

    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针
};
