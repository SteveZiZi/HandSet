// CommonCtrlDemoDlg.h : ͷ�ļ�
//

#pragma once

#include "src\utils\CommonControl\TimeEditCtrl\TimeEditCtrl.h"
#include "src\utils\CommonControl\MacEditCtrl\MacEditCtrl.h"

// CCommonCtrlDemoDlg �Ի���
class CCommonCtrlDemoDlg : public CDialog
{
// ����
public:
	CCommonCtrlDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_COMMONCTRLDEMO_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	afx_msg LRESULT OnTimeEditApply(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimeEditCancle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMacEditApply(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMacEditCancle(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeCombo1();

	void TimeEditTest(CPoint point);
	void MacEditTest(CPoint point);

// ʵ��
protected:
	HICON m_hIcon;

	CTimeEditCtrl m_TimeEdit;
	CMacEditCtrl  m_MacEdit;
	bool m_bIsModal;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
private:
	int m_CtrlSelected;
};
