/// @file
///
/// @brief
///     故障计算 阻抗 头文件
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
#include "src/service/datalayer/config/FaultCalc.h"
#if CHUN_HUA
#include "src/main/statesequence/StateFaultCalcDlg.h"
#else
#include "src/main/statesequence/StateSetDlg.h"
#endif

// CStateFaultImpedanceDlg 对话框

class CStateFaultImpedanceDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultImpedanceDlg)

public:
	CStateFaultImpedanceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateFaultImpedanceDlg();

// 对话框数据
	enum { IDD = IDD_STATE_FAULT_IMPEDANCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnNaviMenuCompensate(UINT nID);
    virtual BOOL PreTranslateMessage(MSG* pMsg);


protected:

    ///
    /// @brief
    ///      创建阻抗报表
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      重新加载报表
    ///
    /// @return
    ///      void    
    ///
    void  _updateReport();


public:
    CReportH       m_ctlImpedanceReport;        ///< 阻抗报表
    CImpedance     m_impedance;                 ///< 阻抗
    eStateFaultSetType   m_eStateFaultSetType;  ///< 设置阻抗的类型
};
