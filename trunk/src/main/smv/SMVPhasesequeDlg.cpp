/**
 * \file
 *      SMVPhasesequeDlg.cpp
 *
 * \brief
 *      序量页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */

// SMVPhasesequeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVPhasesequeDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"


/**
 *      序量页面控制块关联数据
 */
class CCtrlBlkDataPhasaSeq  : public ICtrlBlkData
{
public:

    CCtrlBlkDataPhasaSeq()
    {
        for(int i = 0; i < 3; i++)
        {
            nChannelIndex[i] = UINT_MAX;
            bChannelValid[i] = false;
        }
    }


public:
    int nFoucs;                     ///< 焦点行号
    uint32_t nChannelIndex[3];      ///< 通道号
    bool bChannelValid[3];          ///< 通道有效性
    int nTimes;                     ///< 缩放倍数
};



#define PHASASEQ_MODE_SELECT            0x01        ///< 选择
#define PHASASEQ_MODE_ZOOM              0x02        ///< 缩放

#define PHASASEQ_TIME_MAX               4           ///< 最大放大倍数
#define PHASASEQ_TIME_MIN               -4          ///< 最小缩小倍数

#if !CHUN_HUA
COLORREF m_sColor[] ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

// CSMVPhasesequeDlg 对话框

IMPLEMENT_DYNAMIC(CSMVPhasesequeDlg, DSM_SMVBASEDLG)

CSMVPhasesequeDlg::CSMVPhasesequeDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVPhasesequeDlg::IDD, DSM_STR_SMV_PHASESEQU_CAPTION, pParent)
    , m_dwMode(PHASASEQ_MODE_SELECT)
    , m_nTimes(0)
    , m_fScaleInterval(0.0f)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
    for(int i = 0; i < 3; i++)
    {
        m_fAngel[i] = 0.0f;
        m_fValue[i] = 0.0f;

        m_strDesc[i].Format(dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_POS + i));
    }
}

CSMVPhasesequeDlg::~CSMVPhasesequeDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVPhasesequeDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVPhasesequeDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVPhasesequeDlg::OnMenuF4)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_PHASESEQU_REPORT, &CSMVPhasesequeDlg::OnItemSelectChanged)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_PHASESEQU_REPORT, &CSMVPhasesequeDlg::OnInplaceButtonDown)
END_MESSAGE_MAP()


// CSMVPhasesequeDlg 消息处理程序


BOOL CSMVPhasesequeDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
    m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // 获取通道额定值类型

    _createReport();
    _createPhaseGram();

    m_reportPhase.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

/**
 *      创建相位报表
 */
void CSMVPhasesequeDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 7;

    // 调整高度
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;
#if SU_FEI
    rcReport.OffsetRect(DSM_MARGIN_IN, 0);
#endif
    m_reportPhase.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_PHASESEQU_REPORT);

    // 设置字体
    m_reportPhase.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportPhase.PostInitTemplateStyle();
    m_reportPhase.SetHeaderHeight(0);
	m_reportPhase.SetHasGridline(FALSE, TRUE);

    // 插入列元素
    m_reportPhase.InsertColumn(0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);     // 标题
    m_reportPhase.InsertColumn(1, L"", EL_LEFT, DSM_PHASOR_REPORT_VALUE);       // 内容

    // 设置选择焦点样式
    m_reportPhase.SetRowSelectionVisible(TRUE);
    m_reportPhase.SetRowFocusEnable(TRUE);
    m_reportPhase.SetSubItemFocusEnable(TRUE);
    m_reportPhase.SetFreezeColumnsCount(1);
    m_reportPhase.SetFocusedColIndex(1);

    // 重设报表值
    _resetReport();
}

/**
 *      创建序量图
 */
void CSMVPhasesequeDlg::_createPhaseGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhasor.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_PHASESEQU_PHASESEQUGRAM);
    m_ctrlPhasor.ModifyStyle(0, SS_OWNERDRAW);

    // 设置相别线条颜色
    m_ctrlPhasor.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhasor.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhasor.SetPhasorColor(2, DSM_PHASE_COLOR_C);

    m_ctrlPhasor.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhasor.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhasor.Init();

    m_ctrlPhasor.ShowWindow(SW_SHOW);
}

/**
 *      重设报表内通道数据
 */
void CSMVPhasesequeDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    m_reportPhase.DeleteAllRecordRow();

    int nIndex = 0;

    // 创建ABC三相通道行
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_A), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_B), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_C), g_csBlankValue));

    // 三个空行
  //  m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));
   // m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));

    // 创建ABC三相通道数值行
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_POSITIVE), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_NEGATIVE), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_ZERO), g_csBlankValue));

    // 禁止非通道行选中
    for(int i = 3; i < 7; i++)
    {
        m_reportPhase.GetRecordRow(i)->SetFocusable(FALSE);
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
void CSMVPhasesequeDlg::_adjustChannel()
{
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_reportPhase.GetSafeHwnd());

    // 获取控制块绑定数据，若为空，则新建一个数据
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!pData)
    {
        pData = new CCtrlBlkDataPhasaSeq;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    // 寻找3个合适的通道
    _calcChannel(pData);

    CSmvRecvChannelInfo* pInfo  = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // 设置3个通道的值到报表
    for(int i = 0; i < 3; i++)
    {
        // 通道无效则显示L"--"
        if(!pData->bChannelValid[i])
        {
            m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
        }
        else
        {
            pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[i]);

            if(pInfo)
            {
                m_reportPhase.SetRowItemText(i, 1, pInfo->strChannelDesc.c_str());
            }
            else
            {
                m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
            }
        }
    }

    m_reportPhase.SetFocusedSelectionRowIndex(0);

    pData->nFoucs = 0;
}

/**
 *      寻找3个合适的默认显示通道名
 *
 *  \note
 *      选中的三个默认通道索引将填入该通道关联的页面数据对象中
 */
void CSMVPhasesequeDlg::_calcChannel(CCtrlBlkDataPhasaSeq* pData)
{
    ASSERT(pData);

    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 枚举所有通道
    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 选取首次出现电压电流类型的三相
        if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // 依次往后查找三相通道索引
            int nA = _findChannel(eType, CHANNEL_PHASE_A, i);
            int nB = _findChannel(eType, CHANNEL_PHASE_B, nA + 1);
            int nC = _findChannel(eType, CHANNEL_PHASE_C, nB + 1);

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
                pData->nChannelIndex[0] = nA;
                pData->nChannelIndex[1] = nB;
                pData->nChannelIndex[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    pData->bChannelValid[k] = true;
                }

                // 向数据层设置序量配置
                m_data.SetXLMode(eType);
                m_data.SetABCChannles(nA, nB, nC);

                return;
            }
        }
    }

    // 若到这里，将随便选取三个类型
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 选取首次出现电流电流类型的三相
        if(CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType && CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType)
        {
            continue;
        }

        pData->nChannelIndex[j++] = pInfo->nIndex;

        if(j == 3)
        {
            // 默认设置电压类型
            m_data.SetXLMode(CHANNEL_TYPE_SMV_VOLTAGE);
            m_data.SetABCChannles(pData->nChannelIndex[0], pData->nChannelIndex[1], pData->nChannelIndex[2]);
            return;
        }
    }

    // 检查是否还有通道没有设置
    if(pData->nChannelIndex[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(pData->nChannelIndex[i] == UINT_MAX)
            {
                pData->nChannelIndex[i] = pData->nChannelIndex[0];
                pData->bChannelValid[i] = true;
            }
        }
    }
}


void CSMVPhasesequeDlg::OnTimer( UINT_PTR nIDEvent )
{
    if(nIDEvent == m_nTimerID)
    {
        // 定时刷新序量页面数据
        _updatePhasae();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


/**
 *      刷新界面数据
 */
void CSMVPhasesequeDlg::_updatePhasae()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        // 获取序量数据
        if(!m_pDataLayer->GetXuliangData(&m_data))
        {
            // 清空图形
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);
            return;
        }
    }

    CString csValue;
    float fMaxLen = 0.0;
    m_data.GetPhaseSeqAngle(m_fAngel[2], m_fAngel[0], m_fAngel[1]);
    m_data.GetPhaseSeqContent(m_fValue[2], m_fValue[0], m_fValue[1]);

    // 获取控制块绑定数据
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    // 更新序量数据
    for(int i = 0; i < 3; i++)
    {
        if(!pData->bChannelValid[i])
        {
#if SU_FEI
            m_reportPhase.SetRowItemText(i + 6, 1, g_csBlankValue);
#else
			m_reportPhase.SetRowItemText(i + 4, 1, g_csBlankValue);
#endif
            continue;
        }

        // 换算一次二次值比例
        if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
        {
            CSmvRecvChannelInfo* pChannel = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(pData->nChannelIndex[i]);
            ASSERT(pChannel);

            if(pChannel && pChannel->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                m_fValue[i] = m_fValue[i] / 1000;
            }
        }

        csValue.Format(L"%0.3f %s/%0.3f°", m_fValue[i], m_csUnit[i], m_fAngel[i]);
#if SU_FEI
        m_reportPhase.SetRowItemText(i + 6, 1, csValue);
#else
		m_reportPhase.SetRowItemText(i + 4, 1, csValue);
#endif
        if(fMaxLen < m_fValue[i])
        {
            fMaxLen = m_fValue[i];
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
	for (int i =0; i<3;i++)
	{
		CELRecord*  record = m_reportPhase.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportPhase.GetRecordRow(i+4);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+4,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+4,1);
	}

#endif

    // 擦除并显示新数据
    m_ctrlPhasor.ErasePhasor();
    m_ctrlPhasor.DrawPhasor(m_fValue, m_fAngel, m_strDesc, 3);
    m_ctrlPhasor.Invalidate(TRUE);
}


