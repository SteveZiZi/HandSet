/**
 * \file
 *      GooseScanDlg.h
 *
 * \brief
 *      探测goose控制块页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/8
 */

#pragma once

#include "src/main/dlg/basedlg.h"

class CDsmDataLayer;
class CBaseDetectItem;

// CGooseScanDlg 对话框

class CGooseScanDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CGooseScanDlg)

public:
	CGooseScanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseScanDlg();

// 对话框数据
	enum { IDD = IDD_GOOSE_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuFn(UINT nID);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemChecked(NMHDR* pNotifyStruct, LRESULT* pResult);


private:
    /**
     *      创建探测goose报表
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
     *      设置所选控制块
     */
    bool _setSelect();

    /**
     *      清理数据
     */
    void _clearRes();

    /**
     *      插入探测结果数据
     */
    void _insertRecord();


private:

    UINT                    m_nScanTimerID;     ///< 定时刷新探测结果的定时器ID

    CReportV                m_reportGoose;      ///< 探测goose结果报表

    typedef CList<CBaseDetectItem*, CBaseDetectItem*>   CListItem;

    CListItem               m_lstDetects;       ///< 探测到的信息列表
    CListItem               m_lstSelects;       ///< 选中的信息列表
    CDsmDataLayer*          m_pDataLayer;       ///< 数据层指针
};
