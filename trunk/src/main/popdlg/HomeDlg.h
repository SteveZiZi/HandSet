#pragma once

#include "src/main/resource.h"

#include "src/main/control/buttonex.h"

// CHomeDlg �Ի���

class CHomeDlg : public CDialog
{
	DECLARE_DYNAMIC(CHomeDlg)

public:
	CHomeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHomeDlg();

// �Ի�������
	enum { IDD = IDD_POPUP_HOME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
    virtual void OnOK();

public:

    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
