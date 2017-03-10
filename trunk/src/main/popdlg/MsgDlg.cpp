// MsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MsgDlg.h"


#include "src/main/eventproc.h"
#include "src/main/resourcebmp.h"
#include "src/main/utils/rnavimenu.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/utility/dsm_utility.h"




// CMsgDlg 对话框

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(UINT nMsgID, UINT nBitmapID /* = 0 */, CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
    , m_rcClient(DSM_MSGDLG_RC)
    , m_pNaviBar(NULL)
    , m_rcPic(DSM_MSGDLG_RCBITMAP_TIP_Z)
    , m_rcText(DSM_MSGDLG_RCTEXT_TIP)
    , m_nMenuID(IDR_MENU_MSG)
    , m_nBegin(0)
    , m_nEnd(0)
    , m_pFunc(NULL)
    , m_pData(NULL)
    , m_bBitmap(true)
    , m_pRNaviMenu(NULL)
    , m_bEnableCountDown(false)
{
    HBITMAP bitmap  = dsm_bmpmgr::Get(nBitmapID);
    m_csMsg         = dsm_stringMgr::Get(nMsgID);

    if(bitmap && m_bBitmap)
    {
        std::vector<HBITMAP> v;
        v.push_back(bitmap);
        m_ctlPic._InsertStatus(1, v);
    }
}

CMsgDlg::~CMsgDlg()
{
    _ResetMenu();
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
    ON_WM_PAINT()
#if !SHENG_ZE
	ON_COMMAND(ID_MENU_MSG_F1, &CMsgDlg::OnMenuF1)
#endif
    ON_COMMAND(ID_MENU_MSG_F5, &CMsgDlg::OnMenuF5)
    ON_COMMAND(ID_MENU_MSG_F6, &CMsgDlg::OnMenuF6)
END_MESSAGE_MAP()


// CMsgDlg 消息处理程序

BOOL CMsgDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    DSM_ParentToChild(this, m_rcClient);
    MoveWindow(m_rcClient);

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
    ASSERT(m_pNaviBar);

    m_pRNaviMenu = new CRNaviMenu(this, m_nMenuID);
#if !SHENG_ZE
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(0, m_F1Text);
#endif
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, m_F5Text);
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, m_F6Text);
    // 创建图片控件
    if(m_bBitmap)
    {
        m_ctlPic.Create(NULL, WS_CHILD | WS_VISIBLE, m_rcPic, this, IDC_MSG_ICON);
        m_ctlPic._SetStatus(1);
    }

    // 创建倒计时控件
    if(m_bEnableCountDown)
    {
        m_ctlCountDown.Create(NULL, WS_CHILD | WS_VISIBLE, DSM_MSGDLG_RCCOUNTDOWN_TIP, this, IDC_MSG_COUNTDOWN);
        m_ctlCountDown._Init();
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CMsgDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    CRect rcSerifA1(rcClient.left, rcClient.top, rcClient.right,  rcClient.top + 1);
	CRect rcSerifA2(rcClient.right - 1, rcClient.top, rcClient.right,  rcClient.bottom);
    CRect rcSerifB1(rcClient.left, rcClient.top, rcClient.left + 1,  rcClient.bottom);
	CRect rcSerifB2(rcClient.left, rcClient.bottom - 1, rcClient.right,  rcClient.bottom);

    CRect rcBody(rcClient);
    rcBody.DeflateRect(0, 0, 0, 0);

    dc.FillSolidRect(rcBody, DSM_BASEPOP_CRBK);
	
	
    // 黑线+边距 body 边距
    dc.FillSolidRect(rcSerifA1, DSM_BASEPOP_CRSERIFA);      // 统一宏
	dc.FillSolidRect(rcSerifA2, DSM_BASEPOP_CRSERIFA);
    dc.FillSolidRect(rcSerifB1, DSM_BASEPOP_CRSERIFB);
    dc.FillSolidRect(rcSerifB2, DSM_BASEPOP_CRSERIFB);
	
	
    int nOriDC = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(DSM_BASEPOP_FTSIZE));
    dc.SetTextColor(DSM_BASEPOP_CRTEXT);
    dc.DrawText(m_csMsg, m_rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
    dc.RestoreDC(nOriDC);
}

#if !SHENG_ZE
void CMsgDlg::OnMenuF1()
{
	if (m_F1Text.IsEmpty() )
	{
		return;
	}
	_ResetMenu();

	EndDialog(IDCANCEL);
}
#endif

void CMsgDlg::OnMenuF5()
{
	if (m_F5Text.IsEmpty() )
	{
		return;
	}
    _ResetMenu();

    EndDialog(IDCANCEL);
}

void CMsgDlg::OnMenuF6()
{
	if (m_F6Text.IsEmpty())
	{
		return;
	}
    _ResetMenu();

    EndDialog(IDOK);
}


BOOL CMsgDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类


    // 传给导航菜单处理消息
    if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if(pMsg->message == WM_KEYDOWN && pMsg->wParam == DSM_KEY_SNAPSHOT)
    {
        CEventProc::GetInstance()->SnapShot();
        return TRUE;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CMsgDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(IDR_MENU_MSG != m_nMenuID)
    {
        UINT nMenuID = LOWORD(wParam);

        if(nMenuID >= m_nBegin && nMenuID <= m_nEnd && m_pFunc)
        {
            m_pFunc(wParam, lParam, m_pData);

            return TRUE;
        }
    }

    return CDialog::OnCommand(wParam, lParam);
}

BOOL CMsgDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(IDR_MENU_MSG != m_nMenuID && nID >= m_nBegin && nID <= m_nEnd && nCode == CN_UPDATE_COMMAND_UI)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/**
 *      手动还原菜单
 */
void CMsgDlg::_ResetMenu()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

void CMsgDlg::_ResetF5Text(CString text)
{
	m_F5Text = text;
}

void CMsgDlg::_ResetF6Text(CString text)
{
	m_F6Text = text;
}

#if !SHENG_ZE
void CMsgDlg::_ResetF1Text(CString text)
{
	m_F1Text = text;
}
#endif
