// WebControlDemoDlg.h : ͷ�ļ�
//

#pragma once

// CWebControlDemoDlg �Ի���
#include "WebCtrl.h"
class CWebControlDemoDlg : public CDialog
{
// ����
public:
	CWebControlDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WEBCONTROLDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;
    CWebCtrl m_webCtrl;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
};
