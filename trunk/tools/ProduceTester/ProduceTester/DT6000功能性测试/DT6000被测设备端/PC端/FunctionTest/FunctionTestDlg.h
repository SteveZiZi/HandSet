// FunctionTestDlg.h : header file
//

#pragma once
#include "listctrltestinfo.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "testManager.h"
#include "XPButton.h"

// CFunctionTestDlg dialog
class CFunctionTestDlg : public CDialog
{
    // Construction
public:
    CFunctionTestDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_FUNCTIONTEST_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:

    afx_msg void OnBnClickedButtonExcuteTest();
    HRESULT OnNeedDisp(WPARAM wParam,LPARAM lParam);
    HRESULT OnShowStatus(WPARAM wParam,LPARAM lParam);
    HRESULT OnTestBegin (WPARAM wParam,LPARAM lParam);
    HRESULT OnTestEnd   (WPARAM wParm, LPARAM lParma);
    HRESULT OnConnectDown(WPARAM wParm, LPARAM lParma);
    HRESULT OnConnectFinish(WPARAM wParm, LPARAM lParma);

private:
    // 显示测试状态的静态文本控件
    CStatic m_ctrlStatus;
    // 用于选择本地IP地址的组合框
    CComboBox           m_ctrlComboLocalIP;
    //显示并设定设备的IP地址
    CIPAddressCtrl m_ctrlDevIP;
    // 显示测试信息的列表控件
    CListCtrlTestInfo m_ctrlTestInfo;


    CTestManager        *m_pManager;
    CONNECT_WAY         m_connectWay;

    //测试线程相关的变量
    HANDLE          m_hTestThread;          //测试线程句柄
    BOOL            m_bKillTestThread;      //是否退出测试线程
    HANDLE          m_hTestWaitEvent;       //测试线程等待此事件对象来决定测试哪一项
    int             m_iTestItem;            //欲进行的测试项
    static unsigned int CALLBACK  TestThreadProc(void *pVoid);          //测试线程函数
    static unsigned int CALLBACK  ConnectThreadProc(void *pVoid);       //连接线程
    CString         m_strCeEthIp[MAX_ETHERNET_NUM];                     //CE端以太网列表
    int             m_iCEEthCount;                                      //ce端以太网网卡数量
    CString         m_strDestIP;                                        //已连接到的CE端目标IP

    //由配置文件加载全局配置
    BOOL    LoadGlobalConfig();
    //初始化日志
    BOOL    InitLog();

    bool    bTestSuc[64];

    WCHAR       m_strTitle[256];

public:
    afx_msg void OnCbnDropdownComboLocalIp();
    afx_msg void OnCbnCloseupComboLocalIp();
    afx_msg void OnBnClickedButtonConnect();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonDownload();
    CXPButton m_btnConnect;
    CXPButton m_btnDownLoad;
    CXPButton m_btnExcute;
};
