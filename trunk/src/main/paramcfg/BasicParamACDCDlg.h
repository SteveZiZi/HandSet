/// @file
///
/// @brief
///     交直流设置头文件
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
#include "src/main/dlg/basedlg.h"



// CBasicParamACDCDlg 对话框

///
/// @brief
///    交直流设置
///
class CBasicParamACDCDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamACDCDlg)

public:
	CBasicParamACDCDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamACDCDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_ACDC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnACDCItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnACDCItemCurValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

#if !CHUN_HUA
    afx_msg void OnNaviMenuParamSmvSend(UINT uID);
#endif

public:
    ///
    /// @brief
    ///      页面数据保存
    /// 
    /// @return
    ///      void    
    ///
    void _SavePage();

protected:

    ///
    /// @brief
    ///      创建参数设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();
#if !SU_FEI
	///
	/// @brief
	///      创建参数设置报表
	/// 
	/// @return
	///      void    
	///
	void _createReportCur();
#endif
protected:

    CReportV             m_ctlACDCReport;        ///< 基本参数设置报表
	CReportV			 m_ctlACDCReport_Cur;
public:
//	afx_msg void OnPaint();
};
