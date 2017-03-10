// HomeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HomeDlg.h"

#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/res/dsm_stringmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/popdlg/helpdlg.h"




// CHomeDlg �Ի���

IMPLEMENT_DYNAMIC(CHomeDlg, CDialog)

CHomeDlg::CHomeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHomeDlg::IDD, pParent)
{

}

CHomeDlg::~CHomeDlg()
{
    CNaviBarMgr::GetInstance()->GetNaviBar()->EnableWindow(TRUE);
}

void CHomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHomeDlg, CDialog)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CHomeDlg ��Ϣ�������

BOOL CHomeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    CNaviBarMgr::GetInstance()->GetNaviBar()->EnableWindow(FALSE);

    CRect rcRect(DSM_HOMECFG_RC);
    AfxGetMainWnd()->ClientToScreen(rcRect);
    MoveWindow(rcRect);

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CHomeDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    UNREFERENCED_PARAMETER(pDC);

    return TRUE;
}

void CHomeDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CBaseDlg::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    rcClient.DeflateRect(0, 0, 0, rcClient.Height() - 1);
    dc.FillSolidRect(rcClient, DSM_COLOR_BK);

    // ����
    CRect rcLine(DSM_HOMECFG_RCLINE);
    CRect rcMenuBar(DSM_HOMECFG_RCMENU);

    for(int i = 0; i < 5; i++)
    {
        dc.FillSolidRect(rcLine, DSM_COLOR_BK);
        rcLine.OffsetRect(rcMenuBar.Width() + 1, 0);
    }

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);

    CRect rcMenu(DSM_HOMECFG_RCMENU);

    for(int i = 0; i < 6; i++)
    {
        // ���նԻ���
        if(0 == i || 5 == i)
        {
            CRect rcMenuColor = rcMenu;
            CRect rcSerif     = rcMenu;

            rcMenuColor.DeflateRect(0, 1, 0, 0);
            rcSerif.DeflateRect(0, 0, 0, rcMenu.Height() - 1);

            dc.FillSolidRect(rcSerif, DSM_HOMECFG_COLOR_MARGIN);        // �ϱ���
            dc.FillSolidRect(rcMenuColor, DSM_HOMECFG_COLOR_MENUBAR);   // �ڲ���ɫ���
        }
        else
        {
            // ���ư�ťͼƬ 1 4 7 10
            dcMem.SelectObject(dsm_bmpmgr::Get(DSM_BMP_DLG_HOME_MAIN_FOCUS + (i - 1) * 3));
            dc.BitBlt(rcMenu.left, rcMenu.top, rcMenu.Width(), rcMenu.Height(), &dcMem, 0, 0, SRCCOPY);
        }

        rcMenu.OffsetRect(rcMenu.Width() + 1, 0);
    }
}

BOOL CHomeDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_HOME:
            EndDialog(IDCANCEL);
            return TRUE;

        case VK_F2:
            EndDialog(DSM_HOME_RETVAL_MAIN);
            return TRUE;

        case VK_F3:
            EndDialog(DSM_HOME_RETVAL_SNAP);
            return TRUE;

        case VK_F4:
            {
                this->MoveWindow(CRect(0, 0, 0, 0), FALSE);
                CString csPrevTitle(CStatusMgr::GetTitle());
                CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_HOME_HELP));

                CHelpDlg dlg;
                dlg.DoModal();

                CStatusMgr::SetTitle(csPrevTitle);
                EndDialog(DSM_HOME_RETVAL_NONE);
            }

            return TRUE;

        case VK_F5:
            {
                CMsgDlg msgdlg(DSM_STR_MSG_SHUTDOWN_TIPS, DSM_BMP_DLG_MSG_SHUTDOWN_ICON, this);

                HWND hWnd = ::GetFocus();
                this->ShowWindow(SW_HIDE);
                INT_PTR nRet = msgdlg.DoModal();
                msgdlg._ResetMenu();

                if(nRet == IDOK)
                {
                    CEventProc::GetInstance()->Close();
                }

                this->ShowWindow(SW_SHOW);

                ::SetFocus(hWnd);
            }

            return TRUE;

        case DSM_KEY_SNAPSHOT:
            CEventProc::GetInstance()->SnapShot();
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

HBRUSH CHomeDlg::OnCtlColor(CDC* /* pDC */, CWnd* /* pWnd */, UINT /*nCtlColor*/)
{
    //HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  �ڴ˸��� DC ���κ�����

    // TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
    return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

void CHomeDlg::OnOK()
{

}
