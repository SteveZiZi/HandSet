// SliderBoxDemoDlg.h : ͷ�ļ�
//

#pragma once

#include "src/utils/SliderBoxControl/SliderBox.h"

// CSliderBoxDemoDlg �Ի���
class CSliderBoxDemoDlg : public CDialog
{
// ����
public:
	CSliderBoxDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SLIDERBOXDEMO_DIALOG };


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
    
    CSliderBox m_SliderBox;
};
