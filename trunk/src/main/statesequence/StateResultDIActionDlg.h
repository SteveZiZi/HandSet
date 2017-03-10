/// @file
///
/// @brief
///     ״̬���в��Խ��DI��λ�б� ͷ�ļ�
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

// CStateResultDIActionDlg �Ի���

class CStateTestResult;


///
/// @brief
///      DI��������λ �Ի���
///
///
class CStateResultDIActionDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateResultDIActionDlg)

public:
	CStateResultDIActionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateResultDIActionDlg();

// �Ի�������
	enum { IDD = IDD_STATE_RESULT_DI_ACTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
protected:
    ///
    /// @brief
    ///      �����������
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

public:

    CReportV             m_ctlDIActionReport;          ///< ���ñ���
    CStateTestResult*    m_pStateTestResult;           ///< ״̬���
};
