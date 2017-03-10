/// @file
///
/// @brief
///     �������� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once


#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

class CManualStepSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualStepSetDlg)

public:
	CManualStepSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualStepSetDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_STEP_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnStepItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    ///
    /// @brief
    ///      ��������
    ///
    /// @return
    ///          
    ///
    void _createReport();

protected:
    CReportH                   m_ctlStepReport;         ///< ��������
    CSMVSendVirtualChannel*    m_pVirChannel;
    CParamConfigSMVSend*       m_pParamConfigSMVSend;

};
