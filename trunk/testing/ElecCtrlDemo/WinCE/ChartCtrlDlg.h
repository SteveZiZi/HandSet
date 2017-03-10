// ChartCtrlDlg.h : 头文件
//

#pragma once

// CChartCtrlDlg 对话框
class CChartCtrlDlg : public CDialog
{
// 构造
public:
	CChartCtrlDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CChartCtrlDlg();

// 对话框数据
	enum { IDD = IDD_CHARTCTRL_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHistogramCreate();
	afx_msg void OnBnClickedOscillogram();
	afx_msg void OnBnClickedPhasegram();
private:
	UINT  m_TimerSetting;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSetTimmer();
	afx_msg void OnBnClickedDialCreate();
	afx_msg void OnBnClickedHorHistoCreate();
	afx_msg void OnBnClickedTimeTickClockCreate();
	afx_msg void OnBnClickedOnOffState();
	afx_msg void OnBnClickedKillerTime();
};
