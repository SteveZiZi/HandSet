// DspRMSDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CDspRMSDemoApp:
// �йش����ʵ�֣������ DspRMSDemo.cpp
//

#define IOCTL_HAL_GET_DSP_INFO                CTL_CODE(FILE_DEVICE_HAL, 3101, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROCESSOR_INFORMATION           CTL_CODE(FILE_DEVICE_HAL, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)

class CDspDemoApp : public CWinApp
{
public:
	CDspDemoApp();
	
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDspDemoApp theApp;
