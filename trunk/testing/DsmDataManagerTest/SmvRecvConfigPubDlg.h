#pragma once


// CSmvRecvConfigPubDlg 对话框

class CSmvRecvConfigPubDlg : public CDialog
{
	DECLARE_DYNAMIC(CSmvRecvConfigPubDlg)

public:
	CSmvRecvConfigPubDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmvRecvConfigPubDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_RECV_CONFIG_PUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonRead();
    afx_msg void OnBnClickedButtonWriteSmvConfig();
};
