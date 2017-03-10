// SampleSyncTestDlg.h : 头文件
//

#pragma once

#include "src\utils\SampleInterpSync\include\SampleInterpSync.h"
#include "src\utils\SampleInterpSync\interp.h"

// CSampleSyncTestDlg 对话框
class CSampleSyncTestDlg : public CDialog
{
// 构造
public:
	CSampleSyncTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SAMPLESYNCTEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
