// CDIDOStateDlg 消息处理程序
/// @file
///
/// @brief
///     显示开入开出量 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "DIDOStateDlg.h"
#include "src/utils/res/dsm_bmpmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/resourcebmp.h"
#include "src/main/common.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"

#define DSM_RC_DIDO_WIDTH            529
#define DSM_RC_DIDO_HEIGHT           50
#define DSM_RC_IMAGE_SIZE_X          26
#define DSM_RC_IMAGE_SIZE_Y          22

//#define DSM_RC_STATE_TOP             CRect(30, 84,  30+DSM_RC_DIDO_WIDTH, 84+DSM_RC_DIDO_HEIGHT)
#define DSM_RC_STATE_TOP             CRect(30, 333-DSM_RC_DIDO_HEIGHT, 30+DSM_RC_DIDO_WIDTH, 333)
#define DSM_RC_STATE_BOTTOM          CRect(110, 444-DSM_RC_DIDO_HEIGHT, 110+DSM_RC_DIDO_WIDTH, 444)
#define DSM_DC_BKG_COLOR             RGB(217, 223, 231)

#define DSM_RC_DI_CTRL_POS           CRect(0, 0, 300, DSM_RC_DIDO_HEIGHT)
#define DSM_RC_DO_CTRL_POS           CRect(310, 0, 525, DSM_RC_DIDO_HEIGHT)
#define DSM_RC_DI_WIN_SIZE           CRect(0, 0, DSM_RC_DIDO_WIDTH, DSM_RC_DIDO_HEIGHT)

#define DSM_STATE_FONT_SIZE          -13


enum eDIDOState
{
    eDIInit = 0,
    eDIOff,
    eDIOn,
    eDIBad,
    eDIDisable,
};

enum eDIDOWinPosition
{
    eDIDOWinPosTop = 0,
    eDIDOWinPosBottom,
};

// CDIValueDlg 对话框

IMPLEMENT_DYNAMIC(CDIDOStateDlg, CDialog)

CDIDOStateDlg::CDIDOStateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDIDOStateDlg::IDD, pParent)
, m_eWinPosition(eDIDOWinPosBottom)
, m_pStatus(NULL)
, m_pConfigGooseSend(NULL)
{

}

CDIDOStateDlg::~CDIDOStateDlg()
{
}

void CDIDOStateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDIDOStateDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_MESSAGE(WM_DI_CHANGED, &CDIDOStateDlg::OnDIChanged)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDIValueDlg 消息处理程序

BOOL CDIDOStateDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    m_pConfigGooseSend = CParamConfigGOOSESend::getInstance();
    
    if (m_pConfigGooseSend == NULL)
    {
        return FALSE;
    }
    else
    {
        m_pConfigGooseSend->RebuildGOOSESendChannelMap();
    }

    ModifyStyleEx(0, WS_EX_NOACTIVATE);

    CRect rcRect(DSM_RC_STATE_BOTTOM);
    DSM_ParentToChild(this, rcRect);
    MoveWindow(rcRect);
    m_eWinPosition = eDIDOWinPosBottom;

    m_bmpInit.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_INIT));
    m_bmpOff.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_OFF));
    m_bmpOn.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_ON));
    m_bmpBad.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_BAD));
    m_bmpDisable.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_DISABLE));

    _createDIStateCtl();
    _createDOStateCtl();
    SetTimer(1, 400, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CDIDOStateDlg::_createDIStateCtl()
{

    m_ctlDIState.Create(NULL, WS_CHILD|WS_VISIBLE, DSM_RC_DI_CTRL_POS, this, IDC_DI_STATE_CTRL);
    m_ctlDIState.ModifyStyle(0,SS_OWNERDRAW);
    m_ctlDIState.SetOnOffDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_STATE_FONT_SIZE));
    m_ctlDIState.SetBkgndColor(DSM_DC_BKG_COLOR);
    m_ctlDIState.SetOnOffDICount(8, 1);
    m_ctlDIState.SetMargin(20,2,2,2);
    m_ctlDIState.SetOnOffStyleShowSize(DSM_RC_IMAGE_SIZE_X, DSM_RC_IMAGE_SIZE_Y);
    m_ctlDIState.SetOnOffBoard(FALSE);

    m_ctlDIState.SetOnOffStyle(eDIInit, &m_bmpInit);
    m_ctlDIState.SetOnOffStyle(eDIOff, &m_bmpOff);
    m_ctlDIState.SetOnOffStyle(eDIOn, &m_bmpOn);
    m_ctlDIState.SetOnOffStyle(eDIBad, &m_bmpBad);
    m_ctlDIState.SetOnOffStyle(eDIDisable, &m_bmpDisable);

    m_ctlDIState.Init();
    m_ctlDIState.ShowWindow(SW_SHOW);

    m_ctlDIState.SetOnOffState(0, eDIDisable, strGOOSERecvVCName[0].c_str());
    m_ctlDIState.SetOnOffState(1, eDIDisable, strGOOSERecvVCName[1].c_str());
    m_ctlDIState.SetOnOffState(2, eDIDisable, strGOOSERecvVCName[2].c_str());
    m_ctlDIState.SetOnOffState(3, eDIDisable, strGOOSERecvVCName[3].c_str());
    m_ctlDIState.SetOnOffState(4, eDIDisable, strGOOSERecvVCName[4].c_str());
    m_ctlDIState.SetOnOffState(5, eDIDisable, strGOOSERecvVCName[5].c_str());
    m_ctlDIState.SetOnOffState(6, eDIDisable, strGOOSERecvVCName[6].c_str());
    m_ctlDIState.SetOnOffState(7, eDIDisable, strGOOSERecvVCName[7].c_str());
    UpdateDIStatus();

    m_ctlDIState.Invalidate(TRUE);
}

