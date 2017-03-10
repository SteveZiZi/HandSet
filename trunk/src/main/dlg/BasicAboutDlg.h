#pragma once

#include "src/main/resource.h"
#if !SHENG_ZE
#include "src\main\popdlg\HelpDlg.h"
#endif
#include "BaseDlg.h"


// CBasicAboutDlg dialog

class CBasicAboutDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicAboutDlg)
public:
	CBasicAboutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicAboutDlg();

// Dialog Data
	enum { IDD = IDD_BASIC_ABOUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:

	/** 
	*		��������ҳ��
	*/
	void _createAboutReport();
	/**
	*		��ʾ����ҳ��
	*/
	void _showAboutReport();

private:
	CReportAbout	m_ctlAboutReport;				///< ���ڱ���
};


