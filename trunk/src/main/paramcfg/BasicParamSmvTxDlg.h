#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/main/resource.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CParamConfigBasic;
class CBaseCb;

#define DSM_MSG_SMV_PARAM               0x02        ///< SMV��������


///
/// @brief
///    ������������
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

	CBaseCb*                m_pSmvSendSelectedCb;       ///< SMV����ѡ�еĿ��ƿ�
	CPageData*              m_pPageData;                ///< ������ҳ������ָ��
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
	void        _showSettingReport();
	///
	/// @brief
	///      ����SMV���ñ���
	///
	/// @return
	///      void    
	///
	void        _createSettingReport();
	///
	/// @brief
	///      ���콻ֱ������ʾ
	///
	/// @return
	///      void    
	///
	CString  _getACDCFormatString();

	///
	/// @brief
	///      ����SMV���Ͳ�������
	///
	/// @return
	///      void    
	///
	void        _refreshSmvReportWithIEC6185091();

	///
	/// @brief
	///      ����SMV���Ͳ�������
	///
	/// @return
	///      void    
	///
	void        _refreshSmvReportWithIEC6185092();

	///
	/// @brief
	///      ����SMV���Ͳ�������
	///
	/// @return
	///      void    
	///
	void        _refreshSmvReportWithIEC60044FT3();

	///
	/// @brief
	///      ����SMV���Ͳ�������
	///
	/// @return
	///      void    
	///
	void        _refreshSmvReportWithIEC60044FT3Ext();

private:
	CReportH		m_ctlSetting;	//���ñ���
};
