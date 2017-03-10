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
    // ������ʾ��ǰ����״̬�Ŀؼ�
    CString m_strStatus;

    CString     m_strIp1,m_strIp2;              //�ͻ�����·��̫��IP
    CString     m_strServerIP;
    CString     m_strIp1SubKey,m_strIp2SubKey;  //�����̫��IP��ַ��ע����Ӽ�
    CString     m_strExePath;                   //��ǰ��ִ���ļ����ڵ�Ŀ¼
    CString     m_strDevBaseDir;                //����CE�ͻ��˳����Ŀ��·��
    CString     m_strStartFolder;               //���������ļ���
    int         m_iClientIpNum;                 //�ͻ���IP��Ŀ

    BOOL        m_bExcuteAfterDown;             //�Ƿ�����������Ժ�����ִ��ָ����Ӧ�ó���
    CString     m_strExePathAfterDown;          //������ɺ�����ִ�е�Ӧ�ó����ȫ·���ļ���

    BOOL    ConfigNet();
    // ��ʾ�豸��̫��1IP
    CIPAddressCtrl m_ctrlIp1;
    // ��ʾ�豸��̫��2��IP��ַ
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
