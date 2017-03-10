#pragma once

#define WM_UPDATE_SMV_REPORT WM_USER+100

class CSCLEditorDoc;

class CSclSmvView : public CXTPReportView
{
	DECLARE_DYNCREATE(CSclSmvView)
public:
	CSclSmvView();
	virtual ~CSclSmvView();
	CSCLEditorDoc* GetDocument() const;

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int FindIED(const CString &iedName, const int nIndex);
	int GetCtrlCount();

// Generated message map functions
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnReportColumnRClick(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnReportFocusChanging(NMHDR* pNMHDR, LRESULT* result);
	afx_msg LRESULT OnUpdateReport(WPARAM wParam, LPARAM lParam);
	void UpdateReport();

protected:
	void AddSampleRecords(CXTPReportControl &wndReport);

// Attributes
protected:
	CXTPCommandBarScrollBarCtrl m_wndVScrollBar;
	CXTPCommandBarScrollBarCtrl m_wndHScrollBar;
	BOOL m_bWYSIWYG;
};

#ifndef _DEBUG  // debug version in SclSmvView.cpp
inline CSCLEditorDoc* CSclSmvView::GetDocument() const
   { return reinterpret_cast<CSCLEditorDoc*>(m_pDocument); }
#endif
