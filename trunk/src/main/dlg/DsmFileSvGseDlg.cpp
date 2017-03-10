// DsmFileSvGseDlg.cpp : implementation file
//

#include "stdafx.h"

#include "DsmFileSvGseDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"

#define DSM_FILE_BROWSER_NAME           247                         ///< �������򳤶�
#define DSM_FILE_BROWSER_SIZE           81                          ///< �ļ���С
#define DSM_FILE_BROWSER_TYPE           57                         ///< �ļ�����
#define DSM_FILE_BROWSER_TIME           144                        ///< �ļ��޸�����


/**
 *      �ļ��������¼��
 *
 *  \note
 *      �ļ���������޸�ʱ�䡢��С
 */
class CDsmFileRecordSvGse : public CELRecord
{
public:
    CDsmFileRecordSvGse(CString csFile, CString csSize, CString csType, CString csTime, DWORD dwType, CString csHintsDirectory)
    {
        m_dwType            = dwType;
        m_csHintsDirectory  = csHintsDirectory;

        AddItem(new CELRecordItemText(csFile));
        CELRecordItem* pItemType = AddItem(new CELRecordItemText(csSize));
        AddItem(new CELRecordItemText(csType));
        AddItem(new CELRecordItemText(csTime));

        CELRecordItemText* pItemTypeText = dynamic_cast<CELRecordItemText*>(pItemType);

        if(pItemTypeText && !csSize.IsEmpty())
        {
            pItemTypeText->SetFormatString(L"%s kB");
        }
    }


    virtual ~CDsmFileRecordSvGse()
    {
    }

public:

    inline CString GetHitsDirectory()
    {
        return m_csHintsDirectory;
    }

    inline DWORD GetType()
    {
        return m_dwType;
    }

private:
    DWORD               m_dwType;                   ///< ��¼���ļ�����
    CString             m_csHintsDirectory;         ///< ��¼�����ļ�����
};



// CDsmFileSvGseDlg dialog

IMPLEMENT_DYNAMIC(CDsmFileSvGseDlg, DSM_BASEMODLG)

CDsmFileSvGseDlg::CDsmFileSvGseDlg(BOOL bOpen, 
								   CString csDirectory, 
								   CString csFile, 
								   CString csExt, 
								   DWORD dwMode, 
								   CWnd* pParent /*= NULL*/, 
								   CString csCaption /*= L""*/)
: DSM_BASEMODLG(CDsmFileSvGseDlg::IDD, csCaption, pParent)
, m_bOpen(bOpen)
, m_csDirectory(csDirectory)
, m_csFile(csFile)
, m_csExt(csExt)
, m_csCurDirectory(csDirectory)
, m_dwMode(dwMode)
, m_csSelectDirectory(L"")
, m_csSelectFilePath(L"")
, m_bSelected(false)
, m_pRNaviMenu(NULL)
, m_pNaviBar(NULL)
, m_bFlag(TRUE)//Ĭ����smv�б�TRUE
{
	m_csDirectory.Trim();
	m_csExt.Trim();
	m_csFile.Trim();

	_formatPath(m_csDirectory);
	_formatPath(m_csCurDirectory);

	m_nIDMenu = IDR_MENU_FILE_LIST;
}

CDsmFileSvGseDlg::~CDsmFileSvGseDlg()
{
	_ResetMenu();
}

void CDsmFileSvGseDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEMODLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDsmFileSvGseDlg, DSM_BASEMODLG)
	ON_NOTIFY(ELNM_OKCLICK, IDC_FILEBROWSER_RET, &CDsmFileSvGseDlg::OnItemClicked)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_MENU_FILE_LIST_F1, &CDsmFileSvGseDlg::OnMenuF1)
	ON_COMMAND(ID_MENU_FILE_LIST_F2, &CDsmFileSvGseDlg::OnMenuF2)
END_MESSAGE_MAP()


