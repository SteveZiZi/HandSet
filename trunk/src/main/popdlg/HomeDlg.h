#pragma once

#include "src/main/resource.h"

#include "src/main/control/buttonex.h"

// CHomeDlg 对话框

class CHomeDlg : public CDialog
{
	DECLARE_DYNAMIC(CHomeDlg)

public:
	CHomeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHomeDlg();

// 对话框数据
	enum { IDD = IDD_POPUP_HOME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
