#pragma once

#include "src/utils/ElectricControl/WaveFormGramCtrl/WaveFormGramCtrl.h"

// CWaveFormDlg �Ի���

class CWaveFormDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaveFormDlg)

public:
	CWaveFormDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWaveFormDlg();

// �Ի�������
	enum { IDD = IDD_WAVE_FORM_DLG };

	void Init();
	void Draw();
	void Reset();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedExit();
private:
	DWORD m_InitTime;
	DWORD m_DrawTime;
	BOOL  m_bReset;
	CWaveFormGramCtrl m_ctrl;
	float data1[150];
	float data2[150];
	float data3[150];
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
