#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CParamConfigBasic;
class CBaseCb;

#define DSM_MSG_SMV_PARAM               0x02        ///< SMV基本参数


///
/// @brief
///    参数配置数据
///
class CBasicParamPageDataExtraSmv :public IPageDataExtra
{

public:
	CBasicParamPageDataExtraSmv(CPageData* pPageData)
		: IPageDataExtra()
		, m_pPageData(pPageData)
		, m_nMsgParamType (DSM_MSG_SMV_PARAM)
		, m_nRightReportFocus(0)
		, m_nAppID(-1)
		, m_pSmvSendSelectedCb(NULL)

	{

	}
	virtual ~CBasicParamPageDataExtraSmv()
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

	CBaseCb*                m_pSmvSendSelectedCb;       ///< SMV发送选中的控制块
	CPageData*              m_pPageData;                ///< 关联的页面数据指针
};




// CBasicParamSmvTxDlg dialog

class CBasicParamSmvTxDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamSmvTxDlg)

public:
	CBasicParamSmvTxDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicParamSmvTxDlg();

// Dialog Data
	enum { IDD = IDD_PARAM_SMV_TX_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnNaviMenuParamSmvSend(UINT nID);

	afx_msg void OnSmvSendOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSmvSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSmvSendItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
public:
	///
	/// @brief
	///      保存页面
	///
	/// @return
	///      void    
	///
	virtual void _SavePage();

private:

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
	///      显示SMV设置报表
	///
	/// @return
	///      void    
	///
	void        _showSettingReport();
	///
	/// @brief
	///      创建SMV设置报表
	///
	/// @return
	///      void    
	///
	void        _createSettingReport();
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

private:
	CReportH		m_ctlSetting;	//设置报表
};
