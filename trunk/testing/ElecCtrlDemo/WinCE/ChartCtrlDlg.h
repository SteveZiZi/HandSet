// ChartCtrlDlg.h : ͷ�ļ�
//

#pragma once

// CChartCtrlDlg �Ի���
class CChartCtrlDlg : public CDialog
{
// ����
public:
	CChartCtrlDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CChartCtrlDlg();

// �Ի�������
	enum { IDD = IDD_CHARTCTRL_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;
	

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHistogramCreate();
	afx_msg void OnBnClickedOscillogram();
	afx_msg void OnBnClickedPhasegram();
private:
	UINT  m_TimerSetting;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSetTimmer();
	afx_msg void OnBnClickedDialCreate();
	afx_msg void OnBnClickedHorHistoCreate();
	afx_msg void OnBnClickedTimeTickClockCreate();
	afx_msg void OnBnClickedOnOffState();
	afx_msg void OnBnClickedKillerTime();
};
