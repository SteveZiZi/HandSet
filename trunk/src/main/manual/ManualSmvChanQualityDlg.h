/// @file
///
/// @brief
///     ���ƿ�ͨ��Ʒ���б� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.10.18
///
/// �޸���ʷ��
///
#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

// CManualSmvChanQualityDlg �Ի���


class CManualSmvChanQualityDataExtra :public IPageDataExtra
{

public:
	CManualSmvChanQualityDataExtra(CPageData* pPageData)
		: IPageDataExtra()
		, m_pPageData(pPageData)
		, m_pBaseCb(NULL)
		, m_nFocusRow(-1)
		, m_pChannel(NULL)
	{
	}
	virtual ~CManualSmvChanQualityDataExtra()
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
	int                     m_nFocusRow;            ///< ѡ����
	CBaseCb*                m_pBaseCb;
	CChannel*               m_pChannel;
	CPageData*              m_pPageData;            ///< ������ҳ������ָ��

#if !CHUN_HUA
	CList<CChannel*>		m_pChannelList;
	CList<int>				m_nSelectIndex;
#endif
};

///
/// @brief
///      ���ƿ�ͨ��Ʒ���б�
///
class CManualSmvChanQualityDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvChanQualityDlg)

public:
	CManualSmvChanQualityDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSmvChanQualityDlg();

	// �Ի�������
	enum { IDD = IDD_MANUAL_SMV_CHAN_QUALITY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSmvQualityValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnNaviMenuSmvChanQuality(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNaviMenuCtrlSel(UINT nID);
public:
	///
	/// @brief
	///      ����ҳ������
	///
	/// @return
	///      void    
	///
	virtual void _SavePage();
protected:
	///
	/// @brief
	///      ��������
	///
	/// @return
	///      void    
	///
	void _createReport();

	///
	/// @brief
	///      ����ͨ��Ʒ��
	///
	/// @return
	///      void    
	///
	void _updateChanQualityValue();
	///
	/// @brief
	///      ���¼���ҳ������
	///
	/// @return
	///      void    
	///
	virtual void _restorePage(); 
	///
	/// @brief
	///      ���¼��ز˵���
	///
	/// @return
	///      void    
	///
	void _resetMenu();
	///
	/// @brief
	///      ���±���
	///
	/// @return
	///      void    
	///
	void _updateReport();
	///
	/// @brief
	///      ���±���
	///
	/// @return
	///      void    
	///
	void _showNoneCtrlTips();

protected:

	CReportV                m_ctlChanQualityReport;         ///< ͨ��Ʒ��
	CIEC9_2Cb*              m_pIEC92Cb;                     ///< ���ƿ�

	typedef std::map<UINT, CBaseCb*>        CUINTCtrlCbMap;
	CUINTCtrlCbMap          m_ctrlCbMap;           ///< cmdID��Ӧ��goose���ƿ�ӳ���
	CBaseCb*			    m_pBaseCb;
};
