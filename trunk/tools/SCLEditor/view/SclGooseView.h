#pragma once

#include "../scl/SclManager.h"

#define WM_UPDATE_GOOSE_REPORT WM_USER+200

class CSCLEditorDoc;

class CSclGooseView : public CXTPReportView
{	
	DECLARE_DYNCREATE(CSclGooseView)

public:
	CSclGooseView();
	virtual ~CSclGooseView();
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
	void AddGooseSubChannelRecord(
		int channelLevel,
		const SCDGOOSEANALYSIS_IEDGOOSE_INFO &ctrlInfo,
		const SCDGOOSEANALYSIS_IEDGOOSE_DATASET_INFO &parentChannel,
		int subChannelNum,
		CXTPReportRecord *pParentRecord
		);
	void AddSampleRecords(CXTPReportControl &wndReport);

	// Attributes
protected:
	CXTPCommandBarScrollBarCtrl m_wndVScrollBar;
	CXTPCommandBarScrollBarCtrl m_wndHScrollBar;
	BOOL m_bWYSIWYG;
};

#ifndef _DEBUG  // debug version in CSclGooseView.cpp
inline CSCLEditorDoc* CSclGooseView::GetDocument() const
{ return reinterpret_cast<CSCLEditorDoc*>(m_pDocument); }
#endif
