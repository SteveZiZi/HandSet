/// @file
///
/// @brief
///     ״̬������ �����ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.10.28
///
/// �޸���ʷ��
///

#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/include/datatypes.h"
#include "src/service/datalayer/DsmDataLayer.h"

// CStatusWordSetDlg �Ի���



enum ESWStatusWordType
{
    SWTUnknown = 0,                                ///< δ֪
    SWTStatusWord1,                                ///< ״̬��1
    SWTStatusWord2,                                ///< ״̬��2
};


///
/// @brief
///     ״̬������
///
class CStatusWordSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStatusWordSetDlg)

public:
	CStatusWordSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStatusWordSetDlg();

// �Ի�������
	enum { IDD = IDD_STATUS_WORD_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
        afx_msg void OnStatusWordItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
        afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
    ///
    /// @brief
    ///      ��������
    ///
    /// @return
    ///          
    ///
    void _SavePage();

protected:
    ///
    /// @brief
    ///      ��������
    ///
    /// @return
    ///          
    ///
    void _createReport();
    ///
    /// @brief
    ///      ��ʼ��״̬��1
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord1();
    ///
    /// @brief
    ///      ��ʼ��9-1��Ft3��״̬��2
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord2_9_1orFT3();
    ///
    /// @brief
    ///      ��ʼ��FT3��չ��״̬��2
    ///
    /// @return
    ///          
    ///
    void _initReportStatusWord2FT3Ext();
    ///
    /// @brief
    ///      ����״̬��
    ///
    /// @return
    ///          
    ///
    void _updateStatusWord();
public:
    
    unsigned short    m_usWordValue;
    CBaseCb*          m_pBaseCb;
    ESWStatusWordType m_eStatusWordType;
    enumFrameType     m_nSmvSendType;                                     ///< SMV��������
    CReportH          m_ctlStatusWordReport;              ///< ״̬�ֱ���

};
