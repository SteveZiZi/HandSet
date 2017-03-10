// MulThreadDlg.h : 头文件
//

#pragma once

// CMulThreadDlg 对话框
class CMulThreadDlg : public CDialog
{
// 构造
public:
	CMulThreadDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MULTHREAD_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

    HANDLE m_hThread;

	// 生成的消息映射函数
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