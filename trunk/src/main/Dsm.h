// Dsm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDsmApp:
// �йش����ʵ�֣������ Dsm.cpp
//

#define DSM_PROP_VAL        L"3FA0CA0F183120C91131AB7F480FFB90"
#define DSM_INST_SEM        L"C960301A2F8AB821684CFA56FEACE900"

class CDsmApp : public CWinApp
{
public:
	CDsmApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

private:

    /**
     *      ���SD��Ŀ¼�Ƿ���ڣ������ڽ�������Ҫ��Ŀ¼
     */
    void _initFolder();
};

extern CDsmApp theApp;
