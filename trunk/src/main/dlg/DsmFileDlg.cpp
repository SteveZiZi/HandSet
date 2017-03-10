/**
 * \file
 *      DsmFileDlg.cpp
 *
 * \brief
 *      文件浏览器实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/11
 */

// DsmFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DsmFileDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"

//////////////////////////////////////////////////////////////////////////
// 文件浏览器
#if CHUN_HUA
#define DSM_FILE_BROWSER_NAME           270                         ///< 名字区域长度
#define DSM_FILE_BROWSER_SIZE           95                          ///< 文件大小
#define DSM_FILE_BROWSER_TYPE           85                          ///< 文件类型
#define DSM_FILE_BROWSER_TIME           150                         ///< 文件修改日期
#else
#define DSM_FILE_BROWSER_NAME           210                        ///< 名字区域长度
#define DSM_FILE_BROWSER_SIZE           90                          ///< 文件大小
#define DSM_FILE_BROWSER_TYPE           59                          ///< 文件类型
#define DSM_FILE_BROWSER_TIME           170                      ///< 文件修改日期
#endif

/**
 *      文件浏览器记录项
 *
 *  \note
 *      文件名、最后修改时间、大小
 */
class CDsmFileRecord : public CELRecord
{
public:
    CDsmFileRecord(CString csFile, CString csSize, CString csType, CString csTime, DWORD dwType, CString csHintsDirectory)
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


    virtual ~CDsmFileRecord()
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
    DWORD               m_dwType;                   ///< 记录的文件类型
    CString             m_csHintsDirectory;         ///< 记录关联文件夹名
};


// CDsmFileDlg 对话框

IMPLEMENT_DYNAMIC(CDsmFileDlg, DSM_BASEMODLG)



CDsmFileDlg::CDsmFileDlg( BOOL bOpen, 
                         CString csDirectory, 
                         CString csFile, 
                         CString csExt, 
                         DWORD dwMode, 
                         CWnd* pParent /*= NULL*/ , 
                         CString csCaption /*= L""*/ )
: DSM_BASEMODLG(CDsmFileDlg::IDD, csCaption, pParent)
, m_bOpen(bOpen)
, m_csDirectory(csDirectory)
, m_csFile(csFile)
, m_csExt(csExt)
, m_csCurDirectory(csDirectory)
, m_dwMode(dwMode)
, m_csSelectDirectory(L"")
, m_csSelectFilePath(L"")
, m_bSelected(false)
{
    m_csDirectory.Trim();
    m_csExt.Trim();
    m_csFile.Trim();

    _formatPath(m_csDirectory);
    _formatPath(m_csCurDirectory);
}



CDsmFileDlg::~CDsmFileDlg()
{
}

void CDsmFileDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEMODLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDsmFileDlg, DSM_BASEMODLG)
    ON_NOTIFY(ELNM_OKCLICK, IDC_FILEBROWSER_RET, &CDsmFileDlg::OnItemClicked)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDsmFileDlg 消息处理程序

BOOL CDsmFileDlg::OnInitDialog()
{
    DSM_BASEMODLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    _createReport();

    _splitExt();

    CString csShowDirectory = m_csDirectory;

    if(!m_csSelectDirectory.IsEmpty())
    {
        csShowDirectory = m_csSelectDirectory;
    }

    _enumerateFiles(csShowDirectory);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CDsmFileDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_reportFiles.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_FILEBROWSER_RET);

    // 设置字体
    m_reportFiles.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportFiles.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportFiles.PostInitTemplateStyle();

    // 插入列元素
	m_reportFiles.InsertColumn(0, L"       " + dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILENAME), EL_LEFT, DSM_FILE_BROWSER_NAME);
    m_reportFiles.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILESIZE), EL_LEFT, DSM_FILE_BROWSER_SIZE);
    m_reportFiles.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILETYPE), EL_LEFT, DSM_FILE_BROWSER_TYPE);
    m_reportFiles.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_FILETIME), EL_LEFT, DSM_FILE_BROWSER_TIME);

    // 设置选择焦点样式
    m_reportFiles.SetRowSelectionVisible(TRUE);
    m_reportFiles.SetRowFocusEnable(TRUE);
    m_reportFiles.SetSubItemFocusEnable(TRUE);
    m_reportFiles.SetFreezeColumnsCount(2);
    m_reportFiles.SetFreezeSelectionVisible(TRUE);
    m_reportFiles.SetFocusedColIndex(1);
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	
	CListCtrlEL*   pListCtrl  =   (CListCtrlEL*)   lParamSort;   
	CString      strItem1   =   pListCtrl->GetItemText(lParam1,   3);  
	CString      strItem2   =   pListCtrl->GetItemText(lParam2,   3); 
	COleDateTime dt1;
	COleDateTime dt2;
	dt1.ParseDateTime(strItem1);
	dt2.ParseDateTime(strItem2);
	CTime ct1 = CTime(dt1);
	CTime ct2 = CTime(dt2);

	if (ct1>ct2)
	{
		return 1;
	}
	else
		return -1;
}

