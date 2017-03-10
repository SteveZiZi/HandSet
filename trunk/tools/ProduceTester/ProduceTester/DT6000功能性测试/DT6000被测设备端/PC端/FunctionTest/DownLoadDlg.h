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
     static unsigned int CALLBACK  InitProc(void *pVoid);           //��ʼ���߳�
     static unsigned int CALLBACK  ConfigNetProc(void *pVoid);      //���������߳�
     static unsigned int CALLBACK  DownLoadProc(void *pVoid);       //����Ӧ�ó���
     

    // ��ʾ���ؽ������Ŀؼ�
    CProgressCtrl m_ctrlProcess;
    // ��������ʾ����״̬�Ŀؼ�
    CString m_strStatus;

    CString     m_strIp1SubKey,m_strIp2SubKey;  //�����̫��IP��ַ��ע����Ӽ�
    CString     m_strExePath;                   //��ǰ��ִ���ļ����ڵ�Ŀ¼
    int         m_iClientIpNum;                 //�ͻ���IP��Ŀ


    // ���ذ�ť
    CXPButton m_btnDownLoad;
    // �������簴ť
    CXPButton m_btnConfigNet;
    afx_msg void OnBnClickedButtonConfigNet();

    // ��ʾ��̫��1��IP
    CIPAddressCtrl m_ctrlIp1;
    // ��ʾ��̫��2��IP
    CIPAddressCtrl m_ctrlIp2;
public:
    afx_msg void OnBnClickedButtonDownload();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDestroy();  

    CString     m_strIp1,m_strIp2;              //�ͻ�����·��̫��IP        
};
