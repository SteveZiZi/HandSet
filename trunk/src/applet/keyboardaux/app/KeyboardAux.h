// KeyboardAux.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CKeyboardAuxApp:
// �йش����ʵ�֣������ KeyboardAux.cpp
//

class CKeyboardAuxApp : public CWinApp
{
public:
	CKeyboardAuxApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CKeyboardAuxApp theApp;
