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
     *      ��ȡѡ���ļ���ȫ·��
     *
     *  /note
     *      "x:\example\test.xml" --->  "x:\example\test.xml"
     */
    CString _GetFilePath();

    /**
     *      ��ȡѡ���ļ���
     *
     *  /note
     *      "x:\example\test.xml" --->  "test.xml"
     */
    CString _GetFile();

    /**
     *      ��ȡѡ���ļ�����·��
     *
     *  /note
     *      "x:\example\test.xml" --->  "x:\example"
     */
    CString _GetFolderPath();

    /**
     *      ��ȡѡ���ļ�����չ��
     *
     *  /note
     *      "x:\example\test.xml" --->  "xml"
     */
    CString _GetFileExt();

    /**
     *      ����Ĭ��ѡ�е��ļ�
     */
    void _SetSelectFile(CString csFile);


private:
    typedef CMap<CString, LPCTSTR, BOOL, BOOL>      CMapExt;

	BOOL					m_bFlag;				///< �����sv�б���goose�б�
    BOOL                    m_bOpen;                ///< ���ļ��򱣴��ļ�
    DWORD                   m_dwMode;               ///< ��ʾ���˱�־λ
    CString                 m_csExt;                ///< ��չ��
    CString                 m_csFile;               ///< �ļ���
    CString                 m_csDirectory;          ///< Ŀ¼

    CString                 m_csCurDirectory;       ///< ��ǰ����Ŀ¼


    CString                 m_csRetFilename;        ///< ѡ�е��ļ���
    CString                 m_csRetPathName;        ///< ѡ���ļ���ȫ·��
    CReportV                m_reportFiles;          ///< �ļ���ʾ����

    CMapExt                 m_mapExt;               ///< ��չ��ӳ���

    CString                 m_csSelectDirectory;    ///< ѡ���ļ���Ŀ¼
    CString                 m_csSelectFilePath;     ///< ѡ���ļ���ȫ·��
    bool                    m_bSelected;            ///< �Ƿ��Ѿ�ѡ�й��ļ�


	CNaviToolBar*       m_pNaviBar;         ///< �˵�����
	CRNaviMenu*         m_pRNaviMenu;       ///< �˵���Դ�������

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
     *      �����ļ����������
     */
    void _createReport();

    /**
     *      ö���ļ�
     *
     *  \param[in]
     *      csDirectory         ��ö�ٵ�Ŀ¼
     */
    void _enumerateFiles(CString csDirectory);

    /**
     *      ������˵���չ��
     */
    void _splitExt();

    /**
     *      ��ѯָ����չ���Ƿ�������ʾ
     *
     *  \param[in]
     *      szExt           ����ѯ����չ�� "xml" "scl"
     *  \return
     *      BOOL            �ҵ�����TRUE
     */
    BOOL _loopupExt(LPCTSTR szExt);

    /**
     *      ����һ����¼
     *
     *  \param[in]
     *      pfileData       �����뵽�ļ���Ϣ
     *  return
     *      int             ���ز�����к�
     */
    int _insertRecord(WIN32_FIND_DATA* pfileData);

    /**
     *      ��ʽ���ļ���С
     *
     *  \param[in]
     *      pfileData       ����ʽ�����ļ���Ϣ
     *  \return
     *      CString         �����ļ��Ĵ�С�ַ���
     */
    CString _formatFileSize(WIN32_FIND_DATA* pfileData);

    /**
     *      ��ʽ��ʱ��
     *
     *  \param[in]
     *      pFileTime       �ļ���ʱ����Ϣ
     *  \return
     *      CString         �����ض�ʱ���ʽ
     *                      "2013/1/29 19:30"
     *
     *  /note
     *      FILETIMEתSYSTEMTIME�������ض���ʽ��ʾ
     */
    CString _formatFileTime(LPFILETIME pFileTime);

    /**
     *      ��ȡָ���ļ���׺
     *
     *  \param[in]
     *      csFile          �����ļ���
     *  \return
     *      CString         �����ļ��ĺ�׺�ַ���
     */
    CString _getFileExt(CString csFile);

    /**
     *      ���ݺ�׺��ȡ�ļ�����
     *
     *  \param[in]
     *      csFileExt       ��׺�ַ���
     *  \return
     *      CString         �����ļ����ͣ����Ҳ����򷵻غ�׺��
     */
    CString _lookUpFileType(CString csFileExt);

    /**
     *      ��ʽ��·��
     *
     *  \note
     *      ȥ���ظ���\���ļ���·��ȥ������\
     */
    void _formatPath(CString& csPath);
	    /**
     *      �ֶ���ԭ�˵�
     */
    inline void _ResetMenu();

	void _updateMenu();
};
