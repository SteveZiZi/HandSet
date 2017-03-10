#pragma once

#include "src/utils/ElectricControl/ClockCtrl/ClockCtrl.h"

// CClockDlg 对话框

class CClockDlg : public CDialog
{
	DECLARE_DYNAMIC(CClockDlg)

public:
	CClockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CClockDlg();

// 对话框数据
	enum { IDD = IDD_CLOCK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void Init();
	void Draw();
	void Reset();

	DECLARE_MESSAGE_MAP()
private:
	DWORD      m_InitTime;
	DWORD      m_DrawTime;
	BOOL       m_bReset;
	CClockCtrl m_ctrl;

	CBitmap m_ClockDialBmp;
	CBitmap m_HourHandBmp;
	CBitmap m_MinHandBmp;
	CBitmap m_SecHandBmp;
public:
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
};
