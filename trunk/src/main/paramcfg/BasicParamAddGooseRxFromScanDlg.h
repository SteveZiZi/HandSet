/// @file
///
/// @brief
///     ��ɨ���б������goose�Ի��� �����ļ�
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

// CAddGooseFromScanDlg �Ի���


///
/// @brief
///    ��ɨ���б������goose�Ի���
///
class CBasicParamAddGooseRxFromScanDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamAddGooseRxFromScanDlg)

public:
	CBasicParamAddGooseRxFromScanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamAddGooseRxFromScanDlg();

	// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
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
	///      �������ƿ鱨��
	///
	/// @return
	///      void    
	///
	void _creatCtrlBlockReport();


	///
	/// @brief
	///      ɨ�赽��goose���ƿ�
	///
	/// @param[in]
	/// 
	/// @return
	///      void    
	///
	void _updateDetectGooseReport();

protected:

	int                m_nMsgParamType;                                    ///< ��ʶ�����goose���ջ��Ƿ��͵�
	CReportV           m_ctlCtrlBlockReport;                               ///< goose���ƿ�
	CDsmDataLayer*     m_pDataLayer;                                       ///< ���ݲ�ָ��
};
