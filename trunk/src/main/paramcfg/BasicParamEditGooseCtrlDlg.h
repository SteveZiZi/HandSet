/// @file
///
/// @brief
///     �༭goose�Ի��� �����ļ�
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
///    �������ƿ鱨��
///
enum EGShowReportType;

class CGooseCb;


///
/// @brief
///    �༭goose�Ի���
///
class CBasicParamEditGooseCtrlDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicParamEditGooseCtrlDlg)

public:
	CBasicParamEditGooseCtrlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicParamEditGooseCtrlDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_PARAM_EDIT_GOOSE_CTRL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();


    afx_msg void OnGooseCtrlInplaceButtonClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseCtrlItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseCtrlItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnGooseChanItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGooseChanItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);

    afx_msg void OnMenuParamEditGoosesCtrlF1();
#if !SU_FEI
	afx_msg void OnMenuParamEditGoosesCtrlF2();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
#endif
	afx_msg void OnNaviMenuParamEditGoosesChan(UINT uID);

    afx_msg void OnTimer(UINT_PTR nIDEvent);

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
    ///      ���ñ���ͨ������
    ///
    /// @return
    ///      void    
    ///
    void _updateChanReport();

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
    ///      ����GOOSE���ƿ�ͨ������
    ///
    /// @return
    ///      void    
    ///
    void _updateCtrlBlockChanCount();

    ///
    /// @brief
    ///      ��ʾͨ������
    ///
    /// @return
    ///      void    
    ///
    void _showChannelReport();

private:

    CReportV                m_ctlChannelReport;         ///< gooseͨ��
    CReportH                m_ctlCtrlBlockReport;       ///< goose���ƿ�
    EGShowReportType        m_nShowReportType;          ///< ��ʾ��������
    CGooseCb*               m_pGooseCb;                 ///< ���༭�Ŀ��ƿ�
};