// CDsmFileSvGseDlg message handlers
BOOL CDsmFileSvGseDlg::OnInitDialog()
{
	DSM_BASEMODLG::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	_createReport();

	m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
	ASSERT(m_pNaviBar);

	m_pRNaviMenu = new CRNaviMenu(this, m_nIDMenu);



	_splitExt();

	CString csShowDirectory = m_csDirectory;

	if(!m_csSelectDirectory.IsEmpty())
	{
		csShowDirectory = m_csSelectDirectory;
	}

	_enumerateFiles(csShowDirectory);

	_updateMenu();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDsmFileSvGseDlg::_createReport()
{
	CRect rcReport;
	GetClientRect(rcReport);

	rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

	m_reportFiles.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
		rcReport, this, IDC_FILEBROWSER_RET);

	// ��������
	m_reportFiles.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_reportFiles.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	// ������ʽ
	m_reportFiles.PostInitTemplateStyle();

	// ������Ԫ��
	m_reportFiles.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILENAME), EL_LEFT, DSM_FILE_BROWSER_NAME);
	m_reportFiles.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILESIZE), EL_LEFT, DSM_FILE_BROWSER_SIZE);
	m_reportFiles.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILETYPE), EL_LEFT, DSM_FILE_BROWSER_TYPE);
	m_reportFiles.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILETIME), EL_LEFT, DSM_FILE_BROWSER_TIME);

	// ����ѡ�񽹵���ʽ
	m_reportFiles.SetRowSelectionVisible(TRUE);
	m_reportFiles.SetRowFocusEnable(TRUE);
	m_reportFiles.SetSubItemFocusEnable(TRUE);
	m_reportFiles.SetFreezeColumnsCount(2);
	m_reportFiles.SetFreezeSelectionVisible(TRUE);
	m_reportFiles.SetFocusedColIndex(1);
}

void CDsmFileSvGseDlg::_enumerateFiles(CString csDirectory)
{
	_formatPath(csDirectory);
	int nRowIndex = 0;
	int nRowFocus = 0;
	int nDirectory= 0;
	CString csExt;
	WIN32_FIND_DATA findFileData;
	m_csCurDirectory    = csDirectory;
	CString csFolder    = csDirectory + L"\\*.*";
	HANDLE hFind        = FindFirstFile(csFolder, &findFileData);

	m_reportFiles.DeleteAllRecordRow();

	// ��ӷ�����һ��Ŀ¼�ļ�¼��
	if(csDirectory.CompareNoCase(m_csDirectory) != 0)
	{
		CString csDirectHints = csDirectory.Left(csDirectory.ReverseFind(L'\\'));

		// β������
		nDirectory = m_reportFiles.InsertRecordRow(m_reportFiles.GetRecordRowCount(), 
			new CDsmFileRecordSvGse(dsm_stringMgr::Get(DSM_STR_FILEBROWSER_PREVDIRECTORY), 
			L"", 
			dsm_stringMgr::Get(DSM_STR_FILEBROWSER_DIRECTORY), 
			L"", 
			FILE_ATTRIBUTE_DIRECTORY, 
			csDirectHints));

		nRowFocus++;
	}

	if(hFind == INVALID_HANDLE_VALUE)
	{
		if(m_reportFiles.GetRecordRowCount() > 0)
		{
			m_reportFiles.SetFocusedSelectionRowIndex(0);
		}

		return;
	}

	do
	{
		// �ļ�
		if(findFileData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE)
		{
			if(_loopupExt(DSM_GetFileExtension(findFileData.cFileName)))
			{
				nRowIndex = _insertRecord(&findFileData);

				if(!m_bSelected && m_csFile.CompareNoCase(findFileData.cFileName) == 0)
				{
					m_bSelected = true;
					nRowFocus   = nRowIndex;
				}
			}
		}
		// �ļ���
		else if(findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			CString csFileLastTime  = _formatFileTime(&findFileData.ftLastWriteTime);
			CString csDesc, csDirectH;
			csDesc.Format(L"<%s>", findFileData.cFileName);
			csDesc.MakeUpper();
			csDirectH.Format(L"%s\\%s", csDirectory, findFileData.cFileName);

			nDirectory = m_reportFiles.InsertRecordRow(nDirectory, 
				new CDsmFileRecordSvGse(csDesc, 
				L"", 
				dsm_stringMgr::Get(DSM_STR_FILEBROWSER_DIRECTORY), 
				csFileLastTime, 
				findFileData.dwFileAttributes, 
				csDirectH));

			nRowFocus++;
		}
	}while(::FindNextFile(hFind, &findFileData));

	FindClose(hFind);

	if(nRowFocus >= 0 && nRowFocus < m_reportFiles.GetRecordRowCount())
	{
		m_reportFiles.SetFocusedSelectionRowIndex(nRowFocus);
		m_reportFiles.EnsureRowVisibleScrollRow(nRowFocus);
	}
	else if(m_reportFiles.GetRecordRowCount() > 0)
	{
		m_reportFiles.SetFocusedSelectionRowIndex(0);
	}
}

