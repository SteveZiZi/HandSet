/**
 * \file
 *      BasicCfgDlg.cpp
 *
 * \brief
 *      dsm基本设置实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */


// BasicCfgDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "BasicCfgDlg.h"

#include <algorithm>

#include "src/main/common.h"
#include "src/main/EventProc.h"
#include "src/main/resourcestr.h"
#include "src/main/resourcebmp.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/dlg/pcapbrowserdlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/utility/usb_function_switch.h"
#include "src/utils/commoncontrol/timeeditctrl/timeeditctrl.h"
#include "src/utils/commoncontrol/progressslider/progressslider.h"

/////////////////////////////////////////////////////////////////////////

#define WM_BASICCFG_BEGIN_EDIT          (PRIVATE_WND_USERID + 200)
#define WM_BASICCFG_EXIT_PW_CFG         (PRIVATE_WND_USERID + 201)


#define DSM_BASICCFG_MENU_BASE                  0x00        ///< 基本设置
#define DSM_BASICCFG_MENU_LINKMGR               0x01        ///< 连接管理
#define DSM_BASICCFG_MENU_STORAGE               0x02        ///< 存储器
#define DSM_BASICCFG_MENU_RESTORE               0x03        ///< 还原设置
#define DSM_BASICCFG_MENU_ABOUT                 0x04        ///< 关于


#define DSM_BASICCFG_TIME_PIC                   350
#define DSM_BASICCFG_TIME_LINK_STATUS           1000
#define DSM_BASICCFG_TIME_USBLINK_TIME          30          ///< 30次 每次一秒

#define DSM_BASICCFG_RPT_CAPTION                0
#define DSM_BASICCFG_RPT_VALUE                  1

#define DSM_BASICCFG_RPT_BRIGHTNESS             0       ///< 背光亮度
#define DSM_BASICCFG_RPT_BRIGHTTIME             1       ///< 背光时间
#define DSM_BASICCFG_RPT_TIMEDATE               2       ///< 日期时间
#define DSM_BASICCFG_RPT_TIMEZONE               3       ///< 时区设置
#define DSM_BASICCFG_RPT_TIMESYNC               4       ///< 时间同步
#define DSM_BASICCFG_RPT_PASSWORD               5       ///< 密码设置
#define DSM_BASICCFG_RPT_PASSWORD_NEW           6       ///< 新密码
#define DSM_BASICCFG_RPT_PASSWORD_CONFIRM       7       ///< 确认新密码

#define DSM_BASICFG_STR_TIME_FMT    L"%04d-%02d-%02d %02d:%02d:%02d"    ///< 时间格式化字符串


#define DSM_BASICCFG_PIC_UNLINK                 0           ///< 断开连接
#define DSM_BASICCFG_PIC_LINKED                 1           ///< 连接成功
#define DSM_BASICCFG_PIC_LINKING                2           ///< 正在连接

#define DSM_BASICCFG_TID_LINK_STATUS            2


/**
 *      密码记录项
 */
class CDsmPasswordRecord : public CELRecord
{
public:

    CDsmPasswordRecord(CString csCaption, CString csContent)
    {
        AddItem(new CELRecordItemText(csCaption));
        AddItem(new CDsmRecordItemPassword(csContent));
    }

    virtual ~CDsmPasswordRecord(void) {}
};

class CDsmPasswordNewRecord : public CELRecord
{
public:

    CDsmPasswordNewRecord(CString csCaption, CString csContent)
    {
        AddItem(new CDsmRecordItem(csCaption));
        AddItem(new CDsmRecordItemPasswordNew(csContent));
    }
    virtual ~CDsmPasswordNewRecord(void) {}
};




// CBasicCfgDlg 对话框

IMPLEMENT_DYNAMIC(CBasicCfgDlg, CBaseDlg)

CBasicCfgDlg::CBasicCfgDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CBasicCfgDlg::IDD, DSM_STR_BASIC_BASECAPTION, pParent)
    , m_bWizard(false)
    , m_pCurReport(NULL)
    , m_bModifyPassword(false)
    , m_eUSBMode(USB_UNLINK)
    , m_nTimerLinkStatus(0)
    , m_bPWNull(false)
    , m_csDefaultTZS(L"China Standard Time")
    , m_eInfo(INFO_BASE)
    , m_nCountDown(0)
{
    // 未初始化则进入向导模式
    m_bWizard = !dsm_config::GetSystemInit();

    // 加载向导菜单
    if(m_bWizard)
    {
        m_nMenuID = IDR_MENU_WIZARD;
    }
    else
    {
        m_nMenuID = IDR_MENU_SYSTEM_CFG;
        m_nIDCaption = DSM_STR_BASIC_SYSBASECAPTION;
    }
}

CBasicCfgDlg::~CBasicCfgDlg()
{
    std::vector<stTimeInfo*>::iterator it = m_vecTimezone.begin();
    std::vector<stTimeInfo*>::iterator itEnd = m_vecTimezone.end();

    for(; it != itEnd; it++)
    {
        delete *it;
    }

    m_vecTimezone.clear();
}

void CBasicCfgDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBasicCfgDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_BASICCFG_RET_BASE, &CBasicCfgDlg::OnItemValueChangedBasic)
    ON_NOTIFY(ELNM_ITEM_VALUECHANGED, IDC_BASICCFG_RET_BASE, &CBasicCfgDlg::OnItemValueChangedBasic)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_BASICCFG_RET_BASE, &CBasicCfgDlg::OnItemButtonClickBasic)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_BASICCFG_RET_BASE, &CBasicCfgDlg::OnItemSelectChangeBasic)
    ON_NOTIFY(ELNM_OKCLICK, IDC_BASICCFG_RET_RESTORE, &CBasicCfgDlg::OnItemRestoreOK)
    ON_NOTIFY(ELNM_OKCLICK, IDC_BASICCFG_RET_STORAGE, &CBasicCfgDlg::OnItemStorage)

    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_WIZARD_F6, &CBasicCfgDlg::OnNaviMenuWF6)
    ON_COMMAND(IDR_MENU_SYSTEM_F6, &CBasicCfgDlg::OnNaviMenuF6)
    ON_LBN_SELCHANGE(IDC_BASICCFG_MENUNAVI, &CBasicCfgDlg::OnMenuChanged)
    ON_NOTIFY(ELNM_ITEM_BEGINEDIT, IDC_BASICCFG_RET_BASE,  &CBasicCfgDlg::OnInplaceEditAction)
    ON_NOTIFY(ELNM_ITEM_ENDEDIT, IDC_BASICCFG_RET_BASE,  &CBasicCfgDlg::OnInplaceEditAction)
    ON_MESSAGE(WM_COMCTRL_PROGRESSSLIDER_CHANGE, &CBasicCfgDlg::OnBrightnessChange)
    ON_WM_DESTROY()
    ON_MESSAGE(WM_BASICCFG_BEGIN_EDIT, &CBasicCfgDlg::OnRptBeginEdit)
    ON_MESSAGE(WM_BASICCFG_EXIT_PW_CFG, &CBasicCfgDlg::OnExitPasswordCfg)
    ON_NOTIFY(ELNM_ITEM_EDIT_CANCELED, IDC_BASICCFG_RET_BASE, &CBasicCfgDlg::OnInplcaseEditEsc)
    ON_MESSAGE(WM_DSM_TIME_TICK, &CBasicCfgDlg::OnDMTimeTick)
END_MESSAGE_MAP()


BOOL CBasicCfgDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 加载时区信息列表
    _loadTimezone();

    // 默认进入基本设置页面
    _enterBase();

    // 初始化左侧菜单
    _initMenuNavi();

    m_menuNavi.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


// CBasicCfgDlg 消息处理程序

void CBasicCfgDlg::OnItemValueChangedBasic( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    *pResult = TRUE;
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    ASSERT(pItem);

    switch(pItemNotify->rowIndex)
    {
        // 设置时间同步方式
    case DSM_BASICCFG_RPT_TIMESYNC:
        if(pItem->GetHasCheckbox())
        {
            bool bAuto = pItem->IsChecked() ? true : false;

            dsm_config::SetTimeSync(bAuto);
            pItem->SetValue(bAuto ? dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO): dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL));

            if(bAuto)
            {
                // 立即同步本地时间
                ::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_FPGA_TO_LOCAL, 0);
            }
        }

        break;

        // 密码设置
    case DSM_BASICCFG_RPT_PASSWORD:
        {
            CString csPassword = pItem->GetValue();
            pItem->SetValue(L"");

            // 验证密码
            if(dsm_config::VerifyPasswrod(csPassword))
            {
                // 进入修改密码模式
                _enterPWMode();
            }
        }
        break;

        // 设置新密码
    case DSM_BASICCFG_RPT_PASSWORD_NEW:
        {
            CDsmRecordItemPasswordNew* pItemPWN = dynamic_cast<CDsmRecordItemPasswordNew*>(pItem);
            if(pItemPWN)
            {
                // 保存第一次新密码到变量中
                m_csPasswordNew = pItem->GetValue();
                pItemPWN->SetPasswordValue(m_csPasswordNew);
                pItem->SetValue(L"");

                m_bPWNull = m_csPasswordNew.IsEmpty() == TRUE ? true : false;
            }
        }
        break;

        // 设置确认新密码
    case DSM_BASICCFG_RPT_PASSWORD_CONFIRM:
        {
            CDsmRecordItemPasswordNew* pItemPWN = dynamic_cast<CDsmRecordItemPasswordNew*>(pItem);
            if(pItemPWN)
            {
                // 保存第二次新密码到变量中
                m_csPasswordConfirm = pItem->GetValue();
                pItem->SetValue(L"");

                m_bPWNull = m_csPasswordConfirm.IsEmpty() == TRUE ? true : false;

                // 比较两次密码是否相同，相同则设置新密码并返回初始状态
                if(!m_csPasswordConfirm.IsEmpty() && !m_csPasswordNew.IsEmpty())
                {
                    if(m_csPasswordConfirm.Compare(m_csPasswordNew) == 0)
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_PASSWORD_MODIFY_SUCCESS));

                        pItemPWN->SetPasswordValue(m_csPasswordConfirm);
                        m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_PASSWORD_CONFIRM, DSM_BASICCFG_RPT_VALUE);

                        dsm_config::SetPassword(m_csPasswordNew);
                        this->PostMessage(WM_BASICCFG_EXIT_PW_CFG, 0, 0);
                    }
                    // 两次密码输入不对，返回上一栏
                    else
                    {
                        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_PASSWORD_MODIFY_ERROR));

                        pItemPWN->SetPasswordValue(L"");
                        m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_PASSWORD_CONFIRM, DSM_BASICCFG_RPT_VALUE);

                        this->PostMessage(WM_BASICCFG_BEGIN_EDIT, MAKELONG(DSM_BASICCFG_RPT_VALUE, DSM_BASICCFG_RPT_PASSWORD_NEW));
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}

void CBasicCfgDlg::_createReport( CReportH& report, UINT nID )
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    report.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, nID);

    // 设置字体
    report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置横向报表样式
    report.PostInitTemplateStyle();

    switch(nID)
    {
    case IDC_BASICCFG_RET_BASE:
        // 插入列元素
        report.InsertColumn(0, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION - 70);   // 标题
        report.InsertColumn(1, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT + 70);   // 内容

        // 设置选择焦点样式
        report.SetRowSelectionVisible(TRUE);
        report.SetRowFocusEnable(TRUE);
        report.SetSubItemFocusEnable(TRUE);
        report.SetFreezeColumnsCount(1);
        report.SetFreezeSelectionVisible(TRUE);
        report.SetFocusedColIndex(1);

        break;

    case IDC_BASICCFG_RET_RESTORE:
    case IDC_BASICCFG_RET_STORAGE:
        // 插入列元素
        report.InsertColumn(0, L"", EL_CENTER, DSM_REPORT_WIDTH_CAPTION + DSM_REPORT_WIDTH_CONTENT);  // 内容

        // 设置选择焦点样式
        report.SetRowSelectionVisible(TRUE);
        report.SetRowFocusEnable(TRUE);
        report.SetFreezeSelectionVisible(TRUE);
        break;

    default:
        ASSERT(FALSE);
        break;
    }
}

