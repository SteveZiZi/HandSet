/// @file
///
/// @brief
///     手动实验具体结果 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///


#pragma once

#include "src/main/resource.h"
#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/DsmDataLayer.h"

enum eMRDShowReportType
{
    MRDSmvReport = 0,
    MRDGooseReport ,
    MRDDIActionReport,
};

///
/// @brief
///      手动实验具体结果 对话框
/// 
class CManualResultDetailDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultDetailDlg)

public:
	CManualResultDetailDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualResultDetailDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_RESULT_DETAIL_DLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNaviMenuResultDetail(UINT nID);
    afx_msg void OnUpdateNaviMenuResultDetail(CCmdUI* pCmdUI);
    afx_msg void OnNaviMenuDetailCbSel( UINT nID );
public:

    ///
    /// @brief
    ///     保存页面
    ///
    /// @return
    ///      void
    ///
    void _SavePage();
protected:
    ///
    /// @brief
    ///      创建SMV设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvReport();
    ///
    /// @brief
    ///      显示Smv报表
    ///
    /// @return
    ///      void    
    ///
    void _showSmvReport();
    ///
    /// @brief
    ///      更新Smv报表
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvReport();

    ///
    /// @brief
    ///      创建GOOSE设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createGooseReport();
    ///
    /// @brief
    ///      显示goose报表
    ///
    /// @return
    ///      void    
    ///
    void _showGooseReport();
    ///
    /// @brief
    ///      更新Goose报表
    ///
    /// @return
    ///      void    
    ///
    void _updateGooseReport();

    ///
    /// @brief
    ///      高亮F2中选中的goose控制块
    ///
    /// @return
    ///      void    
    ///
    void _highlightGooseF2( BOOL bHighLight = TRUE);
    ///
    /// @brief
    ///      创建DI改变报表
    /// 
    /// @return
    ///      void    
    ///
    void _createDIActionReport();
    ///
    /// @brief
    ///      显示DIAction报表
    ///
    /// @return
    ///      void    
    ///
    void _showDIActionReport();
    ///
    /// @brief
    ///      更新DIAction报表
    ///
    /// @return
    ///      void
    ///
    void _updateDIActionReport();

    ///
    /// @brief
    ///      重置菜单
    ///
    /// @return
    ///      void
    ///
    void _resetMenu();

    ///
    /// @brief
    ///      重载加载菜单选项，使自动更新菜单
    ///
    /// @return
    ///      void
    ///
    void _loadNaviMenu();
    
    ///
    /// @brief
    ///      设置tips，
    ///
    /// @param
    ///      bAssign   FALSE:默认获取焦点报表的焦点Item值， TRUE：显示指定的字符串 strTips
    ///
    /// @param
    ///      strTips   指定的字符串 strTips
    ///
    /// @return
    ///      void
    ///
    void _updateTips(BOOL bAssign = FALSE, CString strTips = _T(""));


public:
    typedef std::map<UINT, CGooseCb*>        CUINTGooseCbMap;

    CUINTGooseCbMap         m_gooseCbMap;                 ///< cmdID对应的goose控制块映射表
    eMRDShowReportType      m_eShowReportType;            ///< 显示的报表类型
    CReportV                m_ctlSmvReport;               ///< smv报表
    CReportV                m_ctlGooseReport;             ///< goose报表
    CReportV                m_ctlDIActionReport;          ///< 设置报表

    
    BOOL                    m_bShowAllData;               ///< 标识是否过滤没有改变的数据通道

    CGooseCb*               m_pGooseCb;                   ///< 当前goose控制块
    CStateTestResult*       m_pStateTestResult;           ///< 当前状态结果
    CDsmDataLayer*          m_pDataLayer;                 ///< 数据层指针

    int                     m_nSmvGseFocusRowIndex;       ///< 当前选中的行（用于显示提示）
    int                     m_nSmvGseFocusColIndex;       ///< 当前选中的列（用于显示提示）
};
