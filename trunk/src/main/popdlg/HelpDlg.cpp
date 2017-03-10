// HelpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HelpDlg.h"

#include "src/main/common.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"


// CHelpDlg �Ի���
IMPLEMENT_DYNAMIC(CHelpDlg, CDialog)

CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpDlg::IDD, pParent)
    , m_pRNaviMenu(NULL)
{

}

CHelpDlg::~CHelpDlg()
{

    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }

}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
END_MESSAGE_MAP()


// CHelpDlg ��Ϣ�������

BOOL CHelpDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    CRect rect(DSM_RCBODY);
    m_sliderbox.Create(WS_VISIBLE|WS_CHILD|WS_TABSTOP, CRect(0, 0, rect.Width(), rect.Height()),this, IDC_HELP_SLIDERBOX);

    AfxGetMainWnd()->ClientToScreen(rect);
    MoveWindow(rect);


    m_pRNaviMenu = new CRNaviMenu(this, IDR_MENU_NULL);


    CString strImgDir = DSM_GetAppDir() + L"help\\";
    BOOL ret = m_sliderbox.LoadImages(strImgDir);
    if(!ret)
    {
        RETAILMSG(true, (L"dsm: failed to load helpp picture\n"));
    }

    m_sliderbox.SetAnimateFreq(12);
    m_sliderbox.SetScrollTimeout(350);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


