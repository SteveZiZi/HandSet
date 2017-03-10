/// @file
///
/// @brief
///     步长设置 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once


#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CManualStepSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualStepSetDlg)

public:
	CManualStepSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualStepSetDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_STEP_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnStepItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///          
    ///
    void _createReport();

protected:
    CReportH                   m_ctlStepReport;         ///< 步长报表
    CSMVSendVirtualChannel*    m_pVirChannel;
    CParamConfigSMVSend*       m_pParamConfigSMVSend;

};
