// SCLAnalysisDLLTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CSCLAnalysisDLLTestApp:
// �йش����ʵ�֣������ SCLAnalysisDLLTest.cpp
//

class CSCLAnalysisDLLTestApp : public CWinApp
{
public:
	CSCLAnalysisDLLTestApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSCLAnalysisDLLTestApp theApp;
