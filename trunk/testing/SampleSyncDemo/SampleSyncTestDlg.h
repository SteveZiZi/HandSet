// SampleSyncTestDlg.h : ͷ�ļ�
//

#pragma once

#include "src\utils\SampleInterpSync\include\SampleInterpSync.h"
#include "src\utils\SampleInterpSync\interp.h"

// CSampleSyncTestDlg �Ի���
class CSampleSyncTestDlg : public CDialog
{
// ����
public:
	CSampleSyncTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SAMPLESYNCTEST_DIALOG };


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

private:
	CString m_csTestData;
	UINT    m_iSmpSyncCost;
	double  m_dSampleSpace;
	double  m_dSampleT0;
	int     m_nInterpType;
	uint32_t m_nCtrl2Delay;
	BOOL m_bIsDelayCompensate;

public:
	afx_msg void OnBnClickedInterp();
	afx_msg void OnBnClickedSmvInterp();
};
