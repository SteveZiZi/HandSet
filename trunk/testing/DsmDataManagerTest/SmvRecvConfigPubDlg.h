#pragma once


// CSmvRecvConfigPubDlg �Ի���

class CSmvRecvConfigPubDlg : public CDialog
{
	DECLARE_DYNAMIC(CSmvRecvConfigPubDlg)

public:
	CSmvRecvConfigPubDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmvRecvConfigPubDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_RECV_CONFIG_PUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonRead();
    afx_msg void OnBnClickedButtonWriteSmvConfig();
};
