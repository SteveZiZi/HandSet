// ReliabilityTestNoEth.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CReliabilityTestNoEthApp:
// See ReliabilityTestNoEth.cpp for the implementation of this class
//

class CReliabilityTestNoEthApp : public CWinApp
{
public:
	CReliabilityTestNoEthApp();
	
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CReliabilityTestNoEthApp theApp;
