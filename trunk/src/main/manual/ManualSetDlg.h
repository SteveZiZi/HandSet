/// @file
///
/// @brief
///     �ֶ������������ ͷ�ļ�
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

///
/// @brief
///      �ֶ������������ �Ի���
///
class CManualSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSetDlg)

public:
	CManualSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSetDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult );

protected:

    ///
    /// @brief
    ///      ����Set���ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createSetReport();

protected:
    CReportH             m_ctlSetReport;         ///< ���ñ���
};
