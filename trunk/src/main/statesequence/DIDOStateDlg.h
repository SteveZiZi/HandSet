/// @file
///
/// @brief
///     显示开入开出量 头文件
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
#include "src/service/datalayer/config/Status.h"


// CDIDOStateDlg 对话框


enum eDIDOWinPosition;

class CDIDOStateDlg : public CDialog
{
	DECLARE_DYNAMIC(CDIDOStateDlg)

public:
	CDIDOStateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDIDOStateDlg();

// 对话框数据
	enum { IDD = IDD_DI_DO_STATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
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
    ///      更新DO状态
    ///
    /// @return
    ///      void    
    ///
    void UpdateDOStatus(CStatus* pStatus = NULL);
protected:


    ///
    /// @brief
    ///      更新DI状态
    ///
    /// @return
    ///      void    
    ///
    void UpdateDIStatus();


    ///
    /// @brief
    ///      创建ID报表控件
    ///
    /// @return
    ///      void    
    ///
    void _createDIStateCtl();

    ///
    /// @brief
    ///      创建DO控件
    ///
    /// @return
    ///      void    
    ///
    void _createDOStateCtl();

protected:

    CParamConfigGOOSESend* m_pConfigGooseSend;
    CStatus*          m_pStatus;
    COnOffStateCtrl   m_ctlDIState;              ///< 显示控件
    COnOffStateCtrl   m_ctlDOState;              ///< DI显示控件
    eDIDOWinPosition  m_eWinPosition;            ///< 显示位置
    CBitmap           m_bmpInit;                 ///< DI 状态Init
    CBitmap           m_bmpOff;                  ///< DI 状态OFF
    CBitmap           m_bmpOn;                   ///< DI 状态ON
    CBitmap           m_bmpBad;                  ///< DI 状态Bad
    CBitmap           m_bmpDisable;              ///< DI 状态Disable
};

