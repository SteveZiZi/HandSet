// SCLAnalyzer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSCLAnalyzerApp:
// �йش����ʵ�֣������ SCLAnalyzer.cpp
//

class CSCLAnalyzerApp : public CWinApp
{
public:
	CSCLAnalyzerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSCLAnalyzerApp theApp;