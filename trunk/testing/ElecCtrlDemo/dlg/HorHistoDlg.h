#pragma once

#include "src/utils/ElectricControl/HorHistogramCtrl/HorHistogramCtrl.h"

// CHorHistoDlg �Ի���

class CHorHistoDlg : public CDialog
{
	DECLARE_DYNAMIC(CHorHistoDlg)

public:
	CHorHistoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHorHistoDlg();

// �Ի�������
	enum { IDD = IDD_HOR_HISTO_DLG };

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
	CHorHistogramCtrl m_ctrl;

	CBitmap m_pillarBmp;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
