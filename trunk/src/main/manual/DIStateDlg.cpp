/// @file
///
/// @brief
///     显示开入量 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "DIStateDlg.h"
#include "src/utils/res/dsm_bmpmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/resourcebmp.h"
#include "src/main/common.h"
#include "src/service/datalayer/config/ParamConfigGOOSERecv.h"

//手动试验- 开入量
//
#define DSM_RC_DI_WIDTH              640 - 110
#define DSM_RC_DI_HEIGHT             50
#define DSM_RC_DI_IMAGE_SIZE_X       26
#define DSM_RC_DI_IMAGE_SIZE_Y       22

#define DSM_RC_DI_TOP       CRect(110, 84,  640, 84+DSM_RC_DI_HEIGHT)
#define DSM_RC_DI_BOTTOM    CRect(110, 444-DSM_RC_DI_HEIGHT, 640, 444)
#define DSM_RC_DI_WIN_SIZE  CRect(0, 0, DSM_RC_DI_WIDTH, DSM_RC_DI_HEIGHT)

#define DSM_DI_FONT_SIZE                         -13

enum eDIState
{
    eDIInit = 0,
    eDIOff,
    eDIOn,
    eDIBad,
    eDIDisable,
};

enum eDIWinPosition
{
    eDIWinPosTop = 0,
    eDIWinPosBottom,
};

// CDIValueDlg 对话框

IMPLEMENT_DYNAMIC(CDIStateDlg, CDialog)

CDIStateDlg::CDIStateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDIStateDlg::IDD, pParent)
    , m_eWinPosition(eDIWinPosBottom)
{

}

CDIStateDlg::~CDIStateDlg()
{
}

void CDIStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDIStateDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_MESSAGE(WM_DI_CHANGED, &CDIStateDlg::OnDIChanged)
END_MESSAGE_MAP()


// CDIValueDlg 消息处理程序

BOOL CDIStateDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    ModifyStyleEx(0, WS_EX_NOACTIVATE);

    CRect rcRect(DSM_RC_DI_BOTTOM);
    DSM_ParentToChild(this, rcRect);
    MoveWindow(rcRect);
    m_eWinPosition = eDIWinPosBottom;
    
    _createDIStateCtl();
    
    SetTimer(1, 400, NULL);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CDIStateDlg::_createDIStateCtl()
{

    m_ctlOnOffState.Create(NULL, WS_CHILD|WS_VISIBLE, DSM_RC_DI_WIN_SIZE, this, IDC_DI_STATE_CTRL);
    m_ctlOnOffState.ModifyStyle(0,SS_OWNERDRAW);
    m_ctlOnOffState.SetOnOffDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_DI_FONT_SIZE));
    m_ctlOnOffState.SetBkgndColor(RGB(217, 223, 231));
    m_ctlOnOffState.SetOnOffDICount(8, 1);
    m_ctlOnOffState.SetMargin(110,2,2,2);
    m_ctlOnOffState.SetOnOffStyleShowSize(DSM_RC_DI_IMAGE_SIZE_X, DSM_RC_DI_IMAGE_SIZE_Y);
    m_ctlOnOffState.SetOnOffBoard(FALSE);

    m_bmpInit.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_INIT));
    m_ctlOnOffState.SetOnOffStyle(eDIInit, &m_bmpInit);

    m_bmpOff.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_OFF));
    m_ctlOnOffState.SetOnOffStyle(eDIOff, &m_bmpOff);

    m_bmpOn.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_ON));
    m_ctlOnOffState.SetOnOffStyle(eDIOn, &m_bmpOn);

    m_bmpBad.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_BAD));
    m_ctlOnOffState.SetOnOffStyle(eDIBad, &m_bmpBad);

    m_bmpDisable.Attach(dsm_bmpmgr::Get(DSM_BMP_DI_DISABLE));
    m_ctlOnOffState.SetOnOffStyle(eDIDisable, &m_bmpDisable);

    m_ctlOnOffState.Init();
    m_ctlOnOffState.ShowWindow(SW_SHOW);

    m_ctlOnOffState.SetOnOffState(0, eDIDisable, strGOOSERecvVCName[0].c_str());
    m_ctlOnOffState.SetOnOffState(1, eDIDisable, strGOOSERecvVCName[1].c_str());
    m_ctlOnOffState.SetOnOffState(2, eDIDisable, strGOOSERecvVCName[2].c_str());
    m_ctlOnOffState.SetOnOffState(3, eDIDisable, strGOOSERecvVCName[3].c_str());
    m_ctlOnOffState.SetOnOffState(4, eDIDisable, strGOOSERecvVCName[4].c_str());
    m_ctlOnOffState.SetOnOffState(5, eDIDisable, strGOOSERecvVCName[5].c_str());
    m_ctlOnOffState.SetOnOffState(6, eDIDisable, strGOOSERecvVCName[6].c_str());
    m_ctlOnOffState.SetOnOffState(7, eDIDisable, strGOOSERecvVCName[7].c_str());
    UpdateDIStatus();

    m_ctlOnOffState.Invalidate(TRUE);
}

