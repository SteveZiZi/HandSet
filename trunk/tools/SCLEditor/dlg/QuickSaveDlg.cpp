// D:\DSM800\trunk\tools\SCLEditor\dlg\QuickSaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\SCLEditor.h"
#include "QuickSaveDlg.h"
#include "..\ResourceStr.h"
#include "..\base\StrManager.h"

#define STR_DEFAULT_DIRECTORY _T("%sSCL")
#define STR_DEFAULT_FILENAME  _T("%sSCL\\%s.scx")

// CQuickSaveDlg 对话框

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


// CQuickSaveDlg 消息处理程序

BOOL CQuickSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// 添加盘符信息，只保留移动存储设备
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

	// 添加文件名历史记录及设置默认文件名
	for ( size_t i = 0; i < m_vFileNameOpts.size(); i++ )
	{
		m_cSaveFileName.AddString(m_vFileNameOpts[i].GetString());
	}
	m_cSaveFileName.SetWindowText(m_csFileName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CQuickSaveDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	m_cSaveDrive.GetWindowText(m_csDrive);
	m_cSaveFileName.GetWindowText(m_csFileName);

	// 盘符为空，则提示插入移动存储设备
	if ( m_csDrive.IsEmpty() )
	{
		AfxMessageBox(CStrManager::Get(RCSTR_INSERT_REMOVABLE_DRIVE),MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	// 移除文件名中最前面的' '字符
	while ( m_csFileName.GetAt(0) == _T(' ') )
	{
		m_csFileName.Delete(0);
	}
	if ( m_csFileName.GetLength() == 0 )
		return;

	// 判断文件是否存在，存在则提示是否替换
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

	// 判断文件目录是否存在，不存在则创建
	str.Format(STR_DEFAULT_DIRECTORY,m_csDrive);
	if ( IsDirExist(str) == FALSE )
	{
		CreateDir(str);
	}

	OnOK();
}

void CQuickSaveDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CQuickSaveDlg::OnCbnDropdownSaveDrive()
{
	// TODO: 在此添加控件通知处理程序代码
	for ( int i = m_cSaveDrive.GetCount()-1; i >= 0; i-- )
	{
		m_cSaveDrive.DeleteString(i);
	}

	// 添加盘符信息，只保留移动存储设备
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
///     设置默认文件名
///
/// @param[in]
///     fileName - 默认文件名
///
void CQuickSaveDlg::SetDefaultFileName(const CString &fileName)
{
	m_csFileName = fileName;
}

///
/// @brief
///     添加文件名历史，最多有5项
///
/// @param[in]
///     fileName - 要添加的文件名
///
void CQuickSaveDlg::AddFileNameHistory(const CString &fileName)
{
	m_vFileNameOpts.push_back(fileName);
}

///
/// @brief
///     获取文件名
///
/// @return
///     文件名
///
CString CQuickSaveDlg::GetFileName()
{
	return m_csFileName;
}

///
/// @brief
///     获取文件路径
///
/// @return
///     文件路径
///
CString CQuickSaveDlg::GetFilePath()
{
	CString filePath;
	filePath.Format(STR_DEFAULT_FILENAME,m_csDrive,m_csFileName);
	return filePath;
}

///
/// @brief
///     探测指定文件是否存在
///
/// @param[in]
///     csPath - 待探测的文件路径
///
/// @return
///     存在返回TRUE，否则返回FALSE
///
BOOL CQuickSaveDlg::IsFileExist(const CString &csPath)
{
	DWORD dwAttri = GetFileAttributes(csPath);

	// 判断文件是否已存在
	if(dwAttri != 0xFFFFFFFF)
	{
		return TRUE;
	}
	return FALSE;
}

///
/// @brief
///     探测指定目录是否存在
///
/// @param[in]
///     csPath - 待探测的目录路径
///
/// @return
///     存在返回TRUE，否则返回FALSE
///
BOOL CQuickSaveDlg::IsDirExist(const CString &csPath)
{
	WIN32_FIND_DATA fd;
	BOOL ret = FALSE;
	HANDLE hFind = FindFirstFile(csPath, &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		//目录存在
		ret = TRUE;
	}
	FindClose(hFind);
	return ret;
}

///
/// @brief
///     创建目录
///
/// @param[in]
///     csPath - 待创建目录路径
///
/// @return
///     创建目录成功返回TRUE，失败返回FALSE
///
BOOL CQuickSaveDlg::CreateDir(const CString &csPath)
{
	DWORD dwAttri = GetFileAttributes(csPath);

	// 判断目录是否已存在
	if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
	{
		return TRUE;
	}

	CString csDirect(csPath);
	CList<CString, LPCTSTR> lstDirect;

	// 尾部添加 L'\\'
	if(csDirect.GetAt(csDirect.GetLength() - 1) != L'\\')
	{
		csDirect += L"\\";
	}

	int nBegin = csDirect.Find(L'\\', 0);
	int nEnd;
	// 记录盘符
	CString csCreatePath(csDirect.Mid(0,nBegin));

	// 分离每一级目录字符串
	while(nBegin >= 0 && nBegin < csPath.GetLength())
	{
		nEnd = csDirect.Find(L'\\', nBegin + 1);

		if(nEnd > nBegin)
		{
			lstDirect.AddTail(csDirect.Mid(nBegin, nEnd - nBegin));
		}

		nBegin = nEnd;
	}

	// 逐级创建目录
	POSITION pos = lstDirect.GetHeadPosition();
	while(pos)
	{
		csCreatePath += lstDirect.GetNext(pos);

		dwAttri = GetFileAttributes(csCreatePath);

		// 目录存在则跳过
		if(dwAttri != 0xFFFFFFFF && (FILE_ATTRIBUTE_DIRECTORY & dwAttri) == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		// 创建目录
		if(!CreateDirectory(csCreatePath, NULL))
		{
			return FALSE;
		}
	}

	return TRUE;
}
