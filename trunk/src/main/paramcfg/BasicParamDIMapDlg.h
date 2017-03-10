/// @file
///
/// @brief
///     开入映射表对话框 声明文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#pragma once

#include "src/main/resource.h"
#include "src/main/dlg/BaseDlg.h"



///
/// @brief
///    开入映射表对话框
///
class CBasicParamDIMapDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamDIMapDlg)

public:
	CBasicParamDIMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamDIMapDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_DI_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


    DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuParamDiMapF1();

protected:

    ///
    /// @brief
    ///      创建开入映射报表
    ///
    /// @return
    ///      void    
    ///
    void _creatDIMapReport();
protected:

    UINT               m_uPrevDlgID;
    CReportV           m_ctlDIMapReport;            ///< 开入映射报表
};
