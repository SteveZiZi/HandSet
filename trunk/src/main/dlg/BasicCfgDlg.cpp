/**
 * \file
 *      BasicCfgDlg.cpp
 *
 * \brief
 *      dsm��������ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/8
 */


// BasicCfgDlg.cpp : ʵ���ļ�
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


#define DSM_BASICCFG_MENU_BASE                  0x00        ///< ��������
#define DSM_BASICCFG_MENU_LINKMGR               0x01        ///< ���ӹ���
#define DSM_BASICCFG_MENU_STORAGE               0x02        ///< �洢��
#define DSM_BASICCFG_MENU_RESTORE               0x03        ///< ��ԭ����
#define DSM_BASICCFG_MENU_ABOUT                 0x04        ///< ����


#define DSM_BASICCFG_TIME_PIC                   350
#define DSM_BASICCFG_TIME_LINK_STATUS           1000
#define DSM_BASICCFG_TIME_USBLINK_TIME          30          ///< 30�� ÿ��һ��

#define DSM_BASICCFG_RPT_CAPTION                0
#define DSM_BASICCFG_RPT_VALUE                  1

#define DSM_BASICCFG_RPT_BRIGHTNESS             0       ///< ��������
#define DSM_BASICCFG_RPT_BRIGHTTIME             1       ///< ����ʱ��
#define DSM_BASICCFG_RPT_TIMEDATE               2       ///< ����ʱ��
#define DSM_BASICCFG_RPT_TIMEZONE               3       ///< ʱ������
#define DSM_BASICCFG_RPT_TIMESYNC               4       ///< ʱ��ͬ��
#define DSM_BASICCFG_RPT_PASSWORD               5       ///< ��������
#define DSM_BASICCFG_RPT_PASSWORD_NEW           6       ///< ������
#define DSM_BASICCFG_RPT_PASSWORD_CONFIRM       7       ///< ȷ��������

#define DSM_BASICFG_STR_TIME_FMT    L"%04d-%02d-%02d %02d:%02d:%02d"    ///< ʱ���ʽ���ַ���


#define DSM_BASICCFG_PIC_UNLINK                 0           ///< �Ͽ�����
#define DSM_BASICCFG_PIC_LINKED                 1           ///< ���ӳɹ�
#define DSM_BASICCFG_PIC_LINKING                2           ///< ��������

#define DSM_BASICCFG_TID_LINK_STATUS            2


/**
 *      �����¼��
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




// CBasicCfgDlg �Ի���

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
    // δ��ʼ���������ģʽ
    m_bWizard = !dsm_config::GetSystemInit();

    // �����򵼲˵�
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

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ����ʱ����Ϣ�б�
    _loadTimezone();

    // Ĭ�Ͻ����������ҳ��
    _enterBase();

    // ��ʼ�����˵�
    _initMenuNavi();

    m_menuNavi.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


// CBasicCfgDlg ��Ϣ�������

void CBasicCfgDlg::OnItemValueChangedBasic( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    *pResult = TRUE;
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_INPLACEBUTTON_ARGS*) pNotifyStruct;

    CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
    ASSERT(pItem);

    switch(pItemNotify->rowIndex)
    {
        // ����ʱ��ͬ����ʽ
    case DSM_BASICCFG_RPT_TIMESYNC:
        if(pItem->GetHasCheckbox())
        {
            bool bAuto = pItem->IsChecked() ? true : false;

            dsm_config::SetTimeSync(bAuto);
            pItem->SetValue(bAuto ? dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO): dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL));

            if(bAuto)
            {
                // ����ͬ������ʱ��
                ::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_DSM_SYNC_NOW, DSM_SYNC_FPGA_TO_LOCAL, 0);
            }
        }

        break;

        // ��������
    case DSM_BASICCFG_RPT_PASSWORD:
        {
            CString csPassword = pItem->GetValue();
            pItem->SetValue(L"");

            // ��֤����
            if(dsm_config::VerifyPasswrod(csPassword))
            {
                // �����޸�����ģʽ
                _enterPWMode();
            }
        }
        break;

        // ����������
    case DSM_BASICCFG_RPT_PASSWORD_NEW:
        {
            CDsmRecordItemPasswordNew* pItemPWN = dynamic_cast<CDsmRecordItemPasswordNew*>(pItem);
            if(pItemPWN)
            {
                // �����һ�������뵽������
                m_csPasswordNew = pItem->GetValue();
                pItemPWN->SetPasswordValue(m_csPasswordNew);
                pItem->SetValue(L"");

                m_bPWNull = m_csPasswordNew.IsEmpty() == TRUE ? true : false;
            }
        }
        break;

        // ����ȷ��������
    case DSM_BASICCFG_RPT_PASSWORD_CONFIRM:
        {
            CDsmRecordItemPasswordNew* pItemPWN = dynamic_cast<CDsmRecordItemPasswordNew*>(pItem);
            if(pItemPWN)
            {
                // ����ڶ��������뵽������
                m_csPasswordConfirm = pItem->GetValue();
                pItem->SetValue(L"");

                m_bPWNull = m_csPasswordConfirm.IsEmpty() == TRUE ? true : false;

                // �Ƚ����������Ƿ���ͬ����ͬ�����������벢���س�ʼ״̬
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
                    // �����������벻�ԣ�������һ��
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

    // ��������
    report.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ���ú��򱨱���ʽ
    report.PostInitTemplateStyle();

    switch(nID)
    {
    case IDC_BASICCFG_RET_BASE:
        // ������Ԫ��
        report.InsertColumn(0, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION - 70);   // ����
        report.InsertColumn(1, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT + 70);   // ����

        // ����ѡ�񽹵���ʽ
        report.SetRowSelectionVisible(TRUE);
        report.SetRowFocusEnable(TRUE);
        report.SetSubItemFocusEnable(TRUE);
        report.SetFreezeColumnsCount(1);
        report.SetFreezeSelectionVisible(TRUE);
        report.SetFocusedColIndex(1);

        break;

    case IDC_BASICCFG_RET_RESTORE:
    case IDC_BASICCFG_RET_STORAGE:
        // ������Ԫ��
        report.InsertColumn(0, L"", EL_CENTER, DSM_REPORT_WIDTH_CAPTION + DSM_REPORT_WIDTH_CONTENT);  // ����

        // ����ѡ�񽹵���ʽ
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

    // ��������
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS, 
        new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_BRIGHTNESS), 
                        dsm_config::GetBrightness()));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();

    CELRecordItemText* pItemBrightness = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_BRIGHTNESS)->GetItem(DSM_BASICCFG_RPT_VALUE));
    pItemBrightness->SetFormatString(L"%s%%");

    // ����ʱ��
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

    // ����ʱ��
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_TIMEDATE, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIME), csTime));
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMEDATE)->GetItem(DSM_BASICCFG_RPT_VALUE)->GetEditOptions(NULL)->AddExpandButton();


    // ʱ��
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

    // ����ʱ���б�
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

    // ͬ����ʽ
    CString csTimeSync = bTimeSync ? 
                    dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_AUTO) :
                    dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC_MANUAL);

    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_TIMESYNC, new CDsmBasicRecord(dsm_stringMgr::Get(DSM_STR_BASIC_TIMESYNC), csTimeSync));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->HasCheckbox(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_TIMESYNC)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetChecked(bTimeSync ? TRUE : FALSE);


    // ����
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

    // �������˵�
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == DSM_BASICCFG_TID_LINK_STATUS)
    {
        m_nCountDown++;
        bool bSD = DSM_STATU_DISCONNECT_SD == CStatusMgr::GetSD();

        // ����usb����״̬Ϊ��δ���ӡ�
        if((bSD || DSM_STATU_DISCONNECT_USB == CStatusMgr::GetUSB()) && m_ctlPic._GetStatus() != DSM_BASICCFG_PIC_LINKING)
        {
            if(bSD)
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT2));
            }

            _setUnLink();
        }
        // ����usb����״̬Ϊ�������ӡ�
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
    // �����ʼ��ʱ����ɡ�

    dsm_config::SetSystemInit(true);
    CEventProc::GetInstance()->ShowMain();
}

void CBasicCfgDlg::OnMenuChanged()
{
    // �л����˵�ǰ�����ص�ǰ��ʾ�ı���
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
        // �˳��޸�����ģʽ
        _leavePWMode();
    }

    // �ָ��˵�
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, L"");
    m_pNaviBar->UpdateNaviMenu();

    switch(m_menuNavi.GetCurSel())
    {
        // �л���������
    case DSM_BASICCFG_MENU_BASE:
        _enterBase();
        break;

        // �л����ӹ���
    case DSM_BASICCFG_MENU_LINKMGR:
        _enterLinkmgr();
        break;

    case DSM_BASICCFG_MENU_STORAGE:
        _enterStorage();
        break;

        // �л���ԭ����
    case DSM_BASICCFG_MENU_RESTORE:
        _enterRestore();
        break;

        // ����
    case DSM_BASICCFG_MENU_ABOUT:
        _enterAbout();
        break;

    default:
        break;
    }
}

BOOL CBasicCfgDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_RIGHT:
            // ���������˵������ƶ����㵽�Ҳ�ؼ�
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
            // �������Ҳ౨�����ƶ����㵽���˵�
            if(GetFocus()->GetSafeHwnd() != m_menuNavi.GetSafeHwnd())
            {
                m_menuNavi.SetFocus();
            }
            return TRUE;
        }

        // ����U��ģʽ��������
        if(USB_LINKED == m_eUSBMode)
        {
            // ���γ���F6֮�����а���
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

    // ���±���
    _updateCaption(DSM_STR_BASIC_BASECAPTION);

    m_pCurReport = &m_reportBase;
}


void CBasicCfgDlg::_enterLinkmgr()
{
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_LINK_START));

    // ���±���
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

    // ���±���
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

    // ��������
    m_reportAbout.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ���ñ�����ʽ
    m_reportAbout.PostInitTemplateStyle();
    m_reportAbout.SetRowFocusEnable(FALSE);

    CString csSoftVer;
    csSoftVer.Format(L"%s.%s", dsm_config::GetSoftwareVersion(), dsm_config::GetSoftwareBuild());

    // �����ü�����Ϣ
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

    // ������Ϣ
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

    // ���±���
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

    // ��ȡĬ��ʱ������
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

    // ����������Чʱ��
    while(ERROR_SUCCESS == RegEnumKeyEx(hKey, dwIndex, szKeyName, &dwLen, NULL, NULL, NULL, NULL))
    {
        dwIndex++;
        dwLen = MAX_PATH;

        // ��ָ��ʱ��
        if(ERROR_SUCCESS != RegOpenKeyEx(hKey, szKeyName, 0, 0, &hSubKey))
        {
            continue;
        }

        dwSubLen = sizeof(tziSub);
        ZeroMemory(&tziSub, dwSubLen);

        // ��ȡTIME_ZONE_INFORMATION������ֵ
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"TZI", NULL, &dwSubType, (LPBYTE)&tziSub, &dwSubLen) || 
            REG_BINARY != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // ��ȡDisplayֵ��������ʾֵ
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Display", NULL, &dwSubType, (LPBYTE)szDisplayValue, &dwSubLen) || 
            REG_SZ != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // ��ȡDltֵ
        if(ERROR_SUCCESS != RegQueryValueEx(hSubKey, L"Dlt", NULL, &dwSubType, (LPBYTE)szDlt, &dwSubLen) || 
            REG_SZ != dwSubType)
        {
            RegCloseKey(hSubKey);
            continue;
        }

        dwSubLen = MAX_PATH;

        // ��ȡStdֵ
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
        // ���ñ���ʱ��
    case DSM_BASICCFG_RPT_BRIGHTTIME:
            CStatusMgr::SetBrightTime(pItemNotify->pConstraint->GetConstraintData());
        break;

        // ����ʱ��
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
            // ���ñ�������
        case DSM_BASICCFG_RPT_BRIGHTNESS:
            _modifyBrightness(pItemNotify, pResult);
            break;

            // ����ʱ��
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

    // ����ʱ�����ÿؼ�
    if(IDOK == ctrlTime.GoModal(&m_reportBase, rcItem, st))
    {
        CString csTime;
        csTime.Format(DSM_BASICFG_STR_TIME_FMT, 
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(pItemNotify->pItem);
        pItem->SetValue(csTime);

        SetLocalTime(&st);

        // ����ͬ��FPGAʱ��
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

    // �����������ý�����
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
    // �л�usbģʽ
    if(m_menuNavi.GetCurSel() == DSM_BASICCFG_MENU_LINKMGR)
    {
        // ����usb
        if(USB_UNLINK == m_eUSBMode)
        {
            // ���SD��״̬
            if(CStatusMgr::GetSD() == DSM_STATU_CONNECT_SD)
            {
                _setLink();
            }
            else
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT));
            }
        }
        // �Ͽ�usb
        else
        {
            _setUnLink();
        }
    }
    // ����ҳ��
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

    // �޸Ĳ˵���ʾ ���������롻
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_BASIC_M_MODIFY_PW));

    // ��������������row
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW, new CDsmPasswordNewRecord(L"", L""));
    m_reportBase.InsertRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM, new CDsmPasswordNewRecord(L"", L""));

    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);
    m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM)->GetItem(DSM_BASICCFG_RPT_VALUE)->SetEditable(TRUE);

     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_NEW)->GetItem(DSM_BASICCFG_RPT_CAPTION)->SetCaption(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD_NEW));
     m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD_CONFIRM)->GetItem(DSM_BASICCFG_RPT_CAPTION)->SetCaption(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD_CONFIRM));
}

void CBasicCfgDlg::_leavePWMode()
{
    // �˳��޸�����ģʽ
    m_bModifyPassword       = false;
    m_csPasswordNew         = L"";
    m_csPasswordConfirm     = L"";

    // �ָ��˵���ʾ ����
    m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, L"");

    int nFocusRow = m_reportBase.GetFocusedRowIndex();

    if(DSM_BASICCFG_RPT_PASSWORD_NEW == nFocusRow||
        DSM_BASICCFG_RPT_PASSWORD_CONFIRM == nFocusRow)
    {
        m_reportBase.SetFocusedSelectionRowIndex(DSM_BASICCFG_RPT_PASSWORD);
    }

    // ��β�п�ʼɾ���½�������������record
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
        // inplaceEdit����ʱ, ����༭�˵�
        pSrc = CNaviBarMgr::GetInstance()->EnterInplaceEdit(m_pCurReport->GetInplaceEdit());


        // ���á��������á����ı��� ������������롻
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
        // inplace�˳�ʱ���˳��༭�˵�
        CNaviBarMgr::GetInstance()->LeaveInplaceEdit(pSrc);


        // �ָ����������á��ı���
        //# ELNM_ITEM_ENDEDITû��row col��Ϣ
        int nFocus = m_reportBase.GetSelectionRowIndex();

        if(DSM_BASICCFG_RPT_PASSWORD == nFocus)
        {
            CELRecordItemText* pText = dynamic_cast<CELRecordItemText*>(m_reportBase.GetRecordRow(DSM_BASICCFG_RPT_PASSWORD)->GetItem(DSM_BASICCFG_RPT_CAPTION));
            pText->SetValue(dsm_stringMgr::Get(DSM_STR_BASIC_PASSWORD));
            m_reportBase.RedrawRowItem(DSM_BASICCFG_RPT_PASSWORD, DSM_BASICCFG_RPT_CAPTION);
        }

        // ����������״̬ʱ�Զ��л������
        if(m_bModifyPassword)
        {
            // �ж��Ƿ�Ϊ���ַ���
            if((nFocus == DSM_BASICCFG_RPT_PASSWORD_NEW && m_bPWNull) || (nFocus == DSM_BASICCFG_RPT_PASSWORD_CONFIRM && m_bPWNull))
            {
                // ������ý���
                this->PostMessage(WM_BASICCFG_BEGIN_EDIT, MAKELONG(DSM_BASICCFG_RPT_VALUE, nFocus));
                return;
            }

            if(nFocus == DSM_BASICCFG_RPT_PASSWORD || nFocus == DSM_BASICCFG_RPT_PASSWORD_NEW)
            {
                // ��һ��������ȡ����
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
    // wParam��λ row�� ��λ col
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
        // ����Ĭ��ʱ��
        _setDefaultTZS();

        // ��ԭ��������
        CEventProc::GetInstance()->ResetFactory();

        return;
    }
    ::SetFocus(hWnd);
}

void CBasicCfgDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: �ڴ˴������Ϣ����������

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

    // ������ʱ�����usb����״̬�Ƿ�Ͽ�
    m_nTimerLinkStatus = SetTimer(DSM_BASICCFG_TID_LINK_STATUS, DSM_BASICCFG_TIME_LINK_STATUS, NULL);
    m_nCountDown = 0;

    // ���ô������洢ģʽ
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

    // �ر���ѯusb״̬��ʱ��
    if(m_nTimerLinkStatus)
    {
        KillTimer(m_nTimerLinkStatus);
        m_nTimerLinkStatus = 0;
    }

    // ���ô������洢ģʽ
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

    // ʧȥ����״̬
    std::vector<HBITMAP> v;
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_LINK_FAILED));
    m_ctlPic._InsertStatus(DSM_BASICCFG_PIC_UNLINK, v);

    // ���ӳɹ�״̬
    v.clear();
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_DLG_BASIC_LINK_SUCCESS));
    m_ctlPic._InsertStatus(DSM_BASICCFG_PIC_LINKED, v);

    // ��������״̬
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

    // ����ͼƬ�ռ�����
    CRect rcPic;
    rcPic.left   = rcRight.left + (rcRight.Width() - bm.bmWidth) / 2;
    rcPic.right  = rcPic.left + bm.bmWidth;
    rcPic.top    = (rcRight.Height() - bm.bmHeight) / 2;
    rcPic.bottom = rcPic.top + bm.bmHeight;

    m_ctlPic.Create(NULL, WS_CHILD | WS_VISIBLE, rcPic, this, IDC_BASICCFG_PIC);

    // ����usb��ʼ״̬
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
        // �������޸���esc�˳�ʱ��Ӧ�˳��������޸ġ�ģʽ
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
 *      ����Ĭ��ʱ��
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
 *      ��������ʼ�����ڱ������ϸ��Ϣ
 */
