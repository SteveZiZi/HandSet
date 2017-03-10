/**
 * \file
 *      BaseMoDlg.cpp
 *
 * \brief
 *      ģ̬�Ի���Ļ���ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2014/1/2
 */

#include "stdafx.h"
#include "BaseMoDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"


// CBaseMoDlg �Ի���

IMPLEMENT_DYNAMIC(CBaseMoDlg, CDialog)

CBaseMoDlg::CBaseMoDlg( UINT nIDTemplate, CString csCaption, CWnd* pParent /*= NULL*/ )
: CDialog(nIDTemplate, pParent)
, m_csCaption(csCaption)
{
    m_nIDMenu = IDR_MENU_NULL;
}

CBaseMoDlg::~CBaseMoDlg()
{
    _ClearResource();

    if(m_nModalResult == IDBACKTOMAIN)
    {
        CEventProc::GetInstance()->ShowMain();
    }
}

void CBaseMoDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseMoDlg, CDialog)
END_MESSAGE_MAP()


BOOL CBaseMoDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_HOME:
            if(CEventProc::GetInstance()->OnKeyHome(true) == DSM_HOME_RETVAL_MAIN)
            {
                _EndDialog(IDBACKTOMAIN);
            }
            return TRUE;

        case DSM_KEY_SNAPSHOT:
            CEventProc::GetInstance()->SnapShot();
            return TRUE;

        default:
            break;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CBaseMoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    CStatusMgr::SetTitle(m_csCaption);

    CRect rcRect(DSM_RCBODY);
    DSM_ParentToChild(this, rcRect);

    MoveWindow(rcRect);

    m_pRNaviMenu = new CRNaviMenu(this, m_nIDMenu);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CBaseMoDlg::_ClearResource()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

void CBaseMoDlg::_EndDialog( int nResult )
{
    EndDialog(nResult);
    _ClearResource();
}
