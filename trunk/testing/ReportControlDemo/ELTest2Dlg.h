#pragma once


#include "src/utils/ReportControl/Template/ListCtrlExt.h"

// CELTest2Dlg 对话框

class CELTest2Dlg : public CDialog
{
	DECLARE_DYNAMIC(CELTest2Dlg)

    CListCtrlT_VLineMidH        m_ctlListEL;
    CFont                       m_font;

    CListCtrlT_AboutCtrl        m_aboutCtrl;

public:
	CELTest2Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CELTest2Dlg();

// 对话框数据
	enum { IDD = IDD_ELTEST2_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
