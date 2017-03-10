// SCLAnalysisDLLTestDlg.h : ͷ�ļ�
//

#include "src/utils/SclAnalysis/include/SCDGOOSEAnalysis.h"
#include "src/utils/SclAnalysis/include/SCDSMV92Analysis.h"
#include "src/utils/SclAnalysis/include/ISCLCacheManager.h"

// CSCLAnalysisDLLTestDlg �Ի���
class CSCLAnalysisDLLTestDlg : public CDialog
{
// ����
public:
	CSCLAnalysisDLLTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSCLAnalysisDLLTestDlg();

// �Ի�������
	enum { IDD = IDD_SCLANALYSISDLLTEST_DIALOG };


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

	afx_msg void OnBnClickedLoadFile();
	afx_msg void OnBnClickedGetIedInfo();
	afx_msg void OnBnClickedGetDatasetInfo();
	afx_msg void OnBnClickedLoadFileGse();
	afx_msg void OnBnClickedGetIedgseInfo();
	afx_msg void OnBnClickedGetGsedatasetInfo();
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
	afx_msg void OnBnClickedGetGseinputsInfo();
	afx_msg void OnBnClickedGetSmvinputsInfo();
	afx_msg void OnBnClickedSaveTest();
};