void CDIDOStateDlg::_createDOStateCtl()
{
    m_ctlDOState.Create(NULL, WS_CHILD|WS_VISIBLE, DSM_RC_DO_CTRL_POS, this, IDC_DO_STATE_CTRL);
    m_ctlDOState.ModifyStyle(0,SS_OWNERDRAW);
    m_ctlDOState.SetOnOffDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_STATE_FONT_SIZE));
    m_ctlDOState.SetBkgndColor(DSM_DC_BKG_COLOR);
    m_ctlDOState.SetOnOffDICount(6, 1);
    m_ctlDOState.SetMargin(2,2,2,2);
    m_ctlDOState.SetOnOffStyleShowSize(DSM_RC_IMAGE_SIZE_X, DSM_RC_IMAGE_SIZE_Y);
    m_ctlDOState.SetOnOffBoard(FALSE);

    m_ctlDOState.SetOnOffStyle(eDIInit, &m_bmpInit);
    m_ctlDOState.SetOnOffStyle(eDIOff, &m_bmpOff);
    m_ctlDOState.SetOnOffStyle(eDIOn, &m_bmpOn);
    m_ctlDOState.SetOnOffStyle(eDIBad, &m_bmpBad);
    m_ctlDOState.SetOnOffStyle(eDIDisable, &m_bmpDisable);

    m_ctlDOState.Init();
    m_ctlDOState.ShowWindow(SW_SHOW);

    UpdateDOStatus();
}
void CDIDOStateDlg::MoveWindowBottom()
{
    if (m_eWinPosition != eDIDOWinPosBottom)
    {
        CRect rcRect(DSM_RC_STATE_BOTTOM);
        AfxGetMainWnd()->ClientToScreen(rcRect);
        MoveWindow(rcRect);
        m_eWinPosition = eDIDOWinPosBottom;
    }
}

void CDIDOStateDlg::MoveWindowTop()
{
    if (m_eWinPosition != eDIDOWinPosTop)
    {
        CRect rcRect(DSM_RC_STATE_TOP);
        AfxGetMainWnd()->ClientToScreen(rcRect);
        MoveWindow(rcRect);
        m_eWinPosition = eDIDOWinPosTop;
    }
}