/**
 *      修改菜单HMENU
 */
void CSMVPhasesequeDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // 设置F4缩放选择图标
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASASEQ_MODE_ZOOM ?  DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL ), NULL, NULL);

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
 *      nCmdID          命令菜单ID，与控制有一个映射的关系
 */
bool CSMVPhasesequeDlg::_switchCtrlBlk( UINT nCmdID )
{
    ASSERT(m_pPageDataExtra);

    // 保留切换前的控制块对象
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // 设置原控制块的通道焦点与缩放倍数
    if(pData)
    {
        pData->nFoucs = m_reportPhase.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }

    // 根据新的控制块重设报表数据
    _resetReport();

    return true;
}


void CSMVPhasesequeDlg::OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ASSERT(m_pCurrentCtrlBlk);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);
    ASSERT(pItemNotify->rowIndex >= 0 && pItemNotify->rowIndex < 3);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        pResult = FALSE;
        return;
    }

    *pResult = TRUE;

    // 获取控制块绑定的数据
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    int nSelChann = (int)pItemNotify->pConstraint->GetConstraintData();

    // 设置新选择的通道信息
    pData->nChannelIndex[pItemNotify->rowIndex] = nSelChann;
    pData->bChannelValid[pItemNotify->rowIndex] = true;

    // 如果第一行选中了A相类型的通道，则自动就近为下两行选中BC相通道
    if(0 == pItemNotify->rowIndex)
    {
        CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
        ASSERT(pCfg);

        CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSelChann);
        ASSERT(pInfo);

        // 第一行并且是A相类型通道
        if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
        {
            // 依序查找BC相通道
            uint32_t nA = nSelChann;
            uint32_t nB = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_B, nA + 1);
            uint32_t nC = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

            if(nB != UINT_MAX && nC != UINT_MAX)
            {
                pData->nChannelIndex[1] = nB;
                pData->nChannelIndex[2] = nC;

                pData->bChannelValid[1] = true;
                pData->bChannelValid[2] = true;

                // 向数据层设置相量通道信息
                m_data.SetXLMode(pInfo->enChannelType);
                m_data.SetABCChannles(nA, nB, nC);

                m_reportPhase.SetRowItemText(1, 1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
                m_reportPhase.SetRowItemText(2, 1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());

                // 更新新通道的显示单位
                _updateUnit();

                return;
            }
        }
    }

    // 更换通道后，判断通道的电压电流类型信息
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfoA = pCfg->GetOneChannelInfo(pData->nChannelIndex[0]);
    CSmvRecvChannelInfo* pInfoB = pCfg->GetOneChannelInfo(pData->nChannelIndex[1]);
    CSmvRecvChannelInfo* pInfoC = pCfg->GetOneChannelInfo(pData->nChannelIndex[2]);

    if(pInfoA && pInfoB && pInfoC && 
        pInfoA->enChannelType == pInfoB->enChannelType &&
        pInfoB->enChannelType == pInfoC->enChannelType)
    {
        // 设置序量模式
        m_data.SetXLMode(pInfoA->enChannelType);
        m_data.SetABCChannles(pInfoA->nIndex, pInfoB->nIndex, pInfoC->nIndex);
    }

    // 更新新通道的显示单位
    _updateUnit();
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
 *      int             返回符合条件的通道号，找不到则返回 UINT_MAX
 */
uint32_t CSMVPhasesequeDlg::_findChannel( enumChannelType eType, ChannelPhase ePhase, int nStart )
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
 *      保存页面
 */
void CSMVPhasesequeDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(pData)
    {
        // 保存行焦点与放大倍数
        pData->nFoucs = m_reportPhase.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }
}

/**
 *      使用页面数据设置通道
 */
