#pragma once

#include "DspTest.h"

// CDspDemoDlg 对话框
class CDspDemoDlg : public CDialog
{
	DECLARE_DYNAMIC(CDspDemoDlg)

public:
	CDspDemoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDspDemoDlg();

// 对话框数据
	enum { IDD = IDD_DspDemo_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    void UpdateDspData();
	HICON m_hIcon;

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif

	DECLARE_MESSAGE_MAP()
private:
	CString cpu_info;
	DWORD dsp_speed;
	CString str_time;
    CString str_msgdif;
    CString str_timedif;
    CString str_time_smv10calc;
    CString str_msgtime;
    CString str_time_smvgen;
    CString str_time_msgdisp;
    CString str_time_totaltime;
	DWORD data;
	CString str_dsp;
    CString str_dsp2;
    CDspTest *m_pDspTest;
    bool m_fRefresh;
    DWORD m_dwStartTime;
    DWORD m_dwStopTime;
    DWORD m_dwStartMsg;
    DWORD m_dwStopMsg;
    int m_nMsgCount;

    Error *pErr;
    DspInfo *pDspInfo;
    SmvData *pSmvData;
    SMV_DATA_CALC_RT *pSmvCalcData;
    SMV_GEN_DATA_CALC_RT *pSmvGenData;
    float *pMsgDisp;    
public:
	afx_msg void DspTest();
	afx_msg void OnDestroy();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton4();
};
