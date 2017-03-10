/// @file
///
/// @brief
///     ��ȫվ���������goose�Ի��� �����ļ�
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


//
//��ȫվ������ѡ��GOOSE
//
class CBasicParamAddGooseRxFromSclDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamAddGooseRxFromSclDlg)

public:
	CBasicParamAddGooseRxFromSclDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamAddGooseRxFromSclDlg();

	// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCL_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGooseSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:

	///
	/// @brief
	///      �������ƿ鱨��
	///
	/// @return
	///      void    
	///
	void _creatCtrlBlockReport();

public:

	///
	/// @brief
	///      ����ҳ��
	///
	/// @param[in]
	/// 
	/// @return
	///      void    
	///
	virtual void _SavePage();

protected:

	int                m_nMsgParamType;             ///< ��ʶ�����goose���ջ��Ƿ��͵�
	CReportV           m_ctlCtrlBlockReport;        ///< goose���ƿ�
};
