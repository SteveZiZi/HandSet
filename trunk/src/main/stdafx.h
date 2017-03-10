// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
//  �ض�����Ŀ�İ����ļ�

#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// ע�� - ���ֵ����ΪĿ��� Windows CE OS �汾�Ĺ����Բ���ǿ
#define WINVER _WIN32_WCE

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��
#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#include <afxcmn.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif



#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT



#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif


#if (_WIN32_WCE < 0x500) && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_	
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
#endif

#include <altcecrt.h>


#include "src/include/stdint.h"

#include "src/utils/log/dsm_log.h"
#include "src/utils/res/dsm_bmpmgr.h"
#include "src/utils/res/dsm_fontmgr.h"
#include "src/utils/config/dsm_config.h"
#include "src/utils/res/dsm_stringmgr.h"

#include "src/utils/reportcontrol/template/listboxext.h"
#include "src/utils/reportcontrol/template/ListCtrlExt.h"

#include "src/main/record/dsmrecorditem.h"
#include "src/main/record/dsmrecord.h"
#include "src/main/record/dsmrecord_cfg.h"
#include "src/main/record/dsmrecord_recv.h"
#include "src/main/record/dsmrecord_send.h"

#include "src/include/p_wndusermsg_def.h"


#define DSM_STRING2(x) #x
#define DSM_STRING(x) DSM_STRING2(x)

#define SU_FEI		0
#define	CHUN_HUA	0
#define SHENG_ZE    0


