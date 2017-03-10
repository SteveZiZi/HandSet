// MulThreadDlg.h : ͷ�ļ�
//

#pragma once

// CMulThreadDlg �Ի���
class CMulThreadDlg : public CDialog
{
// ����
public:
	CMulThreadDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MULTHREAD_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

    HANDLE m_hThread;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButtonStartThread();

public:
    BOOL   m_bThreadRunning;

    afx_msg void OnBnClickedButtonStopThread();
    afx_msg void OnPaint();
};

DWORD FloatThread(PVOID pArg);