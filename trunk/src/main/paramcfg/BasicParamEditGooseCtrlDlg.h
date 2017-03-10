/// @file
///
/// @brief
///     编辑goose对话框 声明文件
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
///    创建控制块报表
///
enum EGShowReportType;

class CGooseCb;


///
/// @brief
///    编辑goose对话框
///
class CBasicParamEditGooseCtrlDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamEditGooseCtrlDlg)

public:
	CBasicParamEditGooseCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamEditGooseCtrlDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_EDIT_GOOSE_CTRL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();


    afx_msg void OnGooseCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseCtrlItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnGooseChanItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnMenuParamEditGoosesCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuParamEditGoosesCtrlF2();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
#endif
	afx_msg void OnNaviMenuParamEditGoosesChan(UINT uID);

    afx_msg void OnTimer(UINT_PTR nIDEvent);

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
    ///      重置报表通道报表
    ///
    /// @return
    ///      void    
    ///
    void _updateChanReport();

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
    ///      更新GOOSE控制块通道个数
    ///
    /// @return
    ///      void    
    ///
    void _updateCtrlBlockChanCount();

    ///
    /// @brief
    ///      显示通道报表
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();

private:

    CReportV                m_ctlChannelReport;         ///< goose通道
    CReportH                m_ctlCtrlBlockReport;       ///< goose控制块
    EGShowReportType        m_nShowReportType;          ///< 显示报表类型
    CGooseCb*               m_pGooseCb;                 ///< 待编辑的控制块
};
