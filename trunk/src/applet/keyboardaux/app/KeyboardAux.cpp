// KeyboardAux.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "KeyboardAux.h"
#include "KeyboardAuxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKeyboardAuxApp

BEGIN_MESSAGE_MAP(CKeyboardAuxApp, CWinApp)
END_MESSAGE_MAP()


// CKeyboardAuxApp ����
CKeyboardAuxApp::CKeyboardAuxApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CKeyboardAuxApp ����
CKeyboardAuxApp theApp;

// CKeyboardAuxApp ��ʼ��

BOOL CKeyboardAuxApp::InitInstance()
{
    // ʹ�ó���ֻ������һ��
    if (CreateEvent(NULL, FALSE, FALSE, L"CKeyboardAuxApp"))
    {
        if(ERROR_ALREADY_EXISTS == GetLastError())
        {
            KB_DEBUGLOG(L"\r\nCKeyboardAuxApp running...\r\n");
            return FALSE;
        }
    }


	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CKeyboardAuxDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
