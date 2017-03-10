// SMVPhasorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVPhasorDlg.h"

#include "src/main/utils/utility.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



#define PHASOR_MODE_SELECT          0x01        ///< 选择
#define PHASOR_MODE_ZOOM            0x02        ///< 缩放


#define PHASOR_TIME_MAX             4           ///< 最大放大倍数
#define PHASOR_TIME_MIN             -4          ///< 最小缩小倍数

#if !CHUN_HUA
COLORREF m_Color[] ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

/**
 *      相量页面控制块关联数据
 */
class CCtrlBlkDataPhasor : public ICtrlBlkData
{
public:

    CCtrlBlkDataPhasor()
        : nFoucs(0)
        , nTimes(0)
    {
        for(int i = 0; i < 4; i++)
        {
            nChannelIndex[i] = UINT_MAX;
            bChannelValid[i] = false;
        }
    }

    virtual ~CCtrlBlkDataPhasor()
    {

    }


public:
    int nFoucs;                 ///< 焦点行号
    uint32_t nChannelIndex[4];  ///< 通道号
    bool bChannelValid[4];      ///< 通道有效性
    int nTimes;                 ///< 放大倍数
};


// CSMVPhasorDlg 对话框

IMPLEMENT_DYNAMIC(CSMVPhasorDlg, DSM_SMVBASEDLG)

CSMVPhasorDlg::CSMVPhasorDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVPhasorDlg::IDD, DSM_STR_SMV_PHASOR_CAPTION, pParent)
    , m_dwMode(PHASOR_MODE_SELECT)
    , m_fScaleInterval(0.0f)
    , m_nTimes(0)
    , m_pCurCtrlBlkData(NULL)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
    for(int i = 0; i < 4; i++)
    {
        m_fLen[i]       = 0.0f;
        m_fValue[i]     = 0.0f;

        m_strDesc[i].Format(L"%d", i + 1);
    }
}

CSMVPhasorDlg::~CSMVPhasorDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVPhasorDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVPhasorDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVPhasorDlg::OnMenuF4)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_PHASOR_REPROT, &CSMVPhasorDlg::OnItemSelectChange)
END_MESSAGE_MAP()


// CSMVPhasorDlg 消息处理程序

BOOL CSMVPhasorDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
    m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // 获取通道额定值类型

    _createReport();
    _createPhasorGram();

    m_reportChannel.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSMVPhasorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        // 刷新相量数据
        _updatePhasor();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

/**
 *      创建相量报表
 */
void CSMVPhasorDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // 调整高度
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;
#if SU_FEI
    rcReport.OffsetRect(DSM_MARGIN_IN, 0);
#endif
    m_reportChannel.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_PHASOR_REPROT);

    // 设置字体
    m_reportChannel.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportChannel.PostInitTemplateStyle();
    m_reportChannel.SetHeaderHeight(0);
	m_reportChannel.SetHasGridline(FALSE, TRUE);

    // 插入列元素
    m_reportChannel.InsertColumn(0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);   // 标题
    m_reportChannel.InsertColumn(1, L"", EL_LEFT, DSM_PHASOR_REPORT_VALUE);     // 内容

    // 设置选择焦点样式
    m_reportChannel.SetRowSelectionVisible(TRUE);
    m_reportChannel.SetRowFocusEnable(TRUE);
    m_reportChannel.SetSubItemFocusEnable(TRUE);
    m_reportChannel.SetFreezeColumnsCount(1);
    m_reportChannel.SetFocusedColIndex(1);

    // 重新设置相量报表的值
    _resetReport();
}


/**
 *      创建相量图
 */
void CSMVPhasorDlg::_createPhasorGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhasor.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_PHASOR_PHASORGRAM);

    m_ctrlPhasor.ModifyStyle(0, SS_OWNERDRAW);

    // 设置相别线条颜色
    m_ctrlPhasor.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhasor.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhasor.SetPhasorColor(2, DSM_PHASE_COLOR_C);
    m_ctrlPhasor.SetPhasorColor(3, DSM_PHASE_COLOR_N);

    m_ctrlPhasor.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhasor.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhasor.Init();
}