void CBasicCfgDlg::_initReportBase()
{
    _createReport(m_reportBase, IDC_BASICCFG_RET_BASE);

    CELRecordItemEditOptions* pOptions      = NULL;
    CELRecordItemConstraint* pCon           = NULL;

    m_reportBase.GetColumnExAttr(DSM_BASICCFG_RPT_VALUE)->GetEditOptions()->SetAllowEdit(TRUE);

    // 背光亮度
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS, 
        new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_BRIGHTNESS), 
                        dsm_config::GetBrightness()));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();

    CELRecordItemText* pItemBrightness = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE));
    pItemBrightness->SetFormatString(L"%s%%");

    // 背光时间
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_BRIGHTIME), dsm_config::GetBrightTime()));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    pOptions = m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL);
    pOptions->AddComboButton();
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_1MIN), 60);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_2MIN), 120);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_5MIN), 300);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_10MIN), 600);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_30MIN), 1800);
    pOptions->AddConstraint(dsm_stringMgr::Get(DSM_STR_BASIC_BT_NEVER), 864000);

    pCon = pOptions->FindConstraint(dsm_config::GetBrightTime());
    ASSERT(pCon);
    CELRecordItemText* pItemBrighttime = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTTIME)->GetItem(DSM_BASICCFG_RPT_VALUE));

    pItemBrighttime->SetValue(pCon->GetConstraintCaption());

    SYSTEMTIME st;
    GetLocalTime(&st);
    CString csTime;
    csTime.Format(DSM_BASICFG_STR_TIME_FMT, 
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // 日期时间
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_TIMEDATE, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIME), csTime));
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();


    // 时区
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_TIMEZONE, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIMEZONE), L""));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    pOptions = m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL);
    pOptions->AddComboButton();

    TIME_ZONE_INFORMATION tzi;
    ::ZeroMemory(&tzi, sizeof(tzi));
    GetTimeZoneInformation(&tzi);

    stTimeInfo* pTIInfo = NULL;
    std::vector<stTimeInfo*>::const_iterator it = m_vecTimezone.begin();
    std::vector<stTimeInfo*>::const_iterator itEnd = m_vecTimezone.end();

    bool bInited = false;
    CString csTimezoneDisp;

    // 生成时区列表
    for(; it != itEnd; it++)
    {
        pTIInfo = *it;
        pOptions->AddConstraint(pTIInfo->csDisp, (DWORD_PTR)pTIInfo);

        CString cs(pTIInfo->csDisp);
        CString cs1(pTIInfo->tzinfo.StandardName);
        CString cs2(tzi.StandardName);

        if(!bInited &&
            0 == _tcscmp(pTIInfo->csStd, tzi.StandardName) && 
            pTIInfo->tzinfo.Bias == tzi.Bias && 
            pTIInfo->tzinfo.StandardBias == tzi.StandardBias)

        {
            bInited = true;
            csTimezoneDisp = pTIInfo->csDisp;
        }
    }

    CELRecordItemText* pItemTimezone = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEZONE)->GetItem(DSM_BASICCFG_RPT_VALUE));
    if(pItemTimezone)
    {
        pItemTimezone->SetValue(csTimezoneDisp);
    }

    bool bTimeSync = dsm_config::GetTimeAutoSync();

    // 同步方式
    CString csTimeSync = bTimeSync ? 
                    dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO) :
                    dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL);

    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_TIMESYNC, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC), csTimeSync));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->HasCheckbox(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetChecked(bTimeSync ? TRUE : FALSE);


    // 密码
//     m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_PASSWORD, new CDsmPasswordRecord(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD), L""));
//     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetFormatString(L"******");
//     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);

    m_reportBase.SetFocusedSelectionRowIndex(0);
}


void CBasicCfgDlg::_initMenuNavi()
{
    CRect rcNavi(DSM_BASICCFG_RCLEFT);
    DSM_ParentToChild(this, rcNavi);

    m_menuNavi.Create(WS_TABSTOP | WS_CHILD | WS_VISIBLE, rcNavi, this, IDC_BASICCFG_MENUNAVI);

    m_menuNavi.SetFont(dsm_fontmgr::GetInstance()->GetFont(LBT_3L_TEXTLFHEIGHT));
    m_menuNavi.SetHeight(LBT_3L_ITEMHEIGHT);
    m_menuNavi.PostInitTemplateStyle();

    // 加载左侧菜单
    m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_BASECAPTION));

    if(!m_bWizard)
    {
        m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_LINKMGRCAPTION));
        m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_STORAGECAPTION));
        m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_RESTORECAPTION));
        m_menuNavi.AddString(dsm_stringMgr::Get(DSM_STR_BASIC_ABOUTCAPTION));
    }

    m_menuNavi.SetCurSel(0);
}

void CBasicCfgDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == DSM_BASICCFG_TID_LINK_STATUS)
    {
        m_nCountDown++;
        bool bSD = DSM_STATU_DISCONNECT_SD == CStatusMgr::GetSD();

        // 设置usb连接状态为『未链接』
        if((bSD || DSM_STATU_DISCONNECT_USB == CStatusMgr::GetUSB()) && m_ctlPic._GetStatus() != DSM_BASICCFG_PIC_LINKING)
        {
            if(bSD)
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT2));
            }

            _setUnLink();
        }
        // 设置usb连接状态为『已连接』
        else if(DSM_STATU_CONNECT_USB == CStatusMgr::GetUSB())
        {
            m_ctlPic._SetStatus(DSM_BASICCFG_PIC_LINKED);
        }
        else if(m_nCountDown == DSM_BASICCFG_TIME_USBLINK_TIME)
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_USBLINK_TIMEOUT), 5000);
            _setUnLink();
        }
    }

    CBaseDlg::OnTimer(nIDEvent);
}


void CBasicCfgDlg::OnNaviMenuWF6()
{
    // 处理初始向导时『完成』

    dsm_config::SetSystemInit(true);
    CEventProc::GetInstance()->ShowMain();
}

