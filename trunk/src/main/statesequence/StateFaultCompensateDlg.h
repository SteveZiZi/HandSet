/// @file
///
/// @brief
///     零序补偿 头文件
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
// CStateFaultCompensateDlg 对话框

class CStateFaultCompensateDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultCompensateDlg)

public:
	CStateFaultCompensateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateFaultCompensateDlg();

// 对话框数据
	enum { IDD = IDD_STATE_FAULT_COMPENSATE_DLG };

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
    ///      创建故障计算设置报表
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      更新报表
    ///
    /// @return
    ///      void    
    ///
    void  _updateReport();

public:
    CReportH       m_ctlCompensateReport;       ///< 零序补偿系数设置报表
    CCompensate    m_compensate;                ///< 零序补偿系数
    eStateFaultSetType   m_eStateFaultSetType;  ///< 设置阻抗的类型

};
