/// @file
///
/// @brief
///     状态序列状态设置 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///


#pragma once

#include "src/service/datalayer/config/ParamConfigStateSequence.h"
#include "src/main/dlg/basedlg.h"

#include "src/utils/ReportControl/ListBoxEL/TreeBoxEL.h"
//#include "src/service/datalayer/config/FaultCalc.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"

class CStatus;

enum SSSmvFocusColType;
///
/// 报表显示的类型
///
#if SU_FEI
enum SSShowReportType
{
    SSShowSetReport = 0,                                ///< 通道报表
    SSShowSmvDataReport                                   ///< 控制块信息报表
};
#else
enum SSShowReportType
{
	SSShowNone =0,
	SSShowSetReport,                                ///< 通道报表
	SSShowSmvDataReport,                                  ///< 控制块信息报表
	SSShowHarmonicReport,
	SSShowFaultComensateReport,
	SSShowSMVListReport,
	SSShowSmvChanMap
};
enum eStateFaultSetType
{
	eSFSTOther = 0,
	eSFSTImpedanceZ1,
	eSFSTCompensateK1,
	eSFSTImpedanceZs,
	eSFSTCompensateKs,
};

#endif
///
/// @brief
///    状态序列状态列表 扩展数据
///
class CStateSetDataExtra :public IPageDataExtra
{

public:
    CStateSetDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_nFocusColIndex(-1)
        , m_pStatus(NULL)
        , m_nIndex(0)
        , m_eShowReportType(SSShowSetReport)
#if !CHUN_HUA
		, m_eStateFaultSetType(eSFSTOther)
#endif
    {

    }
    virtual ~CStateSetDataExtra()
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
    SSShowReportType        m_eShowReportType;          ///< 显示的报表类型
    int                     m_nFocusRowIndex;           ///< 用于恢复选中的行
    int                     m_nFocusColIndex;           ///< 用于恢复选中的列
    CStatus*                m_pStatus;                  ///< 状态值
    int                     m_nIndex;                   ///< 状态索引
    CPageData*              m_pPageData;                ///< 关联的页面数据指针
#if !CHUN_HUA
	eStateFaultSetType		m_eStateFaultSetType;		///
#endif

};


///
/// @brief
///      状态序列状态设置 对话框
///
class CStateSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateSetDlg)

public:
	CStateSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateSetDlg();


// 对话框数据
	enum { IDD = IDD_STATE_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
    afx_msg void OnNaviMenuChangeState(UINT nID);
    afx_msg void OnNaviMenuStateSet(UINT nID);
    afx_msg void OnStateSetItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStateSetItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStateSmvDataItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnDOItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
#if !CHUN_HUA
	//谐波叠加
	afx_msg void OnHarmonicItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnHarmonicItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInplaceHarmonicEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnStateHarmonicContextMenu(UINT nID);

	//短路计算
	afx_msg void OnFaultCalcItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnFaultCalcItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnFaultCalcItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnFaultCalcInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
#endif
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	

	afx_msg void OnTimer(UINT_PTR nIDEvent);
#if !CHUN_HUA
	afx_msg void OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult);
	void confirmFaultCalExit();
#endif
public:
    ///
    /// @brief
    ///      显示状态参数报表
    ///
    /// @return
    ///      void    
    ///
    void SetStatus(int nIndex, CStatus* pStatus);

    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();
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
    ///      修改菜单
    ///
    /// @return
    ///      void    
    ///
    virtual void _modifyMenu();

    ///
    /// @brief
    ///      重设菜单项
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

    ///
    /// @brief
    ///      修改SMV菜单F6
    ///
    /// @return
    ///      void    
    ///
    void _resetMenuSmvF6(SSSmvFocusColType smvFoucsColType);

    ///
    /// @brief
    ///      创建参数设置报表
    ///
    /// @return
    ///      void    
    ///
    void _createSetReport();

#if !SHENG_ZE
	///
	/// @brief
	///      创建参数设置DI报表
	///
	/// @return
	///      void    
	///
	void _createSetReportDI();
