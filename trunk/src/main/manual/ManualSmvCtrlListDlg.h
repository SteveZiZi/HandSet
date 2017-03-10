/// @file
///
/// @brief
///     SMV控制块列表 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once

#include "src/main/dlg/BaseDlg.h"

#include "src/service/datalayer/dsmdatalayer.h"
// CManualSmvCtrlListDlg 对话框


class CManualSmvCtrlListDataExtra :public IPageDataExtra
{

public:
    CManualSmvCtrlListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_pBaseCb(NULL)
        , m_nFocusRow(-1)
    {
    }
    virtual ~CManualSmvCtrlListDataExtra()
    {
    }

public:

    ///
    /// @brief
    ///      获取附加数据类绑定的数据页面数据指针
    ///
    /// @return
    ///      CPageData*    
    ///
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }
public:
    int                     m_nFocusRow;            ///< 选中行
    CBaseCb*                m_pBaseCb;
    CPageData*              m_pPageData;            ///< 关联的页面数据指针
};





///
/// @brief
///      SMV发送控制块列表
///
class CManualSmvCtrlListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvCtrlListDlg)

public:
	CManualSmvCtrlListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSmvCtrlListDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_SMV_CTRL_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSmvCtrlOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);


public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      重新加载页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _restorePage();

protected:

    CReportV                m_ctlSmvCtrlReport;         ///< smv报表
    CParamConfigSMVSend*    m_pParamConfigSMVSend;


};
