// DT6000_FiberTestTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDT6000_FiberTestToolApp:
// �йش����ʵ�֣������ DT6000_FiberTestTool.cpp
//

class CDT6000_FiberTestToolApp : public CWinApp
{
public:
	CDT6000_FiberTestToolApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDT6000_FiberTestToolApp theApp;
