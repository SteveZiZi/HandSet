/**
 * \file
 *      MainDlg.h
 *
 * \brief
 *      dsm��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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

#define DSM_MAIN_RP             2                   ///< ÿҳ2��
#if SU_FEI
#define DSM_MAIN_PAGE           2                   ///< �ܹ�2ҳ
#else
#define DSM_MAIN_PAGE           1                  ///< �ܹ�2ҳ
#endif
#define DSM_MAIN_ROWS           2 * DSM_MAIN_PAGE   ///< �ܹ�4��
#define DSM_MAIN_COL            3                   ///< ÿҳ3��


class CMainPageDataExtra;

// CMainDlg �Ի���

class CMainDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CMainDlg();

// �Ի�������
	enum { IDD = IDD_MAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:

    /**
     *      ����ҳ��
     */
    virtual void _SavePage();


private:
    CMainPageDataExtra*         m_pPageDataExtra;
    CString                     m_csSelectPcapFile;

private:
    /**
     *      ��ԭҳ��
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

