#pragma once


#include "src/main/dlg/basedlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CParamConfigBasic;
class CGooseCb;

#define DSM_MSG_GOOSE_PARAM             0x03        ///< Goose��������



///
/// @brief
///    ������������
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
	///      ��ȡ����������󶨵�����ҳ������ָ��
	///
	/// @return
	///      CPageData*    
	///
	virtual CPageData* GetPageData()
	{
		return m_pPageData;
	}


public:

	int                     m_nMsgParamType;            ///< ������������
	int                     m_nRightReportFocus;        ///< �Ҳ�report����
	int                     m_nAppID;                   ///< ѡ�еĿ��ƿ�APPID

	CGooseCb*               m_pGooseSendSelectedCb;     ///< goose����ѡ�еĿ��ƿ�
	CPageData*              m_pPageData;                ///< ������ҳ������ָ��
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
	///      ����ҳ��
	///
	/// @return
	///      void    
	///
	virtual void _SavePage();

private:

	///
	/// @brief
	///      ��װҳ��
	///
	/// @return
	///      void    
	///
	void       _restorePage();

	///
	/// @brief
	///      ��ʾSMV���ñ���
	///
	/// @return
	///      void    
	///
	void        _showGooseSettingReport();
	///
	/// @brief
	///      ����SMV���ñ���
	///
	/// @return
	///      void    
	///
	void        _createGooseSettingReport();

private:
	CReportH		m_ctlSetting;	//���ñ���

};
