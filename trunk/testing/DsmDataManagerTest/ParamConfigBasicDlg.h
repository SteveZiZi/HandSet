#pragma once


// CParamConfigBasicDlg �Ի���

class CParamConfigBasicDlg : public CDialog
{
	DECLARE_DYNAMIC(CParamConfigBasicDlg)

public:
	CParamConfigBasicDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamConfigBasicDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PARAM_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonRead();
    afx_msg void OnBnClickedButtonWrite();
};
