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
    // //显示当前音乐的时间
    CString m_strAudioTime;
    // //显示当前的音量
    CProgressCtrl m_ctrlVolume;

    BOOL    m_bCloseNormal;             //窗口是否被正常关闭


public:   
    CConnect     *pConnect;
    UINT16       uiParamCmd;
    UINT16       uiCmd;                 //PACKET用到的命令码

    TEST_STATUS  status;

    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
};
