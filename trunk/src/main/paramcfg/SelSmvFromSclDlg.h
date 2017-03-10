/// @file
///
/// @brief
///     ��ȫվ���������smv���ͶԻ��� �����ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///

#pragma once
#include "src/main/dlg/BaseDlg.h"


///
/// @brief
///     ��ȫվ���������smv���ͶԻ���
///
class CSelSmvFromSclDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSelSmvFromSclDlg)

public:
	CSelSmvFromSclDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelSmvFromSclDlg();

// �Ի�������
	enum { IDD = IDD_SEL_SMV_FROM_SCL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
   afx_msg void OnSmvSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    ///
    /// @brief
    ///      �����������ñ���
    ///
    /// @return
    ///      void    
    ///
    void        _createReport();

public:
    ///
    /// @brief
    ///      ����ҳ��
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:

    CReportV                m_ctlSmvReport;         ///< Smv����

};
