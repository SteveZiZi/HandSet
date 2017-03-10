/// @file
///
/// @brief
///     �鿴goose�Ի��� �����ļ�
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


class  CGooseCb;
// CBasicParamViewGooseCtrlDlg �Ի���

///
/// @brief
///     ��ʾ�ı�������
///
enum VGShowReportType
{
    VGShowChannelReport = 0,
    VGShowCtrlBlockReport
};


///
/// @brief
///     �鿴goose�Ի���
///
class CBasicParamViewGooseCtrlDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamViewGooseCtrlDlg)

public:
	CBasicParamViewGooseCtrlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamViewGooseCtrlDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_VIEW_GOOSE_CTRL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuParamViewGoosesCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuParamViewGoosesCtrlF2();
	virtual BOOL  PreTranslateMessage(MSG* pMsg);
#endif
    afx_msg void OnViewChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();

protected:
    ///
    /// @brief
    ///      �������ƿ鱨��
    ///
    /// @return
    ///      void    
    ///
    void _createCtrlBlockReport();
    
    ///
    /// @brief
    ///      ����ͨ������
    ///
    /// @return
    ///      void    
    ///
    void _createChannelReport();

    ///
    /// @brief
    ///      ��ʾgoose���ƿ���Ϣ
    ///
    /// @return
    ///      void    
    ///
    void _showCtrlBlockReport();

    ///
    /// @brief
    ///      ��ʾͨ����Ϣ
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();


public:

    CReportV                m_ctlChannelReport;                                 ///< gooseͨ��
    CReportH                m_ctlCtrlBlockReport;                               ///< goose���ƿ�
    VGShowReportType        m_nShowReportType;                                  ///< ��ʾ��������
    CGooseCb*               m_pGooseCb;                                         ///< ���鿴�Ŀ��ƿ�
};
