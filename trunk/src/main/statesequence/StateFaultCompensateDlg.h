/// @file
///
/// @brief
///     ���򲹳� ͷ�ļ�
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
#include "src/service/datalayer/config/FaultCalc.h"
#if CHUN_HUA
#include "src/main/statesequence/StateFaultCalcDlg.h"
#else
#include "src/main/statesequence/StateSetDlg.h"
#endif
// CStateFaultCompensateDlg �Ի���

class CStateFaultCompensateDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultCompensateDlg)

public:
	CStateFaultCompensateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateFaultCompensateDlg();

// �Ի�������
	enum { IDD = IDD_STATE_FAULT_COMPENSATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnNaviMenuCompensate(UINT nID);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

    ///
    /// @brief
    ///      �������ϼ������ñ���
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      ���±���
    ///
    /// @return
    ///      void    
    ///
    void  _updateReport();

public:
    CReportH       m_ctlCompensateReport;       ///< ���򲹳�ϵ�����ñ���
    CCompensate    m_compensate;                ///< ���򲹳�ϵ��
    eStateFaultSetType   m_eStateFaultSetType;  ///< �����迹������

};
