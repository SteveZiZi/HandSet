// FunctionTestCE.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CFunctionTestCEApp:
// See FunctionTestCE.cpp for the implementation of this class
//

class CFunctionTestCEApp : public CWinApp
{
public:
	CFunctionTestCEApp();
	
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
    virtual int ExitInstance();
};

extern CFunctionTestCEApp theApp;
