// FpgaUpdateDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"
// CFpgaUpdateDlg �Ի���
class CFpgaUpdateDlg : public CDialog
{
// ����
public:
	CFpgaUpdateDlg(CWnd* pParent = NULL);	// ��׼���캯��
    ~CFpgaUpdateDlg();
// �Ի�������
	enum { IDD = IDD_FPGAUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;
    CBitmap m_mainBG;
    CBitmap m_SuccBG;
    CBitmap m_FailBG;
    CBitmap m_scroll;
    CBitmap m_scrollBG;
    CBitmap m_runScroll;

    //��ǰ�����Ľ������
    double m_curRange;

    //��������Ľ������
    double m_curDetailRange;

    CString m_strText;
    CString m_strDetailText;

    //�����Ƿ�ɹ�
    bool m_bIsSuccess;

    //���ȹ��������Ͻ�����
    POINT m_scrollLeftP;  

    //������������Ͻ�����
    POINT m_scrollDetailLeftP;

    //������ȵ��ı���ʾ�������Ͻ�����
    POINT m_textLeftP;    

    //����������ı���ʾ�������Ͻ�����
    POINT m_textDetailLeftP;

    //�������ͼƬ���Ͻ�����
    POINT m_resultLeftP;    

    CFont m_fntContent;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
    void DrawBG(CDC * pDC);
    
public:
    void SetProcessRange(double curRange, CString strText, CString strDetailTextbool = L"", bool bIsSuccess = FALSE);

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    static UINT StartUpdateThread( LPVOID pParam );

};
