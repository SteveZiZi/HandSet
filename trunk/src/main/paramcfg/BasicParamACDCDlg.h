/// @file
///
/// @brief
///     ��ֱ������ͷ�ļ�
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
#include "src/main/dlg/basedlg.h"



// CBasicParamACDCDlg �Ի���

///
/// @brief
///    ��ֱ������
///
class CBasicParamACDCDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamACDCDlg)

public:
	CBasicParamACDCDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamACDCDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_ACDC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnACDCItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnACDCItemCurValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

#if !CHUN_HUA
    afx_msg void OnNaviMenuParamSmvSend(UINT uID);
#endif

public:
    ///
    /// @brief
    ///      ҳ�����ݱ���
    /// 
    /// @return
    ///      void    
    ///
    void _SavePage();

protected:

    ///
    /// @brief
    ///      �����������ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();
#if !SU_FEI
	///
	/// @brief
	///      �����������ñ���
	/// 
	/// @return
	///      void    
	///
	void _createReportCur();
#endif
protected:

    CReportV             m_ctlACDCReport;        ///< �����������ñ���
	CReportV			 m_ctlACDCReport_Cur;
public:
//	afx_msg void OnPaint();
};
