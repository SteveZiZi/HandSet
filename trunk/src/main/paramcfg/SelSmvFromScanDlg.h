/// @file
///
/// @brief
///     从扫描列表中添加smv发送对话框 声明文件
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
#include "src/service/datalayer/dsmdatalayer.h"


///
/// @brief
///     从扫描列表中添加smv发送对话框
///
class CSelSmvFromScanDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSelSmvFromScanDlg)

public:
	CSelSmvFromScanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSelSmvFromScanDlg();

// 对话框数据
	enum { IDD = IDD_SEL_SMV_FROM_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
    ///
    /// @brief
    ///      保存页面
    /// 
    /// @return
    ///      void    
    ///
    virtual void _SavePage();
protected:

    ///
    /// @brief
    ///      创建参数设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      更新扫描到的SMV控制块
    /// 
    /// @return
    ///      void    
    ///
    void _updateDetectSmvReport();

protected:

    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针
    CParamConfigSMVSend*    m_pConfigSmvSend;       ///< smv发送参数配置
    CReportV                m_ctlSmvReport;         ///< Smv报表

};
