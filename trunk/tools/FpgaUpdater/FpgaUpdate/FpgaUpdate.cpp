// FpgaUpdate.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FpgaUpdate.h"
#include "FpgaUpdateDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFpgaUpdateApp

///< SD��·��
#define STORAGE_CRAD_PATH L"\\Storage Card"

///< FPGA�̼����·��
#define FPGA_BOOT_FILE	L"\\Storage Card\\fpgaBoot"

///< ��־�ļ�·��
#define FPGA_UPGRADE_FLAG_FILE  L"\\Storage Card\\FpgaBootFlag.cfg"

BEGIN_MESSAGE_MAP(CFpgaUpdateApp, CWinApp)
END_MESSAGE_MAP()


// CFpgaUpdateApp ����
CFpgaUpdateApp::CFpgaUpdateApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFpgaUpdateApp ����
CFpgaUpdateApp theApp;

// CFpgaUpdateApp ��ʼ��

BOOL CFpgaUpdateApp::InitInstance()
{

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

    if(!IsUpgradeFpga())
    {
        return FALSE;
    }

    //����������
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

    //ɾ����������ļ�
    DeleteFile(FPGA_UPGRADE_FLAG_FILE);

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
//   return TRUE;
    return FALSE;
}

///
/// @brief
///     �ж��Ƿ�����FPGA
///
/// @return
///     �������TRUE��ʾ��������������
///
bool CFpgaUpdateApp::IsUpgradeFpga()
{
    int nTryTimes = 0;
    bool isExist = FALSE;

    //ѭ��10�������SD���Ƿ����
    while(nTryTimes++ < 10)
    {
        isExist = (::GetFileAttributes(STORAGE_CRAD_PATH) != 0xFFFFFFFF);

        //����Ѿ����SD�����Ƴ�ѭ��
        if(isExist)
        {
            break;
        }

        //���û��⵽��˯��200ms��������
        Sleep(200);
    }

    if(!isExist)
    {
        return FALSE;
    }

    //�ж�FPGA�̼��Լ�������־�ļ��治����
    if((::GetFileAttributes(FPGA_UPGRADE_FLAG_FILE)) == 0xFFFFFFFF
        || (::GetFileAttributes(FPGA_UPGRADE_FLAG_FILE)) == 0xFFFFFFFF)
    {
        return FALSE;
    }

    return TRUE;

}
