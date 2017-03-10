/// @file
///
/// @brief
///     编辑SMV发送控制块对话框 声明文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///

#pragma once
#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/include/datatypes.h"

// CEditSmvSendDlg 对话框


class CBaseCb;

///
/// @brief
///     显示的报表类型
///
enum ESSShowReportType
{
    ESSShowCtrlBlockReport = 0,
    ESSShowChannelReport,
};


enum ESSStatusWordType
{
    ESSStatusWordUnknown = 0,                      ///< 未知
    ESSStatusWord1,                                ///< 状态字1
    ESSStatusWord2,                                ///< 状态字2
};

///
/// @brief
///    状态序列状态列表 扩展数据
///
class CEditSmvSendDataExtra :public IPageDataExtra
{

public:
    CEditSmvSendDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_nFocusColIndex(-1)
        , m_pBaseCb(NULL)
        , m_eStatusWordType(ESSStatusWordUnknown)
        , m_eShowReportType(ESSShowCtrlBlockReport)
    {

    }
    virtual ~CEditSmvSendDataExtra()
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
    ESSShowReportType       m_eShowReportType;          ///< 显示的报表类型
    int                     m_nFocusRowIndex;           ///< 用于恢复选中的行
    int                     m_nFocusColIndex;           ///< 用于恢复选中的列
    CBaseCb*                m_pBaseCb;                  ///< SMV发送控制块
    ESSStatusWordType       m_eStatusWordType;          ///< 状态字类型
    CPageData*              m_pPageData;                ///< 关联的页面数据指针

};




///
/// @brief
///     编辑SMV发送控制块对话框
///
class CEditSmvSendDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CEditSmvSendDlg)

public:
	CEditSmvSendDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEditSmvSendDlg();

// 对话框数据
    enum { IDD = IDD_EDIT_SMV_SEND_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void OnMenuEditSmvCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuEditSmvCtrlF2();
#endif
    afx_msg void OnMenuEditSmvCtrlF6();
    afx_msg void OnSmvCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvCtrlItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnNaviMenuEditSmvChan(UINT uID);
    afx_msg void OnSmvChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvChanItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

#if !CHUN_HUA
	afx_msg void OnPopUpMenu(UINT uID);	

#endif

public:

    ///
    /// @brief
    ///      保存页面参数
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
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
    ///      创建控制块报表
    ///
    /// @return
    ///      void    
    ///
    void _createCtrlBlockReport();

    ///
    /// @brief
    ///      初始化控制块报表 9-1
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC6185091();

    ///
    /// @brief
    ///      初始化控制块报表 9-2
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC6185092();

    ///
    /// @brief
    ///      初始化控制块报表 ft3
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC60044FT3();

    ///
    /// @brief
    ///      初始化控制块报表 ft3扩展
    ///
    /// @return
    ///      void    
    ///
    void _initCtrlReportWithIEC60044FT3Ext();

    ///
    /// @brief
    ///      更新SMV通道数
    ///
    /// @return
    ///      void    
    ///
    void _updateCtrlBlockChanCount();

    ///
    /// @brief
    ///      显示控制块报表
    ///
    /// @return
    ///      void    
    ///
    void _showCtrlBlockReport();

    ///
    /// @brief
    ///      更新F6
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvCtrlMenuF6();

    ///
    /// @brief
    ///      创建通道报表
    ///
    /// @return
    ///      void    
    ///
    void _createChannelReport();
    
    ///
    /// @brief
    ///      更新通道信息（只有9-2 通道数可以修改）
    ///
    /// @return
    ///      void    
    ///
    void _updateChanReport();

    ///
    /// @brief
    ///      显示通道报表
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();

    ///
    /// @brief
    ///      更新smv通道 测量电流<- -> 保护电流
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvChanMenuF4();

    ///
    /// @brief
    ///      重设置菜单
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

#if !CHUN_HUA
	///
	/// @brief
	///      弹出右键菜单
	///
	/// @return
	///      void    
	///
	void _showContexMenu();
	///
	/// @brief
	///      更新右键菜单
	///
	/// @return
	///      void    
	///
	void _updateContexMenu(int col);
	///
	/// @brief
	///      更新相位显示
	///
	/// @return
	///      void    
	///
	void _setPhase( CChannel* &pChannel, int nRowIndex, int nIndex ) ;
	///
	/// @brief
	///      自动映射
	///
	/// @return
	///      void    
	///
	void _setAutoMap();
	///
	/// @brief
	///      映射到第一组
	///
	/// @return
	///      void    
	///
	void _setMapGroup1();
	///
	/// @brief
	///      映射到第二组
	///
	/// @return
	///      void    
	///
	void _setMapGroup2();
	///
	/// @brief
	///      全电流
	///
	/// @return
	///      void    
	///
	void _setTypeAllCurrent();
	///
	/// @brief
	///      全电压 
	///
	/// @return
	///      void    
	///
	void _setTypeAllVolate();
	///
	/// @brief
	///      全电压 
	///
	/// @return
	///      void    
	///
	void setType(CChannel* pChannel, enumChannelType nChanType, int rowIndex);

	CNaviMenu*				m_pPopupMenu;			///<右键菜单
#endif

public:

    CReportV                m_ctlChannelReport;                                 ///< SMV发送通道报表
    CReportH                m_ctlCtrlBlockReport;                               ///< SMV发送控制块报表
    ESSShowReportType       m_eShowReportType;                                  ///< 显示报表类型
    CBaseCb*                m_pBaseCb;                                          ///< SMV发送控制块
    enumFrameType           m_nSmvSendType;                                     ///< SMV发送类型
};