bool CSMVPhasesequeDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVPhasesequeDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);


    // 获取当前的控制块索引
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }

    uint32_t nChannels[3]           = {UINT_MAX, UINT_MAX, UINT_MAX};
    m_nTimes                        = pData->nTimes;
    CELRecordItem* pItem            = NULL;
    CELRecordItemConstraint* pCon   = NULL;

    // 设置原来的通道信息
    for(int i = 0; i < 3; i++ )
    {
        pItem = m_reportPhase.GetRecordRow(i)->GetItem(1);
        ASSERT(pItem);

        if(pItem)
        {
            // 查找指定通道索引的通道描述
            pCon = pItem->GetEditOptions(NULL)->FindConstraint(pData->nChannelIndex[i]);

            if(pCon)
            {
                nChannels[i]            = pData->nChannelIndex[i];
                pData->bChannelValid[i] = true;

                // 设置选中通道的描述到报表
                m_reportPhase.SetRowItemText(i, 1, pCon->GetConstraintCaption());
            }
            else
            {
                pData->bChannelValid[i] = false;

                m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
#if SU_FEI
                m_reportPhase.SetRowItemText(i + 6, 1, g_csBlankValue);
#else
				m_reportPhase.SetRowItemText(i+4, 1, g_csBlankValue);
#endif
            }
        }
    }

    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[0]);

    // 设置序量模式的计算的类型
    if(pInfo)
    {
        m_data.SetXLMode(pInfo->enChannelType);
    }
    else
    {
        m_data.SetXLMode(CHANNEL_TYPE_SMV_VOLTAGE);
    }

    // 设置序量模式的ABC三相通道索引
    m_data.SetABCChannles(nChannels[0], nChannels[1], nChannels[2]);

    if(pData->nFoucs >= 0 && pData->nFoucs < m_reportPhase.GetRecordRowCount())
    {
        m_reportPhase.SetFocusedSelectionRowIndex(pData->nFoucs);
    }

    return true;
}

void CSMVPhasesequeDlg::OnMenuF4()
{
    if(!m_bChannelChange)
    {
        // 切换缩放/选择模式
        m_dwMode = m_dwMode == PHASASEQ_MODE_ZOOM ? PHASASEQ_MODE_SELECT : PHASASEQ_MODE_ZOOM;

        // 切换模式为『选择』
        if(PHASASEQ_MODE_SELECT == m_dwMode)
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


BOOL CSMVPhasesequeDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        // 若是缩放模式
        if(!m_bChannelChange && PHASASEQ_MODE_ZOOM == m_dwMode)
        {
            switch(pMsg->wParam)
            {
                // 放大图形
            case VK_UP:
                if(PHASASEQ_TIME_MIN < m_nTimes)
                {
                    m_nTimes--;
                    _updatePhasae();
                }

                return TRUE;

                // 缩小图形
            case VK_DOWN:
                if(PHASASEQ_TIME_MAX > m_nTimes)
                {
                    m_nTimes++;
                    _updatePhasae();
                }

                return TRUE;
            }
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      更新所选通道的单位
 */
void CSMVPhasesequeDlg::_updateUnit()
{
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    // 获取控制块绑定的数据
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    CSmvRecvChannelInfo* pInfo      = NULL;

    for(int i = 0; i < 3; i++)
    {
        // 通道有效将设置单位
        if(pData->bChannelValid[i])
        {
            pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[i]);

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
 *      检查通道是否发生变化
 */
bool CSMVPhasesequeDlg::_checkChannel()
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
            m_reportPhase.GetRecordRow(0)->GetItem(1)->SetEditable(FALSE);
            m_reportPhase.GetRecordRow(1)->GetItem(1)->SetEditable(FALSE);
            m_reportPhase.GetRecordRow(2)->GetItem(1)->SetEditable(FALSE);
#if SU_FEI
            // 设置通道的有效值item为L"--"
            m_reportPhase.SetRowItemText(6, 1, g_csBlankValue);
            m_reportPhase.SetRowItemText(7, 1, g_csBlankValue);
            m_reportPhase.SetRowItemText(8, 1, g_csBlankValue);
#else
			// 设置通道的有效值item为L"--"
			m_reportPhase.SetRowItemText(4, 1, g_csBlankValue);
			m_reportPhase.SetRowItemText(5, 1, g_csBlankValue);
			m_reportPhase.SetRowItemText(6, 1, g_csBlankValue);
#endif
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
            m_reportPhase.GetRecordRow(0)->GetItem(1)->SetEditable(TRUE);
            m_reportPhase.GetRecordRow(1)->GetItem(1)->SetEditable(TRUE);
            m_reportPhase.GetRecordRow(2)->GetItem(1)->SetEditable(TRUE);

            // 恢复部分菜单标题
           // m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASASEQ_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVPhasesequeDlg::OnInplaceButtonDown( NMHDR* , LRESULT* pResult )
{
    if(!m_bEnableTimer)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
        *pResult = TRUE;
    }
}


