/**
 * \file
 *      MainDlg.cpp
 *
 * \brief
 *      ��ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */

// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MainDlg.h"

#include <algorithm>

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/pagedata/pagedata.h"
#include "src/main/dlg/pcapbrowserdlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"

#include "src/utils/commoncontrol/timeeditctrl/timeeditctrl.h"
#include "src/utils/commoncontrol/progressslider/progressslider.h"



// CMainDlg �Ի���

IMPLEMENT_DYNAMIC(CMainDlg, CBaseDlg)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CMainDlg::IDD, DSM_STR_MAIN_CAPTION, pParent)
    , m_pPageDataExtra(NULL)
    , m_csSelectPcapFile(L"")
{
    m_nMenuID = IDR_MENU_MAIN;
	
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CBaseDlg)
    ON_WM_PAINT()
    ON_COMMAND(ID_MENU_MAIN_F1, &CMainDlg::OnNaviMenuF1)
	ON_COMMAND(ID_MENU_MAIN_F2, &CMainDlg::OnNaviMenuF2)
	ON_COMMAND(ID_MENU_MAIN_F3, &CMainDlg::OnNaviMenuF3)
	ON_COMMAND(ID_MENU_MAIN_F4, &CMainDlg::OnNaviMenuF4)
	ON_COMMAND(ID_MENU_MAIN_F5, &CMainDlg::OnNaviMenuF5)
    ON_COMMAND(ID_MENU_MAIN_F6, &CMainDlg::OnNaviMenuF6)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMainDlg ��Ϣ�������


BOOL CMainDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    bool bInited = false;
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    // �½�ҳ�棬����ҳ������
    if(pData->m_eAction == PageEnter)
    {
        if(!pData->m_pPageDataExtra)
        {
            m_pPageDataExtra = new CMainPageDataExtra(pData);
            pData->m_pPageDataExtra = m_pPageDataExtra;
        }
        else
        {
            bInited = true;
        }
    }

    // ����ҳ�棬ȡ���ɵ�ҳ������
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtra = dynamic_cast<CMainPageDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtra);
    }

    CDsmDataLayer::getInstance()->SetCurrentMode(DSM_MODE_MAIN_PAGE);

    _restorePage();


    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CMainDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CBaseDlg::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);
    //dc.FillSolidRect(rcClient, DSM_REPORT_COLOR_BK);

    CDC dcMemBmp;
    dcMemBmp.CreateCompatibleDC(&dc);
    HGDIOBJ hOriObj  = dcMemBmp.SelectObject(dsm_bmpmgr::Get(DSM_BMP_BKGROUND));
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMemBmp, 0, 0, SRCCOPY);
    dcMemBmp.SelectObject(hOriObj);
    dcMemBmp.DeleteDC();

}

void CMainDlg::OnNaviMenuF1()
{
	CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
}
//���ķ���
void CMainDlg::OnNaviMenuF2()
{
	CEventProc::GetInstance()->ShowPage(IDD_MAIN_SEND_DLG);
}
//��������
void CMainDlg::OnNaviMenuF3()
{
	CEventProc::GetInstance()->ShowPage(IDD_LP_LIGHT_POWER_DLG);
}
//IEC����
void CMainDlg::OnNaviMenuF4()
{
	CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DLG);
}
//��������
void CMainDlg::OnNaviMenuF5()
{
	CEventProc::GetInstance()->ShowPage(IDD_BASIC_SETTING_DLG);
}

void CMainDlg::OnNaviMenuF6()
{
    CEventProc::GetInstance()->ShowPage(IDD_BASIC_ABOUT_DLG);
}

BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

  //  UNREFERENCED_PARAMETER(pDC);

	return TRUE;

}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    return CBaseDlg::PreTranslateMessage(pMsg);
}


void CMainDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CMainDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    // �ָ�֮ǰ�Ľ��㰴ť
    if(pData->m_eAction == PageBack)
    {

        CWnd* pWnd          = GetDlgItem(pData->m_nCtrlID); 
        m_csSelectPcapFile  = m_pPageDataExtra->GetPcapFile();

        this->SetFocus();
    }
    else
    {
       this->SetFocus();
    }
}

void CMainDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CMainDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    // ���浱ǰ���㰴ť��ID
    pData->m_nCtrlID    = GetFocus()->GetDlgCtrlID();
    m_pPageDataExtra->SetPcapFile(m_csSelectPcapFile);

}

HBRUSH CMainDlg::OnCtlColor( CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */ )
{
    return (HBRUSH)::GetStockObject(NULL_BRUSH);
}
