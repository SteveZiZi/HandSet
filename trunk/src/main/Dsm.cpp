// Dsm.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Dsm.h"
#include "DsmDlg.h"

#include "Authorization.h"

#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/utils/utility/usb_function_switch.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDsmApp

BEGIN_MESSAGE_MAP(CDsmApp, CWinApp)
END_MESSAGE_MAP()


// CDsmApp 构造
CDsmApp::CDsmApp()
	: CWinApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CDsmApp 对象
CDsmApp theApp;

// CDsmApp 初始化

BOOL CDsmApp::InitInstance()
{
    AfxEnableControlContainer();

    // 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

    // 设置usb模式为RNDIS，禁止同步连接、大容量存储功能
//     if(!SetClientMode(USB_Serial))
//     {
//         DSM_DEBUG(L"dsm: set usb rndis mode failed\n");
//     }
#if 0
    if (!IsAuthorized()) {
        ::MessageBox(NULL, _T("*********************"), _T("*****"), MB_OK);
        return FALSE;
    }
#endif


    HANDLE hSema = CreateSemaphore(NULL, 1, 1, DSM_INST_SEM);
    if(ERROR_ALREADY_EXISTS == GetLastError())
    {
        HWND hDsmWnd = ::GetWindow(::GetDesktopWindow(), GW_HWNDFIRST); 

        while(::IsWindow(hDsmWnd)) 
        {
            if(::GetProp(hDsmWnd, DSM_PROP_VAL))
            {
                break;
            }

            hDsmWnd = ::GetWindow(hDsmWnd, GW_HWNDNEXT);
        }

        if(hDsmWnd)
        {
            ::SetForegroundWindow(hDsmWnd); 
        }
        
        CloseHandle(hSema);
        return FALSE;
    }

    // 检查SD的目录是否存在，不存在将创建
    _initFolder();

    // 加载配置文件
    if(!dsm_config::Load())
    {
        RETAILMSG(true, (L"dsm: failed to load config.xml.exit app.\n"));
        return FALSE;
    }

    dsm_fontmgr::GetInstance()->SetStyle(FONT_DEFAULT_STYLE);

    // 根据区域ID加载字符串资源
    dsm_stringMgr::Load(dsm_config::GetLCID());

    // 加载图片资源
    dsm_bmpmgr::Load(dsm_config::GetLCID());

    // 修复系统默认参数
    CStatusMgr::FixDefault();

    // 设置背光时间与亮度
    CStatusMgr::SetBrightness(CStatusMgr::GetBrightness());
    CStatusMgr::SetBrightTime(CStatusMgr::GetBrightTime());

	CDsmDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}

    dsm_fontmgr::GetInstance()->Clear();

    CloseHandle(hSema);

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

void CDsmApp::_initFolder()
{
    CString csSDPath;
    if(!DSM_GetSDDriver(csSDPath))
    {
        TRACE(L"dsm: failed to get sd folder.\n");
        return;
    }

    // 创建SCL目录
    if(!DSM_CreateSDPath(csSDPath + DSM_CFG_SCL_FILE_FOLDER))
    {
        TRACE(L"dsm: failed to create scl folder.\n");
    }

#if SHENG_ZE
    // 创建截图目录
    if(!DSM_CreateSDPath(csSDPath + DSM_SNAPSHOT_FOLDER))
    {
        TRACE(L"dsm: failed to create snapshot folder.\n");
    }
#endif
}