void CDsmFileSvGseDlg::_splitExt()
{
	if(m_csExt.IsEmpty())
	{
		return;
	}

	CString csExt(m_csExt);
	CString csRet;

	TCHAR ch    = L'|';
	int nBegin  = 0;
	int nEnd    = csExt.Find(ch, nBegin);

	while(-1 != nEnd)
	{
		csRet   = csExt.Mid(nBegin, nEnd - nBegin);

		if(!csRet.IsEmpty())
		{
			m_mapExt.SetAt(csRet.MakeLower(), TRUE);
		}

		nBegin  = nEnd + 1;
		nEnd    = csExt.Find(ch, nBegin);
	}
}

BOOL CDsmFileSvGseDlg::_loopupExt( LPCTSTR szExt )
{
	BOOL bFind = FALSE;
	CString csExt;
	csExt.Format(L"*.%s", szExt);
	return m_mapExt.Lookup(csExt.MakeLower(), bFind) ? TRUE : FALSE;
}

int CDsmFileSvGseDlg::_insertRecord( WIN32_FIND_DATA* pfileData )
{
	ASSERT(pfileData);

	CString csFileSize      = _formatFileSize(pfileData);
	CString csFileLastTime  = _formatFileTime(&pfileData->ftLastWriteTime);
	CString csFileType      = _lookUpFileType(_getFileExt(pfileData->cFileName));

	// β������
	return m_reportFiles.InsertRecordRow(m_reportFiles.GetRecordRowCount(), 
		new CDsmFileRecordSvGse(pfileData->cFileName, 
		csFileSize, 
		csFileType, 
		csFileLastTime, 
		pfileData->dwFileAttributes, 
		pfileData->cFileName));
}

CString CDsmFileSvGseDlg::_formatFileSize( WIN32_FIND_DATA* pfileData )
{
	LONGLONG fileSize = pfileData->nFileSizeLow;

	if(0 != pfileData->nFileSizeHigh)
	{
		fileSize += pfileData->nFileSizeHigh * MAXWORD + 1;
	}

	CString csFileSize;
	csFileSize.Format(L"%lld", fileSize / 1024);

	return csFileSize;
}

CString CDsmFileSvGseDlg::_formatFileTime( LPFILETIME pFileTime)
{
	FILETIME fileTime;
	SYSTEMTIME sysTime;
	FileTimeToLocalFileTime(pFileTime, &fileTime);
	FileTimeToSystemTime(&fileTime, &sysTime);

	CString csFileTime;

	csFileTime.Format(L"%04d-%02d-%02d %02d:%02d", 
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
		sysTime.wHour, sysTime.wMinute);

	return csFileTime;
}

void CDsmFileSvGseDlg::OnItemClicked( NMHDR* pNotifyStruct, LRESULT* pResult )
{
	UNREFERENCED_PARAMETER(pNotifyStruct);

	*pResult = TRUE;

	int nSel = m_reportFiles.GetSelectionRowIndex();

	if(nSel < 0 || nSel >= m_reportFiles.GetRecordRowCount())
	{
		m_csCurDirectory    = L"";
		m_csRetPathName     = L"";
		m_csRetFilename     = L"";

		_EndDialog(IDCANCEL);
		return;
	}

	CDsmFileRecordSvGse* pRecord = dynamic_cast<CDsmFileRecordSvGse*>(m_reportFiles.GetRecordRow(nSel));
	ASSERT(pRecord);

	if(pRecord->GetType() == FILE_ATTRIBUTE_ARCHIVE)
	{
		m_csRetFilename = m_reportFiles.GetRowItemText(nSel, 0);
		m_csRetPathName.Format(L"%s\\%s", m_csCurDirectory, m_csRetFilename);

		_EndDialog(IDOK);
	}
	else if(pRecord->GetType() == FILE_ATTRIBUTE_DIRECTORY)
	{
		_enumerateFiles(pRecord->GetHitsDirectory());
	}
	else
	{
		_EndDialog(IDCANCEL);
		return;
	}
}

void CDsmFileSvGseDlg::OnMenuF1()
{
	///<  �ö�ȡ�ļ���·����SMV·��
#if SHENG_ZE
	m_csDirectory = "\\Storage Card\\pcap\\smv";
#else
	m_csDirectory = "\\Storage Card\\Record\\SV";
#endif
	//m_csDirectory = "\\FlashDisk2\\pcap\\smv";
	m_csDirectory.Trim();
	_formatPath(m_csDirectory);

	m_bFlag = TRUE;
	_updateMenu();

	m_csExt = "*.pcap|";
	m_csExt.Trim();
	_splitExt();

	_enumerateFiles(m_csDirectory);
}

