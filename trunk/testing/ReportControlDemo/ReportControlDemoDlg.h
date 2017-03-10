// ReportControlDemoDlg.h : ͷ�ļ�
//

#pragma once


#include "src/utils/ReportControl/Template/ListBoxExt.h"
#include "src/utils/ReportControl/Template/ListCtrlExt.h"
#include "src/applet/keyboardaux/app/AppInfoIPC.h"
#include "src/include/p_wndusermsg_def.h"


// CReportControlDemoDlg �Ի���
class CReportControlDemoDlg : public CDialog
{
    CTreeBoxT_VLine              m_ctlTree;
    CListBoxT_3dLine              m_ctlBox;
    CFont                   m_bodyFont;

    CBitmap                 m_bmp1;
    CBitmap                 m_bmp2;
    CBitmap                 m_bmp3;
    CBitmap                 m_bmp4;


// ����
public:
	CReportControlDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REPORTCONTROLDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()


public:
    afx_msg void OnBnClickedBtnTest1();
    afx_msg void OnBnClickedBtnTest2();
    afx_msg LRESULT  OnUpdateInputMode (WPARAM, LPARAM);
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();

    void OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult);
    afx_msg void OnBnClickedButton3();
};
