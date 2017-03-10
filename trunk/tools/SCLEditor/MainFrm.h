// MainFrm.h : interface of the CMainFrame class
//


#pragma once

class CMainFrame : public CXTPFrameWnd
{

protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void SetStatusBarText(UINT uStrID);

protected:  // control bar embedded members
	CXTPStatusBar  m_wndStatusBar;
	CXTPDockingPaneManager m_paneManager;
	CImageList m_toolbarIcons;
	CXTPOfficeBorder<CStatic> m_wndOptions;
	CXTPOfficeBorder<CEdit> m_wndProperties;

	BOOL CreateToolBarButtons(CXTPToolBar *pToolBar);
	BOOL AddButton(CRuntimeClass *pRuntimeClass, CXTPControls *pToolBarCtrls, int nID, CString szPath, CString szTips, BOOL bBeginGroup = FALSE);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnCustomize();
	afx_msg LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateStatusBarText(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};
