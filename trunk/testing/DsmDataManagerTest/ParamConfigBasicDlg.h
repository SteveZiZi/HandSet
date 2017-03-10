#pragma once


// CParamConfigBasicDlg 对话框

class CParamConfigBasicDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamConfigBasicDlg)

public:
	CParamConfigBasicDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamConfigBasicDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PARAM_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonRead();
    afx_msg void OnBnClickedButtonWrite();
};
