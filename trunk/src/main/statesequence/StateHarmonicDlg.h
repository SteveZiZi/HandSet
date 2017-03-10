#pragma once

#include "src/main/dlg/basedlg.h"

// CStateHarmonicDlg �Ի���

class CStatus;

class CStateHarmonicDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateHarmonicDlg)

public:
	CStateHarmonicDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateHarmonicDlg();

// �Ի�������
	enum { IDD = IDD_STATE_HARMONIC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuStateHarmonic(UINT nID);
    afx_msg void OnHarmonicItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnHarmonicItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:


    ///
    /// @brief
    ///      ����г������
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      ���¼���report
    ///
    /// @return
    ///      void    
    ///
    void _recontentReport();
    ///
    /// @brief
    ///      ���ò˵�
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();
	///
	/// @brief
	///      ���ò˵�
	///
	/// @return
	///      void    
	///
	void _showContexMenu();


public:

    CReportV                m_ctlHarmonicReport;       ///< г������
    CStatus*                m_pStatus;                 ///< ��ǰ״̬
	CNaviMenu				m_contexMenu;				//
};
