#pragma once
#include "afxcmn.h"


// CAudioTestDlg dialog

class CAudioTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CAudioTestDlg)

public:
	CAudioTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAudioTestDlg();

// Dialog Data
	enum { IDD = IDD_DLG_AUDIO_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    // //��ʾ��ǰ���ֵ�ʱ��
    CString m_strAudioTime;
    // //��ʾ��ǰ������
    CProgressCtrl m_ctrlVolume;

    BOOL    m_bCloseNormal;             //�����Ƿ������ر�


public:   
    CConnect     *pConnect;
    UINT16       uiParamCmd;
    UINT16       uiCmd;                 //PACKET�õ���������

    TEST_STATUS  status;

    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
};
