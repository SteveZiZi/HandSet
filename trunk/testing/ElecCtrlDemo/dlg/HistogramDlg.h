#pragma once

#include "src/utils/ElectricControl/HistogramCtrl/HistogramCtrl.h"

// CHistogramDlg �Ի���

class CHistogramDlg : public CDialog
{
	DECLARE_DYNAMIC(CHistogramDlg)

public:
	CHistogramDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHistogramDlg();

// �Ի�������
	enum { IDD = IDD_HISTOGRAM_DLG };

	void Init();
	void Draw();
	void Reset();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedReset();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	DWORD    m_InitTime;
	DWORD    m_DrawTime;
	CHistogramCtrl m_ctrl;
	BOOL     m_bReset;

};
