// FpgaUpdate.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CFpgaUpdateApp:
// �йش����ʵ�֣������ FpgaUpdate.cpp
//

class CFpgaUpdateApp : public CWinApp
{
public:
	CFpgaUpdateApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

private:
    bool IsUpgradeFpga();
};

extern CFpgaUpdateApp theApp;
