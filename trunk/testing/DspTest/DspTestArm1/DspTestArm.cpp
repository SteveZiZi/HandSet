// DspTestArm.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "DspTestArm.h"
#include "DspTestArmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDspTestArmApp

BEGIN_MESSAGE_MAP(CDspTestArmApp, CWinApp)
END_MESSAGE_MAP()


// CDspTestArmApp ����
CDspTestArmApp::CDspTestArmApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CDspTestArmApp ����
CDspTestArmApp theApp;

// CDspTestArmApp ��ʼ��

BOOL CDspTestArmApp::InitInstance()
{

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CDspTestArmDlg dlg;
//	m_pMainWnd = &dlg;
//	INT_PTR nResponse = dlg.DoModal();
	if (dlg.DoModal() == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
