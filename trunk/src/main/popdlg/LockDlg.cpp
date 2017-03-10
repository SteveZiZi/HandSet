// LockDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LockDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/utility/dsm_utility.h"



// CLockDlg �Ի���

IMPLEMENT_DYNAMIC(CLockDlg, CDialog)

CLockDlg::CLockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLockDlg::IDD, pParent)
    , m_rcClient(DSM_LOCKDLG_RC)
    , m_pRNaviMenu(NULL)
    , m_nLockTimer(0)
    , m_nUnlockTimer(0)
    , m_bLock(true)
{
    CStatusMgr::SetScreenLock(true);
}

CLockDlg::~CLockDlg()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

void CLockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLockDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CLockDlg ��Ϣ�������

BOOL CLockDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    DSM_ParentToChild(this, m_rcClient);
    MoveWindow(m_rcClient);

    // �����˳��༭״̬
    CNaviBarMgr::GetInstance()->LeaveInplaceEdit();

    m_pRNaviMenu = new CRNaviMenu(this, IDR_MENU_NULL);

    m_nLockTimer = SetTimer(3, 3000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CLockDlg::OnPaint()
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

    CRect rcBitmap(DSM_LOCK_RCBITMAP_TIP);
    CRect rcTips(DSM_LOCK_RCTEXT_TIP);

    UINT nBmpID  = DSM_BMP_DLG_LOCK_LOCKINFO;
    UINT nTipsID = DSM_STR_LOCK_TIPS;

    // ���Ľ�����ͼƬ����Ϣ������λ��
	rcTips.OffsetRect(60, 0);
    if(!m_bLock)
    {
        nBmpID  = DSM_BMP_DLG_LOCK_UNLOCKINFO;
        nTipsID = DSM_STR_LOCK_UTIPS;

        rcBitmap.OffsetRect(-20, 0);
        //rcTips.OffsetRect(60, 0);
    }

    CDC dcBitmap;
    dcBitmap.CreateCompatibleDC(&dc);

    HGDIOBJ hOriObj = dcBitmap.SelectObject(dsm_bmpmgr::Get(nBmpID));

    CString csTips = dsm_stringMgr::Get(nTipsID);

    dc.BitBlt(rcBitmap.left, rcBitmap.top, rcBitmap.Width(), rcBitmap.Height(), &dcBitmap, 0, 0, SRCCOPY);

    int nOriDC = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(DSM_BASEPOP_FTSIZE));
    dc.SetTextColor(DSM_BASEPOP_CRTEXT); 
    //�����޸�
    /***************************************************************/
    dc.DrawText(csTips, rcTips, DT_LEFT | DT_VCENTER | DT_EXPANDTABS);
    dc.RestoreDC(nOriDC);

    dcBitmap.SelectObject(hOriObj);
    dcBitmap.DeleteDC();
}


BOOL CLockDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYUP == pMsg->message)
    {
        switch(pMsg->wParam)
        {
		case DSM_KEY_POWER:

            if(m_nLockTimer != 0)
            {
                KillTimer(m_nLockTimer);
                m_nLockTimer = 0;
            }

            if(m_bLock)
            {
                m_bLock = false;

                // �ָ����ڸ�������ͼƬ
                this->MoveWindow(m_rcClient, TRUE);
                this->Invalidate(TRUE);
                m_nUnlockTimer = SetTimer(4, 1500, NULL);

                CStatusMgr::SetScreenLock(false);
            }

            return TRUE;

        default:
            return TRUE;
        }
    }
    else if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_SNAPSHOT:
            CEventProc::GetInstance()->SnapShot();
            break;

        case DSM_KEY_POWER:
            break;

        default:
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_UNLOCK_MSG), 1000);
        }

        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CLockDlg::OnTimer( UINT_PTR nIDEvent )
{
    if(nIDEvent == m_nLockTimer)
    {
        KillTimer(m_nLockTimer);

        m_nLockTimer = 0;

        this->MoveWindow(CRect(0, 0, 0, 0), FALSE);
    }
    else if(nIDEvent == m_nUnlockTimer)
    {
        KillTimer(m_nUnlockTimer);

        m_nUnlockTimer = 0;

        EndDialog(IDOK);
    }

    CDialog::OnTimer(nIDEvent);
}