void CBasicCfgDlg::OnMenuChanged()
{
    // 切换左侧菜单前先隐藏当前显示的报表
    if(m_pCurReport)
    {
        m_pCurReport->ShowWindow(SW_HIDE);
    }

    if(m_ctlPic.GetSafeHwnd())
    {
        m_ctlPic.ShowWindow(SW_HIDE);
    }

    if(m_menuNavi.GetCurSel() != DSM_BASICCFG_MENU_BASE &&
        m_bModifyPassword)
    {
        // 退出修改密码模式
        _leavePWMode();
    }

    // 恢复菜单
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, L"");
    m_pNaviBar->UpdateNaviMenu();

    switch(m_menuNavi.GetCurSel())
    {
        // 切换基本设置
    case DSM_BASICCFG_MENU_BASE:
        _enterBase();
        break;

        // 切换连接管理
    case DSM_BASICCFG_MENU_LINKMGR:
        _enterLinkmgr();
        break;

    case DSM_BASICCFG_MENU_STORAGE:
        _enterStorage();
        break;

        // 切换还原设置
    case DSM_BASICCFG_MENU_RESTORE:
        _enterRestore();
        break;

        // 关于
    case DSM_BASICCFG_MENU_ABOUT:
        _enterAbout();
        break;

    default:
        break;
    }
}

BOOL CBasicCfgDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_RIGHT:
            // 焦点在左侧菜单，则移动焦点到右侧控件
            if(GetFocus()->GetSafeHwnd() == m_menuNavi.GetSafeHwnd() &&
                m_pCurReport)
            {
                if(m_pCurReport->GetStyle() & WS_TABSTOP)
                {
                    m_pCurReport->SetFocus();
                }
            }
            return TRUE;

        case VK_LEFT:
            // 焦点在右侧报表，则移动焦点到左侧菜单
            if(GetFocus()->GetSafeHwnd() != m_menuNavi.GetSafeHwnd())
            {
                m_menuNavi.SetFocus();
            }
            return TRUE;
        }

        // 若在U盘模式，则屏蔽
        if(USB_LINKED == m_eUSBMode)
        {
            // 屏蔽除了F6之外所有按键
            switch(pMsg->wParam)
            {
            case VK_F6:
                return CBaseDlg::PreTranslateMessage(pMsg);

            default:
                return TRUE;

            }
        }
    }

    return CBaseDlg::PreTranslateMessage(pMsg);
}


void CBasicCfgDlg::_enterBase()
{
    if(m_reportBase.GetSafeHwnd())
    {
        m_reportBase.ShowWindow(SW_SHOW);
    }
    else
    {
        _initReportBase();
    }

    // 更新标题
    _updateCaption(DSM_STR_BASIC_BASECAPTION);

    m_pCurReport = &m_reportBase;
}


void CBasicCfgDlg::_enterLinkmgr()
{
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_LINK_START));

    // 更新标题
    _updateCaption(DSM_STR_BASIC_LINKMGRCAPTION);

    m_pCurReport = NULL;

    _showPic();
}

void CBasicCfgDlg::_enterRestore()
{
    if(m_reportRestore.GetSafeHwnd())
    {
        m_reportRestore.ShowWindow(SW_SHOW);
    }
    else
    {
        _initReportRestore();
    }

    // 更新标题
    _updateCaption(DSM_STR_BASIC_RESTORECAPTION);

    m_pCurReport = &m_reportRestore;
}


void CBasicCfgDlg::_initReportRestore()
{
    _createReport(m_reportRestore, IDC_BASICCFG_RET_RESTORE);

    m_reportRestore.InsertRecordRow(0, dsm_stringMgr::Get(DSM_STR_BASIC_RT_FACTORY));

    m_reportRestore.GetRecordRow(0)->GetItem(0)->SetEditable(TRUE);
    m_reportRestore.GetRecordRow(0)->GetItem(0)->GetEditOptions(NULL)->AddExpandButton();

    m_reportRestore.SetFocusedSelectionRowIndex(0);
}

void CBasicCfgDlg::_initReportAbout()
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    DWORD dwStyle = (DSM_WINSTYLE_REPORT_DEFAULT) & ~WS_TABSTOP;
    m_reportAbout.Create(dwStyle, rcReport, this, IDC_BASICCFG_RET_ABOUT);

    // 设置字体
    m_reportAbout.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置报表样式
    m_reportAbout.PostInitTemplateStyle();
    m_reportAbout.SetRowFocusEnable(FALSE);

    CString csSoftVer;
    csSoftVer.Format(L"%s.%s", dsm_config::GetSoftwareVersion(), dsm_config::GetSoftwareBuild());

    // 从配置加载信息
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_PRODUCT_NAME, CReportAbout::COL_DESC, dsm_config::GetProductName());
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_HW_VER, CReportAbout::COL_DESC, dsm_config::GetHardwareVersion());
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_SYS_VER, CReportAbout::COL_DESC, dsm_config::GetSystemVersion());
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_SW_VER, CReportAbout::COL_DESC, csSoftVer);
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_COMPANY_NAME, CReportAbout::COL_DESC, dsm_config::GetCompanyName());
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_COMPANY_ADDR, CReportAbout::COL_DESC, dsm_config::GetCompanyAddress());
    m_reportAbout.SetAboutItemText(CReportAbout::ROW_SUPPORT, CReportAbout::COL_DESC, dsm_config::GetTelephoneSupport());
}


void CBasicCfgDlg::_enterAbout()
{
    CString csMenu(dsm_stringMgr::Get(DSM_STR_BASIC_DEVICEBASE));

    // 基本信息
    if(m_eInfo == INFO_BASE)
    {
        if(m_reportAbout.GetSafeHwnd())
        {
            m_reportAbout.ShowWindow(SW_SHOW);
        }
        else
        {
            _initReportAbout();
        }

        m_pCurReport = &m_reportAbout;
        csMenu = dsm_stringMgr::Get(DSM_STR_BASIC_DEVICEINFO);
    }
    else
    {
        if(m_reportDetail.GetSafeHwnd())
        {
            m_reportDetail.ShowWindow(SW_SHOW);
        }
        else
        {
            _initReportDetail();
        }

        m_pCurReport = &m_reportDetail;
    }

    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, csMenu);
    m_pNaviBar->UpdateNaviMenu();

    // 更新标题
    _updateCaption(DSM_STR_BASIC_ABOUTCAPTION);
}


