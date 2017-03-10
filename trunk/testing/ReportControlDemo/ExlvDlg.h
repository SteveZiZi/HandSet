#pragma once
#include "afxcmn.h"

#include "ExListCtrl/HeaderCtrlCl.h"
#include "ExListCtrl/ListCtrlCl.h"


// CExlvDlg 对话框

class CExlvDlg : public CDialog
{
	DECLARE_DYNAMIC(CExlvDlg)


    int     m_counter;

public:
	CExlvDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExlvDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXLV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    CListCtrlCl  m_ListCtrl;
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
