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
    CString         m_strDeviveType;                    //��¼�豸������
    BOOL            LoadGlobalConfig();                 //�������������Ӧ�ó����������Ϣ


    CTestManager    *m_pTestManager;
    CString         m_strServerIP;                      //Զ�̷�������IP��ַ,������̫������
    BOOL            m_bTesting;                         //�Ƿ����ڲ���

    //����־��صĳ�Ա������
    BOOL            InitLog();                          //��ʼ����¼��־
    CString         m_strLogDiskDir;                    //���ڼ�¼��־�Ĵ�����WinCE�µ�Ŀ¼
                                                        //��LoadGlobalConfig�����г�ʼ��
    CString         m_strLogDir;                        //����ȷ�����ڼ�¼��־��Ŀ¼

  

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
