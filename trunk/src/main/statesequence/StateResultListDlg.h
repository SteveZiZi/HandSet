/// @file
///
/// @brief
///     状态序列测试结果列表 头文件
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

// CStateResultListDlg 对话框

///
/// @brief
///      状态序列测试结果列表页面 附加绑定数据
///
///
class CStateResultListDataExtra :public IPageDataExtra
{

public:
    CStateResultListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRow(-1)
        , m_pStateTestResult(NULL)
    {
    }
    virtual ~CStateResultListDataExtra()
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
    int                     m_nFocusRow;                ///< 选中的行
    CStateTestResult*       m_pStateTestResult;         ///< 选中状态结果数据
    CPageData*              m_pPageData;                ///< 关联的页面数据指针
};



///
/// @brief
///      状态序列测试结果列表页面 对话框
///
///
class CStateResultListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateResultListDlg)

public:
	CStateResultListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateResultListDlg();

// 对话框数据
	enum { IDD = IDD_STATE_RESULT_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuStateResultList(UINT nID);
public:
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

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
    ///      恢复页面设置
    /// 
    /// @return
    ///      void    
    ///
    void _restorePage();
    ///
    /// @brief
    ///      创建开入量报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

public:
      CReportV            m_ctlResultReport;        ///< 结果报表
};
