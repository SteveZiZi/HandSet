#pragma once


#include "src/main/dlg/basedlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CParamConfigBasic;
class CGooseCb;

#define DSM_MSG_GOOSE_PARAM             0x03        ///< Goose基本参数



///
/// @brief
///    参数配置数据
///
class CBasicParamPageDataExtraGoose :public IPageDataExtra
{

public:
	CBasicParamPageDataExtraGoose(CPageData* pPageData)
		: IPageDataExtra()
		, m_pPageData(pPageData)
		, m_nMsgParamType (DSM_MSG_GOOSE_PARAM)
		, m_nRightReportFocus(0)
		, m_nAppID(-1)
		, m_pGooseSendSelectedCb(NULL)

	{

	}
	virtual ~CBasicParamPageDataExtraGoose()
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

	CGooseCb*               m_pGooseSendSelectedCb;     ///< goose发送选中的控制块
	CPageData*              m_pPageData;                ///< 关联的页面数据指针
};



// CBasicParamGooseTxDlg dialog

class CBasicParamGooseTxDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamGooseTxDlg)

public:
	CBasicParamGooseTxDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicParamGooseTxDlg();

// Dialog Data
	enum { IDD = IDD_PARAM_GOOSE_TX_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnNaviMenuParamGooseSend(UINT nID);

	afx_msg void OnGooseSendItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
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
	void        _showGooseSettingReport();
	///
	/// @brief
	///      创建SMV设置报表
	///
	/// @return
	///      void    
	///
	void        _createGooseSettingReport();

private:
	CReportH		m_ctlSetting;	//设置报表

};
