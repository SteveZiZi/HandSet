/**
 * \file
 *      MainDlg.h
 *
 * \brief
 *      dsm主页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */

#pragma once

#include "src/main/resource.h"

#include "BaseDlg.h"
#include "src/main/control/ButtonEx.h"
//#include "src/utils/navicontrol/navimenubar/navitoolbar.h"

#include <vector>

#define DSM_MAIN_RP             2                   ///< 每页2行
#if SU_FEI
#define DSM_MAIN_PAGE           2                   ///< 总共2页
#else
#define DSM_MAIN_PAGE           1                  ///< 总共2页
#endif
#define DSM_MAIN_ROWS           2 * DSM_MAIN_PAGE   ///< 总共4行
#define DSM_MAIN_COL            3                   ///< 每页3列


class CMainPageDataExtra;

// CMainDlg 对话框

class CMainDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
    virtual ~CMainDlg();

// 对话框数据
	enum { IDD = IDD_MAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:

    /**
     *      保存页面
     */
    virtual void _SavePage();


private:
    CMainPageDataExtra*         m_pPageDataExtra;
    CString                     m_csSelectPcapFile;

private:
    /**
     *      还原页面
     */
    virtual void _restorePage();

public:
    afx_msg void OnNaviMenuF1();
	afx_msg void OnNaviMenuF2();
	afx_msg void OnNaviMenuF3();
	afx_msg void OnNaviMenuF4();
	afx_msg void OnNaviMenuF5();
    afx_msg void OnNaviMenuF6();
    BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

