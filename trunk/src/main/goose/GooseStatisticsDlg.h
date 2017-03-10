/**
 * \file
 *      GooseStatisticsDlg.h
 *
 * \brief
 *      goose报文统计页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"


class CPageDataExtra;

// CGooseStatisticsDlg 对话框

class CGooseStatisticsDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseStatisticsDlg)

public:
	CGooseStatisticsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseStatisticsDlg();

// 对话框数据
	enum { IDD = IDD_GOOSE_STATISTICS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      保存页面
     */
    virtual void _SavePage();

protected:
    /**
     *      修改底部菜单
     */
    virtual void _modifyMenu();

private:
    /**
     *      创建报文监视报表
     */
    void _createReport();

    /**
     *      更新报文统计信息
     */
    void _updateData();

private:

    CRect                   m_rcTop1;               ///< 报表标题
    CGooseFrameStatistics   m_data;                 ///< 报文统计数据
    CReportV                m_reportGoose;          ///< 报文统计报表

private:
    CString                 m_csTotal;              ///< 报文总计数
    CString                 m_csLostFrame;          ///< 丢帧计数
    CString                 m_csMisOrder;           ///< 错序计数
    CString                 m_csRetry;              ///< 重发计数
    CString                 m_csStLost;             ///< StNum丢帧计数
    CString                 m_csStMis;              ///< StNum错序计数
    CString                 m_csLostData;           ///< 报文丢失计数
    CString                 m_csCommBreak;          ///< 通信中断计数
    CString                 m_csInvalidTTL;         ///< 存活时间无效计数
    CString                 m_csTimeNotSync;        ///< 时钟未同步计数
    CString                 m_csTimeFault;          ///< 时钟故障计数
};
