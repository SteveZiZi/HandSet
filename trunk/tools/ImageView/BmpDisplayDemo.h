// BmpDisplayDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CBmpDisplayDemoApp:
// �йش����ʵ�֣������ BmpDisplayDemo.cpp
//

class CBmpDisplayDemoApp : public CWinApp
{
public:
	CBmpDisplayDemoApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBmpDisplayDemoApp theApp;