void CDIStateDlg::MoveWindowBottom()
{
    if (m_eWinPosition != eDIWinPosBottom)
    {
        CRect rcRect(DSM_RC_DI_BOTTOM);
        AfxGetMainWnd()->ClientToScreen(rcRect);
        MoveWindow(rcRect);
        m_eWinPosition = eDIWinPosBottom;
    }
}

void CDIStateDlg::MoveWindowTop()
{
    if (m_eWinPosition != eDIWinPosTop)
    {
        CRect rcRect(DSM_RC_DI_TOP);
        AfxGetMainWnd()->ClientToScreen(rcRect);
        MoveWindow(rcRect);
        m_eWinPosition = eDIWinPosTop;
    }
}

void CDIStateDlg::UpdateDIStatus()
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
                        m_ctlOnOffState.SetOnOffState(i, eDIOn, strDIName);
                    }
                    else
                    {
                        //off
                        m_ctlOnOffState.SetOnOffState(i, eDIOff, strDIName);
                    }
                }
                else if (gooseDataVar.GetType() == GSEDT_BIT_STR)
                {
                    if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_INIT])
                    {
                        m_ctlOnOffState.SetOnOffState(i, eDIInit, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_OFF])
                    {
                        m_ctlOnOffState.SetOnOffState(i, eDIOff, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_ON])
                    {
                        m_ctlOnOffState.SetOnOffState(i, eDIOn, strDIName);
                    }
                    else if (gooseDataVar.m_stBitString.GetString() == strDPValue[DP_VALUE_BAD])
                    {
                        m_ctlOnOffState.SetOnOffState(i, eDIBad, strDIName);
                    }
                }
            }
        }
    }
    m_ctlOnOffState.DrawOnOffState();
    m_ctlOnOffState.Invalidate(TRUE);
}

LRESULT CDIStateDlg::OnDIChanged( WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    //
    //DI数据更新
    //
    UpdateDIStatus();
    return 1;
}
BOOL CDIStateDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    m_bmpInit.Detach();
    m_bmpOff.Detach();
    m_bmpOn.Detach();
    m_bmpBad.Detach();
    m_bmpDisable.Detach();

    return CDialog::DestroyWindow();
}


BOOL CDIStateDlg::PreTranslateMessage(MSG* pMsg)
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

void CDIStateDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();
    //CDialog::OnCancel();
}

void CDIStateDlg::OnSize(UINT nType, int /*cx*/, int /*cy*/)
{
    CDialog::OnSize(nType, DSM_RC_DI_WIDTH, DSM_RC_DI_HEIGHT);

    // TODO: 在此处添加消息处理程序代码
}

void CDIStateDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        UpdateDIStatus();
    }
    CDialog::OnTimer(nIDEvent);
}
