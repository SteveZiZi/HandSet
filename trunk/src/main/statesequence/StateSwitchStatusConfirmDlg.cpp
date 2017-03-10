// StateSwitchStatusConfirmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StateSwitchStatusConfirmDlg.h"

#include "src/main/popdlg/common_pop.h"

// CStateSwitchStatusConfirmDlg �Ի���

IMPLEMENT_DYNAMIC(CStateSwitchStatusConfirmDlg, CDialog)

CStateSwitchStatusConfirmDlg::CStateSwitchStatusConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStateSwitchStatusConfirmDlg::IDD, pParent)
{
    m_nMenuID = IDR_MENU_STATE_SWITCH_STATUS_CONFIRM;
}

CStateSwitchStatusConfirmDlg::~CStateSwitchStatusConfirmDlg()
{
    CNaviBarMgr::GetInstance()->RestoreMenu(m_pWndPrev, m_pMenuPrev);
}

void CStateSwitchStatusConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateSwitchStatusConfirmDlg, CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CStateSwitchStatusConfirmDlg ��Ϣ�������

BOOL CStateSwitchStatusConfirmDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    
    m_pMenuPrev = CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu();
    m_pWndPrev = CNaviBarMgr::GetInstance()->GetNaviBar()->GetCommandTarget();

    CNaviBarMgr::GetInstance()->ShowMenu(this, m_nMenuID);

    CRect rcRect(DSM_STATE_CONFIRM_DLG_RC);
    AfxGetMainWnd()->ClientToScreen(rcRect);
    MoveWindow(rcRect);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CStateSwitchStatusConfirmDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    CRect rcSerifA1(rcClient.left, rcClient.top, rcClient.right,  rcClient.top + 1);
    CRect rcSerifB1(rcClient.left, rcClient.top + 1, rcClient.right,  rcClient.top + 2);

    CRect rcSerifB2(rcClient.left, rcClient.bottom - 1, rcClient.right,  rcClient.bottom);

    CRect rcBody(rcClient);
    rcBody.DeflateRect(0, 2, 0, 1);

    dc.FillSolidRect(rcBody, DSM_BASEPOP_CRBK);

    // ����+�߾� body �߾�
    dc.FillSolidRect(rcSerifA1, DSM_BASEPOP_CRSERIFA);      // ͳһ��
    dc.FillSolidRect(rcSerifB1, DSM_BASEPOP_CRSERIFB);
    dc.FillSolidRect(rcSerifB2, DSM_BASEPOP_CRSERIFB);


}

BOOL CStateSwitchStatusConfirmDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message && VK_F6 == pMsg->wParam)
    {
        CDialog::OnOK();
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CStateSwitchStatusConfirmDlg::OnOK()
{
    // TODO: �ڴ����ר�ô����/����û���
}

void CStateSwitchStatusConfirmDlg::OnCancel()
{
    // TODO: �ڴ����ר�ô����/����û���
}
