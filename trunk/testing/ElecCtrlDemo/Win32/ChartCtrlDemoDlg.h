// ChartCtrlDemoDlg.h : 头文件
//

#pragma once

// CChartCtrlDemoDlg 对话框
class CChartCtrlDemoDlg : public CDialog
{
// 构造
public:
	CChartCtrlDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CChartCtrlDemoDlg();

// 对话框数据
	enum { IDD = IDD_CHARTCTRLDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
		HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedHistogramCreate();
	afx_msg void OnBnClickedWaveCreate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedPhasorCreate();
	afx_msg void OnBnClickedPolarityCreate();
	afx_msg void OnBnClickedHorhistoCreate();
	afx_msg void OnBnClickedClockCreate();
	afx_msg void OnBnClickedOnoffCreate();
};
