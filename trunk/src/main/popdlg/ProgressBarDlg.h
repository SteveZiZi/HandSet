#pragma once

#include "src/main/resource.h"
#include "src/main/control/statusdisp.h"

class CRNaviMenu;
class CNaviToolBar;

typedef DWORD (WINAPI *DSMProgressFnTask)(WPARAM wParam, LPARAM lParam, void* arg);

typedef DWORD (WINAPI *DSMProgressTask)(void* arg);

// CProgressBarDlg 对话框

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressBarDlg();

// 对话框数据
	enum { IDD = IDD_POPUP_PROGRESS_BAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) { return TRUE; }
    afx_msg void OnPaint();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


protected:
    virtual void OnOK() {}
    virtual void OnCancel() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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

    inline void _SetTitle(CString csTitle)
    {
        m_csTitle = csTitle;
    }

    inline void _SetPicRC(CRect rc)
    {
        m_rcPic = rc;
    }

    inline CRect _GetPicRC()
    {
        return m_rcPic;
    }

    inline void _SetTextRC(CRect rc)
    {
        m_rcText = rc;
    }

    inline CRect _GetTextRC()
    {
        return m_rcText;
    }

    /**
     *      设置菜单
     */
    void _SetMenu(UINT menu)
    {
        m_nMenuID = menu;
    }

    /**
     *      设置菜单ID范围
     */
    void _SetCommandRange(UINT begin, UINT end, DSMProgressFnTask func, void* data = NULL)
    {
        m_nBegin    = begin;
        m_nEnd      = end;
        m_pFunc     = func;
        m_pData     = data;
    }

private:

    UINT                m_nMenuID;          ///< 菜单ID

    UINT                m_nBegin;
    UINT                m_nEnd;
    DSMProgressFnTask   m_pFunc;            ///< Fn响应函数
    void*               m_pData;

    DSMProgressTask     m_pTask;            ///< 线程回调函数

    CRect               m_rcText;           ///< 提示字符串区域
    CString             m_csTips;           ///< 界面提示字符串

    CString             m_csTitlePrev;      ///< 之前标题
    CString             m_csTitle;          ///< 进度条标题

    LPVOID              m_pArgs;            ///< 线程参数
    HANDLE              m_hThread;          ///< 工作线程

    CRect               m_rcPic;            ///< 图片区域
    CStatusDisp         m_ctlPic;           ///< 图片控件

    CRNaviMenu*         m_pRNaviMenu;       ///< 菜单资源管理对象
    CNaviToolBar*       m_pNaviBar;         ///< 菜单对象
};
