// MsgBtnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MsgBtnDlg.h"

#include "src/main/eventproc.h"
#include "src/main/resourcebmp.h"
#include "src/main/utils/rnavimenu.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/resourcestr.h"

#if SHENG_ZE
#define DSM_BTN_RECT1		CRect(107,120,147,145)
#define DSM_BTN_RECT2		CRect(197,120,237,145)
#else
#define DSM_BTN_RECT1		CRect(46,115,138,149)
#define DSM_BTN_RECT2		CRect(185,115,277,149)
#endif
//#define	DSM_BTN_RECT3		CRect(346,65,386,90)
// CMsgBtnDlg dialog

IMPLEMENT_DYNAMIC(CMsgBtnDlg, CDialog)

CMsgBtnDlg::CMsgBtnDlg(UINT nMsgID, UINT nBitmapID /* = 0 */, CWnd* pParent /*=NULL*/)
	: CDialog(CMsgBtnDlg::IDD, pParent)
	, m_rcClient(DSM_MSGDLG_RC)
	, m_pNaviBar(NULL)
	, m_rcPic(DSM_MSGDLG_RCBITMAP_TIP)
	, m_rcText(DSM_MSGDLG_RCTEXT_TIP)
	, m_nMenuID(IDR_MENU_MSG)
	, m_nBegin(0)
	, m_nEnd(0)
	, m_pFunc(NULL)
	, m_pData(NULL)
	, m_bBitmap(true)
	, m_pRNaviMenu(NULL)
	, m_bEnableCountDown(false)
	, m_btnNum2(L"确定")
	, m_btnCancel(L"取消")
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

CMsgBtnDlg::~CMsgBtnDlg()
{
	 _ResetMenu();
}

void CMsgBtnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_NUM2, m_btnNum2);
	DDX_Control(pDX, IDC_BTN_NUM3, m_btnCancel);
}



// CMsgBtnDlg message handlers
BEGIN_MESSAGE_MAP(CMsgBtnDlg, CDialog)
    ON_WM_PAINT()
    ON_COMMAND(ID_MENU_MSG_F5, &CMsgBtnDlg::OnMenuF5)
    ON_COMMAND(ID_MENU_MSG_F6, &CMsgBtnDlg::OnMenuF6)

	ON_BN_CLICKED(IDC_BTN_NUM2, &CMsgBtnDlg::OnClickBtnNum2)
	ON_BN_CLICKED(IDC_BTN_NUM3, &CMsgBtnDlg::OnClickBtnCancel)
END_MESSAGE_MAP()


// CMsgDlg 消息处理程序

BOOL CMsgBtnDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	//m_btnNum2.SubclassDlgItem(IDC_BTN_NUM2, this);
	//m_btnCancel.SubclassDlgItem(IDC_BTN_NUM3, this);


    DSM_ParentToChild(this, m_rcClient);
    MoveWindow(m_rcClient);
	

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
    ASSERT(m_pNaviBar);

    m_pRNaviMenu = new CRNaviMenu(this, m_nMenuID);
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(4, m_F5Text);
	m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, m_F6Text);
	
#if SHENG_ZE
	//设置按钮的纯色图片
	m_btnNum2._SetColorMode(RGB(0xFF, 0x80, 0x80), RGB(0x73, 0xD3, 0xBD), RGB(0x42, 0xBE, 0xA5), RGB(0x00, 0x00, 0x00));
	m_btnCancel._SetColorMode(RGB(0xFF, 0x80, 0x80), RGB(0x73, 0xD3, 0xBD), RGB(0x42, 0xBE, 0xA5), RGB(0x00, 0x00, 0x00));
#else
	m_btnNum2._SetBmp(DSM_BMP_OK_UNSEL, 
		DSM_BMP_OK_SEL, 
		DSM_BMP_OK_SEL);
	m_btnCancel._SetBmp(DSM_BMP_CANSEL_UNSEL, 
		DSM_BMP_CANSEL_SEL, 
		DSM_BMP_CANSEL_SEL);
#endif
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

void CMsgBtnDlg::OnPaint()
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



	/* pwnd = GetDlgItem(IDC_BTN_NUM1);
	pwnd->MoveWindow(DSM_BTN_RECT1, FALSE);*/

	CWnd* pwnd  = GetDlgItem(IDC_BTN_NUM2);
	pwnd->MoveWindow(DSM_BTN_RECT1, TRUE);

	pwnd  = GetDlgItem(IDC_BTN_NUM3);
	pwnd->MoveWindow(DSM_BTN_RECT2, TRUE);
}


void CMsgBtnDlg::OnMenuF5()
{
	/*if (m_F5Text.IsEmpty() )
	{
		return;
	}*/
    _ResetMenu();

    EndDialog(IDCANCEL);
}

void CMsgBtnDlg::OnMenuF6()
{
	/*if (m_F6Text.IsEmpty())
	{
		return;
	}*/
    _ResetMenu();

    EndDialog(IDOK);
}


BOOL CMsgBtnDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
	
	///< 传给数字键处理消息
	if (pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		
		case VK_ESCAPE:
			_ResetMenu();
			EndDialog(IDCANCEL);
			break;
		default:
			break;
		}
		/*switch(pMsg->wParam)
		{
		case _T('2'):
		case VK_NUMPAD2:
			SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BTN_NUM2, BN_CLICKED), NULL);
			break;
		case _T('3'):
		case VK_NUMPAD3:
			SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BTN_NUM3, BN_CLICKED), NULL);
			break;
		default:
			break;
		}*/
	}


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

BOOL CMsgBtnDlg::OnCommand(WPARAM wParam, LPARAM lParam)
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

BOOL CMsgBtnDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
void CMsgBtnDlg::_ResetMenu()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}


void CMsgBtnDlg::OnClickBtnNum2()
{
	OnMenuF6();
}

void CMsgBtnDlg::OnClickBtnCancel()
{
	OnMenuF5();
}