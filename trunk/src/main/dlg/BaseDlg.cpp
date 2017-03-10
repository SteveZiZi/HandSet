/**
 * \file
 *      BaseDlg.cpp
 *
 * \brief
 *      dsmҵ��ҳ��Ļ���ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */


// BaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BaseDlg.h"

#include "src/main/common.h"
#include "src/main/mgr/statusmgr.h"



// CBaseDlg �Ի���

IMPLEMENT_DYNAMIC(CBaseDlg, CDialog)

CBaseDlg::CBaseDlg(UINT nIDTemplate, UINT nIDCaption, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
    , m_nIDCaption(nIDCaption)
    , m_pNaviBar(NULL)
    , m_nMenuID(UINT_MAX)
{
}

CBaseDlg::~CBaseDlg()
{
    m_brBackground.DeleteObject();
}

void CBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDlg, CDialog)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CBaseDlg ��Ϣ�������

BOOL CBaseDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    // ���������˵�������Ϣ
    if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if(AfxGetMainWnd()->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}


BOOL CBaseDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_brBackground.CreateSolidBrush(DSM_COLOR_BK);

    CStatusMgr::SetTitle(dsm_stringMgr::Get(m_nIDCaption));

    _loadNaviMenu();

    m_pNaviBar->UpdateNaviMenu();

    MoveWindow(DSM_RCBODY);
    ShowWindow(SW_SHOW);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


HBRUSH CBaseDlg::OnCtlColor(CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */)
{
    return m_brBackground;
}


BOOL CBaseDlg::_AllowExit()
{
    return TRUE;
}


void CBaseDlg::_loadNaviMenu()
{
    ASSERT(-1 != m_nMenuID);

    // ��ӵײ��˵�
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);
    m_menu.DestroyMenu();

    // �����麯���޸Ĳ˵�
    _modifyMenu();

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();

    if(m_pNaviBar)
    {
        // ���ò˵�Ŀ���������ݶ���
        m_pNaviBar->SetCommandTarget(this);
        m_pNaviBar->SetNaviMenu(&m_menuMain);
    }
}

/**
 *      ���¼��ز˵�HMENU
 */
void CBaseDlg::_reloadNaviMenu(UINT nMenuID)
{
    if(!m_pNaviBar)
    {
        return;
    }

    ASSERT(-1 != nMenuID);

    m_nMenuID = nMenuID;

    if(m_menu.GetSafeHmenu())
    {
        m_menu.DestroyMenu();
    }

    // ��ӵײ��˵�
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);

    // ���ò˵�Ŀ���������ݶ���
    m_pNaviBar->SetCommandTarget(this);
    m_pNaviBar->SetNaviMenu(&m_menuMain);
}

