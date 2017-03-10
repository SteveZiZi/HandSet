#pragma once


#include "src/utils/ReportControl/Template/ListCtrlExt.h"
#include "src/utils/NaviControl/NaviMenuBar/NaviToolBar.h"

// CElTestDlg 对话框

class CElTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CElTestDlg)

    CNaviMenu               m_inplaceEditMenu;
    CNaviMenu               m_inplaceListMenu;
    CNaviMenu*              m_lastMenu;
    CFont                   m_menuFont;

    CBitmap                 m_bmp3;
    CBitmap                 m_bmp4;
    CBitmap                 m_defMenuBkImage;
    CNaviMenu               m_menu;
    CNaviMenu               m_menu2;
    CNaviToolBar            m_ctlMenu;
    CListCtrlT_VLine        m_ctlListEL;
    CFont                   m_hdrFont;
    CFont                   m_bodyFont;
    CImageList              m_hdrImageList;
    CImageList              m_bodyImageList;


    void  InitReportCtrl ();
    void  InitMenuCtrl ();

public:
	CElTestDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CElTestDlg();

// 对话框数据
	enum { IDD = IDD_ELTEST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBtnScope();
    afx_msg void OnUpdateBtnScope(CCmdUI *pCmdUI);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void OnInplaceButtonClick(NMHDR * pNotifyStruct, LRESULT* pResult);
    void OnInplaceListAction(NMHDR * pNotifyStruct, LRESULT* pResult);
    void OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult);
};