#endif
    
    ///
    /// @brief
    ///      创建SMV数据报表
    ///
    /// @return
    ///      void    
    ///
    void _createSMVDataReport();
    ///
    /// @brief
    ///      切换类型变化时更新报表设置
    ///
    /// @return
    ///      void    
    ///
    void _updateOnSwitchTypeChange();
    ///
    /// @brief
    ///      显示状态参数报表
    ///
    /// @return
    ///      void    
    ///
    void _showSetReport(bool bIsHide);
    ///
    /// @brief
    ///      高亮F2中选中的状态
    ///
    /// @return
    ///      void    
    ///
    void _highlightF2(BOOL bHighLight = TRUE);


    ///
    /// @brief
    ///      显示Smv数据报表
    ///
    /// @return
    ///      void    
    ///
    void _showSmvDataReport(bool bIsHide);
    ///
    /// @brief
    ///      状态跳转
    ///
    /// @return
    ///      void    
    ///
    void _changeStateTo(int nStateIndex);

    ///
    /// @brief
    ///      更新设置报表数据
    ///
    /// @return
    ///      void    
    ///
    void _updateStateSetReport();

    ///
    /// @brief
    ///      更新Smv报表数据
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvDataReport();

	///
	/// @brief
	///      显示右键菜单
	///
	/// @return
	///      void    
	///
	void _showContexMenu();
#if !CHUN_HUA
	///
	/// @brief
	///      创建谐波报表
	///
	/// @return
	///      void    
	///
	void  _createHarmonicReport();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void  _showHarmonicReport(bool bIsHide);
	///
	/// @brief
	///      重新加载report
	///
	/// @return
	///      void    
	///
	void _recontentReport();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void  _showReport(CListCtrlEL report);
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void  _hideReport();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void _createSmvChanMapTree();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void _showSmvChanMapTree(bool bIsHide);
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void _setHarmonicContextMenu();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///
	void _setSmvContextMenu();
	///
	/// @brief
	///      显示谐波报表
	///
	/// @return
	///      void    
	///

	///
	/// @brief
	///      创建故障计算设置报表
	///
	/// @return
	///      void    
	///
	void  _createFaultCalcReport();

	///
	/// @brief
	///      根据故障类型设置report
	///
	/// @return
	///      void    
	///
	void _resetFaultCalcPartReport();
	///
	/// @brief
	///      显示故障报表
	///
	/// @return
	///      void    
	///
	void _showFaultCalcReport(BOOL bIsHide);


#endif
protected:

    typedef std::map<UINT, CStatus*>        CUINTStatusMap;
    CUINTStatusMap          m_statusMap;               ///< cmdID对应的状态映射表
    SSShowReportType        m_eShowReportType;
    CReportH                m_ctlStateSetReport;       ///< 状态设置报表
    CReportV                m_ctlStateSMVDataReport;   ///< SMV数据报表
    CStatus*                m_pStatus;                 ///< 当前状态
    int                     m_nIndex;                  ///< 状态索引
    CParamConfigStateSequence*  m_pConfigStateSequ;    ///< 状态序列
#if !CHUN_HUA
	CNaviMenu				m_contextMenu;			   ///右键菜单
	CReportV                m_ctlHarmonicReport;       ///< 谐波报表
	CListCtrlEL*			m_pCurrentReport;
	CTreeV                  m_ctlSmvChanMapTree;    ///< 通道映射树

	CParamconfigManualTest* m_pConfigManualTest;    ///< 手动试验参数配置管理器

	CReportH       m_ctlFaultCalcReport;		///< 故障计算设置报表
	CFaultCalc*    m_pFaultCalc;
	eStateFaultSetType		m_eStateFaultType;		//故障类型
#endif
#if !SHENG_ZE
	CReportH                m_ctlStateSetReportDI;       ///< 状态设置报表
	bool					m_DIFlag;					 ///< 标志位
	bool					m_flag;
#endif
};
