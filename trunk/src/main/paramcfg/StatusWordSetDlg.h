/// @file
///
/// @brief
///     状态字设置 声明文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.10.28
///
/// 修改历史：
///

#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/service/datalayer/DsmDataLayer.h"

// CStatusWordSetDlg 对话框



enum ESWStatusWordType
{
    SWTUnknown = 0,                                ///< 未知
    SWTStatusWord1,                                ///< 状态字1
    SWTStatusWord2,                                ///< 状态字2
};


///
/// @brief
///     状态字设置
///
class CStatusWordSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStatusWordSetDlg)

public:
	CStatusWordSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStatusWordSetDlg();

// 对话框数据
	enum { IDD = IDD_STATUS_WORD_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
        afx_msg void OnStatusWordItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
        afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///          
    ///
    void _SavePage();

protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///          
    ///
    void _createReport();
    ///
    /// @brief
    ///      初始化状态字1
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord1();
    ///
    /// @brief
    ///      初始化9-1或Ft3的状态字2
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord2_9_1orFT3();
    ///
    /// @brief
    ///      初始化FT3扩展的状态字2
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord2FT3Ext();
    ///
    /// @brief
    ///      更新状态字
    ///
    /// @return
    ///          
    ///
    void _updateStatusWord();
public:
    
    unsigned short    m_usWordValue;
    CBaseCb*          m_pBaseCb;
    ESWStatusWordType m_eStatusWordType;
    enumFrameType     m_nSmvSendType;                                     ///< SMV发送类型
    CReportH          m_ctlStatusWordReport;              ///< 状态字报表

};
