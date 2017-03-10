/// @file
///
/// @brief
///     �⹦�� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once
#include "src/main/resource.h"
#include "src/main/dlg/BaseDlg.h"

#include "src/service/datalayer/OpticalPower/OpticalPowerManager.h"




///
/// @brief
///     �⹦�ʶԻ��� ͷ�ļ�
///
class CLpLightPowerDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CLpLightPowerDlg)

public:
	CLpLightPowerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLpLightPowerDlg();

// �Ի�������
	enum { IDD = IDD_LP_LIGHT_POWER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
#if !SU_FEI
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	afx_msg void OnMenuF3();
#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
#if SU_FEI
    afx_msg void OnMenuF6();
#endif
protected:
    ///
    /// @brief
    ///      �����˿ڱ���
    ///
    /// @return
    ///      void    
    ///
    void _createPortReport();
    ///
    /// @brief
    ///      ���¶˿ڱ�������
    ///
    /// @return
    ///      void    
    ///
    void _updatePortReport();
    ///
    /// @brief
    ///      ����һ���˿ڼ�¼
    ///
    /// @return
    ///      void    
    ///
    void _insertPortToReport(int nIndex, CLightPort* pLightPort);
#if !SU_FEI
	/** 
	*	��ʾ�⹦�ʽ���	
	*
	**/
	void _showLightPower();
#endif
protected:  
    CReportV                m_ctlPortReport;                                    ///< �˿ڱ���
    COpticalPowerManager*   m_pOptPowMgr;                                       ///< �⹦�ʹ����� 
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void _modifyMenu();
};


