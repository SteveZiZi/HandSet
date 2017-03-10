// BmpDisplayDemoDlg.h : 头文件
//

#pragma once

// CBmpDisplayDemoDlg 对话框
class CBmpDisplayDemoDlg : public CDialog
{
// 构造
public:
	CBmpDisplayDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BMPDISPLAYDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	CString m_csBmpPath;
	CString m_csBmpDir;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	void DrawBitmap(CDC *pDC,CString path);
	void OpenBmpFile();

	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
