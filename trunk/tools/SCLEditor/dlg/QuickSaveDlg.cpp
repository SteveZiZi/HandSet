// D:\DSM800\trunk\tools\SCLEditor\dlg\QuickSaveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "..\SCLEditor.h"
#include "QuickSaveDlg.h"
#include "..\ResourceStr.h"
#include "..\base\StrManager.h"

#define STR_DEFAULT_DIRECTORY _T("%sSCL")
#define STR_DEFAULT_FILENAME  _T("%sSCL\\%s.scx")

// CQuickSaveDlg �Ի���

IMPLEMENT_DYNAMIC(CQuickSaveDlg, CDialog)

CQuickSaveDlg::CQuickSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuickSaveDlg::IDD, pParent)
	, m_csDrive(_T(""))
	, m_csFileName(_T(""))
{
	m_vFileNameOpts.reserve(5);
}

CQuickSaveDlg::~CQuickSaveDlg()
{
}

void CQuickSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAVE_DRIVE, m_cSaveDrive);
	DDX_Control(pDX, IDC_SAVE_FILE_NAME, m_cSaveFileName);
}


BEGIN_MESSAGE_MAP(CQuickSaveDlg, CDialog)
	ON_BN_CLICKED(IDC_SAVE, &CQuickSaveDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL, &CQuickSaveDlg::OnBnClickedCancel)
	ON_CBN_DROPDOWN(IDC_SAVE_DRIVE, &CQuickSaveDlg::OnCbnDropdownSaveDrive)
END_MESSAGE_MAP()


// CQuickSaveDlg ��Ϣ�������

