/// @file
///
/// @brief
///     光功率 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once
#include "src/main/resource.h"
#include "src/main/dlg/BaseDlg.h"

#include "src/service/datalayer/OpticalPower/OpticalPowerManager.h"




///
/// @brief
///     光功率对话框 头文件
///
class CLpLightPowerDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CLpLightPowerDlg)

public:
	CLpLightPowerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLpLightPowerDlg();

// 对话框数据
	enum { IDD = IDD_LP_LIGHT_POWER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
#if !SU_FEI
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	afx_msg void OnMenuF3();
#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
#if SU_FEI
    afx_msg void OnMenuF6();
#endif
protected:
    ///
    /// @brief
    ///      创建端口报表
    ///
    /// @return
    ///      void    
    ///
    void _createPortReport();
    ///
    /// @brief
    ///      更新端口报表数据
    ///
    /// @return
    ///      void    
    ///
    void _updatePortReport();
    ///
    /// @brief
    ///      插入一条端口记录
    ///
    /// @return
    ///      void    
    ///
    void _insertPortToReport(int nIndex, CLightPort* pLightPort);
#if !SU_FEI
	/** 
	*	显示光功率界面	
	*
	**/
	void _showLightPower();
#endif
protected:  
    CReportV                m_ctlPortReport;                                    ///< 端口报表
    COpticalPowerManager*   m_pOptPowMgr;                                       ///< 光功率管理器 
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void _modifyMenu();
};


