#pragma once

#include "src/main/resource.h"


class CRNaviMenu;

// CLockDlg 对话框

class CLockDlg : public CDialog
{
	DECLARE_DYNAMIC(CLockDlg)

public:
	CLockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLockDlg();

// 对话框数据
	enum { IDD = IDD_POPUP_LOCK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void OnOK() {}
    virtual void OnCancel() {}

private:

    UINT                m_nLockTimer;       ///< 锁屏时，隐藏窗口的定时器
    UINT                m_nUnlockTimer;     ///< 解锁时，显示解锁窗口停留的定时器
    bool                m_bLock;            ///< 锁屏true or 解锁false
    CRect               m_rcClient;         ///< 客户区大小
    CRNaviMenu*         m_pRNaviMenu;       ///< 菜单资源管理对象
};
