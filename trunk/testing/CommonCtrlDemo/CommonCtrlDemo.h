// CommonCtrlDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CCommonCtrlDemoApp:
// �йش����ʵ�֣������ CommonCtrlDemo.cpp
//

class CCommonCtrlDemoApp : public CWinApp
{
public:
	CCommonCtrlDemoApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCommonCtrlDemoApp theApp;