void CBasicCfgDlg::_loadTimezone()
{
    HKEY hKey;
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Time Zones", 0, 0, &hKey))
    {
        ASSERT(FALSE);
        return ;
    }

    DWORD dwDefaultType             = REG_NONE;
    DWORD dwDefaultLen              = MAX_PATH;
    wchar_t szDefaultTZS[MAX_PATH]  = {0};

    // 获取默认时区项名
    if(ERROR_SUCCESS == RegQueryValueEx(hKey, L"Default", NULL, &dwDefaultType, (LPBYTE)szDefaultTZS, &dwDefaultLen) && REG_SZ == dwDefaultType)
    {
        m_csDefaultTZS = szDefaultTZS;
    }

    DWORD dwIndex               = 0;
    DWORD dwLen                 = MAX_PATH;
    wchar_t szKeyName[MAX_PATH] = {0};

    HKEY hSubKey;
    DWORD dwSubLen;
    DWORD dwSubType;
    wchar_t szDlt[MAX_PATH] = {0};
    wchar_t szStd[MAX_PATH] = {0};
    wchar_t szDisplayValue[MAX_PATH] = {0};
    TIME_ZONE_INFORMATION tziSub;

    stTimeInfo* pInfo = NULL;

    // 遍历所有有效时区
    while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex, szKeyName, &dwLen, NULL, NULL, NULL, NULL))
    {
        dwIndex++;
        dwLen = MAX_PATH;

        // 打开指定时区
        if(ERROR_SUCCESS != RegOpenKeyEx(hKey, szKeyName, 0, 0, &hSubKey))
        {
            continue;
        }

        dwSubLen = sizeof(tziSub);
        ZeroMemory(&tziSub, dwSubLen);

        // 获取TIME_ZONE_INFORMATION二进制值
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"TZI", NULL, &dwSubType, (LPBYTE)&tziSub, &dwSubLen) || 
            REG_BINARY != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // 获取Display值，界面显示值
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Display", NULL, &dwSubType, (LPBYTE)szDisplayValue, &dwSubLen) || 
            REG_SZ != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // 获取Dlt值
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Dlt", NULL, &dwSubType, (LPBYTE)szDlt, &dwSubLen) || 
            REG_SZ != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // 获取Std值
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Std", NULL, &dwSubType, (LPBYTE)szStd, &dwSubLen) || 
            REG_SZ != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        RegCloseKey(hSubKey);

        pInfo               = new stTimeInfo;
        pInfo->csDisp       = szDisplayValue;
        pInfo->csDlt        = szDlt;
        pInfo->csStd        = szStd;
        pInfo->tzinfo       = tziSub;
        pInfo->csKeyName    = szKeyName;

        m_vecTimezone.push_back(pInfo);
    }

    RegCloseKey(hKey);

    _adjustTimezone();
}

void CBasicCfgDlg::_adjustTimezone()
{
    _qsort(0, m_vecTimezone.size() - 1);
}

void CBasicCfgDlg::_qsort( int nleft, int nRight )
{
    if(nleft >= nRight)
    {
        return;
    }

    int nIndex = _partition(nleft, nRight);

    _qsort(nleft, nIndex -1);
    _qsort(nIndex + 1, nRight);
}

int CBasicCfgDlg::_partition( int nLeft, int nRight )
{
    int nIndex  = nLeft;

    stTimeInfo *pPivot = m_vecTimezone[nIndex];

    std::swap(m_vecTimezone[nIndex], m_vecTimezone[nRight]);

    for(int i = nLeft; i < nRight; i++)
    {
        if(m_vecTimezone[i]->tzinfo.Bias < pPivot->tzinfo.Bias)
        {
            std::swap(m_vecTimezone[nIndex++], m_vecTimezone[i]);
        }
    }

    std::swap(m_vecTimezone[nRight], m_vecTimezone[nIndex]);

    return nIndex;
}

void CBasicCfgDlg::OnItemSelectChangeBasic( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;

    if(!pItemNotify || ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        return;
    }

    *pResult = TRUE;

    switch(pItemNotify->rowIndex)
    {
        // 设置背光时间
    case DSM_BASICCFG_RPT_BRIGHTTIME:
            CStatusMgr::SetBrightTime(pItemNotify->pConstraint->GetConstraintData());
        break;

        // 设置时区
    case DSM_BASICCFG_RPT_TIMEZONE:
        {
            TIME_ZONE_INFORMATION tzi;
            ::ZeroMemory(&tzi, sizeof(tzi));
            GetTimeZoneInformation(&tzi);

            stTimeInfo* pTI = (stTimeInfo*)pItemNotify->pConstraint->GetConstraintData();

            TIME_ZONE_INFORMATION tziNew = pTI->tzinfo;

            _tcsncpy(tziNew.DaylightName, pTI->csDlt, 32);
            _tcsncpy(tziNew.StandardName, pTI->csStd, 32);

            SYSTEMTIME st1;
            GetSystemTime(&st1);

            if(SetTimeZoneInformation(&tziNew))
            {
                SetSystemTime(&st1);
            }
            else
            {
                SetTimeZoneInformation(&tzi);
            }

            RegFlushKey(HKEY_LOCAL_MACHINE);
        }
        break;

    default:
        break;
    }
}


void CBasicCfgDlg::OnItemButtonClickBasic( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

    if(pItemNotify)
    {
        switch(pItemNotify->rowIndex)
        {
            // 设置背光亮度
        case DSM_BASICCFG_RPT_BRIGHTNESS:
            _modifyBrightness(pItemNotify, pResult);
            break;

            // 设置时间
        case DSM_BASICCFG_RPT_TIMEDATE:
            _modifyTime(pItemNotify, pResult);
            break;

        default:
            return;
        }
    }
}

