/// @file
///
/// @brief
///     ��ɨ���б������smv���ͶԻ��� �����ļ�
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
#include "src/service/datalayer/dsmdatalayer.h"


///
/// @brief
///     ��ɨ���б������smv���ͶԻ���
///
class CSelSmvFromScanDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSelSmvFromScanDlg)

public:
	CSelSmvFromScanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSelSmvFromScanDlg();

// �Ի�������
	enum { IDD = IDD_SEL_SMV_FROM_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

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

    ///
    /// @brief
    ///      �����������ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      ����ɨ�赽��SMV���ƿ�
    /// 
    /// @return
    ///      void    
    ///
    void _updateDetectSmvReport();

protected:

    CDsmDataLayer*          m_pDataLayer;           ///< ���ݲ�ָ��
    CParamConfigSMVSend*    m_pConfigSmvSend;       ///< smv���Ͳ�������
    CReportV                m_ctlSmvReport;         ///< Smv����

};
