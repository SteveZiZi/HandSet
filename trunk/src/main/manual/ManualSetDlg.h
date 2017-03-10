/// @file
///
/// @brief
///     手动试验参数设置 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once
#include "src/main/dlg/basedlg.h"

///
/// @brief
///      手动试验参数设置 对话框
///
class CManualSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSetDlg)

public:
	CManualSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSetDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult );

protected:

    ///
    /// @brief
    ///      创建Set设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createSetReport();

protected:
    CReportH             m_ctlSetReport;         ///< 设置报表
};