void CDsmFileSvGseDlg::OnMenuF2()
{
	///<  �ö�ȡ�ļ���·����GOOSE·��
#if SHENG_ZE

	m_csDirectory = "\\Storage Card\\pcap\\goose";
#else
	m_csDirectory = "\\Storage Card\\Record\\GOOSE";
#endif
	//m_csDirectory = "\\FlashDisk2\\pcap\\goose";
	m_csDirectory.Trim();
	_formatPath(m_csDirectory);

	m_bFlag = FALSE;

	_updateMenu();

	m_csExt = "*.pcap|";
	m_csExt.Trim();
	_splitExt();

	_enumerateFiles(m_csDirectory);
}
CString CDsmFileSvGseDlg::_GetFilePath()
{
	return m_csRetPathName;
}

CString CDsmFileSvGseDlg::_GetFile()
{
	return m_csRetFilename;
}

CString CDsmFileSvGseDlg::_GetFolderPath()
{
	return m_csCurDirectory;
}

CString CDsmFileSvGseDlg::_GetFileExt()
{
	return _getFileExt(m_csRetFilename);
}


CString CDsmFileSvGseDlg::_getFileExt( CString csFile )
{
	return csFile.Right(
		csFile.GetLength() - 
		csFile.ReverseFind(L'.') - 1);
}

CString CDsmFileSvGseDlg::_lookUpFileType( CString csFileExt )
{
	csFileExt.MakeUpper();

	if(csFileExt.CompareNoCase(L"SCD") == 0)
	{
		return dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TYPE_SCL);
	}

	return csFileExt;
}

void CDsmFileSvGseDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	CRect rcClient;
	GetClientRect(rcClient);

	dc.FillSolidRect(rcClient, DSM_COLOR_BK);
}

BOOL CDsmFileSvGseDlg::OnEraseBkgnd(CDC* /* pDC */)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}

void CDsmFileSvGseDlg::_SetSelectFile( CString csFile )
{
	csFile.Trim();

	CString csTempDirectory = csFile.Left(m_csDirectory.GetLength());

	if(csTempDirectory.CompareNoCase(m_csDirectory) != 0)
	{
		return;
	}

	// ��ȡָ��ѡ���ļ���Ŀ¼
	CString csSelFileDirectory = csFile.Left(csFile.ReverseFind(L'\\') + 1);
	DWORD dwAttr = GetFileAttributes(csSelFileDirectory);

	// �ж�Ŀ¼�Ƿ���Ч
	if((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		m_csSelectDirectory = csSelFileDirectory;
		m_csSelectFilePath  = csFile;
		m_csFile            = csFile.Right(csFile.GetLength() - csSelFileDirectory.GetLength());
	}
}

void CDsmFileSvGseDlg::_formatPath( CString& csPath )
{
	while(csPath.Find(L"\\\\") != -1)
	{
		csPath.Replace(L"\\\\", L"\\");
	}

	if(csPath.GetAt(csPath.GetLength() - 1) == L'\\')
	{
		csPath = csPath.Left(csPath.GetLength() - 1);
	}
}




/**
 *      �ֶ���ԭ�˵�
 */
void CDsmFileSvGseDlg::_ResetMenu()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

BOOL CDsmFileSvGseDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���


	// ���������˵�������Ϣ
	if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
	{
		return TRUE;
	}
	return DSM_BASEMODLG::PreTranslateMessage(pMsg);
}

void CDsmFileSvGseDlg::_updateMenu()
{
	CString csStr,csCaption;
	CString csMsg = L"���λط�";

	CNaviMenu* pNavi = CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu();

	if (m_bFlag)
	{	
		csStr = L"SV�б�";
		csCaption.Format(L"%s->%s", csMsg, csStr);
		
		pNavi->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SMV_LIST_SEL));
		pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_LIST_UNSEL));
	}
	else
	{
		csStr = L"GSE�б�";
		csCaption.Format(L"%s->%s", csMsg, csStr);

		pNavi->SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SMV_LIST_UNSEL));
		pNavi->SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GOOSE_LIST_SEL));
	}
	CStatusMgr::SetTitle(csCaption);
	CNaviBarMgr::GetInstance()->GetNaviBar()->UpdateNaviMenu();
}