void CBasicCfgDlg::_initReportDetail()
{
    CRect rcReport(DSM_BASICCFG_RCRIGHT);
    DSM_ParentToChild(this, rcReport);

    DWORD dwStyle = (DSM_WINSTYLE_REPORT_DEFAULT) & ~WS_TABSTOP;
    m_reportDetail.Create(dwStyle, rcReport, this, IDC_DEVICE_INFO_RET);

    // ���ñ���
    m_reportDetail.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.PostInitTemplateStyle();
    m_reportDetail.SetRowFocusEnable(FALSE);

    m_reportDetail.InsertColumn(0, L"", EL_LEFT, DSM_REPORT_WIDTH_CAPTION);   // ����
    m_reportDetail.InsertColumn(1, L"", EL_LEFT, DSM_REPORT_WIDTH_CONTENT);   // ����

    // �����豸��ϸ��Ϣ

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

    // ���±���
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
        // δ����SD��
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT));
        return;
    }

    // ����pcap�ļ�
    CString csExt;
    csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
    CDsmFileDlg dlg(0, L"\\Storage Card\\pcap\\", L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

    CString csTitle(CStatusMgr::GetTitle());

    HWND hWnd       = ::GetFocus();
    INT_PTR nRet    = dlg.DoModal();
    if(nRet == IDOK)
    {
        CString csTitle = CStatusMgr::GetTitle();

        // ����pcap��
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

