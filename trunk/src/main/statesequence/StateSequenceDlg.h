/// @file
///
/// @brief
///     状态序列状态列表 头文件
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
#include "src/main/statesequence/DIDOStateDlg.h"
#include "src/main/popdlg/MsgDlg.h"
// CStateSequenceDlg 对话框


///
///    状态列表中状态数最大值
///
const int  g_nSSStateSequenceCountMax = 10;


class CStatus;

///
/// @brief
///    状态序列状态列表 扩展数据
///
class CStateSequenceDataExtra :public IPageDataExtra
{

public:
    CStateSequenceDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_pStatus(NULL)
    {

    }
    virtual ~CStateSequenceDataExtra()
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

    int                     m_nFocusRowIndex;           ///< 用于恢复选中的行
    CStatus*                m_pStatus;                  ///< 状态值
    CPageData*              m_pPageData;                ///< 关联的页面数据指针
    
};



///
/// @brief
///    状态序列状态列表 扩展数据
///
class CStateSequenceDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateSequenceDlg)

public:
	CStateSequenceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateSequenceDlg();

// 对话框数据
	enum { IDD = IDD_STATE_SEQUENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    LRESULT OnDIChanged( WPARAM wParam, LPARAM lParam );
    LRESULT OnWhetherSwitch( WPARAM wParam, LPARAM lParam );
    LRESULT OnStatusChanged( WPARAM wParam, LPARAM lParam );
    LRESULT OnTestCompleted( WPARAM wParam, LPARAM lParam );
    afx_msg void OnNaviMenuStateSequence(UINT nID);
    afx_msg void OnUpdateNaviMenuStateSequence(CCmdUI* pCmdUI);
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
protected:
    ///
    /// @brief
    ///      创建状态列表报表
    ///
    /// @return
    ///      void    
    ///
    void _createReport();
    ///
    /// @brief
    ///      更新状态列表报表
    ///
    /// @return
    ///      void    
    ///
    void _updateReport();
    ///
    /// @brief
    ///      添加状态
    ///
    /// @return
    ///      void    
    ///
    void _addStatus();
	///
	/// @brief
	///      编辑状态
	///
	/// @return
	///      void    
	///
	void _editStatus();
	///
	/// @brief
	///      删除状态
	///
	/// @return
	///      void    
	///
	void _deleteStatus();
    ///
    /// @brief
    ///      清除状态
    ///
    /// @return
    ///      void    
    ///
    void _clearStatus();
    ///
    /// @brief
    ///      更新状态索引
    ///
    /// @return
    ///      void    
    ///
    void _updateTestIndex();
    ///
    /// @brief
    ///      设置菜单
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

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
    ///      是否正显示开入量
    ///
    /// @return
    ///      BOOL    
    ///
    virtual BOOL _isDIDOShowing();

    ///
    /// @brief
    ///      设置开入量显示
    ///
    /// @return
    ///      void    
    ///
    virtual void _showDIDO(BOOL bShow);

    ///
    /// @brief
    ///      更新开入量
    ///
    /// @return
    ///      void    
    ///
    virtual void _updateDOStatus(int nRowIndex);


public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();

    ///
    /// @brief
    ///      重载加载菜单选项，使自动更新菜单
    ///
    /// @return
    ///      void
    ///
    void _loadNaviMenu();

public:

    CParamConfigStateSequence*  m_pConfigStateSequ;
    CReportV                    m_ctlStateSequReport;   ///< 状态列表报表
    CDIDOStateDlg*              m_pDIDOStateDlg;        ///< 开入量
    BOOL                        m_bTesting;             ///< 是否正在试验
    int                         m_nTestStateIndex;      ///< 当前正在试验的状态索引
};
