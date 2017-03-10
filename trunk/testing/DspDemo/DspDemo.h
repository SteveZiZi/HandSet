// DspRMSDemo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDspRMSDemoApp:
// 有关此类的实现，请参阅 DspRMSDemo.cpp
//

#define IOCTL_HAL_GET_DSP_INFO                CTL_CODE(FILE_DEVICE_HAL, 3101, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROCESSOR_INFORMATION           CTL_CODE(FILE_DEVICE_HAL, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

class CDspDemoApp : public CWinApp
{
public:
	CDspDemoApp();
	
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CDspDemoApp theApp;