void CBasicCfgDlg::_modifyTime( ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult )
{
    *pResult = TRUE;

    CTimeEditCtrl ctrlTime;
    CRect rcItem = m_reportBase.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

    ctrlTime.SetBkColor(DSM_REPORT_COLOR_BK);
    ctrlTime.SetTextColor(DSM_REPORT_COLOR_TEXT);
    ctrlTime.SetBorderColor(LBT_FB_BORDERCOLOR);

    SYSTEMTIME st;
    GetLocalTime(&st);

    CWnd* pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(&ctrlTime);

    // 弹出时间设置控件
    if(IDOK == ctrlTime.GoModal(&m_reportBase, rcItem, st))
    {
        CString csTime;
        csTime.Format(DSM_BASICFG_STR_TIME_FMT, 
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
        pItem->SetValue(csTime);

        SetLocalTime(&st);

        // 立刻同步FPGA时间
        ::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_LOCAL_TO_FPGA, 0);

        m_reportBase.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
    }

    CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);
}

void CBasicCfgDlg::_modifyBrightness( ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult )
{
    *pResult = TRUE;

    CProgressSlider ctrlSlider;
    CRect rcItem = m_reportBase.GetRowItemRect(pItemNotify->rowIndex, pItemNotify->colIndex);

    ctrlSlider._SetVal(dsm_config::GetBrightness());
    ctrlSlider._SetRange(0, 100);
    ctrlSlider._SetRangeOp(10, 100);
    ctrlSlider._SetDisplayFmt(L"%d%%");
    ctrlSlider._SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    ctrlSlider._SetProgressColor(LBT_FB2_BKCOLOR, LBT_FB_HLCOLOR, LBT_FB_TEXTCOLOR);

    // 弹出亮度设置进度条
    if (IDOK == ctrlSlider._GoModal(rcItem, &m_reportBase))
    {
        int nVal = ctrlSlider._GetVal();

        CStatusMgr::SetBrightness((BYTE)nVal);

        CString csBrightness;
        csBrightness.Format(L"%d", nVal);

        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
        pItem->SetValue(csBrightness);
        m_reportBase.RedrawRowItem(pItemNotify->rowIndex, pItemNotify->colIndex);
    }
}

void CBasicCfgDlg::_updateCaption( UINT nCaption )
{
    CString csCaption;

    if(m_bWizard)
    {
        csCaption.Format(L"%s - %s", 
            dsm_stringMgr::Get(DSM_STR_BASIC_WIZARD),
            dsm_stringMgr::Get(nCaption));
    }
    else
    {

        csCaption.Format(L"%s - %s", 
            dsm_stringMgr::Get(DSM_STR_BASIC_SYSTEMCFG), 
            dsm_stringMgr::Get(nCaption));
    }

    CStatusMgr::SetTitle(csCaption);
}

void CBasicCfgDlg::OnNaviMenuF6()
{
    // 切换usb模式
    if(m_menuNavi.GetCurSel() == DSM_BASICCFG_MENU_LINKMGR)
    {
        // 连接usb
        if(USB_UNLINK == m_eUSBMode)
        {
            // 检查SD卡状态
            if(CStatusMgr::GetSD() == DSM_STATU_CONNECT_SD)
            {
                _setLink();
            }
            else
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT));
            }
        }
        // 断开usb
        else
        {
            _setUnLink();
        }
    }
    // 关于页面
    else if(m_menuNavi.GetCurSel() == DSM_BASICCFG_MENU_ABOUT)
    {
        m_pCurReport->ShowWindow(SW_HIDE);

        if(m_eInfo == INFO_BASE)
        {
            m_eInfo         = INFO_DETAIL;
        }
        else
        {
            m_eInfo         = INFO_BASE;
        }

        _enterAbout();
    }
}

void CBasicCfgDlg::_enterPWMode()
{
    m_bModifyPassword = true;

    // 修改菜单显示 『更新密码』
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_M_MODIFY_PW));

    // 插入两行新密码row
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW, new CDsmPasswordNewRecord(L"", L""));
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM, new CDsmPasswordNewRecord(L"", L""));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);

     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW)->GetItem(DSM_BASICCFG_RPT_CAPTION)->SetCaption(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD_NEW));
     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM)->GetItem(DSM_BASICCFG_RPT_CAPTION)->SetCaption(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD_CONFIRM));
}

void CBasicCfgDlg::_leavePWMode()
{
    // 退出修改密码模式
    m_bModifyPassword       = false;
    m_csPasswordNew         = L"";
    m_csPasswordConfirm     = L"";

    // 恢复菜单显示 『』
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, L"");

    int nFocusRow = m_reportBase.GetFocusedRowIndex();

    if(DSM_BASICCFG_RPT_PASSWORD_NEW == nFocusRow||
        DSM_BASICCFG_RPT_PASSWORD_CONFIRM == nFocusRow)
    {
        m_reportBase.SetFocusedSelectionRowIndex(DSM_BASICCFG_RPT_PASSWORD);
    }

    // 从尾行开始删除新建的两行新密码record
    if(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM))
    {
        m_reportBase.DeleteRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM);
    }

    if(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW))
    {
        m_reportBase.DeleteRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW);
    }
}

void CBasicCfgDlg::OnInplaceEditAction( NMHDR * pNotifyStruct, LRESULT* pResult )
{
    *pResult = TRUE;

    static CWnd* pSrc = NULL;

    ELNM_INPLACELIST_ARGS*  pItemNotify = (ELNM_INPLACELIST_ARGS*) pNotifyStruct;

    if (pItemNotify->hdr.code == ELNM_ITEM_BEGINEDIT)
    {
        // inplaceEdit弹出时, 进入编辑菜单
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_pCurReport->GetInplaceEdit());


        // 设置『密码设置』栏的标题 『请输入旧密码』
        if(pItemNotify->rowIndex == DSM_BASICCFG_RPT_PASSWORD)
        {
            CELRecordItemText* pText = dynamic_cast<CELRecordItemText*>
                (m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD)->GetItem(DSM_BASICCFG_RPT_CAPTION));

            pText->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD_OLD));
            m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_PASSWORD, DSM_BASICCFG_RPT_CAPTION);
        }
    }
    else if (pItemNotify->hdr.code == ELNM_ITEM_ENDEDIT)
    {
        // inplace退出时，退出编辑菜单
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);


        // 恢复『密码设置』的标题
        //# ELNM_ITEM_ENDEDIT没有row col信息
        int nFocus = m_reportBase.GetSelectionRowIndex();

        if(DSM_BASICCFG_RPT_PASSWORD == nFocus)
        {
            CELRecordItemText* pText = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD)->GetItem(DSM_BASICCFG_RPT_CAPTION));
            pText->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD));
            m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_PASSWORD, DSM_BASICCFG_RPT_CAPTION);
        }

        // 在设置密码状态时自动切换输入框
        if(m_bModifyPassword)
        {
            // 判断是否为空字符串
            if((nFocus == DSM_BASICCFG_RPT_PASSWORD_NEW && m_bPWNull) || (nFocus == DSM_BASICCFG_RPT_PASSWORD_CONFIRM && m_bPWNull))
            {
                // 继续获得焦点
                this->PostMessage(WM_BASICCFG_BEGIN_EDIT, MAKELONG(DSM_BASICCFG_RPT_VALUE, nFocus));
                return;
            }

            if(nFocus == DSM_BASICCFG_RPT_PASSWORD || nFocus == DSM_BASICCFG_RPT_PASSWORD_NEW)
            {
                // 下一行密码框获取焦点
                this->PostMessage(WM_BASICCFG_BEGIN_EDIT, MAKELONG(DSM_BASICCFG_RPT_VALUE, nFocus + 1));
            }
        }
    }
}