/**
 *      重设报表内通道数据
 */
void CSMVPhasorDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    m_reportChannel.DeleteAllRecordRow();

    int nIndex = 0;

    // 插入通道行 ABCN相
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN1), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN2), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN3), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN4), g_csBlankValue));

    // 空行
     m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));

     // 插入通道值行
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN1_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN2_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN3_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN4_VAL), g_csBlankValue));

    // 设置通道值行不可选中
    for(int i = 4; i < 9; i++)
    {
        m_reportChannel.GetRecordRow(i)->SetFocusable(FALSE);
    }

    // 根据页面数据设置所选通道
    if(!_restorePage())
    {
         // 若页面数据没有相关信息，则自动选中合适的通道
        _adjustChannel();
    }

    // 更新通道数值的单位信息
    _updateUnit();
}

/**
 *      调整初始选中的3个通道，尽量选中合适的ABC三相
 */
void CSMVPhasorDlg::_adjustChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // 获取控制块绑定数据，若为空，则新建一个数据
    m_pCurCtrlBlkData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!m_pCurCtrlBlkData)
    {
        m_pCurCtrlBlkData = new CCtrlBlkDataPhasor;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, m_pCurCtrlBlkData);
    }

    // 寻找3个合适的通道
    _calcChannel();

    CSmvRecvChannelInfo* pInfo  = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // 设置3个通道的值到报表
    for(int i = 0; i < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

        if(pInfo)
        {
            m_reportChannel.SetRowItemText(i, 1, pInfo->strChannelDesc.c_str());
        }
        else
        {
            m_reportChannel.SetRowItemText(i, 1, g_csBlankValue);
        }
    }

    // 默认没选中通道显示 L"--"
    m_reportChannel.SetRowItemText(3, 1, g_csBlankValue);
    m_reportChannel.SetRowItemText(8, 1, g_csBlankValue);

    m_reportChannel.SetFocusedSelectionRowIndex(0);

    m_pCurCtrlBlkData->nFoucs = 0;
}

/**
 *      寻找3个合适的默认显示通道名
 *
 *  \note
 *      选中的三个默认通道索引将填入该通道关联的页面数据对象中
 */
void CSMVPhasorDlg::_calcChannel()
{
    ASSERT(m_pCurCtrlBlkData);

    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 枚举所有通道
    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 选取首次出现电压类型的三相
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // 依次往后查找三相通道索引
            uint32_t nA = _findChannel(eType, CHANNEL_PHASE_A, i);
            uint32_t nB = _findChannel(eType, CHANNEL_PHASE_B, nA + 1);
            uint32_t nC = _findChannel(eType, CHANNEL_PHASE_C, nB + 1);

            // 若其中有一相通道索引未找到，则从索引i开始往后找第一个出现的三相通道
            if(nA == UINT_MAX || nB == UINT_MAX || nC == UINT_MAX)
            {
                nA = _findChannel(eType, CHANNEL_PHASE_A, i);
                nB = _findChannel(eType, CHANNEL_PHASE_B, i);
                nC = _findChannel(eType, CHANNEL_PHASE_C, i);
            }

            // 如果ABC三相通道都找到合适的，则设置到控制块关联的数据中并返回
            if(nA != UINT_MAX && nB != UINT_MAX && nC != UINT_MAX)
            {
                m_pCurCtrlBlkData->nChannelIndex[0] = nA;
                m_pCurCtrlBlkData->nChannelIndex[1] = nB;
                m_pCurCtrlBlkData->nChannelIndex[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pCurCtrlBlkData->bChannelValid[k] = true;
                }

                return;
            }
        }
    }

    // 若到这里，则随便选取三个类型
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 只要类型是电压或电流都可以
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            m_pCurCtrlBlkData->nChannelIndex[j] = pInfo->nIndex;
            m_pCurCtrlBlkData->bChannelValid[j] = true;
            j++;
        }
    }

    // 检查是否还有通道没有设置
    if(m_pCurCtrlBlkData->nChannelIndex[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pCurCtrlBlkData->nChannelIndex[i] == UINT_MAX)
            {
                m_pCurCtrlBlkData->nChannelIndex[i] = m_pCurCtrlBlkData->nChannelIndex[0];
                m_pCurCtrlBlkData->bChannelValid[i] = true;
            }
        }
    }
}

/**
 *      刷新界面的值
 */
void CSMVPhasorDlg::_updatePhasor()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pCurCtrlBlkData);

    // 若启用定时器，则获取最新数据
    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        // 获取相量数据
        if(!m_pDataLayer->GetPhasorData(&m_data))
        {
            // 清空相量图
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);
            return;
        }
    }

    CString csValue;
    int nCounts = 3;
    float fMaxLen = 0.0f;
    float fPhaseA = 0.0f;
    CEffectiveChanelInfo*       pInfo = NULL;           ///< 通道数据信息

    for(int i = 0; i < 4; i++)
    {
        // 无效通道
        if(!m_pCurCtrlBlkData->bChannelValid[i])
        {
            m_reportChannel.SetRowItemText(i + 5, 1, g_csBlankValue);

            m_fLen[i]       = 0.0f;
            m_fValue[i]     = 0.0f;

            continue;
        }

        // 获取选中通道的信息
        pInfo = m_data.GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

        if(!pInfo)
        {
            continue;
        }

        // 换算一次二次值比例
        if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
        {
            CSmvRecvChannelInfo* pChannel = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);
            ASSERT(pChannel);

            if(pChannel && pChannel->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                pInfo->m_fEffectiveValue = pInfo->m_fEffectiveValue / 1000;
            }
        }

        // 第四个通道有效，则画四条线
        if(i == 3)
        {
            nCounts = 4;
        }

        // 保留A相相位，将以A相为0起始值
        if(0 == i)
        {
            fPhaseA = pInfo->m_fPhase;
        }

        m_fLen[i]   = pInfo->m_fEffectiveValue;

        // 所有通道已通道1的相位角为基准
        m_fValue[i] = DSM_GetRelativePhase(fPhaseA, pInfo->m_fPhase);

        csValue.Format(L"%0.2f %s/%0.2f°", m_fLen[i], m_csUnit[i], m_fValue[i]);

        m_reportChannel.SetRowItemText(i + 5, 1, csValue);

        // 获取最大值
        if(fMaxLen < m_fLen[i])
        {
            fMaxLen = m_fLen[i];
        }
    }

    // 减少频繁设置
    float fMaxTemp  = float(fMaxLen / 4.0 * pow(2, m_nTimes));
    float fMargin   = fMaxTemp * 0.1f;
    float fDiff     = m_fScaleInterval - fMaxTemp;

    if(fDiff < 0.0f || fDiff > fMargin)
    {
        m_fScaleInterval = fMaxTemp;
        m_ctrlPhasor.SetLenScaleInterval(m_fScaleInterval);
    }

#if !CHUN_HUA
	for (int i =0; i<4;i++)
	{
		CELRecord*  record = m_reportChannel.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportChannel.GetRecordRow(i+5);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i+5,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i+5,1);
	}
	
#endif

    // 擦除并显示新数据
    m_ctrlPhasor.ErasePhasor();
    m_ctrlPhasor.DrawPhasor(m_fLen, m_fValue, m_strDesc, nCounts);
    m_ctrlPhasor.Invalidate(TRUE);
}

