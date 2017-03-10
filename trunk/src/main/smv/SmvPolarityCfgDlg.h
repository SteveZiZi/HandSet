#pragma once
#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/DsmDataLayer.h"

// CSmvPolarityCfgDlg �Ի���


class CSmvPolarityCfgDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CSmvPolarityCfgDlg)

public:
	CSmvPolarityCfgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmvPolarityCfgDlg();

// �Ի�������
	enum { IDD = IDD_SMV_POLARITY_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCfgItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    ///
    /// @brief
    ///      ����Set���ñ���
    /// 
    /// @return
    ///      void    
    ///
    void _createCfgReport();

protected:
    CReportH             m_ctlCfgReport;         ///< ���ñ���
    CDsmDataLayer*       m_pDataLayer;
};
