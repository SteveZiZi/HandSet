#pragma once

#include "src/utils/ElectricControl/OnOffStateCtrl/OnOffStateCtrl.h"

// COnOffStateDlg 对话框

class COnOffStateDlg : public CDialog
{
	DECLARE_DYNAMIC(COnOffStateDlg)

public:
	COnOffStateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COnOffStateDlg();

// 对话框数据
	enum { IDD = IDD_ON_OFF_STATE_DLG };

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
	COnOffStateCtrl m_ctrl;

	CBitmap m_cOnOffBmp1;
	CBitmap m_cOnOffBmp2;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
