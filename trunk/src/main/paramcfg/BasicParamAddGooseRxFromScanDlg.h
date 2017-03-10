/// @file
///
/// @brief
///     从扫描列表中添加goose对话框 声明文件
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
#include "src/service/datalayer/dsmdatalayer.h"

// CAddGooseFromScanDlg 对话框


///
/// @brief
///    从扫描列表中添加goose对话框
///
class CBasicParamAddGooseRxFromScanDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamAddGooseRxFromScanDlg)

public:
	CBasicParamAddGooseRxFromScanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamAddGooseRxFromScanDlg();

	// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

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

	///
	/// @brief
	///      创建控制块报表
	///
	/// @return
	///      void    
	///
	void _creatCtrlBlockReport();


	///
	/// @brief
	///      扫描到的goose控制块
	///
	/// @param[in]
	/// 
	/// @return
	///      void    
	///
	void _updateDetectGooseReport();

protected:

	int                m_nMsgParamType;                                    ///< 标识是添加goose接收还是发送的
	CReportV           m_ctlCtrlBlockReport;                               ///< goose控制块
	CDsmDataLayer*     m_pDataLayer;                                       ///< 数据层指针
};
