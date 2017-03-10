#pragma once

#include "src/utils/ElectricControl/OnOffStateCtrl/OnOffStateCtrl.h"

// COnOffStateDlg �Ի���

class COnOffStateDlg : public CDialog
{
	DECLARE_DYNAMIC(COnOffStateDlg)

public:
	COnOffStateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COnOffStateDlg();

// �Ի�������
	enum { IDD = IDD_ON_OFF_STATE_DLG };

	void Init();
	void Draw();
	void Reset();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
