// ReliabilityTestNoEthDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "testManager.h"
#include "resource.h"
// CReliabilityTestNoEthDlg dialog
class CReliabilityTestNoEthDlg : public CDialog
{
// Construction
public:
	CReliabilityTestNoEthDlg(CWnd* pParent = NULL);	// standard constructor
    
// Dialog Data
	enum { IDD = IDD_RELIABILITYTESTNOETH_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
private:
    CListCtrl       m_ctrlListInfo;
    CString         m_strDeviveType;                    //记录设备的类型
    BOOL            LoadGlobalConfig();                 //加载适用于这个应用程序的配置信息


    CTestManager    *m_pTestManager;
    CString         m_strServerIP;                      //远程服务器的IP地址,用于以太网测试
    BOOL            m_bTesting;                         //是否正在测试

    //于日志相关的成员及函数
    BOOL            InitLog();                          //初始化记录日志
    CString         m_strLogDiskDir;                    //用于记录日志的磁盘在WinCE下的目录
                                                        //在LoadGlobalConfig函数中初始化
    CString         m_strLogDir;                        //最终确定用于记录日志的目录

  

public:
    afx_msg HRESULT OnTestCountChange(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnTestStatusChange(WPARAM wParam,LPARAM lParam);
    afx_msg void    OnBnClickedButton1StartStopTest();
    afx_msg void    OnDestroy();
    afx_msg void OnBnClickedButtonAbout();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedButton3();

    HANDLE GetDeviceHandle();
    void OpenBackLight();

private:
    HANDLE              m_hDevice;
};
