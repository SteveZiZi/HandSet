/// @file
///
/// @brief
///     品质设置 头文件
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
#include "src/service/datalayer/DsmDataLayer.h"
// CManualQualitySetDlg 对话框

///
/// @brief
///      通道品质设置
///
class CManualQualitySetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualQualitySetDlg)

public:
	CManualQualitySetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualQualitySetDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_QUALITY_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnQualitySetItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuQualitySet(UINT nID);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///      void    
    ///
    void _createReport();
public:
    ///
    /// @brief
    ///      保存页面
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
protected:

    CReportH        m_ctlQualityReport;          ///< 品质报表
    CIEC9_2Cb*      m_pIEC92Cb;                  ///< 设置的控制块
    CChannel*       m_pChannel;                  ///< 设置的通道
#if !CHUN_HUA
	CList<CChannel*>		m_pChannelList;
#endif
};
