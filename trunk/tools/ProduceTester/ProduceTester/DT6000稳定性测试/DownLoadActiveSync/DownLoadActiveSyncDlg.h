// DownLoadActiveSyncDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#define UM_SHOW_STATUS      WM_USER +100
#define UM_INIT_FINISHED    WM_USER +101
// CDownLoadActiveSyncDlg dialog
class CDownLoadActiveSyncDlg : public CDialog
{
// Construction
public:
	CDownLoadActiveSyncDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_DOWNLOADACTIVESYNC_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
    afx_msg HRESULT OnShowStatus(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnInitFinished(WPARAM wParam,LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    static unsigned int CALLBACK  InitProc(void *pVoid);
    static unsigned int CALLBACK  DownLoadProc(void *pVoid);
    CIPAddressCtrl m_ctrlServerIP;
    // 关联显示当前程序状态的控件
    CString m_strStatus;

    CString     m_strIp1,m_strIp2;              //客户端两路以太网IP
    CString     m_strServerIP;
    CString     m_strIp1SubKey,m_strIp2SubKey;  //存放以太网IP地址的注册表子键
    CString     m_strExePath;                   //当前可执行文件所在的目录
    CString     m_strDevBaseDir;                //拷贝CE客户端程序的目标路径
    CString     m_strStartFolder;               //自启动的文件夹
    int         m_iClientIpNum;                 //客户端IP数目

    BOOL        m_bExcuteAfterDown;             //是否在下载完成以后立即执行指定的应用程序
    CString     m_strExePathAfterDown;          //下载完成后，立即执行的应用程序的全路径文件名

    BOOL    ConfigNet();
    // 显示设备以太网1IP
    CIPAddressCtrl m_ctrlIp1;
    // 显示设备以太网2的IP地址
    CIPAddressCtrl m_ctrlIp2;

    CButton m_btnDownLoad;
    CButton m_btnCancel;    
public:
    afx_msg void OnBnClickedButtonDown();
    CProgressCtrl m_ctrlProcess;
    afx_msg void OnBnClickedButtonRemove();
private:
    void  DelLoop(const WCHAR  *strDir,int &count,const int iTotalCount);

};
