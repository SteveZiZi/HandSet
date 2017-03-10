/// @file
///
/// @brief
///     SMV通道映射 头文件
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
#include "src/utils/ReportControl/ListBoxEL/TreeBoxEL.h"
// CManualSmvChanMapDlg 对话框

class CParamconfigManualTest;


///
/// @brief
///      SMV通道映射 对话框
///
class CManualSmvChanMapDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvChanMapDlg)

public:
	CManualSmvChanMapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSmvChanMapDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_SMV_CHAN_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL DestroyWindow();

protected:

    ///
    /// @brief
    ///      创建SMV通道映射
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvChanMapTree();
protected:
    
    CTreeV                  m_ctlSmvChanMapTree;    ///< 通道映射树
    CParamconfigManualTest* m_pConfigManualTest;    ///< 手动试验参数配置管理器
};
