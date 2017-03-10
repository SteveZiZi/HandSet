// SCLEditor.h : main header file for the SCLEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <deque>

// CSCLEditorApp:
// See SCLEditor.cpp for the implementation of this class
//

enum VIEW_TYPE{VIEW_TYPE_SMV = 0, VIEW_TYPE_GOOSE};

class CSCLEditorApp : public CWinApp
{
public:
	CSCLEditorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnSCLImport();
	afx_msg void OnSCXOpen();
	afx_msg void OnSCXSave();
	afx_msg void OnSCXQuickSave();
	afx_msg void OnSwitchToSmvView();
	afx_msg void OnSwitchToGooseView();
	afx_msg void OnFindIED();
	DECLARE_MESSAGE_MAP()

protected:
	CView*  SwitchView(VIEW_TYPE type);

private:
	CString   m_csLastSCLOpenDir;
	CString   m_csLastSCXOpenDir;
	CString   m_csLastSaveDir;
	CString   m_csDefSaveFileName;
	CView*    m_pSmvView;
	CView*    m_pGooseView;
	CView*    m_pCurView;
	VIEW_TYPE m_eActiveViewType;
	std::deque<CString> m_dqFileNameHistory;
};

extern CSCLEditorApp theApp;