// PasswordDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PasswordDlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/utility/dsm_utility.h"


// CPasswordDlg �Ի���

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
    , m_pProc(NULL)
    , m_rcClient(DSM_PASSWORD_RC)
    , m_pWndPrev(NULL)
{
}

CPasswordDlg::~CPasswordDlg()
{
    CNaviBarMgr::GetInstance()->LeaveInplaceEdit(m_pWndPrev);
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CPasswordDlg ��Ϣ�������


BOOL CPasswordDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    DSM_ParentToChild(this, m_rcClient);
    MoveWindow(m_rcClient);

    m_editPassword.SubclassDlgItem(IDC_PASSWORD_EDIT, this);

    CRect rcEdit(DSM_PASSWORD_RC_EDIT);

    m_rcTitle = rcEdit;

    m_rcTitle.OffsetRect(-rcEdit.Width() / 2, 0);
    rcEdit.OffsetRect(rcEdit.Width() / 2, 0);

    m_editPassword.SetFont(dsm_fontmgr::GetInstance()->GetFont(DSM_BASEPOP_FTSIZE));
    m_editPassword._SetBkColor(ELT_T1_EDITBKCOLOR);
    m_editPassword._SetTextColor(ELT_T1_EDITTEXTCOLOR);
    m_editPassword._SetBorderColor(ELT_T1_EDITBORDERCOLOR);
    // �߿�?

    m_editPassword.MoveWindow(rcEdit, FALSE);

    m_editPassword.SetPasswordChar(L'*');

    // ���ô�ֱ����
    m_editPassword._SetVCenter();

     m_editPassword.SetLimitText(8);

     m_pWndPrev = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&m_editPassword);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CPasswordDlg::OnPaint()
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

    int nOri = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SetBkColor(ELT_T1_BKCOLOR);
    dc.SetTextColor(ELT_T1_TEXTCOLOR);
    dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(DSM_BASEPOP_FTSIZE));
    dc.DrawText(dsm_stringMgr::Get(DSM_STR_PASSWORD_TITLE), m_rcTitle, DT_VCENTER |DT_CENTER | DT_SINGLELINE);
    dc.RestoreDC(nOri);
}


BOOL CPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    // ���������˵�������Ϣ
    if (CNaviBarMgr::GetInstance()->GetNaviBar() && 
        CNaviBarMgr::GetInstance()->GetNaviBar()->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    // ������home
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_HOME:
            return TRUE;

        case VK_RETURN:
            OnOK();
            return TRUE;

        case DSM_KEY_SNAPSHOT:
            CEventProc::GetInstance()->SnapShot();
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}


void CPasswordDlg::OnOK()
{
    UpdateData();

    // TODO: �ڴ����ר�ô����/����û���
    if(m_pProc && !m_pProc(m_editPassword._GetPassword(), m_dwParam))
    {
        return;
    }

    EndDialog(IDOK);
}

