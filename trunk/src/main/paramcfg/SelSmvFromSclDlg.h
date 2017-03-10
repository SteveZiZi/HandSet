/// @file
///
/// @brief
///     从全站配置中添加smv发送对话框 声明文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#pragma once
#include "src/main/dlg/BaseDlg.h"


///
/// @brief
///     从全站配置中添加smv发送对话框
///
class CSelSmvFromSclDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSelSmvFromSclDlg)

public:
	CSelSmvFromSclDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelSmvFromSclDlg();

// 对话框数据
	enum { IDD = IDD_SEL_SMV_FROM_SCL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
   afx_msg void OnSmvSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    ///
    /// @brief
    ///      创建参数设置报表
    ///
    /// @return
    ///      void    
    ///
    void        _createReport();

public:
    ///
    /// @brief
    ///      保存页面
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:

    CReportV                m_ctlSmvReport;         ///< Smv报表

};