BOOL CQuickSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	// ����̷���Ϣ��ֻ�����ƶ��洢�豸
	size_t szAllDriveStrings = GetLogicalDriveStrings(0,NULL);
	TCHAR *pDriveStrings     = new TCHAR[szAllDriveStrings + sizeof(_T( " "))];
	GetLogicalDriveStrings(szAllDriveStrings,pDriveStrings);
	for ( TCHAR *s=pDriveStrings; *s != NULL; s+=_tcslen(s)+1 )
	{
		UINT nDiskType = GetDriveType(s);
		if ( nDiskType != DRIVE_REMOVABLE )
			continue;
		m_cSaveDrive.AddString(s);
	}
	int i = m_cSaveDrive.GetCount();
	m_cSaveDrive.SetCurSel(0);

	// ����ļ�����ʷ��¼������Ĭ���ļ���
	for ( size_t i = 0; i < m_vFileNameOpts.size(); i++ )
	{
		m_cSaveFileName.AddString(m_vFileNameOpts[i].GetString());
	}
	m_cSaveFileName.SetWindowText(m_csFileName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CQuickSaveDlg::OnBnClickedSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_cSaveDrive.GetWindowText(m_csDrive);
	m_cSaveFileName.GetWindowText(m_csFileName);

	// �̷�Ϊ�գ�����ʾ�����ƶ��洢�豸
	if ( m_csDrive.IsEmpty() )
	{
		AfxMessageBox(CStrManager::Get(RCSTR_INSERT_REMOVABLE_DRIVE),MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// �Ƴ��ļ�������ǰ���' '�ַ�
	while ( m_csFileName.GetAt(0) == _T(' ') )
	{
		m_csFileName.Delete(0);
	}
	if ( m_csFileName.GetLength() == 0 )
		return;

	// �ж��ļ��Ƿ���ڣ���������ʾ�Ƿ��滻
	CString str;
	str.Format(STR_DEFAULT_FILENAME,m_csDrive,m_csFileName);
	if ( IsFileExist(str) == TRUE )
	{
		CString tip;
		tip.Format(_T("%s%s\r\n%s"),str,CStrManager::Get(RCSTR_FILE_EXISTED_TIP),CStrManager::Get(RCSTR_FILE_REPLACE_TIP));
		int ret = AfxMessageBox(tip,MB_YESNO|MB_ICONEXCLAMATION);
		if ( ret != IDYES )
			return;
	}

	// �ж��ļ�Ŀ¼�Ƿ���ڣ��������򴴽�
	str.Format(STR_DEFAULT_DIRECTORY,m_csDrive);
	if ( IsDirExist(str) == FALSE )
	{
		CreateDir(str);
	}

	OnOK();
}

void CQuickSaveDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CQuickSaveDlg::OnCbnDropdownSaveDrive()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for ( int i = m_cSaveDrive.GetCount()-1; i >= 0; i-- )
	{
		m_cSaveDrive.DeleteString(i);
	}

	// ����̷���Ϣ��ֻ�����ƶ��洢�豸
	size_t szAllDriveStrings = GetLogicalDriveStrings(0,NULL);
	TCHAR *pDriveStrings     = new TCHAR[szAllDriveStrings + sizeof(_T( " "))];
	GetLogicalDriveStrings(szAllDriveStrings,pDriveStrings);
	for ( TCHAR *s=pDriveStrings; *s != NULL; s+=_tcslen(s)+1 )
	{
		UINT nDiskType = GetDriveType(s);
		if ( nDiskType != DRIVE_REMOVABLE )
			continue;
		m_cSaveDrive.AddString(s);
	}
	int i = m_cSaveDrive.GetCount();
	m_cSaveDrive.SetCurSel(0);
}

///
/// @brief
///     ����Ĭ���ļ���
///
/// @param[in]
///     fileName - Ĭ���ļ���
///
void CQuickSaveDlg::SetDefaultFileName(const CString &fileName)
{
	m_csFileName = fileName;
}

///
/// @brief
///     ����ļ�����ʷ�������5��
///
/// @param[in]
///     fileName - Ҫ��ӵ��ļ���
///
void CQuickSaveDlg::AddFileNameHistory(const CString &fileName)
{
	m_vFileNameOpts.push_back(fileName);
}

///
/// @brief
///     ��ȡ�ļ���
///
/// @return
///     �ļ���
///
CString CQuickSaveDlg::GetFileName()
{
	return m_csFileName;
}

///
/// @brief
///     ��ȡ�ļ�·��
///
/// @return
///     �ļ�·��
///
CString CQuickSaveDlg::GetFilePath()
{
	CString filePath;
	filePath.Format(STR_DEFAULT_FILENAME,m_csDrive,m_csFileName);
	return filePath;
}

///
/// @brief
///     ̽��ָ���ļ��Ƿ����
///
/// @param[in]
///     csPath - ��̽����ļ�·��
///
/// @return
///     ���ڷ���TRUE�����򷵻�FALSE
///
BOOL CQuickSaveDlg::IsFileExist(const CString &csPath)
{
	DWORD dwAttri = GetFileAttributes(csPath);

	// �ж��ļ��Ƿ��Ѵ���
	if(dwAttri != 0xFFFFFFFF)
	{
		return TRUE;
	}
	return FALSE;
}

///
/// @brief
///     ̽��ָ��Ŀ¼�Ƿ����
///
/// @param[in]
///     csPath - ��̽���Ŀ¼·��
///
/// @return
///     ���ڷ���TRUE�����򷵻�FALSE
///
BOOL CQuickSaveDlg::IsDirExist(const CString &csPath)
{
	WIN32_FIND_DATA fd;
	BOOL ret = FALSE;
	HANDLE hFind = FindFirstFile(csPath, &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		//Ŀ¼����
		ret = TRUE;
	}
	FindClose(hFind);
	return ret;
}

///
/// @brief
///     ����Ŀ¼
///
/// @param[in]
///     csPath - ������Ŀ¼·��
///
/// @return
///     ����Ŀ¼�ɹ�����TRUE��ʧ�ܷ���FALSE
///
BOOL CQuickSaveDlg::CreateDir(const CString &csPath)
{
	DWORD dwAttri = GetFileAttributes(csPath);

	// �ж�Ŀ¼�Ƿ��Ѵ���
	if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}

	CString csDirect(csPath);
	CList<CString, LPCTSTR> lstDirect;

	// β����� L'\\'
	if(csDirect.GetAt(csDirect.GetLength() - 1) != L'\\')
	{
		csDirect += L"\\";
	}

	int nBegin = csDirect.Find(L'\\', 0);
	int nEnd;
	// ��¼�̷�
	CString csCreatePath(csDirect.Mid(0,nBegin));

	// ����ÿһ��Ŀ¼�ַ���
	while(nBegin >= 0 && nBegin < csPath.GetLength())
	{
		nEnd = csDirect.Find(L'\\', nBegin + 1);

		if(nEnd > nBegin)
		{
			lstDirect.AddTail(csDirect.Mid(nBegin, nEnd - nBegin));
		}

		nBegin = nEnd;
	}

	// �𼶴���Ŀ¼
	POSITION pos = lstDirect.GetHeadPosition();
	while(pos)
	{
		csCreatePath += lstDirect.GetNext(pos);

		dwAttri = GetFileAttributes(csCreatePath);

		// Ŀ¼����������
		if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		// ����Ŀ¼
		if(!CreateDirectory(csCreatePath, NULL))
		{
			return FALSE;
		}
	}

	return TRUE;
}
