// WebControlDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CWebControlDemoApp:
// �йش����ʵ�֣������ WebControlDemo.cpp
//

class CWebControlDemoApp : public CWinApp
{
public:
	CWebControlDemoApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWebControlDemoApp theApp;