LRESULT CBasicCfgDlg::OnBrightnessChange( WPARAM wParam, LPARAM /* lParam */ )
{
    CStatusMgr::SetBrightness((BYTE)wParam);

    CString csBrightness;
    csBrightness.Format(L"%d", wParam);

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE));
    pItem->SetValue(csBrightness);

    return 1;
}


LRESULT CBasicCfgDlg::OnRptBeginEdit( WPARAM wParam, LPARAM /* lParam */ )
{
    // wParam高位 row， 低位 col
    int nRow(HIWORD(wParam));   //, nCol(LOWORD(wParam));

    m_bPWNull = true;

    if(nRow >= 0 && nRow < m_reportBase.GetRecordRowCount())
    {
        m_reportBase.SetFocusedSelectionRowIndex(nRow);
        m_reportBase.KeyBeginEdit();
    }

    return 1;
}


void CBasicCfgDlg::OnItemRestoreOK( NMHDR* /*pNotifyStruct*/, LRESULT * pResult )
{
    *pResult = TRUE;

    CMsgDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, 0, this);
    msgdlg._SetBitmapDisp(false);

    HWND hWnd = ::GetFocus();

    INT_PTR nRet = msgdlg.DoModal();
    msgdlg._ResetMenu();

    if(nRet == IDOK)
    {
        // 设置默认时区
        _setDefaultTZS();

        // 还原出厂设置
        CEventProc::GetInstance()->ResetFactory();

        return;
    }
    ::SetFocus(hWnd);
}

void CBasicCfgDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    if(m_nTimerLinkStatus != 0)
    {
        KillTimer(m_nTimerLinkStatus);
        m_nTimerLinkStatus = 0;
    }
}


void CBasicCfgDlg::_setLink()
{
    m_eUSBMode = USB_LINKED;

    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_LINK_STOP));
    m_pNaviBar->UpdateNaviMenu();

    // 开启定时器检测usb连接状态是否断开
    m_nTimerLinkStatus = SetTimer(DSM_BASICCFG_TID_LINK_STATUS, DSM_BASICCFG_TIME_LINK_STATUS, NULL);
    m_nCountDown = 0;

    // 设置大容量存储模式
    HANDLE hUSBctrl = GetUfnController();
    if(ChangeClient(hUSBctrl, L"Mass_Storage_Class") != TRUE)
    {
        DSM_DEBUG(L"dsm: set usb Mass Storage Class mode failed\n");
    }

    CloseHandle(hUSBctrl);

    m_ctlPic._SetStatus(DSM_BASICCFG_PIC_LINKING);
}

void CBasicCfgDlg::_setUnLink()
{
    m_eUSBMode = USB_UNLINK;

    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_LINK_START));
    m_pNaviBar->UpdateNaviMenu();

    // 关闭轮询usb状态定时器
    if(m_nTimerLinkStatus)
    {
        KillTimer(m_nTimerLinkStatus);
        m_nTimerLinkStatus = 0;
    }

    // 禁用大容量存储模式
    HANDLE hUSBctrl = GetUfnController();
    if(ChangeClient(hUSBctrl, L"") != TRUE)
    {
        DSM_DEBUG(L"dsm: set usb RNDIS mode failed\n");
    }

    CloseHandle(hUSBctrl);

    m_ctlPic._SetStatus(DSM_BASICCFG_PIC_UNLINK);
}

void CBasicCfgDlg::_showPic()
{
    if(m_ctlPic.GetSafeHwnd())
    {
        m_ctlPic.ShowWindow(SW_SHOW);
        return;
    }

    m_ctlPic._SetTimeInterval(500);

    // 失去连接状态
    std::vector<HBITMAP> v;
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_LINK_FAILED));
    m_ctlPic._InsertStatus(DSM_BASICCFG_PIC_UNLINK, v);

    // 连接成功状态
    v.clear();
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_LINK_SUCCESS));
    m_ctlPic._InsertStatus(DSM_BASICCFG_PIC_LINKED, v);

    // 正在连接状态
    v.clear();
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_PIC_1));
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_PIC_2));
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_PIC_3));
    m_ctlPic._InsertStatus(DSM_BASICCFG_PIC_LINKING, v);

    v.clear();

    HBITMAP hLinked = dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_LINK_SUCCESS);

    BITMAP bm;
    ::GetObject(hLinked, sizeof(bm), &bm);
    CRect rcRight(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcRight);

    // 计算图片空间区域
    CRect rcPic;
    rcPic.left   = rcRight.left + (rcRight.Width() - bm.bmWidth) / 2;
    rcPic.right  = rcPic.left + bm.bmWidth;
    rcPic.top    = (rcRight.Height() - bm.bmHeight) / 2;
    rcPic.bottom = rcPic.top + bm.bmHeight;

    m_ctlPic.Create(NULL, WS_CHILD | WS_VISIBLE, rcPic, this, IDC_BASICCFG_PIC);

    // 设置usb初始状态
    m_ctlPic._SetStatus(DSM_BASICCFG_PIC_UNLINK);
}

