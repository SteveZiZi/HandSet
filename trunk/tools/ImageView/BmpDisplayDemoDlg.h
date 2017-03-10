// BmpDisplayDemoDlg.h : ͷ�ļ�
//

#pragma once

// CBmpDisplayDemoDlg �Ի���
class CBmpDisplayDemoDlg : public CDialog
{
// ����
public:
	CBmpDisplayDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BMPDISPLAYDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	CString m_csBmpPath;
	CString m_csBmpDir;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	void DrawBitmap(CDC *pDC,CString path);
	void OpenBmpFile();

	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
