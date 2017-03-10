#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/DsmDataLayer.h"
#include "DIStateDlg.h"



// CManualSmvGooseSend 对话框

class CGooseCb;
class CParamConfigGOOSESend;
class CParamconfigManualTest;

enum MSSSmvChanValCalc;
enum MSSModifyGooseValType;
enum MSSSmvFocusColType;

enum MSSShowReportType
{
    SRTShowSmvReport = 0,
    SRTShowGooseReport,
};

class CManualSmvGooseSendDataExtra :public IPageDataExtra
{

public:
    CManualSmvGooseSendDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_uNextPageID(0)
        , m_pPageData(pPageData)
        , m_pGooseCb(NULL)
        , m_nFocusCol(-1)
        , m_nFocusRow(-1)
        , m_eShowReportType(SRTShowSmvReport)
        , m_pVirChannel(NULL)
    {
    }
    virtual ~CManualSmvGooseSendDataExtra()
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
    UINT                    m_uNextPageID;          ///< 进入新页面的ID（现只在点击菜单 状态品质或状态字 进入SMV控制列表时设置使用）
    DWORD                   m_dwSendStatus;         ///< 发送的状态
    MSSShowReportType       m_eShowReportType;      ///< 显示哪个的report
    int                     m_nFocusRow;            ///< 选中行
    int                     m_nFocusCol;            ///< 选中列
    CGooseCb*               m_pGooseCb;             ///< 当前控制块
    CSMVSendVirtualChannel* m_pVirChannel;          ///< 编辑的SMV通道
    CPageData*              m_pPageData;            ///< 关联的页面数据指针
};



class CManualSmvGooseSendDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvGooseSendDlg)

public:
	CManualSmvGooseSendDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSmvGooseSendDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_SMV_GOOSE_SEND_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStepOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseSendInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGoosePointItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamSmvGooseSend(UINT nID);
    afx_msg void OnUpdateNaviMenuParamSmvGooseSend(CCmdUI* pCmdUI);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnDIChanged(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnManualTestResult(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNaviMenuGooseCtrlSel(UINT nID);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
protected:

    ///
    /// @brief
    ///      创建SMV参数设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvReport();

    ///
    /// @brief
    ///      更新SMV参数设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _updateSmvReport();

    ///
    /// @brief
    ///      创建GOOSE参数设置报表
    /// 
    /// @return
    ///      void    
    ///
    void _createGooseReport();

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
    ///      显示goose报表
    ///
    /// @return
    ///      void    
    ///
    void _showGooseReport();

    ///
    /// @brief
    ///      高亮显示GooseF3的控制块
    ///
    /// @return
    ///      void    
    ///
    void _highlightGooseF3(BOOL bHighLight = TRUE);

    ///
    /// @brief
    ///      更新goose报表
    ///
    /// @return
    ///      void    
    ///
    void _updateGooseReport();

    ///
    /// @brief
    ///      对通道中值做加法、减操作
    ///
    /// @return
    ///      void    
    ///
    void _smvAddSub(MSSSmvChanValCalc nScvc);

    ///
    /// @brief
    ///      下一控制块
    ///
    /// @return
    ///      void    
    ///
    void _nextCtrlBlock();

    ///
    /// @brief
    ///      生效修改的SMV数据设置
    ///
    /// @return
    ///      void    
    ///
    void _smvModifyEffImed();

    ///
    /// @brief
    ///      生效修改的GOOSE数据设置
    ///
    /// @return
    ///      void    
    ///
    void _gooseModifyEffImed();

    ///
    /// @brief
    ///      修改goose状态设置
    ///
    /// @return
    ///      void    
    ///
    void _modifyAllGooseValue(MSSModifyGooseValType mgvModifyGooseValType);
    
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

    ///
    /// @brief
    ///      重新加载页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _restorePage();
    ///
    /// @brief
    ///      修改菜单HMENU
    ///
    /// @return
    ///      void    
    ///
    virtual void _resetMenu();
    ///
    /// @brief
    ///      修改SMV菜单F6
    ///
    /// @return
    ///      void    
    ///
    virtual void _updateMenuSmv(MSSSmvFocusColType smvFoucsColType);

    ///
    /// @brief
    ///      是否正显示开入量
    ///
    /// @return
    ///      BOOL    
    ///
    virtual BOOL IsDIShowing();

    ///
    /// @brief
    ///      设置开入量显示
    ///
    /// @return
    ///      void    
    ///
    virtual void ShowDI(BOOL bShow);
#if !SHENG_ZE
	///
	/// @brief
	///      角度转换，将角度转换为在-180~180范围内
	///
	/// @return
	///      void    
	///
	double AngleConversion(double Val);
#endif
protected:
    ///
    /// @brief
    ///      重新加载菜单HMENU
    ///
    /// @return
    ///      void    
    ///
    virtual void _reloadNaviMenu(UINT nMenuID);
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
    ///      设置SMV修改标识
    ///
    /// @return
    ///      void
    ///
    void _setSMVModify(BOOL bModify);
    ///
    /// @brief
    ///      设置GOOSE修改标识
    ///
    /// @return
    ///      void
    ///
    void _setGooseModify(BOOL bModify);

    ///
    /// @brief
    ///      判断SMV是否为空
    ///
    /// @return
    ///      BOOL
    ///
    BOOL _isSmvEmpty();
    ///
    /// @brief
    ///      判断goose是否为空
    ///
    /// @return
    ///      BOOL
    ///
    BOOL _isGooseEmpty();
	///
	void _showContexMenu();

protected:

    typedef std::map<UINT, CGooseCb*>        CUINTGooseCbMap;

    CUINTGooseCbMap         m_gooseCbMap;           ///< cmdID对应的goose控制块映射表
    CReportV                m_ctlSmvReport;         ///< smv报表
    CReportV                m_ctlGooseReport;       ///< goose报表
    CDIStateDlg*            m_pDIStateDlg;          ///< 开入量
    
    DWORD                   m_dwSendStatus;         ///< 发送的状态
    MSSShowReportType       m_eShowReportType;      ///< 显示哪个的report
    CGooseCb*               m_pGooseCb;             ///< 当前控制块
    CParamConfigGOOSESend*  m_pConfigGooseSend;     ///< goose发送
    CParamconfigManualTest* m_pConfigManualTest;    ///< 手动试验参数配置管理器
    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针
    
    BOOL                    m_bSmvModify;           ///< 标识SMV是否被修改
    BOOL                    m_bGooseModify;         ///< 标识GOOSE是否被修改

	CNaviMenu*				m_pPopupMenu;			///<右键菜单
	BOOL					m_bIsEditing;			///是否正在编辑

};
