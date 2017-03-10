#pragma once

#include "src/utils/ElectricControl/PolarityDialCtrl/PolarityDialCtrl.h"

// CPolarityDialDlg 对话框

class CPolarityDialDlg : public CDialog
{
	DECLARE_DYNAMIC(CPolarityDialDlg)

public:
	CPolarityDialDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPolarityDialDlg();

// 对话框数据
	enum { IDD = IDD_POLARITY_DIAL_DLG };

	void Init();
	void Draw();
	void Reset();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedExit();
private:
	DWORD m_InitTime;
	DWORD m_DrawTime;
	BOOL  m_bReset;
	CPolarityDialCtrl m_ctrl;

	CBitmap m_HandBmp;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