void CDIDOStateDlg::UpdateDIStatus()
{
    CParamConfigGOOSERecv* pConfigGooseRecv = CParamConfigGOOSERecv::getInstance();
    if (pConfigGooseRecv == NULL)
    {
        return;
    }
    CString strDIName = _T("");
    CGOOSERecvVirtualChannel* pVirChannel = NULL;
    CChannel* pChannel = NULL;

    GOOSE_DATA_VARIANT  gooseDataVar;
    for (int i = 0 ;  i < 8; ++i)
    {
        strDIName = strGOOSERecvVCName[i].c_str();

        pVirChannel = pConfigGooseRecv->GetVirtualChannelByName((LPCTSTR)strDIName);
        if (pVirChannel)
        {
            pChannel = pVirChannel->GetChannel();
            if (pChannel)
            {
                gooseDataVar = pChannel->GetGooseRecvValue();
                if (gooseDataVar.GetType() == GSEDT_BOOLEAN)
                {
                    //单点类型
                    if (gooseDataVar.m_stBoolean.boolVal)
                    {
                        //on
                        m_ctlDIState.SetOnOffState(i, eDIOn, strDIName);
                    }
                    else
                    {
                        //off
                        m_ctlDIState.SetOnOffState(i, eDIOff, strDIName);
                    }
                }
                else if (gooseDataVar.GetType() == GSEDT_BIT_STR)
                {
                    if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
                    {
                        m_ctlDIState.SetOnOffState(i, eDIInit, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
                    {
                        m_ctlDIState.SetOnOffState(i, eDIOff, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
                    {
                        m_ctlDIState.SetOnOffState(i, eDIOn, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
                    {
                        m_ctlDIState.SetOnOffState(i, eDIBad, strDIName);
                    }
                }
            }
        }
    }
    m_ctlDIState.DrawOnOffState();
    m_ctlDIState.Invalidate(TRUE);
}


void CDIDOStateDlg::UpdateDOStatus(CStatus* pStatus)
{
    m_pStatus = pStatus;

    if (m_pStatus == NULL)
    {
        m_ctlDOState.SetOnOffState(0, eDIDisable, PREDEFINED_DO_INFO[0].m_strName.c_str());
        m_ctlDOState.SetOnOffState(1, eDIDisable, PREDEFINED_DO_INFO[1].m_strName.c_str());
        m_ctlDOState.SetOnOffState(2, eDIDisable, PREDEFINED_DO_INFO[2].m_strName.c_str());
        m_ctlDOState.SetOnOffState(3, eDIDisable, PREDEFINED_DO_INFO[3].m_strName.c_str());
        m_ctlDOState.SetOnOffState(4, eDIDisable, PREDEFINED_DO_INFO[4].m_strName.c_str());
        m_ctlDOState.SetOnOffState(5, eDIDisable, PREDEFINED_DO_INFO[5].m_strName.c_str());
    }
    else
    {
        for (int i = 0; i < 6; ++i)
        {
            CGOOSESendVirtualChannel* pVirChannel = m_pConfigGooseSend->GetGOOSESendVirtualChannel(PREDEFINED_DO_INFO[i].m_strName.c_str());
            if (pVirChannel && pVirChannel->IsMaped())
            {
                if (pStatus->GetDOValue(PREDEFINED_DO_INFO[i].m_strName))
                {
                    m_ctlDOState.SetOnOffState(i, DP_VALUE_ON, PREDEFINED_DO_INFO[i].m_strName.c_str());
                }
                else
                {
                    m_ctlDOState.SetOnOffState(i, DP_VALUE_OFF, PREDEFINED_DO_INFO[i].m_strName.c_str());
                }
            }
            else
            {
                m_ctlDOState.SetOnOffState(i, eDIDisable, PREDEFINED_DO_INFO[i].m_strName.c_str());
            }
        }
    }
    m_ctlDOState.DrawOnOffState();
    m_ctlDOState.Invalidate(TRUE);
}
LRESULT CDIDOStateDlg::OnDIChanged( WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    //
    //DI数据更新
    //
    UpdateDIStatus();
    return 1;
}
BOOL CDIDOStateDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    m_bmpInit.Detach();
    m_bmpOff.Detach();
    m_bmpOn.Detach();
    m_bmpBad.Detach();
    m_bmpDisable.Detach();

    return CDialog::DestroyWindow();
}


BOOL CDIDOStateDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    //// 传给导航菜单处理消息

    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
                OnCancel();
                return TRUE;
            }
        case VK_F7:
            {
                //结束本对话框
                EndDialog(IDOK);
                //OnCancel();
                return AfxGetMainWnd()->PreTranslateMessage(pMsg);
            }
        }
    }

    if(AfxGetMainWnd()->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    return this->GetParent()->PreTranslateMessage(pMsg);
}

void CDIDOStateDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();
    //CDialog::OnCancel();
}

void CDIDOStateDlg::OnSize(UINT nType, int /*cx*/, int /*cy*/)
{
    CDialog::OnSize(nType, DSM_RC_DIDO_WIDTH, DSM_RC_DIDO_HEIGHT);

    // TODO: 在此处添加消息处理程序代码
}

void CDIDOStateDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        UpdateDIStatus();
    }
    CDialog::OnTimer(nIDEvent);
}
BOOL CDIDOStateDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    RECT     rect;
    CBrush   br;
    br.CreateSolidBrush(DSM_DC_BKG_COLOR);
    GetClientRect( &rect );       //取得客户区边界
    pDC->FillRect( &rect, &br );  //用刚才创建的刷子画客户区

    return TRUE;
    //return CDialog::OnEraseBkgnd(pDC);
}
