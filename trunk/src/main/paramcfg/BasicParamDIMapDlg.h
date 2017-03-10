/// @file
///
/// @brief
///     ����ӳ���Ի��� �����ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///

#pragma once

#include "src/main/resource.h"
#include "src/main/dlg/BaseDlg.h"



///
/// @brief
///    ����ӳ���Ի���
///
class CBasicParamDIMapDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamDIMapDlg)

public:
	CBasicParamDIMapDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamDIMapDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_DI_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


    DECLARE_MESSAGE_MAP()
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuParamDiMapF1();

protected:

    ///
    /// @brief
    ///      ��������ӳ�䱨��
    ///
    /// @return
    ///      void    
    ///
    void _creatDIMapReport();
protected:

    UINT               m_uPrevDlgID;
    CReportV           m_ctlDIMapReport;            ///< ����ӳ�䱨��
};
