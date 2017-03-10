#pragma once

#include "src/main/dlg/basedlg.h"

// CStateHarmonicDlg 对话框

class CStatus;

class CStateHarmonicDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateHarmonicDlg)

public:
	CStateHarmonicDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateHarmonicDlg();

// 对话框数据
	enum { IDD = IDD_STATE_HARMONIC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuStateHarmonic(UINT nID);
    afx_msg void OnHarmonicItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnHarmonicItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:


    ///
    /// @brief
    ///      创建谐波报表
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      重新加载report
    ///
    /// @return
    ///      void    
    ///
    void _recontentReport();
    ///
    /// @brief
    ///      设置菜单
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();
	///
	/// @brief
	///      设置菜单
	///
	/// @return
	///      void    
	///
	void _showContexMenu();


public:

    CReportV                m_ctlHarmonicReport;       ///< 谐波报表
    CStatus*                m_pStatus;                 ///< 当前状态
	CNaviMenu				m_contexMenu;				//
};
