/// @file
///
/// @brief
///     状态序列测试结果DI变位列表 头文件
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

// CStateResultDIActionDlg 对话框

class CStateTestResult;


///
/// @brief
///      DI开入量变位 对话框
///
///
class CStateResultDIActionDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateResultDIActionDlg)

public:
	CStateResultDIActionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateResultDIActionDlg();

// 对话框数据
	enum { IDD = IDD_STATE_RESULT_DI_ACTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
protected:
    ///
    /// @brief
    ///      创建结果报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

public:

    CReportV             m_ctlDIActionReport;          ///< 设置报表
    CStateTestResult*    m_pStateTestResult;           ///< 状态结果
};
