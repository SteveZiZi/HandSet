#pragma once
#include "afxcmn.h"

#include "ExListCtrl/HeaderCtrlCl.h"
#include "ExListCtrl/ListCtrlCl.h"


// CExlvDlg �Ի���

class CExlvDlg : public CDialog
{
	DECLARE_DYNAMIC(CExlvDlg)


    int     m_counter;

public:
	CExlvDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CExlvDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_EXLV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    CListCtrlCl  m_ListCtrl;
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
