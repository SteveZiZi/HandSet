// FpgaUpdate.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "FpgaUpdate.h"
#include "FpgaUpdateDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFpgaUpdateApp

///< SD卡路径
#define STORAGE_CRAD_PATH L"\\Storage Card"

///< FPGA固件存放路径
#define FPGA_BOOT_FILE	L"\\Storage Card\\fpgaBoot"

///< 标志文件路径
#define FPGA_UPGRADE_FLAG_FILE  L"\\Storage Card\\FpgaBootFlag.cfg"

BEGIN_MESSAGE_MAP(CFpgaUpdateApp, CWinApp)
END_MESSAGE_MAP()


// CFpgaUpdateApp 构造
CFpgaUpdateApp::CFpgaUpdateApp()
	: CWinApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CFpgaUpdateApp 对象
CFpgaUpdateApp theApp;

// CFpgaUpdateApp 初始化

BOOL CFpgaUpdateApp::InitInstance()
{

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

    if(!IsUpgradeFpga())
    {
        return FALSE;
    }

    //隐藏任务栏
    HWND hWnd = ::FindWindow(L"HHTaskBar",NULL);
    if (NULL != hWnd)
    {
        ::ShowWindow(hWnd,SW_HIDE);
    }
    
    /*CFpgaUpdateDlg * pFpgaUpdateDlg = new CFpgaUpdateDlg();
    pFpgaUpdateDlg->Create(IDD_FPGAUPDATE_DIALOG);
    pFpgaUpdateDlg->ShowWindow(SW_SHOWMAXIMIZED);
    m_pMainWnd = pFpgaUpdateDlg;*/
//    pFpgaUpdateDlg->SetTimer(2, 1000, NULL);

    CFpgaUpdateDlg dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();

    //删除升级标记文件
    DeleteFile(FPGA_UPGRADE_FLAG_FILE);

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
//   return TRUE;
    return FALSE;
}

///
/// @brief
///     判断是否升级FPGA
///
/// @return
///     如果返回TRUE表示升级，否则不升级
///
bool CFpgaUpdateApp::IsUpgradeFpga()
{
    int nTryTimes = 0;
    bool isExist = FALSE;

    //循环10次来检测SD卡是否存在
    while(nTryTimes++ < 10)
    {
        isExist = (::GetFileAttributes(STORAGE_CRAD_PATH) != 0xFFFFFFFF);

        //如果已经检测SD卡则推出循环
        if(isExist)
        {
            break;
        }

        //如果没检测到则睡眠200ms后继续检测
        Sleep(200);
    }

    if(!isExist)
    {
        return FALSE;
    }

    //判断FPGA固件以及升级标志文件存不存在
    if((::GetFileAttributes(FPGA_UPGRADE_FLAG_FILE)) == 0xFFFFFFFF
        || (::GetFileAttributes(FPGA_UPGRADE_FLAG_FILE)) == 0xFFFFFFFF)
    {
        return FALSE;
    }

    return TRUE;

}
