// Dsm.cpp : ����Ӧ�ó��������Ϊ��
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


// CDsmApp ����
CDsmApp::CDsmApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDsmApp ����
CDsmApp theApp;

// CDsmApp ��ʼ��

BOOL CDsmApp::InitInstance()
{
    AfxEnableControlContainer();

    // ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

    // ����usbģʽΪRNDIS����ֹͬ�����ӡ��������洢����
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

    // ���SD��Ŀ¼�Ƿ���ڣ������ڽ�����
    _initFolder();

    // ���������ļ�
    if(!dsm_config::Load())
    {
        RETAILMSG(true, (L"dsm: failed to load config.xml.exit app.\n"));
        return FALSE;
    }

    dsm_fontmgr::GetInstance()->SetStyle(FONT_DEFAULT_STYLE);

    // ��������ID�����ַ�����Դ
    dsm_stringMgr::Load(dsm_config::GetLCID());

    // ����ͼƬ��Դ
    dsm_bmpmgr::Load(dsm_config::GetLCID());

    // �޸�ϵͳĬ�ϲ���
    CStatusMgr::FixDefault();

    // ���ñ���ʱ��������
    CStatusMgr::SetBrightness(CStatusMgr::GetBrightness());
    CStatusMgr::SetBrightTime(CStatusMgr::GetBrightTime());

	CDsmDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}

    dsm_fontmgr::GetInstance()->Clear();

    CloseHandle(hSema);

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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

    // ����SCLĿ¼
    if(!DSM_CreateSDPath(csSDPath + DSM_CFG_SCL_FILE_FOLDER))
    {
        TRACE(L"dsm: failed to create scl folder.\n");
    }

#if SHENG_ZE
    // ������ͼĿ¼
    if(!DSM_CreateSDPath(csSDPath + DSM_SNAPSHOT_FOLDER))
    {
        TRACE(L"dsm: failed to create snapshot folder.\n");
    }
#endif
}

