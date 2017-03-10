/**
 * \file
 *      BaseDlg.h
 *
 * \brief
 *      dsmҵ��ҳ��Ļ���ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */


#pragma once

#include "src/main/resource.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/navicontrol/navimenubar/navitoolbar.h"

#define PARAM_CFG_SAVE_TIMER        4800         ///< ��ʱ�ж��Ƿ񱣴���������ļ�

#define DSM_BASEDLG                 CBaseDlg

// CBaseDlg �Ի���

class CBaseDlg : public CDialog
{
	DECLARE_DYNAMIC(CBaseDlg)

public:
	CBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBaseDlg();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:

    /**
     *      �˳���ǰ����
     */
    virtual BOOL _AllowExit();

    /**
     *      �Ƿ�����ģ̬���
     *
     *  \note
     *      �ⲿ�������뵯��ģ̬ʱ��
     *      ��Ҫѯ�ʵ�ǰ��ҳ���Ƿ������䵯��
     */
    virtual BOOL _AllowInterrup()
    {
        return TRUE;
    }

    /**
     *      ����ҳ��
     */
    virtual void _SavePage() {}


protected:

    /**
     *      ���صײ��������˵�
     */
    virtual void _loadNaviMenu();

    /**
     *      �麯���޸Ĳ˵�HMENU������
     */
    virtual void _modifyMenu(){}

    /**
     *      ���¼��ز˵�HMENU
     */
    virtual void _reloadNaviMenu(UINT nMenuID);

protected:
    UINT                    m_nIDCaption;           ///< �����ַ���

    CBrush                  m_brBackground;         ///< ������ˢ

    UINT                    m_nMenuID;              ///< �˵�ID
    CMenu                   m_menu;                 ///< ��α���˵����ݶ���
    CNaviMenu               m_menuMain;             ///< ���棩�˵����ݶ���
    CNaviToolBar*           m_pNaviBar;             ///< �˵�����
};