void CSMVPhasorDlg::OnMenuF4()
{
    if(!m_bChannelChange)
    {
        // 切换缩放/选择模式
        m_dwMode = m_dwMode == PHASOR_MODE_ZOOM ? PHASOR_MODE_SELECT : PHASOR_MODE_ZOOM;

        // 切换模式为『选择』
        if(PHASOR_MODE_SELECT == m_dwMode)
        {
            m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
        }
        // 切换模式为『缩放』
        else
        {
            m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STRETCH_SEL), NULL, NULL);
        }

        m_pNaviBar->UpdateNaviMenu();
    }
}


BOOL CSMVPhasorDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        // 若是缩放模式
        if(!m_bChannelChange && PHASOR_MODE_ZOOM == m_dwMode)
        {
            switch(pMsg->wParam)
            {
            case VK_UP:
                // 放大相量图
                if(PHASOR_TIME_MIN < m_nTimes)
                {
                    m_nTimes--;

                    // 更新界面
                    _updatePhasor();
                }

                return TRUE;

            case VK_DOWN:
                // 放大相量图
                if(PHASOR_TIME_MAX > m_nTimes)
                {
                    m_nTimes++;

                    // 更新界面
                    _updatePhasor();
                }

                return TRUE;
            }
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      保存页面状态信息
 */
void CSMVPhasorDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurCtrlBlkData);

    // 保存行焦点与放大倍数
    m_pCurCtrlBlkData->nFoucs = m_reportChannel.GetFocusedRowIndex();
    m_pCurCtrlBlkData->nTimes = m_nTimes;
}

/**
 *      修改菜单HMENU
 */
void CSMVPhasorDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // 设置F4缩放选择图标
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASOR_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL ), NULL, NULL);


	//需要根据设置中选中的一次值还是二次值进行判断，才能正常显示
	CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();

	if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
	}
	else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
	}
}


/**
 *      根据菜单ID切换到指定的控制块
 *
 *  \param[in]
 *      nCmdID          控制块映射的命令菜单ID
 */
bool CSMVPhasorDlg::_switchCtrlBlk( UINT nCmdID )
{
    // 保留切换前的控制块对象
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    // 切换控制块
    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    // 设置原控制块的通道焦点
    CCtrlBlkDataPhasor* pData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // 保存原控制块关联的报表焦点与缩放倍数
    if(pData)
    {
        pData->nFoucs = m_reportChannel.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }

    // 根据新的控制块重设报表
    _resetReport();

    return true;
}


/**
 *      使用页面数据设置通道
 */
bool CSMVPhasorDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVPhasorDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块关联的页面数据
    CCtrlBlkDataPhasor* pData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }

    m_pCurCtrlBlkData               = pData;
    m_nTimes                        = m_pCurCtrlBlkData->nTimes;
    CELRecordItem* pItem            = NULL;
    CELRecordItemConstraint* pCon   = NULL;

    // 设置原来的通道信息
    for(int i = 0; i < 4; i++ )
    {
        pItem = m_reportChannel.GetRecordRow(i)->GetItem(1);
        ASSERT(pItem);

        if(pItem)
        {
            // 查找指定通道索引的通道描述
            pCon = pItem->GetEditOptions(NULL)->FindConstraint(m_pCurCtrlBlkData->nChannelIndex[i]);

            if(pCon)
            {
                m_pCurCtrlBlkData->bChannelValid[i] = true;         // 设置通道有效

                // 设置选中通道的描述到报表
                m_reportChannel.SetRowItemText(i, 1, pCon->GetConstraintCaption());
            }
            else
            {
                m_pCurCtrlBlkData->bChannelValid[i] = false;        // 设置通道无效

                m_reportChannel.SetRowItemText(i, 1, g_csBlankValue);
                m_reportChannel.SetRowItemText(i + 5, 1, g_csBlankValue);
            }
        }
    }

    if(m_pCurCtrlBlkData->nFoucs >= 0 && m_pCurCtrlBlkData->nFoucs < m_reportChannel.GetRecordRowCount())
    {
        m_reportChannel.SetFocusedSelectionRowIndex(m_pCurCtrlBlkData->nFoucs);
    }

    return true;
}

void CSMVPhasorDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);
    ASSERT(pItemNotify->rowIndex >= 0 && pItemNotify->rowIndex < 3);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        pResult = FALSE;
        return;
    }

    uint32_t nSelChan = (uint32_t)pItemNotify->pConstraint->GetConstraintData();

    // 设置新选择的通道号
    m_pCurCtrlBlkData->nChannelIndex[pItemNotify->rowIndex] = nSelChan;
    m_pCurCtrlBlkData->bChannelValid[pItemNotify->rowIndex] = true;

    // 如果第一行选中了A相类型的通道，则自动就近为下两行选中BC相通道
    if(0 == pItemNotify->rowIndex)
    {
        CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
        ASSERT(pCfg);

        CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSelChan);
        ASSERT(pInfo);

        if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
        {
            // 依序查找BC相通道
            int nB = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_B, nSelChan + 1);
            int nC = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

            // 找到合适的BC相通道，则设置到控制块关联的数据对象中
            if(nB != UINT_MAX && nC != UINT_MAX)
            {
                m_pCurCtrlBlkData->nChannelIndex[1] = nB;
                m_pCurCtrlBlkData->nChannelIndex[2] = nC;

                m_pCurCtrlBlkData->bChannelValid[1] = true;
                m_pCurCtrlBlkData->bChannelValid[2] = true;

                m_reportChannel.SetRowItemText(1, 1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
                m_reportChannel.SetRowItemText(2, 1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());
            }
        }
    }

    // 更新通道的单位
    _updateUnit();

    // 刷新更换通道后的值
    _updatePhasor();

    *pResult = TRUE;
}

/**
 *      返回符合条件的通道号
 *
 *  \param[in]
 *      eType           指定的通道类型
 *  \param[in]
 *      ePhase          指定的通道相别
 *  \param[in]
 *      nStart          通道的查询起始位置
 *  \return
 *      uint32_t             返回符合条件的通道号，找不到则返回 UINT_MAX
 */
uint32_t CSMVPhasorDlg::_findChannel( enumChannelType eType, ChannelPhase ePhase, int nStart )
{
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 选3个有效的通道名ABC
    for(size_t i = nStart; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 找到类型与相位合适的通道索引 返回
        if(pInfo && eType == pInfo->enChannelType && ePhase == pInfo->enChannelPhase)
        {
            return pInfo->nIndex;
        }
    }

    return UINT_MAX;
}

/**
 *      更新所选通道的单位
 */
void CSMVPhasorDlg::_updateUnit()
{
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo      = NULL;

    for(int i = 0; i < 4; i++)
    {
        // 通道有效将设置单位
        if(m_pCurCtrlBlkData->bChannelValid[i])
        {
            pInfo = pCfg->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

            if(pInfo)
            {
                // 设置通道的单位
                m_csUnit[i] = DM_GetUnitFromType(pInfo->enChannelType, m_eDispType, m_eFrameType);
            }
            else
            {
                m_csUnit[i] = L"";
            }
        }
        else
        {
            m_csUnit[i] = L"";
        }
    }
}

/**
 *      检查通道是否变化
 *
 *  \return
 *      bool        发生变化返回true，没变化返回false
 */
bool CSMVPhasorDlg::_checkChannel()
{
    // 通道发生变化，
    if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
    {

        if(CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // 界面tips提示
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;

            // 清空相量图
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);

            // 禁用通道选择
            m_reportChannel.GetRecordRow(0)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(1)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(2)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(3)->GetItem(1)->SetEditable(FALSE);

            // 设置通道的有效值item为L"--"
            m_reportChannel.SetRowItemText(5, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(6, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(7, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(8, 1, g_csBlankValue);

            // 隐藏部分菜单标题
            //m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);

            // 恢复通道选择
            m_reportChannel.GetRecordRow(0)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(1)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(2)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(3)->GetItem(1)->SetEditable(TRUE);

            // 恢复部分菜单标题
         //   m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASOR_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

