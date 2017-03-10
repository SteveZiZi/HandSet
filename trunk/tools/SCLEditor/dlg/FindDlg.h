#pragma once

#include "../SCLEditor.h"
#include "../view/SclSmvView.h"
#include "../view/SclGooseView.h"
#include "afxwin.h"

// CFindDlg 对话框

class CFindDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindDlg)

public:
	CFindDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindDlg();

// 对话框数据
	enum { IDD = IDD_FIND_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFind();

	void SetActiveView(CView *pView, VIEW_TYPE eViewType);

private:
	int       m_iRecordIdx;
	CString   m_strIEDName;
	CView    *m_pActiveView;
	VIEW_TYPE m_eActiveViewType;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnChangeIedName();
	afx_msg void OnPaint();
	CEdit m_editFind;
};
