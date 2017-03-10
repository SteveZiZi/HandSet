#pragma once
#include "src/main/resource.h"

#include "src/main/common.h"
#include "src/main/dlg/basemodlg.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

class CRNaviMenu;
class CNaviToolBar;
// CDsmFileSvGseDlg dialog

class CDsmFileSvGseDlg : public DSM_BASEMODLG
{
	DECLARE_DYNAMIC(CDsmFileSvGseDlg)

public:
	CDsmFileSvGseDlg(BOOL bOpen, CString csDirectory, CString csFile, CString csExt, DWORD dwMode, CWnd* pParent = NULL, CString csCaption = L"");   // standard constructor
	virtual ~CDsmFileSvGseDlg();

// Dialog Data
	enum { IDD = IDD_DSM_SV_GSE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()


public:
    /**
     *      获取选中文件的全路径
     *
     *  /note
     *      "x:\example\test.xml" --->  "x:\example\test.xml"
     */
    CString _GetFilePath();

    /**
     *      获取选中文件名
     *
     *  /note
     *      "x:\example\test.xml" --->  "test.xml"
     */
    CString _GetFile();

    /**
     *      获取选中文件所在路径
     *
     *  /note
     *      "x:\example\test.xml" --->  "x:\example"
     */
    CString _GetFolderPath();

    /**
     *      获取选中文件的扩展名
     *
     *  /note
     *      "x:\example\test.xml" --->  "xml"
     */
    CString _GetFileExt();

    /**
     *      设置默认选中的文件
     */
    void _SetSelectFile(CString csFile);


private:
    typedef CMap<CString, LPCTSTR, BOOL, BOOL>      CMapExt;

	BOOL					m_bFlag;				///< 标记是sv列表还是goose列表
    BOOL                    m_bOpen;                ///< 打开文件或保存文件
    DWORD                   m_dwMode;               ///< 显示过滤标志位
    CString                 m_csExt;                ///< 扩展名
    CString                 m_csFile;               ///< 文件名
    CString                 m_csDirectory;          ///< 目录

    CString                 m_csCurDirectory;       ///< 当前遍历目录


    CString                 m_csRetFilename;        ///< 选中的文件名
    CString                 m_csRetPathName;        ///< 选中文件的全路径
    CReportV                m_reportFiles;          ///< 文件显示报表

    CMapExt                 m_mapExt;               ///< 扩展名映射表

    CString                 m_csSelectDirectory;    ///< 选中文件的目录
    CString                 m_csSelectFilePath;     ///< 选中文件的全路径
    bool                    m_bSelected;            ///< 是否已经选中过文件


	CNaviToolBar*       m_pNaviBar;         ///< 菜单对象
	CRNaviMenu*         m_pRNaviMenu;       ///< 菜单资源管理对象

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnItemClicked(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnMenuF1();
	afx_msg void OnMenuF2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
    /**
     *      创建文件浏览器报表
     */
    void _createReport();

    /**
     *      枚举文件
     *
     *  \param[in]
     *      csDirectory         待枚举的目录
     */
    void _enumerateFiles(CString csDirectory);

    /**
     *      分离过滤的扩展名
     */
    void _splitExt();

    /**
     *      查询指定扩展名是否允许被显示
     *
     *  \param[in]
     *      szExt           待查询的扩展名 "xml" "scl"
     *  \return
     *      BOOL            找到返回TRUE
     */
    BOOL _loopupExt(LPCTSTR szExt);

    /**
     *      插入一条记录
     *
     *  \param[in]
     *      pfileData       待插入到文件信息
     *  return
     *      int             返回插入的行号
     */
    int _insertRecord(WIN32_FIND_DATA* pfileData);

    /**
     *      格式化文件大小
     *
     *  \param[in]
     *      pfileData       待格式化的文件信息
     *  \return
     *      CString         返回文件的大小字符串
     */
    CString _formatFileSize(WIN32_FIND_DATA* pfileData);

    /**
     *      格式化时间
     *
     *  \param[in]
     *      pFileTime       文件的时间信息
     *  \return
     *      CString         返回特定时间格式
     *                      "2013/1/29 19:30"
     *
     *  /note
     *      FILETIME转SYSTEMTIME并按照特定格式显示
     */
    CString _formatFileTime(LPFILETIME pFileTime);

    /**
     *      获取指定文件后缀
     *
     *  \param[in]
     *      csFile          传入文件名
     *  \return
     *      CString         返回文件的后缀字符串
     */
    CString _getFileExt(CString csFile);

    /**
     *      根据后缀获取文件类型
     *
     *  \param[in]
     *      csFileExt       后缀字符串
     *  \return
     *      CString         返回文件类型，若找不到则返回后缀名
     */
    CString _lookUpFileType(CString csFileExt);

    /**
     *      格式化路径
     *
     *  \note
     *      去除重复的\，文件夹路径去除最后的\
     */
    void _formatPath(CString& csPath);
	    /**
     *      手动还原菜单
     */
    inline void _ResetMenu();

	void _updateMenu();
};
