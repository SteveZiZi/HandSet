/// @file
///
/// @brief
///     ���ϼ��� �迹 ͷ�ļ�
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

// CStateFaultImpedanceDlg �Ի���

class CStateFaultImpedanceDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultImpedanceDlg)

public:
	CStateFaultImpedanceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateFaultImpedanceDlg();

// �Ի�������
	enum { IDD = IDD_STATE_FAULT_IMPEDANCE_DLG };

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
    ///      �����迹����
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      ���¼��ر���
    ///
    /// @return
    ///      void    
    ///
    void  _updateReport();


public:
    CReportH       m_ctlImpedanceReport;        ///< �迹����
    CImpedance     m_impedance;                 ///< �迹
    eStateFaultSetType   m_eStateFaultSetType;  ///< �����迹������
};
