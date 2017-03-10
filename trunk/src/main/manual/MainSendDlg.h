#pragma once
#include "src/main/resourcebmp.h"
#include "src/main/resource.h"
#include "src/main/dlg/BaseDlg.h"

// CMainSendDlg dialog

class CMainSendDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CMainSendDlg)

public:
	CMainSendDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainSendDlg();

// Dialog Data
	enum { IDD = IDD_MAIN_SEND_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	afx_msg void OnMenuF3();
	afx_msg void OnPaint();
private:
	/**
	*	������ӭ����
	*
	**/
	void _createWelcome();
	/**
	*	��ʾ��ӭ����
	*
	**/
	void _showWelcome();


private:

	CReportH			m_ctlWelcome;
#if !SHENG_ZE
	CBitmap				m_bitmap;					///< ��ӭͼƬ
	BOOL				m_flag;						///< ��־λ
#endif
};
