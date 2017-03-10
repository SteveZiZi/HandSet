// DsmDlg.h : ͷ�ļ�
//

#pragma once

#include "EventProc.h"

#include "src/utils/navicontrol/navimenubar/navitoolbar.h"
#include "src/include/p_filemapping_def.h"

#include "src/main/control/ctltopbar.h"
#include "src/main/control/ctlbottombar.h"
#if !SHENG_ZE
#include <devload.h>
typedef struct {
	BOOL  bActive;
	TCHAR szDriverName[DEVNAME_LEN];
}STORAGE_KEY_INFO_t;
#endif






// CDsmDlg �Ի���
class CDsmDlg : public CDialog
{
// ����
public:
	CDsmDlg(CWnd* pParent = NULL);	// ��׼���캯��
    virtual ~CDsmDlg();

// �Ի�������
	enum { IDD = IDD_DSM_DIALOG };


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
    virtual BOOL DestroyWindow();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg LRESULT OnUpdateInputMode(WPARAM, LPARAM);
    afx_msg LRESULT OnLoadSCL(WPARAM, LPARAM);
    afx_msg LRESULT OnSyncTime(WPARAM, LPARAM);
#if !SU_FEI
	afx_msg void OnPaint();
#endif
#if !SHENG_ZE
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
#endif
protected:
    virtual void OnOK() {}
    virtual void OnCancel() {}
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    /**
     *      ���ñ���
     */
    inline void _SetTitle(const CString& csTitle)
    {
        m_ctlTopbar._SetTitle(csTitle);
    }

    /**
     *      ���ñ�������Ч����
     */
    inline void _InvalidateStatusRect(CRect rc, BOOL bRepaint = TRUE)
    {
        m_ctlTopbar.InvalidateRect(rc, bRepaint);
        m_ctlTopbar.UpdateWindow();
    }

    /**
     *      ���µײ�״̬��
     */
    inline void _UpdateBottomBar()
    {
        m_ctlBottom.Invalidate();
        m_ctlBottom.UpdateWindow();
    }


private:

    /**
     *      ��ʼ��������
     */
    void _initNavibar();


    /**
     *      ���������͵ײ�״̬�ؼ�
     */
    void _createUICtl();

    /**
     *      ����FPGAʱ��
     */
    void _setFPGATime();

    /**
     *      ʹ��FPGAʱ��ͬ������ʱ��
     */
    void _setLocalTime();

    /**
     *      ���½���״̬
     */
    void _updateUI();

    /**
     *      ���SCL�ļ��Ƿ����
     */
    bool _checkSCL(const CString csScl, CString& csSCLPath);
#if !SHENG_ZE
private:
	void EnumActiveStorage(BOOL bEnumAll = TRUE);
#endif


private:

    UINT_PTR            m_nTimerStatus;             ///< ����bottombar��ʱ����ʶ
    UINT_PTR            m_nTimerSaveCfg;            ///< ��ʱ�ж��Ƿ񱣴������ļ�
    UINT_PTR            m_nTimerFPGA;               ///< ��ʱͬ��ʱ��

    CBitmap             m_bitmapDefNavi;            ///< Ĭ�ϵ�����itemͼƬ
    CNaviToolBar        m_ctlNaviBar;               ///< �ײ�������


    CNaviMenu           m_menuMain;                 ///< �ײ������˵�

    CEventProc*         m_pEventProc;               ///< ��������Ϣ��ָ��

    CCtlTopBar          m_ctlTopbar;                ///< ����״̬���ؼ�
    CCtlBottomBar       m_ctlBottom;                ///< �ײ�״̬���ؼ�

#if !SHENG_ZE
	STORAGE_KEY_INFO_t  m_tUsbStorageInfo;
	STORAGE_KEY_INFO_t  m_tSDStorageInfo;
#endif
};
