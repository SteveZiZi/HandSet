// ChartCtrlDemoDlg.h : ͷ�ļ�
//

#pragma once

// CChartCtrlDemoDlg �Ի���
class CChartCtrlDemoDlg : public CDialog
{
// ����
public:
	CChartCtrlDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CChartCtrlDemoDlg();

// �Ի�������
	enum { IDD = IDD_CHARTCTRLDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
		HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
