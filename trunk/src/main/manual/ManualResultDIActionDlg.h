/// @file
///
/// @brief
///     手动试验DI变化 头文件
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

// CManualResultDIActionDlg 对话框

class CStateTestResult;

///
/// @brief
///      DI变化
/// 
class CManualResultDIActionDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultDIActionDlg)

public:
	CManualResultDIActionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualResultDIActionDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_RESULT_DI_ACTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

protected:
    ///
    /// @brief
    ///      创建结果报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

protected:

    CReportV             m_ctlDIActionReport;          ///< 设置报表
    CStateTestResult*    m_pStateTestResult;           ///< 状态结果

};
