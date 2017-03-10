// DsmDataManagerTestDlg.h : 头文件
//

#pragma once

#include "src\service\datalayer\DsmDataLayer.h"


// CDsmDataManagerTestDlg 对话框
class CDsmDataManagerTestDlg : public CDialog
{
// 构造
public:
	CDsmDataManagerTestDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CDsmDataManagerTestDlg();
// 对话框数据
	enum { IDD = IDD_DSMDATAMANAGERTEST_DIALOG };


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
public:
    CString m_csTestResult;

private:
    CDsmDataLayer*  m_pDsmDataLayer;
public:
    afx_msg void OnBnClickedButtonBeginDetect();
    afx_msg void OnBnClickedButtonGetDetectResult();
    afx_msg void OnBnClickedButtonLoadScl();
    afx_msg void OnBnClickedButtonStopRecv();
    afx_msg void OnBnClickedButtonSetCurCb();
    afx_msg void OnBnClickedButtonSetRecv();
    afx_msg void OnBnClickedButtonParamConfigBasic();
    afx_msg void OnBnClickedButtonRecvConfigPub();

public:
    afx_msg void OnBnClickedButtonSetFt3CurBlk();
    afx_msg void OnBnClickedButtonSetMode();
    afx_msg void OnBnClickedButtonAddFilter();
    afx_msg void OnBnClickedButtonBegainRecord();
    afx_msg void OnBnClickedButtonStopRecord();
    afx_msg void OnBnClickedButtonRecvInit();
    afx_msg void OnBnClickedButtonRecvUninit();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void UpdateTime();
    afx_msg void OnBnClickedButtonSetTime();
};