LRESULT CBasicCfgDlg::OnExitPasswordCfg( WPARAM /* wParam */, LPARAM /* lParam */ )
{
    _leavePWMode();

    return 1;
}

void CBasicCfgDlg::OnInplcaseEditEsc( NMHDR * pNotifyStruct, LRESULT* pResult )
{
    ELNM_INPLACEBUTTON_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

    if(pItemNotify)
    {
        // 从密码修改中esc退出时，应退出『密码修改』模式
        if(pItemNotify->rowIndex == DSM_BASICCFG_RPT_PASSWORD_NEW || pItemNotify->rowIndex == DSM_BASICCFG_RPT_PASSWORD_CONFIRM)
        {
            this->PostMessage(WM_BASICCFG_EXIT_PW_CFG, 0, 0);

            *pResult = 1;

            return;
        }
    }

    *pResult = 0;
}

/**
 *      设置默认时区
 */
void CBasicCfgDlg::_setDefaultTZS()
{
    std::vector<stTimeInfo*>::iterator it       = m_vecTimezone.begin();
    std::vector<stTimeInfo*>::iterator itEnd    = m_vecTimezone.end();

    for(; it != itEnd; it++)
    {
        if((*it)->csKeyName.CompareNoCase(m_csDefaultTZS) == 0)
        {
            TIME_ZONE_INFORMATION tzi;
            ::ZeroMemory(&tzi, sizeof(tzi));
            GetTimeZoneInformation(&tzi);

            TIME_ZONE_INFORMATION tziNew = (*it)->tzinfo;

            _tcsncpy(tziNew.DaylightName, (*it)->csDlt, 32);
            _tcsncpy(tziNew.StandardName, (*it)->csStd, 32);

            if(FALSE == SetTimeZoneInformation(&tziNew))
            {
                SetTimeZoneInformation(&tzi);
            }

            RegFlushKey(HKEY_LOCAL_MACHINE);

            return;
        }
    }
}

/**
 *      创建并初始化关于报表的详细信息
 */
void CBasicCfgDlg::_initReportDetail()
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    DWORD dwStyle = (DSM_WINSTYLE_REPORT_DEFAULT) & ~WS_TABSTOP;
    m_reportDetail.Create(dwStyle, rcReport, this, IDC_DEVICE_INFO_RET);

    // 设置报表
    m_reportDetail.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.PostInitTemplateStyle();
    m_reportDetail.SetRowFocusEnable(FALSE);

    m_reportDetail.InsertColumn(0, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION);   // 标题
    m_reportDetail.InsertColumn(1, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);   // 内容

    // 设置设备详细信息

    CString csGPM(CDsmDataLayer::getInstance()->GetGPMDriverVersion().c_str());
    if(iswdigit(csGPM.GetAt(0)) == 0)
    {
        csGPM = csGPM.Right(csGPM.GetLength() - 1);
    }

    CString csFpga(CDsmDataLayer::getInstance()->GetGPMFPGAVersion().c_str());

    CString csSoftVer;
    csSoftVer.Format(L"%s.%s", dsm_config::GetSoftwareVersion(), dsm_config::GetSoftwareBuild());

    CString csDSPVer;
    csDSPVer.Format(L"0.%d", CDsmDataLayer::getInstance()->GetDspVersion());

    int i = 0;
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_HARDVER), dsm_config::GetHardwareVersion()));
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_SYSVER), dsm_config::GetSystemVersion()));
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_SOFTVER), csSoftVer));
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_GPMDRIVERVER), csGPM.MakeUpper()));
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_FPGAVER), csFpga.MakeUpper()));
    m_reportDetail.InsertRecordRow(i++, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_DSPVER), csDSPVer));
}

LRESULT CBasicCfgDlg::OnDMTimeTick( WPARAM, LPARAM )
{
    if(m_menuNavi.GetCurSel() == DSM_BASICCFG_MENU_BASE && m_pCurReport == &m_reportBase && m_reportBase.GetSafeHwnd())
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        CString csTime;
        csTime.Format(DSM_BASICFG_STR_TIME_FMT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(1));
        pItem->SetValue(csTime);

        m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_TIMEDATE, 1);
    }

    return 1;
}

void CBasicCfgDlg::_enterStorage()
{
    if(m_reportStorage.GetSafeHwnd())
    {
        m_reportStorage.ShowWindow(SW_SHOW);
    }
    else
    {
        _initReportStorage();
    }

    // 更新标题
    _updateCaption(DSM_STR_BASIC_STORAGECAPTION);

    m_pCurReport = &m_reportStorage;
}

void CBasicCfgDlg::_initReportStorage()
{
    _createReport(m_reportStorage, IDC_BASICCFG_RET_STORAGE);

    m_reportStorage.InsertRecordRow(0, dsm_stringMgr::Get(DSM_STR_BASIC_RECORDANALY));

    m_reportStorage.GetRecordRow(0)->GetItem(0)->SetEditable(TRUE);
    m_reportStorage.GetRecordRow(0)->GetItem(0)->GetEditOptions(NULL)->AddExpandButton();

    m_reportStorage.SetFocusedSelectionRowIndex(0);
}

void CBasicCfgDlg::OnItemStorage( NMHDR* /*pNotifyStruct*/, LRESULT * pResult )
{
    *pResult = TRUE;

    if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
    {
        // 未插入SD卡
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT));
        return;
    }

    // 加载pcap文件
    CString csExt;
    csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
    CDsmFileDlg dlg(0, L"\\Storage Card\\pcap\\", L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

    CString csTitle(CStatusMgr::GetTitle());

    HWND hWnd       = ::GetFocus();
    INT_PTR nRet    = dlg.DoModal();
    if(nRet == IDOK)
    {
        CString csTitle = CStatusMgr::GetTitle();

        // 加载pcap包
        CPcapBrowserDlg dlg(dlg._GetFilePath());
        nRet = dlg.DoModal();
    }

    if(nRet == IDBACKTOMAIN)
    {
        return;
    }

    if(IsWindow(hWnd))
    {
        CStatusMgr::SetTitle(csTitle);
        ::SetFocus(hWnd);
    }
    else
    {
        DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    }
}

