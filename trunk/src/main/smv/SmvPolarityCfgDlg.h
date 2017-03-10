#pragma once
#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/DsmDataLayer.h"

// CSmvPolarityCfgDlg 对话框


class CSmvPolarityCfgDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSmvPolarityCfgDlg)

public:
	CSmvPolarityCfgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmvPolarityCfgDlg();

// 对话框数据
	enum { IDD = IDD_SMV_POLARITY_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCfgItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    ///
    /// @brief
    ///      创建Set设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createCfgReport();

protected:
    CReportH             m_ctlCfgReport;         ///< 设置报表
    CDsmDataLayer*       m_pDataLayer;
};
