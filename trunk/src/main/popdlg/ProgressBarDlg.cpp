// ProgressBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProgressBarDlg.h"

#include "src/main/eventproc.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/rnavimenu.h"
#include "src/main/popdlg/common_pop.h"
#include "src/utils/utility/dsm_utility.h"

// CProgressBarDlg 对话框

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialog)


CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressBarDlg::IDD, pParent)
    , m_pTask(NULL)
    , m_csTips(L"loading")
    , m_pArgs(NULL)
    , m_hThread(NULL)
    , m_pRNaviMenu(NULL)
    , m_rcText(DSM_PROGRESS_BAR_RCTEXT)
    , m_rcPic(DSM_PROGRESS_BAR_RCBITMAP)
    , m_nMenuID(IDR_MENU_NULL)
    , m_pNaviBar(NULL)
    , m_nBegin(0)
    , m_nEnd(0)
    , m_pFunc(NULL)
    , m_pData(NULL)
{
    m_csTitle       = CStatusMgr::GetTitle();
    m_csTitlePrev   = m_csTitle;
}

CProgressBarDlg::~CProgressBarDlg()
{
    CStatusMgr::SetTitle(m_csTitlePrev);

    ::CloseHandle(m_hThread);

    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialog)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CProgressBarDlg 消息处理程序

BOOL CProgressBarDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CRect rcRect(DSM_PROGRESS_BAR_RC);
    AfxGetMainWnd()->ClientToScreen(rcRect);
    MoveWindow(rcRect);

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();
    ASSERT(m_pNaviBar);

    m_pRNaviMenu = new CRNaviMenu(this, m_nMenuID);

    CStatusMgr::SetTitle(m_csTitle);

    std::vector<HBITMAP> v;

    for(int i = 0; i < 12; i++)
    {
        v.push_back(dsm_bmpmgr::Get(DSM_BMP_LOADING_1 + i));
    }

    m_ctlPic._InsertStatus(1, v);
    m_ctlPic._SetTimeInterval(80);

    // 创建图片控件
    m_ctlPic.Create(NULL, WS_CHILD | WS_VISIBLE, m_rcPic, this, IDC_PROGRESS_ICON);
    m_ctlPic._SetStatus(1);

    if(m_pTask)
    {
        m_hThread = CreateThread(NULL, 0, m_pTask, m_pArgs, 
            STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CProgressBarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    CRect rcSerifA1(rcClient.left, rcClient.top, rcClient.right,  rcClient.top + 1);
    CRect rcSerifB1(rcClient.left, rcClient.top + 1, rcClient.right,  rcClient.top + 2);

    CRect rcSerifB2(rcClient.left, rcClient.bottom - 1, rcClient.right,  rcClient.bottom);

    CRect rcBody(rcClient);
    rcBody.DeflateRect(0, 2, 0, 1);

    dc.FillSolidRect(rcBody, DSM_BASEPOP_CRBK);

    // 黑线+边距 body 边距
    dc.FillSolidRect(rcSerifA1, DSM_BASEPOP_CRSERIFA);      // 统一宏
    dc.FillSolidRect(rcSerifB1, DSM_BASEPOP_CRSERIFB);
    dc.FillSolidRect(rcSerifB2, DSM_BASEPOP_CRSERIFB);

    int nOriDC = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(DSM_BASEPOP_FTSIZE));
    dc.SetTextColor(DSM_BASEPOP_CRTEXT);
    dc.DrawText(m_csTips, m_rcText, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
    dc.RestoreDC(nOriDC);
}


BOOL CProgressBarDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(IDR_MENU_NULL != m_nMenuID && nID >= m_nBegin && nID <= m_nEnd && nCode == CN_UPDATE_COMMAND_UI)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }


    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CProgressBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(IDR_MENU_NULL != m_nMenuID)
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

BOOL CProgressBarDlg::PreTranslateMessage(MSG* pMsg)
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