void CDsmFileDlg::_enumerateFiles(CString csDirectory)
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

    // 添加返回上一层目录的记录项
    if(csDirectory.CompareNoCase(m_csDirectory) != 0)
    {
        CString csDirectHints = csDirectory.Left(csDirectory.ReverseFind(L'\\'));

        // 尾部插入
        nDirectory = m_reportFiles.InsertRecordRow(m_reportFiles.GetRecordRowCount(), 
            new CDsmFileRecord(dsm_stringMgr::Get(DSM_STR_FILEBROWSER_PREVDIRECTORY), 
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
        // 文件
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
        // 文件夹
        else if(findFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
        {
            CString csFileLastTime  = _formatFileTime(&findFileData.ftLastWriteTime);
            CString csDesc, csDirectH;
            csDesc.Format(L"<%s>", findFileData.cFileName);
            csDesc.MakeUpper();
            csDirectH.Format(L"%s\\%s", csDirectory, findFileData.cFileName);

            nDirectory = m_reportFiles.InsertRecordRow(nDirectory, 
                new CDsmFileRecord(csDesc, 
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
	//m_reportFiles.SortItems((PFNLVCOMPARE)CompareFunc,(DWORD)&m_reportFiles);
}




void CDsmFileDlg::_splitExt()
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

BOOL CDsmFileDlg::_loopupExt( LPCTSTR szExt )
{
    BOOL bFind = FALSE;
    CString csExt;
    csExt.Format(L"*.%s", szExt);
    return m_mapExt.Lookup(csExt.MakeLower(), bFind) ? TRUE : FALSE;
}

int CDsmFileDlg::_insertRecord( WIN32_FIND_DATA* pfileData )
{
    ASSERT(pfileData);

    CString csFileSize      = _formatFileSize(pfileData);
    CString csFileLastTime  = _formatFileTime(&pfileData->ftLastWriteTime);
    CString csFileType      = _lookUpFileType(_getFileExt(pfileData->cFileName));

    // 尾部插入
    return m_reportFiles.InsertRecordRow(m_reportFiles.GetRecordRowCount(), 
        new CDsmFileRecord(pfileData->cFileName, 
        csFileSize, 
        csFileType, 
        csFileLastTime, 
        pfileData->dwFileAttributes, 
        pfileData->cFileName));
}

CString CDsmFileDlg::_formatFileSize( WIN32_FIND_DATA* pfileData )
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

CString CDsmFileDlg::_formatFileTime( LPFILETIME pFileTime)
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

void CDsmFileDlg::OnItemClicked( NMHDR* pNotifyStruct, LRESULT* pResult )
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

    CDsmFileRecord* pRecord = dynamic_cast<CDsmFileRecord*>(m_reportFiles.GetRecordRow(nSel));
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

CString CDsmFileDlg::_GetFilePath()
{
    return m_csRetPathName;
}

CString CDsmFileDlg::_GetFile()
{
    return m_csRetFilename;
}

CString CDsmFileDlg::_GetFolderPath()
{
    return m_csCurDirectory;
}

CString CDsmFileDlg::_GetFileExt()
{
    return _getFileExt(m_csRetFilename);
}


CString CDsmFileDlg::_getFileExt( CString csFile )
{
    return csFile.Right(
        csFile.GetLength() - 
        csFile.ReverseFind(L'.') - 1);
}

CString CDsmFileDlg::_lookUpFileType( CString csFileExt )
{
    csFileExt.MakeUpper();

    if(csFileExt.CompareNoCase(L"SCD") == 0)
    {
        return dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TYPE_SCL);
    }

    return csFileExt;
}

void CDsmFileDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    dc.FillSolidRect(rcClient, DSM_COLOR_BK);
}

BOOL CDsmFileDlg::OnEraseBkgnd(CDC* /* pDC */)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    return TRUE;
}

void CDsmFileDlg::_SetSelectFile( CString csFile )
{
    csFile.Trim();

    CString csTempDirectory = csFile.Left(m_csDirectory.GetLength());

    if(csTempDirectory.CompareNoCase(m_csDirectory) != 0)
    {
        return;
    }

    // 获取指定选中文件的目录
    CString csSelFileDirectory = csFile.Left(csFile.ReverseFind(L'\\') + 1);
    DWORD dwAttr = GetFileAttributes(csSelFileDirectory);

    // 判断目录是否有效
    if((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
    {
        m_csSelectDirectory = csSelFileDirectory;
        m_csSelectFilePath  = csFile;
        m_csFile            = csFile.Right(csFile.GetLength() - csSelFileDirectory.GetLength());
    }
}

void CDsmFileDlg::_formatPath( CString& csPath )
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

