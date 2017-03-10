/// @file
///
/// @brief
///     查看goose对话框 声明文件
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


class  CGooseCb;
// CBasicParamViewGooseCtrlDlg 对话框

///
/// @brief
///     显示的报表类型
///
enum VGShowReportType
{
    VGShowChannelReport = 0,
    VGShowCtrlBlockReport
};


///
/// @brief
///     查看goose对话框
///
class CBasicParamViewGooseCtrlDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamViewGooseCtrlDlg)

public:
	CBasicParamViewGooseCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamViewGooseCtrlDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_VIEW_GOOSE_CTRL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuParamViewGoosesCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuParamViewGoosesCtrlF2();
	virtual BOOL  PreTranslateMessage(MSG* pMsg);
#endif
    afx_msg void OnViewChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();

protected:
    ///
    /// @brief
    ///      创建控制块报表
    ///
    /// @return
    ///      void    
    ///
    void _createCtrlBlockReport();
    
    ///
    /// @brief
    ///      创建通道报表
    ///
    /// @return
    ///      void    
    ///
    void _createChannelReport();

    ///
    /// @brief
    ///      显示goose控制块信息
    ///
    /// @return
    ///      void    
    ///
    void _showCtrlBlockReport();

    ///
    /// @brief
    ///      显示通道信息
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();


public:

    CReportV                m_ctlChannelReport;                                 ///< goose通道
    CReportH                m_ctlCtrlBlockReport;                               ///< goose控制块
    VGShowReportType        m_nShowReportType;                                  ///< 显示报表类型
    CGooseCb*               m_pGooseCb;                                         ///< 待查看的控制块
};
