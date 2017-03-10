// FunctionTestCEDlg.h : header file
//

#pragma once
#include "connect.h"
#include "testManager.h"

//��PC������ʱ�������ڻ��յ������Ϣ
#define     UWM_PC_CONECT           WM_USER+100
#define     UWM_CONNECT_DOWN        WM_USER+101
// CFunctionTestCEDlg dialog
class CFunctionTestCEDlg : public CDialog
{
    // Construction
public:
    CFunctionTestCEDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_FUNCTIONTESTCE_DIALOG };


protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
private:

//     CServer         *m_pServer[MAX_THERNET_NUM];        //����������
//     CString         m_strEthNameList[MAX_THERNET_NUM];  //������������
//     HANDLE          m_phAcceptThread[MAX_THERNET_NUM];  //��������Ӧ��Accept�߳̾��
//     int             m_iEthNum;                          //����������
    HANDLE              m_hAccpeptThread;
    CServer             *m_pServer;
    static DWORD   CALLBACK AcceptProc(void * pVoid);
    
    
    CString     m_strDeviceType;
    //�������ݰ����߳����
    static DWORD CALLBACK ProcessPacketProc(void *pVoid);
    volatile HANDLE              m_hProcessPacket;
    BOOL                m_bKillProcessPacket;   //�Ƿ��˳����ݰ������߳�
    CTestManager        *m_pTestManager;
    CONNECT_WAY         m_connectWay;

    HANDLE              m_hDevice;

    

    BOOL    LoadGlobalConfig();
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg HRESULT OnPCConnect(WPARAM wParam,LPARAM lParam);
    afx_msg HRESULT OnConnectDown(WPARAM wParam,LPARAM lParam);

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    HANDLE GetDeviceHandle();
    CHAR * UnicodeToMByte( CHAR* destStr, CString sourceStr );
    void OpenBackLight();
    CString             m_strDeviceVer;
};
