/// @file
///
/// @brief
///     试验结果列表 头文件
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

///
/// @brief
///      页面绑定数据扩展
///
///
class CManualResultListDataExtra :public IPageDataExtra
{

public:
    CManualResultListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRow(-1)
        , m_pStateTestResult(NULL)
    {
    }
    virtual ~CManualResultListDataExtra()
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
///      试验结果列表
///
class CManualResultListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultListDlg)

public:
	CManualResultListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualResultListDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_RESULT_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuManualResultList(UINT nID);

public:
    virtual BOOL OnInitDialog();

public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      CPageData*    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      创建开入量报表
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();
    ///
    /// @brief
    ///      恢复设置
    /// 
    /// @return
    ///      void    
    ///
    void _restorePage();
protected:

    CReportV             m_ctlResultReport;          ///< 设置报表

};
