/// @file
///
/// @brief
///     参数配置对话框 声明文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.5.28
///
/// 修改历史：
///


#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/main/control/menuex.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"

class CParamConfigBasic;
class CGooseCb;
class CBaseCb;

#define DSM_MSG_PARAM_BASE              0x01        ///< 基本参数
#define DSM_MSG_PARAM_SMV               0x02        ///< SMV发送
#define DSM_MSG_PARAM_GOOSES            0x03        ///< GOOSE发送
#define DSM_MSG_PARAM_GOOSER            0x04        ///< GOOSE接收

#if !SU_FEI
/************************************************************************/
/* 新添加两种表格类型                                                   */
/************************************************************************/
#define	DSM_MSG_PARAM_IMPORT_SCL		0x05		///< 导入SCL
#define	DSM_MSG_PARAM_TIME_WAY			0x06		///< 对时方案
#endif
///
/// @brief
///    参数配置数据
///
class CBasicParamPageDataExtra :public IPageDataExtra
{

public:
    CBasicParamPageDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nMsgParamType (DSM_MSG_PARAM_BASE)
        , m_nRightReportFocus(0)
        , m_nAppID(-1)
      //  , m_pSmvSendSelectedCb(NULL)
      //  , m_pGooseSendSelectedCb(NULL)
      //  , m_pGooseRecSelectedCb(NULL)

    {

    }
    virtual ~CBasicParamPageDataExtra()
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
    
    int                     m_nMsgParamType;            ///< 参数配置类型
    int                     m_nRightReportFocus;        ///< 右侧report焦点
    int                     m_nAppID;                   ///< 选中的控制块APPID
    
  //  CBaseCb*                m_pSmvSendSelectedCb;       ///< SMV发送选中的控制块
   // CGooseCb*               m_pGooseSendSelectedCb;     ///< goose发送选中的控制块
   // CGooseCb*               m_pGooseRecSelectedCb;      ///< goose接收选中的控制块
    CPageData*              m_pPageData;                ///< 关联的页面数据指针
};


///
/// @brief
///    参数配置对话框
///
class CBasicParamDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamDlg)

public:
	CBasicParamDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicParamDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_PARAM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDestroy();
#if SU_FEI
    afx_msg void OnMenuChanged();
#endif
	/*OnInplaceButtonClick是针对SCL导入按钮的消息映射*/
    afx_msg void OnInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
#if SU_FEI
    afx_msg void OnSmvSendOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnSmvSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamSmvSend(UINT nID);

    afx_msg void OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamGooseSend(UINT nID);

    afx_msg void OnGooseRecItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuParamGooseRec(UINT nID);
#endif	
	afx_msg LRESULT OnSelSCL(WPARAM, LPARAM);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    

	afx_msg void OnMenuF6();
#if !SU_FEI
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	afx_msg void OnMenuF3();
	afx_msg void OnMenuF4();
	afx_msg void OnMenuF5();
#endif

public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();

	//virtual void _modifyMenu();
private:

    ///
    /// @brief
    ///      创建参数设置报表
    ///
    /// @return
    ///      void    
    ///
    void        _createReport(CReportH& report, UINT nID);
#if SU_FEI
    ///
    /// @brief
    ///      初始化左侧报表
    ///
    /// @return
    ///      void    
    ///
    void        _initMenuNavi();
#endif
    ///
    /// @brief
    ///      显示基本参数设置
    ///
    /// @return
    ///      void    
    ///
    void        _showBase();

    ///
    /// @brief
    ///      创建基本参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _createBaseReport();

#if !SU_FEI
/************************************************************************/
/* 新添加创建SCL表和对时方案表                                         */
/************************************************************************/
	///
	/// @brief
	///      显示SCL表格
	///
	/// @return
	///      void    
	///
	void        _showSCLReport();

	///
	/// @brief
	///      创建SCL报表
	///
	/// @return
	///      void    
	///
	void        _createSCLReport();
	///
	/// @brief
	///      显示对时方案
	///
	/// @return
	///      void    
	///
	void        _showTimeReport();

	///
	/// @brief
	///      创建对时方案报表
	///
	/// @return
	///      void    
	///
	void        _createTimeReport();
#endif

#if SU_FEI
    ///
    /// @brief
    ///      显示SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _showSmvSend();

    ///
    /// @brief
    ///      创建SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _createSmvSendReport();

    ///
    /// @brief
    ///      构造交直流电显示
    ///
    /// @return
    ///      void    
    ///
    CString  _getACDCFormatString();
    
    ///
    /// @brief
    ///      创建SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC6185091();

    ///
    /// @brief
    ///      创建SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC6185092();

    ///
    /// @brief
    ///      创建SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC60044FT3();

    ///
    /// @brief
    ///      创建SMV发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _refreshSmvReportWithIEC60044FT3Ext();

    ///
    /// @brief
    ///      显示Goose发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _showGooseSend();

    ///
    /// @brief
    ///      创建Goose发送参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _createGooseSendReport();

    ///
    /// @brief
    ///      显示Goose接收参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _showGooseRec();

    ///
    /// @brief
    ///      创建Goose接收参数报表
    ///
    /// @return
    ///      void    
    ///
    void        _createGooseRecReport();

    ///
    /// @brief
    ///      重装页面
    ///
    /// @return
    ///      void    
    ///
    void       _restorePage();

    ///
    /// @brief
    ///      刷新菜单
    ///
    /// @return
    ///      void    
    ///
    void       _refreshMenu();
#else
  ///
    /// @brief
    ///      重装页面
    ///
    /// @return
    ///      void    
    ///
    void       _restorePage();

#endif

private:
#if SU_FEI
    CMenuEx                 m_menuNavi;             ///< 左侧菜单窗口
    CReportH                m_ctlSmvSendReport;     ///< Smv发送
    CReportH                m_ctlGooseSendReport;   ///< goose发送
    CReportH                m_ctlGooseRecReport;    ///< goose接收
#endif
	CReportH                m_ctlBaseReport;        ///< 基本参数设置报表

    CReportH*               m_pCurReport;           ///< 当前显示报表

    CParamConfigBasic*      m_pCfg;                 ///< 参数配置接口


#if !SU_FEI

	/************************************************************************/
	/* 新添加的导入scl和对时方案表                                          */
	/************************************************************************/
	CReportH				m_ctlImportSCLReport;	///< 导入SCL
	CReportH				m_ctlTimeReport;		///< 对时方案报表
#endif

#if !SHENG_ZE
	enum SclModel
	{
		SclFromSd,
		SclFromU,
	};
	SclModel				m_sclModel;
#endif
};
