// SCLAnalysisDLLTest.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SCLAnalysisDLLTest.h"
#include "SCLAnalysisDLLTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSCLAnalysisDLLTestApp

BEGIN_MESSAGE_MAP(CSCLAnalysisDLLTestApp, CWinApp)
END_MESSAGE_MAP()


// CSCLAnalysisDLLTestApp ����
CSCLAnalysisDLLTestApp::CSCLAnalysisDLLTestApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSCLAnalysisDLLTestApp ����
CSCLAnalysisDLLTestApp theApp;

// CSCLAnalysisDLLTestApp ��ʼ��

BOOL CSCLAnalysisDLLTestApp::InitInstance()
{

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CSCLAnalysisDLLTestDlg dlg;
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
