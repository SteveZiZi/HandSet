// DT6000_FiberTestToolDlg.h : 头文件
//

#pragma once


#include ".\src\gpmcfg.h"
#include ".\src\fiber.h"

#define DEVICE_FIBER_NUM 3

// CDT6000_FiberTestToolDlg 对话框
class CDT6000_FiberTestToolDlg : public CDialog
{
// 构造
public:
	CDT6000_FiberTestToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DT6000_FiberTestTool_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

    BOOL DeviceInit();
    void DeviceDeinit();
    int GetExeDirectory(WCHAR *strPath, int len);

    void DataInit();


    afx_msg LRESULT OnChangeTx(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRecoverTx(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnStopTx(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSendBcode(WPARAM wParam, LPARAM lParam);
    afx_msg void OnDestroy();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()

private:
    HANDLE m_hDevice;

    _FT3_c *m_pFt3Tx;
    _FT3_c *m_pFt3Rx1;
    _FT3_c *m_pFt3Rx2;

    _SMV_92_c *m_pFiber[DEVICE_FIBER_NUM];
    /*_SMV_92_c *m_pFiberB;
    _SMV_92_c *m_pFiberC;*/

    IOCTRL_FIBER_IN_PARAM_t m_tFiberParam;

    Ft3SendCtlWord_t m_tCtrlW;

    //波特率- 1:2.5M  2:5M
    UINT16 m_uBaudRate;

    //采样率- 1:4K  2:12.8K
    UINT16 m_uSampleRate;

    //报文格式-1：南瑞 2：国网
    UINT16 m_uFrameStyle;

    //是否正在发送B码
    bool   m_bSendBcode;

    WCHAR       m_strExtFilePath[MAX_PATH*2];

public:
    afx_msg void OnBnClickedButtonStartSev();
    afx_msg void OnBnClickedButtonStopSev();
};
