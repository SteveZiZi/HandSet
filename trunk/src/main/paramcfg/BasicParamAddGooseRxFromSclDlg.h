/// @file
///
/// @brief
///     从全站配置中添加goose对话框 声明文件
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


//
//从全站配置中选择GOOSE
//
class CBasicParamAddGooseRxFromSclDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamAddGooseRxFromSclDlg)

public:
	CBasicParamAddGooseRxFromSclDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamAddGooseRxFromSclDlg();

	// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCL_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGooseSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:

	///
	/// @brief
	///      创建控制块报表
	///
	/// @return
	///      void    
	///
	void _creatCtrlBlockReport();

public:

	///
	/// @brief
	///      保存页面
	///
	/// @param[in]
	/// 
	/// @return
	///      void    
	///
	virtual void _SavePage();

protected:

	int                m_nMsgParamType;             ///< 标识是添加goose接收还是发送的
	CReportV           m_ctlCtrlBlockReport;        ///< goose控制块
};
