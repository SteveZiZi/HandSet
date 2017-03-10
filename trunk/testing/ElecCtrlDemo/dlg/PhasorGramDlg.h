#pragma once

#include "src/utils/ElectricControl/PhasorGramCtrl/PhasorGramCtrl.h"

// CPhasorGramDlg �Ի���

class CPhasorGramDlg : public CDialog
{
	DECLARE_DYNAMIC(CPhasorGramDlg)

public:
	CPhasorGramDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPhasorGramDlg();

// �Ի�������
	enum { IDD = IDD_PHASOR_GRAM_DLG };

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
	CPhasorGramCtrl m_ctrl;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
