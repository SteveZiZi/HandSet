/// @file
///
/// @brief
///     显示开入量 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once

#include "src/utils/ElectricControl/OnOffStateCtrl/OnOffStateCtrl.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"
// CDIStateDlg 对话框


enum eDIWinPosition;

///
/// @brief
///    显示开入量
///
class CDIStateDlg : public CDialog
{
	DECLARE_DYNAMIC(CDIStateDlg)

public:
	CDIStateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDIStateDlg();

// 对话框数据
	enum { IDD = IDD_DI_STATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    LRESULT OnDIChanged( WPARAM wParam, LPARAM lParam );

protected:
    virtual void OnCancel();
public:

    ///
    /// @brief
    ///      移动显示窗口到底部
    ///
    /// @return
    ///      void    
    ///
    void MoveWindowBottom();

    ///
    /// @brief
    ///      移动显示窗口到顶部
    ///
    /// @return
    ///      void    
    ///
    void MoveWindowTop();

    ///
    /// @brief
    ///      更新DI状态
    ///
    /// @return
    ///      void    
    ///
    void UpdateDIStatus();

protected:

    ///
    /// @brief
    ///      创建ID报表控件
    ///
    /// @return
    ///      void    
    ///
    void _createDIStateCtl();

protected:

    COnOffStateCtrl   m_ctlOnOffState;           ///< 显示控件
    eDIWinPosition    m_eWinPosition;            ///< 显示位置
    CBitmap           m_bmpInit;                 ///< DI 状态Init
    CBitmap           m_bmpOff;                  ///< DI 状态OFF
    CBitmap           m_bmpOn;                   ///< DI 状态ON
    CBitmap           m_bmpBad;                  ///< DI 状态Bad
    CBitmap           m_bmpDisable;              ///< DI 状态Disable
};
