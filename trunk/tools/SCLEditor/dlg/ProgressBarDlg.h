#pragma once

#include "../SCLEditor.h"

typedef DWORD (WINAPI *DSMProgressTask)(void* arg);

// CProgressBarDlg 对话框

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressBarDlg();

// 对话框数据
	enum { IDD = IDD_PROGRESS_BAR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();

protected:
    virtual void OnOK();
    virtual void OnCancel();

public:
	inline void _SetTask(DSMProgressTask pTask, LPVOID arg)
	{
		m_pTask = pTask;
		m_pArgs = arg;
	}

    inline void _SetTips(CString csTips)
    {
        m_csTips = csTips;
    }

    inline void _SetTextColor(COLORREF color)
    {
        m_colorText = color;
    }

    inline void _SetBkColor(COLORREF color)
    {
        m_colorBk = color;
    }

    inline void _SetFont(CFont* pFont)
    {
        m_pFont = pFont;
    }



private:
	DSMProgressTask     m_pTask;            ///< 线程回调函数

    UINT                m_nTimerID;         ///< 刷新界面的定时器ID
    CString             m_csTips;           ///< 界面提示字符串
    
    LPVOID              m_pArgs;            ///< 线程参数
    HANDLE              m_hThread;          ///< 工作线程

    CFont*              m_pFont;            ///< 字体
    COLORREF            m_colorBk;          ///< 背景色
    COLORREF            m_colorText;        ///< 字体颜色

#ifdef PROGRESS_BAR_ENABLE
	CRect               m_rtProgressBar;         ///< 自绘进度条控件的进度条的矩形域
	CPoint              m_ptProgressBarBorder[5];///< 自绘进度条控件的外框的端点
#endif
};
