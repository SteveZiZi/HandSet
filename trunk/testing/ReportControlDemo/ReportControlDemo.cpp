// ReportControlDemo.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ReportControlDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReportControlDemoApp

BEGIN_MESSAGE_MAP(CReportControlDemoApp, CWinApp)
END_MESSAGE_MAP()


// CReportControlDemoApp ����
CReportControlDemoApp::CReportControlDemoApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CReportControlDemoApp ����
CReportControlDemoApp theApp;

// CReportControlDemoApp ��ʼ��

BOOL CReportControlDemoApp::InitInstance()
{
    AfxOleInit();
    AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


#if 0  // ϵͳ�Ѿ����������, �����Լ����ü���
    // �����ź�����
    AddFontResource(L"\\FlashDisk2\\msyh.ttf");
#endif

	CReportControlDemoDlg dlg;
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
