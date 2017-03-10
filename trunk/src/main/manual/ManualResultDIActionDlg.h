/// @file
///
/// @brief
///     �ֶ�����DI�仯 ͷ�ļ�
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

// CManualResultDIActionDlg �Ի���

class CStateTestResult;

///
/// @brief
///      DI�仯
/// 
class CManualResultDIActionDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultDIActionDlg)

public:
	CManualResultDIActionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualResultDIActionDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_RESULT_DI_ACTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

protected:
    ///
    /// @brief
    ///      �����������
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

protected:

    CReportV             m_ctlDIActionReport;          ///< ���ñ���
    CStateTestResult*    m_pStateTestResult;           ///< ״̬���

};
