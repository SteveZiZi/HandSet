/// @file
///
/// @brief
///     Ʒ������ ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once


#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/DsmDataLayer.h"
// CManualQualitySetDlg �Ի���

///
/// @brief
///      ͨ��Ʒ������
///
class CManualQualitySetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualQualitySetDlg)

public:
	CManualQualitySetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualQualitySetDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_QUALITY_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnQualitySetItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuQualitySet(UINT nID);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    ///
    /// @brief
    ///      ��������
    ///
    /// @return
    ///      void    
    ///
    void _createReport();
public:
    ///
    /// @brief
    ///      ����ҳ��
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
protected:

    CReportH        m_ctlQualityReport;          ///< Ʒ�ʱ���
    CIEC9_2Cb*      m_pIEC92Cb;                  ///< ���õĿ��ƿ�
    CChannel*       m_pChannel;                  ///< ���õ�ͨ��
#if !CHUN_HUA
	CList<CChannel*>		m_pChannelList;
#endif
};
