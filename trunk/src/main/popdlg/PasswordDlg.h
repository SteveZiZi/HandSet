#pragma once

#include "src/main/resource.h"
#include "src/main/control/editpassword.h"

typedef bool (* PASSWORDPROC)(CString, DWORD);

// CPasswordDlg 对话框

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPasswordDlg();

// 对话框数据
	enum { IDD = IDD_POPUP_PASSWORD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void OnOK();

public:

    /**
     *      设置工作函数
     */
    void _SetProc(PASSWORDPROC pProc, DWORD param)
    {
        m_pProc     = pProc;
        m_dwParam   = param;
    }

private:

    PASSWORDPROC            m_pProc;                ///< 工作函数
    DWORD                   m_dwParam;              ///< 函数参数

    CRect                   m_rcTitle;              ///< 标题

    CRect                   m_rcClient;             ///< 页面区域

    CEditPassword           m_editPassword;         ///< 输入控件

    CWnd*                   m_pWndPrev;             ///< 之前窗口指针

};
