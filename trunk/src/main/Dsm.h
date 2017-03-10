// Dsm.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDsmApp:
// 有关此类的实现，请参阅 Dsm.cpp
//

#define DSM_PROP_VAL        L"3FA0CA0F183120C91131AB7F480FFB90"
#define DSM_INST_SEM        L"C960301A2F8AB821684CFA56FEACE900"

class CDsmApp : public CWinApp
{
public:
	CDsmApp();
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

private:

    /**
     *      检查SD的目录是否存在，不存在将创建需要的目录
     */
    void _initFolder();
};

extern CDsmApp theApp;
