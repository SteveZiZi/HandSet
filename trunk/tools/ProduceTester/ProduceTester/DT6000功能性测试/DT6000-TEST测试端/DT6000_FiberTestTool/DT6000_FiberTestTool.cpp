// DT6000_FiberTestTool.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DT6000_FiberTestTool.h"
#include "DT6000_FiberTestToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDT6000_FiberTestToolApp

BEGIN_MESSAGE_MAP(CDT6000_FiberTestToolApp, CWinApp)
END_MESSAGE_MAP()


// CDT6000_FiberTestToolApp ����
CDT6000_FiberTestToolApp::CDT6000_FiberTestToolApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDT6000_FiberTestToolApp ����
CDT6000_FiberTestToolApp theApp;

// CDT6000_FiberTestToolApp ��ʼ��

BOOL CDT6000_FiberTestToolApp::InitInstance()
{

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CDT6000_FiberTestToolDlg dlg;
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
