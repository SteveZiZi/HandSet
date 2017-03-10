// SCLAnalyzerDlg.h : ͷ�ļ�
//

#pragma once

#include "src/utils/SclAnalysis/include/SCDGOOSEAnalysis.h"
#include "src/utils/SclAnalysis/include/SCDSMV92Analysis.h"
#include "src/utils/SclAnalysis/include/ISCLCacheManager.h"


// CSCLAnalyzerDlg �Ի���
class CSCLAnalyzerDlg : public CDialog
{
// ����
public:
	CSCLAnalyzerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSCLAnalyzerDlg();

// �Ի�������
	enum { IDD = IDD_SCLANALYZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedLoadFile();
	afx_msg void OnBnClickedGetIedInfo();
	afx_msg void OnBnClickedGetDatasetInfo();
	afx_msg void OnBnClickedLoadFile2();
	afx_msg void OnBnClickedGetIedInfo2();
	afx_msg void OnBnClickedGetDatasetInfo2();

	void DisplayIEDInfo(SCDSMV92ANALYSIS_IEDSMV_INFO *iedInfo);
	void DisplayDataSetInfo(SCDSMV92ANALYSIS_IEDSMV_DATASET_INFO *dataSetInfo);
	void DisplayGOOSEIEDInfo(SCDGOOSEANALYSIS_IEDGOOSE_INFO *iedInfo);
	void DisplayGOOSEDataSetInfo(SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO *dataSetInfo);
	void DisplayInputsExtRefInfo(SCDGOOSEANALYSIS_IEDGOOSE_INPUTS_INFO *inputsInfo);
	void DisplayInputsExtRefInfo(SCDSMV92ANALYSIS_IEDSMV_INPUTS_INFO *inputsInfo);

	DECLARE_MESSAGE_MAP()

private:
	CString m_TestData;
	CString m_LoadTime;
	CString m_ReadTime;
	CString m_WriteTime;
	CString m_SaveTime;
	ISCLCacheManager  *m_pSclCacheMgr;
	ISCDSMV92Analysis *m_SMV92Analysis;
	ISCDGOOSEAnalysis *m_GOOSEAnalysis;
public:
	afx_msg void OnBnClickedGetSmvinputInfo();
	afx_msg void OnBnClickedGetGseinputInfo();
	afx_msg void OnBnClickedSaveTest();
};
