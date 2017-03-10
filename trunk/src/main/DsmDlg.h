// DsmDlg.h : 头文件
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






// CDsmDlg 对话框
class CDsmDlg : public CDialog
{
// 构造
public:
	CDsmDlg(CWnd* pParent = NULL);	// 标准构造函数
    virtual ~CDsmDlg();

// 对话框数据
	enum { IDD = IDD_DSM_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
     *      设置标题
     */
    inline void _SetTitle(const CString& csTitle)
    {
        m_ctlTopbar._SetTitle(csTitle);
    }

    /**
     *      设置标题栏无效区域
     */
    inline void _InvalidateStatusRect(CRect rc, BOOL bRepaint = TRUE)
    {
        m_ctlTopbar.InvalidateRect(rc, bRepaint);
        m_ctlTopbar.UpdateWindow();
    }

    /**
     *      更新底部状态栏
     */
    inline void _UpdateBottomBar()
    {
        m_ctlBottom.Invalidate();
        m_ctlBottom.UpdateWindow();
    }


private:

    /**
     *      初始化导航栏
     */
    void _initNavibar();


    /**
     *      创建顶部和底部状态控件
     */
    void _createUICtl();

    /**
     *      设置FPGA时间
     */
    void _setFPGATime();

    /**
     *      使用FPGA时间同步本地时间
     */
    void _setLocalTime();

    /**
     *      更新界面状态
     */
    void _updateUI();

    /**
     *      检查SCL文件是否存在
     */
    bool _checkSCL(const CString csScl, CString& csSCLPath);
#if !SHENG_ZE
private:
	void EnumActiveStorage(BOOL bEnumAll = TRUE);
#endif


private:

    UINT_PTR            m_nTimerStatus;             ///< 更新bottombar定时器标识
    UINT_PTR            m_nTimerSaveCfg;            ///< 定时判断是否保存配置文件
    UINT_PTR            m_nTimerFPGA;               ///< 定时同步时间

    CBitmap             m_bitmapDefNavi;            ///< 默认导航栏item图片
    CNaviToolBar        m_ctlNaviBar;               ///< 底部导航栏


    CNaviMenu           m_menuMain;                 ///< 底部导航菜单

    CEventProc*         m_pEventProc;               ///< 处理按键消息类指针

    CCtlTopBar          m_ctlTopbar;                ///< 顶部状态栏控件
    CCtlBottomBar       m_ctlBottom;                ///< 底部状态栏控件

#if !SHENG_ZE
	STORAGE_KEY_INFO_t  m_tUsbStorageInfo;
	STORAGE_KEY_INFO_t  m_tSDStorageInfo;
#endif
};
