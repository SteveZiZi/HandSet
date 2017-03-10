#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "XPButton.h"

class CDownLoadDlg : public CDialog
{
	DECLARE_DYNAMIC(CDownLoadDlg)

public:
	CDownLoadDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDownLoadDlg();

// Dialog Data
	enum { IDD = IDD_DOWNLOAD_ASYNC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg HRESULT OnShowStatus(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnInitFinished(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnConfigNetFinished(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnDownloadFinished(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
     static unsigned int CALLBACK  InitProc(void *pVoid);           //初始化线程
     static unsigned int CALLBACK  ConfigNetProc(void *pVoid);      //配置网络线程
     static unsigned int CALLBACK  DownLoadProc(void *pVoid);       //下载应用程序
     

    // 显示下载进度条的控件
    CProgressCtrl m_ctrlProcess;
    // 关联到显示下载状态的控件
    CString m_strStatus;

    CString     m_strIp1SubKey,m_strIp2SubKey;  //存放以太网IP地址的注册表子键
    CString     m_strExePath;                   //当前可执行文件所在的目录
    int         m_iClientIpNum;                 //客户端IP数目


    // 下载按钮
    CXPButton m_btnDownLoad;
    // 配置网络按钮
    CXPButton m_btnConfigNet;
    afx_msg void OnBnClickedButtonConfigNet();

    // 显示以太网1的IP
    CIPAddressCtrl m_ctrlIp1;
    // 显示以太网2的IP
    CIPAddressCtrl m_ctrlIp2;
public:
    afx_msg void OnBnClickedButtonDownload();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDestroy();  

    CString     m_strIp1,m_strIp2;              //客户端两路以太网IP        
};
