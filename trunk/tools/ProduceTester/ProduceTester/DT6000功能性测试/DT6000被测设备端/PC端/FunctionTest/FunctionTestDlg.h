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
    // ��ʾ����״̬�ľ�̬�ı��ؼ�
    CStatic m_ctrlStatus;
    // ����ѡ�񱾵�IP��ַ����Ͽ�
    CComboBox           m_ctrlComboLocalIP;
    //��ʾ���趨�豸��IP��ַ
    CIPAddressCtrl m_ctrlDevIP;
    // ��ʾ������Ϣ���б�ؼ�
    CListCtrlTestInfo m_ctrlTestInfo;


    CTestManager        *m_pManager;
    CONNECT_WAY         m_connectWay;

    //�����߳���صı���
    HANDLE          m_hTestThread;          //�����߳̾��
    BOOL            m_bKillTestThread;      //�Ƿ��˳������߳�
    HANDLE          m_hTestWaitEvent;       //�����̵߳ȴ����¼�����������������һ��
    int             m_iTestItem;            //�����еĲ�����
    static unsigned int CALLBACK  TestThreadProc(void *pVoid);          //�����̺߳���
    static unsigned int CALLBACK  ConnectThreadProc(void *pVoid);       //�����߳�
    CString         m_strCeEthIp[MAX_ETHERNET_NUM];                     //CE����̫���б�
    int             m_iCEEthCount;                                      //ce����̫����������
    CString         m_strDestIP;                                        //�����ӵ���CE��Ŀ��IP

    //�������ļ�����ȫ������
    BOOL    LoadGlobalConfig();
    //��ʼ����־
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
