/**
 * \file
 *      SMVDualphaseDlg.cpp
 *
 * \brief
 *      核相页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */


//  SMVDualphaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVDualphaseDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"


#define DSM_PHASOR_REPORT_VAL_RECT          CRect(316, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< 有效值报表区域

#define RECT_SETV(rc,l,t,r,b)   rc.left = l; \
                                rc.top = t; \
                                rc.right = r; \
                                rc.bottom = b;

#define GRAM_TIPS_COLOR         RGB(128,128,135)      ///< 暖灰色，核相图内四边文字颜色

#define PHASOR_COL0             0               ///< 有效值列索引
#define PHASOR_COL1             1               ///< 同相/相间差索引

#define PHASOR_A                1               ///< SMV组1第1个通道索引
#define PHASOR_B                2               ///< SMV组1第2个通道索引
#define PHASOR_C                3               ///< SMV组1第3个通道索引
#define PHASOR_a                6               ///< SMV组2第1个通道索引
#define PHASOR_b                7               ///< SMV组2第2个通道索引
#define PHASOR_c                8               ///< SMV组2第3个通道索引

#define PHASOR_FREQ1            0               ///< SMV组1频率索引
#define PHASOR_FREQ2            5               ///< SMV组2频率索引

#if !CHUN_HUA
extern COLORREF m_Color[];// ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

namespace SMV_DUALPHASOR
{
    /**
     *      根据格式化字符串与参数获取格式化字符串
     *
     *  \param[in]
     *      csFmt           格式化字符串
     *  \param[in]
     *      fvalue          格式化参数
     *  \return
     *      CString         返回格式化好的字符串
     */
    inline CString g_GetResultString(const CString csFmt, float fvalue)
    {
        CString csRet;

        csRet.Format(csFmt, fvalue);

        return csRet;
    }

    /**
     *      根据格式化字符串与参数获取格式化字符串
     *
     *  \param[in]
     *      csFmt           格式化字符串
     *  \param[in]
     *      csUnit          单位
     *  \param[in]
     *      fvalue1         格式化参数1
     *  \param[in]
     *      fvalue2         格式化参数2
     *  \return
     *      CString         返回格式化好的字符串
     */
    inline CString g_GetResultString(const CString csFmt, const CString csUnit, float fvalue1, float fvalue2)
    {
        CString csRet;

        csRet.Format(csFmt, fvalue1, csUnit, fvalue2);

        return csRet;
    }
}


/**
 *      SMV接收 - 核相 控制块记录
 */

class CDsmSmvDualPhaseCtrlBlkRecord : public CELRecord
{
public:
    CDsmSmvDualPhaseCtrlBlkRecord(CDsmDataLayer* pDataLayer, CString csCaption, CString csSubCaption);
    virtual ~CDsmSmvDualPhaseCtrlBlkRecord() {}
};

/**
 *      SMV接收 - 核相 通道记录
 */
class CDsmSmvDualPhaseChannelRecord : public CELRecord
{
public:
    CDsmSmvDualPhaseChannelRecord(CBaseDetectItem* pCtrlBlk, CString csCaption);
    virtual ~CDsmSmvDualPhaseChannelRecord() {}

public:

    /**
     *      根据指定控制块更新通道列表
     */
    void UpdateChannels(CBaseDetectItem* pCtrlBlk);

private:

    /**
     *      加载指定控制块的通道列表
     */
    void _loadChannels(CBaseDetectItem* pCtrlBlk, CELRecordItem* pItem);
};


CDsmSmvDualPhaseChannelRecord::CDsmSmvDualPhaseChannelRecord( CBaseDetectItem* pCtrlBlk, CString csCaption )
{
    AddItem(new CDsmRecordItem(csCaption));
    CELRecordItem* pItem = AddItem(new CDsmRecordItem(g_csBlankValue));

    if(pCtrlBlk)
    {
        pItem->SetEditable(TRUE);
        pItem->GetEditOptions(NULL)->AddComboButton();

        // 加载通道列表
        _loadChannels(pCtrlBlk, pItem);
    }
}

void CDsmSmvDualPhaseChannelRecord::UpdateChannels( CBaseDetectItem* pCtrlBlk )
{
    CELRecordItem* pItem = GetItem(1);

    if(pCtrlBlk && pItem)
    {
        // 加载通道列表
        _loadChannels(pCtrlBlk, pItem);
    }
}

void CDsmSmvDualPhaseChannelRecord::_loadChannels( CBaseDetectItem* pCtrlBlk, CELRecordItem* pItem )
{
    ASSERT(pCtrlBlk);
    ASSERT(pItem);

    CSmvRecvConfig* pConfig         = pCtrlBlk->pRecvConfig;
    CSmvRecvChannelInfo* pInfo      = NULL;
    size_t nCounts                  = pConfig->GetChannelCount();
    CELRecordItemEditOptions* pEdit = pItem->GetEditOptions(NULL);
    ASSERT(pEdit);

    // 移除旧通道数据
    pEdit->GetConstraints()->RemoveAll();

    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(i);

        if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType &&
            CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
        {
            continue;
        }

        pEdit->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
    }
}

CDsmSmvDualPhaseCtrlBlkRecord::CDsmSmvDualPhaseCtrlBlkRecord( CDsmDataLayer* pDataLayer, CString csCaption, CString csSubCaption )
{
    ASSERT(pDataLayer);

    AddItem(new CDsmRecordItem(csCaption));
    CELRecordItem* pItem = AddItem(new CDsmRecordItem(g_csBlankValue));

    CBaseDetectItem* pCtrlBlkBase       = pDataLayer->GetFirstSelectCtrlBlk();;
    CELRecordItemEditOptions* pOptions  = pItem->GetEditOptions(NULL);

    pItem->SetEditable(TRUE);
    pOptions->AddComboButton();

    CString csCtrlBlk;

    while(pCtrlBlkBase)
    {
        csCtrlBlk = DSM_GetCtrlBlkInfo(pCtrlBlkBase);
        pOptions->AddConstraint(csCtrlBlk, pCtrlBlkBase->nID);

        pCtrlBlkBase    = pDataLayer->GetNextSelectCtrlBlk(pCtrlBlkBase);
    }
}




// CSMVDualphaseDlg 对话框

IMPLEMENT_DYNAMIC(CSMVDualphaseDlg, DSM_SMVBASEDLG)

CSMVDualphaseDlg::CSMVDualphaseDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVDualphaseDlg::IDD, DSM_STR_SMV_DUALPHASE_CAPTION, pParent)
    , m_pCtrlBlkA(NULL)
    , m_pCtrlBlkB(NULL)
    , m_pPageDataExtraDual(NULL)
    , m_bGram(true)
    , m_bSamePhase(true)
    , m_bLastStatus(true)
     , m_fScaleInterval(0.0f)
    , m_csFmtFreq(L"Freq = %0.3f Hz")
    , m_csFmtEffect(L"%0.3f%s/%0.3f°")
{
    for(int i = 0; i < 3; i++)
    {
        m_fInPhaseAngleDiff[i]    = 0.0f;

        m_stPhaseVal1[i].m_fPhase = 0.0f;
        m_stPhaseVal1[i].m_fValue = 0.0f;

        m_stPhaseVal2[i].m_fPhase = 0.0f;
        m_stPhaseVal2[i].m_fValue = 0.0f;

        m_stPhaseData1[i].m_fPhase = 0.0f;
        m_stPhaseData1[i].m_fValue = 0.0f;

        m_stPhaseData2[i].m_fPhase = 0.0f;
        m_stPhaseData2[i].m_fValue = 0.0f;
    }

    for(int i = 0; i < 6; i++)
    {
        m_csDesc[i] = dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_PHASE_A1 + i);
    }
}

CSMVDualphaseDlg::~CSMVDualphaseDlg()
{
    CStatusMgr::SetTips(L"", 0);
}

void CSMVDualphaseDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVDualphaseDlg, DSM_SMVBASEDLG)
    ON_COMMAND(ID_MENU_SMV_F2, &CSMVDualphaseDlg::OnMenuF2)
   // ON_COMMAND(ID_MENU_SMV_F3, &CSMVDualphaseDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVDualphaseDlg::OnMenuF4)
    ON_COMMAND(ID_MENU_SMV_F5, &CSMVDualphaseDlg::OnMenuF5)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_DULAPHASE_REPORT, &CSMVDualphaseDlg::OnItemSelectChange)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_DULAPHASE_REPORT, &CSMVDualphaseDlg::OnInplaceButtonDown)
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CSMVDualphaseDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 值报表创建可以优化
    _createReportVal();
    _createReport();
    _createDualphaseGram();

    // 没有虚函数？？？
    _updateMenuNavi();

    // 根据页面状态数据的状态模式信息，切换状态
    _switchF4();        // 图形/报表
    _switchF5();        // 相间/同相

    // 启动定时器
    _beginTimer(800);

    m_reportPhase.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


// CSMVDualphaseDlg 消息处理程序


/**
 *      创建相量图
 */
void CSMVDualphaseDlg::_createDualphaseGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhaseGram.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_DUALPHASE_PHASEGRAM);

    m_ctrlPhaseGram.ModifyStyle(0, SS_OWNERDRAW);

    // 设置相别线条颜色
    m_ctrlPhaseGram.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhaseGram.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhaseGram.SetPhasorColor(2, DSM_PHASE_COLOR_C);
    m_ctrlPhaseGram.SetPhasorColor(3, DSM_PHASE_COLOR_A);
    m_ctrlPhaseGram.SetPhasorColor(4, DSM_PHASE_COLOR_B);
    m_ctrlPhaseGram.SetPhasorColor(5, DSM_PHASE_COLOR_C);

    m_ctrlPhaseGram.GetClientRect(rcPhasor);

    // 设置图中信息区域
    RECT_SETV(m_rcAa, rcPhasor.right - 110, rcPhasor.top + 10, rcPhasor.right - 10, rcPhasor.top + 34)
    RECT_SETV(m_rcBb, rcPhasor.left + 30, rcPhasor.top + 10, rcPhasor.left + 130, rcPhasor.top + 34)
    RECT_SETV(m_rcCc, rcPhasor.left + 30, rcPhasor.bottom - 34, rcPhasor.left + 130, rcPhasor.bottom -10)
    RECT_SETV(m_rcF1, rcPhasor.right - 110, rcPhasor.bottom - 58, rcPhasor.right - 10, rcPhasor.bottom - 34)
    RECT_SETV(m_rcF2, rcPhasor.right - 110, rcPhasor.bottom - 34, rcPhasor.right - 10, rcPhasor.bottom - 10)

    // 设置相量图样式
    m_ctrlPhaseGram.SetPhasorExtDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_ELEC_CTRL_DEFT_FT));
    m_ctrlPhaseGram.SetPhasorDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_ELEC_CTRL_DEFT_FT));
    m_ctrlPhaseGram.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhaseGram.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhaseGram.Init();
}

/**
 *      初始化核相业务的smv1与smv2控制块以及其选中的ABC三相通道
 */
void CSMVDualphaseDlg::_initPageStatus()
{
    ASSERT(m_pPageDataExtraDual);

    // 获取核相页面状态数据
    size_t nCounts  = m_pDataLayer->GetSelectedCtrlBlkCount();
    m_pCtrlBlkA     = m_pDataLayer->GetSelectCtrlBlkByID(m_pPageDataExtraDual->nSmv1);      // 获取smv1
    m_pCtrlBlkB     = m_pDataLayer->GetSelectCtrlBlkByID(m_pPageDataExtraDual->nSmv2);      // 获取smv2
    m_bGram         = m_pPageDataExtraDual->bGram;                                          // 获取图形模式
    m_bSamePhase    = m_pPageDataExtraDual->bSame;                                          // 获取相间模式

    // 若smv1无效，则从列表选一个
    if(!m_pCtrlBlkA)
    {
        m_pCtrlBlkA = m_pDataLayer->GetFirstSelectCtrlBlk();

        // 选中控制块数量大于1，则会选择一个与smv2不相同的控制块
        if(nCounts > 1)
        {
            for(size_t i = 0; i < nCounts; i++)
            {
                if(!m_pCtrlBlkB || m_pCtrlBlkA->nID != m_pCtrlBlkB->nID)
                {
                    break;
                }

                m_pCtrlBlkA = m_pDataLayer->GetNextSelectCtrlBlk(m_pCtrlBlkA);
            }
        }

        // 指定的控制块寻找合适的ABC三相通道显示
        m_pPageDataExtraDual->nSmv1 = m_pCtrlBlkA->nID;
        _calcChannel(m_pCtrlBlkA, ORDER_PHASE_A);
    }

    if(!m_pCtrlBlkB)
    {
        m_pCtrlBlkB = m_pDataLayer->GetFirstSelectCtrlBlk();

        // 选中控制块数量大于1，则会选择一个与smv1不相同的控制块
        if(nCounts > 1)
        {
            for(size_t i = 0; i < nCounts; i++)
            {
                if(!m_pCtrlBlkA || m_pCtrlBlkA->nID != m_pCtrlBlkB->nID)
                {
                    break;
                }

                m_pCtrlBlkB = m_pDataLayer->GetNextSelectCtrlBlk(m_pCtrlBlkB);
            }
        }

        // 指定的控制块寻找合适的ABC三相通道显示
        m_pPageDataExtraDual->nSmv2 = m_pCtrlBlkB->nID;
        _calcChannel(m_pCtrlBlkB, ORDER_PHASE_B);
    }

    // 设置核相的smv1与smv2
    m_pDataLayer->SetPhaseCheckSmv1CtrBlk(m_pCtrlBlkA);
    m_pDataLayer->SetPhaseCheckSmv2CtrBlk(m_pCtrlBlkB);

    uint32_t nChannelA[3] = {UINT_MAX, UINT_MAX, UINT_MAX};
    uint32_t nChannelB[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    for(int i = 0; i < 3; i++)
    {
        if(m_pPageDataExtraDual->bChannelValidA[i])
        {
            nChannelA[i] = m_pPageDataExtraDual->nChannelA[i];
        }

        if(m_pPageDataExtraDual->bChannelValidB[i])
        {
            nChannelB[i] = m_pPageDataExtraDual->nChannelB[i];
        }
    }

    // 设置核相业务的smv1与smv2选中的通道号
    m_pDataLayer->SetSmv1ABCChannles(nChannelA[0], nChannelA[1], nChannelA[2]);
    m_pDataLayer->SetSmv2ABCChannles(nChannelB[0], nChannelB[1], nChannelB[2]);
}

/**
 *      创建待核相报表
 */
void CSMVDualphaseDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // 调整高度
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;

    //rcReport.OffsetRect(DSM_MARGIN_IN, 0);
	rcReport.DeflateRect(0, 0, 0, 0);

    m_reportPhase.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_DULAPHASE_REPORT);

    // 设置字体
    m_reportPhase.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportPhase.PostInitTemplateStyle();
    m_reportPhase.SetHeaderHeight(0);
	m_reportPhase.SetHasGridline(FALSE, TRUE);

    // 插入列元素
    m_reportPhase.InsertColumn(PHASOR_COL0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);     // 标题
    m_reportPhase.InsertColumn(PHASOR_COL1, L"", EL_CENTER, DSM_PHASOR_REPORT_VALUE);       // 内容

    // 设置选择焦点样式
    m_reportPhase.SetRowSelectionVisible(FALSE);
    m_reportPhase.SetRowFocusEnable(TRUE);
    m_reportPhase.SetSubItemFocusEnable(TRUE);
    m_reportPhase.SetFreezeColumnsCount(1);
    m_reportPhase.SetFocusedColIndex(1);

    // 设置报表的值
    _initReport();
}

/**
 *      创建有效值表
 */
void CSMVDualphaseDlg::_createReportVal()
{
    CRect rcReport(DSM_PHASOR_REPORT_VAL_RECT);
    DSM_ParentToChild(this, rcReport);

    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // 调整高度
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;

   // rcReport.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
	rcReport.DeflateRect(0, 0, 0, 0);
	
    m_reportValue.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL , 
        rcReport, this, IDC_SMV_DULAPHASE_VALUE_REPORT);

    // 设置样式
    m_reportValue.PostInitTemplateStyle();

    m_reportValue.SetHeaderHeight(0);
	m_reportValue.SetHasGridline(FALSE, TRUE);

    // 设置字体
    m_reportValue.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 插入列元素
    m_reportValue.InsertColumn(PHASOR_COL0, L"", EL_CENTER, DSM_PHASOR_REPORT_EFFECTIVE);
    m_reportValue.InsertColumn(PHASOR_COL1, L"", EL_CENTER, DSM_PHASOR_REPORT_ANGEL);

    // 设置选择焦点样式
    m_reportValue.SetRowSelectionVisible(FALSE);
    m_reportValue.SetRowFocusEnable(FALSE);

    for(int i = 0; i < 9; i++)
    {
        m_reportValue.InsertRecordRow(i, new CDsmSmvDualPhaseChannelRecord(NULL, L""));
    }
	m_reportValue.SetRowItemText(4, 1, L"");
    
	

    // 初始化item数组
    m_arrayItem[0] = m_reportValue.GetRecordRow(PHASOR_FREQ1)->GetItem(PHASOR_COL0);
    m_arrayItem[1] = m_reportValue.GetRecordRow(PHASOR_FREQ2)->GetItem(PHASOR_COL0);
    m_arrayItem[2] = m_reportValue.GetRecordRow(PHASOR_A)->GetItem(PHASOR_COL0);
    m_arrayItem[3] = m_reportValue.GetRecordRow(PHASOR_B)->GetItem(PHASOR_COL0);
    m_arrayItem[4] = m_reportValue.GetRecordRow(PHASOR_C)->GetItem(PHASOR_COL0);
    m_arrayItem[5] = m_reportValue.GetRecordRow(PHASOR_a)->GetItem(PHASOR_COL0);
    m_arrayItem[6] = m_reportValue.GetRecordRow(PHASOR_b)->GetItem(PHASOR_COL0);
    m_arrayItem[7] = m_reportValue.GetRecordRow(PHASOR_c)->GetItem(PHASOR_COL0);
    m_arrayItem[8] = m_reportValue.GetRecordRow(PHASOR_A)->GetItem(PHASOR_COL1);
    m_arrayItem[9] = m_reportValue.GetRecordRow(PHASOR_B)->GetItem(PHASOR_COL1);
    m_arrayItem[10] = m_reportValue.GetRecordRow(PHASOR_C)->GetItem(PHASOR_COL1);
    m_arrayItem[11] = m_reportValue.GetRecordRow(PHASOR_a)->GetItem(PHASOR_COL1);
    m_arrayItem[12] = m_reportValue.GetRecordRow(PHASOR_b)->GetItem(PHASOR_COL1);
    m_arrayItem[13] = m_reportValue.GetRecordRow(PHASOR_c)->GetItem(PHASOR_COL1);
}


void CSMVDualphaseDlg::_initReport()
{
    int nIndex = 0;

    // 插入smv1的控制块与通道行
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseCtrlBlkRecord(m_pDataLayer, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SMV1), dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEANGDIFF)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_A)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_B)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_C)));

    int nFocusDisable = m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(NULL, L""));

    m_reportPhase.SetRowItemText(nFocusDisable, 1, L"");

    // 插入smv的控制块与通道行
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseCtrlBlkRecord(m_pDataLayer, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SMV2), dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEDIFF)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_a)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_b)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_c)));

    // 中间的空白行禁止选中
    m_reportPhase.GetRecordRow(nFocusDisable)->SetFocusable(FALSE);

    // 根据页面状态信息自动调整smv控制块与选中的报表
    _adjustChannel();
}


/**
 *      根据页面状态信息自动调整smv控制块与选中的报表
 */
void CSMVDualphaseDlg::_adjustChannel()
{
    ASSERT(m_pPageDataExtraDual);

    _setChannel(ORDER_PHASE_A);         // 设置smv1
    _setChannel(ORDER_PHASE_B);         // 设置smv2

    // 设置焦点
    if(m_pPageDataExtraDual->nFoucs >= 0 && m_pPageDataExtraDual->nFoucs < m_reportPhase.GetRecordRowCount())
    {
        m_reportPhase.SetFocusedSelectionRowIndex(m_pPageDataExtraDual->nFoucs);
    }
}

/**
 *      根据指定信息设置一组通道，包括设置控制块以及ABC三相通道
 */
void CSMVDualphaseDlg::_setChannel( ePhaseOrder order )
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pPageDataExtraDual);

    int nIndex                  = 0;
    bool* pValid                = NULL;
    uint32_t* pChannels         = NULL;
    CBaseDetectItem* pCtrlBlk   = NULL;

    // 设置通用变量
    if(ORDER_PHASE_A == order)
    {
        pCtrlBlk        = m_pCtrlBlkA;
        nIndex          = 0;
        pValid          = &m_pPageDataExtraDual->bChannelValidA[0];
        pChannels       = &m_pPageDataExtraDual->nChannelA[0];
    }
    else
    {
        pCtrlBlk        = m_pCtrlBlkB;
        nIndex          = 5;
        pValid          = &m_pPageDataExtraDual->bChannelValidB[0];
        pChannels       = &m_pPageDataExtraDual->nChannelB[0];
    }

    //
    // 根据页面数据恢复报表选项
    //

    // 设置选中控制块的描述
    CELRecordItemConstraint* pCons = m_reportPhase.GetRecordRow(nIndex)->GetItem(PHASOR_COL1)->GetEditOptions(NULL)->FindConstraint(pCtrlBlk->nID);
    ASSERT(pCons);

    m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, pCons->GetConstraintCaption());

    nIndex++;   // 累加到通道row

    // 设置控制块选中的通道描述
    for(int i = 0; i < 3; i++)
    {
        if(pChannels[i] != UINT_MAX)
        {
            pCons = m_reportPhase.GetRecordRow(nIndex)->GetItem(PHASOR_COL1)->GetEditOptions(NULL)->FindConstraint(pChannels[i]);

            if(pCons)
            {
                pValid[i] = true;

                m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, pCons->GetConstraintCaption());
            }
            else
            {
                pValid[i] = false;

                m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, g_csBlankValue);
            }
        }

        // 更新新通道的单位
        _updateUnit(pCtrlBlk, nIndex++, pChannels[i]);
    }
}

/**
 *      为指定的控制块寻找合适的ABC三相通道显示
 *
 *  \param[in]
 *      pCtrlBlk         指定的控制块对象
 *  \param[in]
 *      order           smv组序，smv1或者smv2
 */
void CSMVDualphaseDlg::_calcChannel( CBaseDetectItem* pCtrlBlk, ePhaseOrder order )
{
    ASSERT(pCtrlBlk);
    ASSERT(m_pPageDataExtraDual);

    CSmvRecvConfig* pCfg        = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo  = NULL;
    size_t nCounts              = pCfg->GetChannelCount();

    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // 选取首次出现电压类型的三相
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // 查找顺序的ABC三相通道
            int nA = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_A, i);
            int nB = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_B, nA + 1);
            int nC = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_C, nB + 1);

            // 若没有，则从i开始找非顺序的ABC三相通道
            if(nA == UINT_MAX || nB == UINT_MAX || nC == UINT_MAX)
            {
                nA = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_A, i);
                nB = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_B, i);
                nC = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_C, i);
            }

            // 若找到则把ABC三相通道索引设置到页面状态数据中
            if(nA != UINT_MAX && nB != UINT_MAX && nC != UINT_MAX)
            {
                if(ORDER_PHASE_A == order)
                {
                    m_pPageDataExtraDual->nChannelA[0] = nA;
                    m_pPageDataExtraDual->nChannelA[1] = nB;
                    m_pPageDataExtraDual->nChannelA[2] = nC;

                    for(int k = 0; k < 3; k++)
                    {
                        m_pPageDataExtraDual->bChannelValidA[k] = true;
                    }
                }
                else
                {
                    m_pPageDataExtraDual->nChannelB[0] = nA;
                    m_pPageDataExtraDual->nChannelB[1] = nB;
                    m_pPageDataExtraDual->nChannelB[2] = nC;

                    for(int k = 0; k < 3; k++)
                    {
                        m_pPageDataExtraDual->bChannelValidB[k] = true;
                    }
                }

                // 完成设置控制块下的通道值，退出
                return;
            }
            else
            {
                // 离开循环
                break;
            }
        }
    }

    // 没找到合适的通道值，则随便选三个类型的通道

    bool* pValids           = &m_pPageDataExtraDual->bChannelValidA[0];
    uint32_t* pChannels     = &m_pPageDataExtraDual->nChannelA[0];

    if(ORDER_PHASE_B == order)
    {
        pValids     = &m_pPageDataExtraDual->bChannelValidB[0];
        pChannels   = &m_pPageDataExtraDual->nChannelB[0];
    }


    // 随便选取三个类型
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            pValids[j]      = true;
            pChannels[j]    = pInfo->nIndex;
        }
    }

    // 检查是否三个通道都设置了通道号

    if(pChannels[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(pChannels[i] == UINT_MAX)
            {
                pValids[i]      = true;
                pChannels[i]    = pChannels[0];
            }
        }
    }
}


/**
 *      返回符合条件的通道号
 *
 *  \param[in]
 *      pCtrlBlk        指定的控制块
 *  \param[in]
 *      eType           指定的通道类型
 *  \param[in]
 *      ePhase          指定的通道相别
 *  \param[in]
 *      nStart          通道的查询起始位置
 *  \return
 *      uint32_t        返回符合条件的通道号，找不到则返回 UINT_MAX
 */
uint32_t CSMVDualphaseDlg::_findChannel( CBaseDetectItem* pCtrlBlk, enumChannelType eType, ChannelPhase ePhase, int nStart )
{
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg        = pCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 从指定位置开始查找合适条件的通道
    for(size_t i = nStart; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);

        if(pInfo && 
            eType == pInfo->enChannelType && 
            ePhase == pInfo->enChannelPhase)
        {
            return pInfo->nIndex;
        }
    }

    return UINT_MAX;
}


/**
 *      保存页面
 */
void CSMVDualphaseDlg::_SavePage()
{
    ASSERT(m_pPageDataExtraDual);

    m_pPageDataExtraDual->nFoucs = m_reportPhase.GetFocusedRowIndex();

    m_pPageDataExtraDual->bGram = m_bGram;          // 设置图形表格模式
    m_pPageDataExtraDual->bSame = m_bSamePhase;     // 设置表格的相间模式
}

void CSMVDualphaseDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT * pResult )
{
    ASSERT(m_pPageDataExtraDual);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        *pResult = FALSE;
        return;
    }

    uint32_t nChannel[3] = {UINT_MAX, UINT_MAX, UINT_MAX};
    uint32_t nSelChan = (uint32_t)pItemNotify->pConstraint->GetConstraintData();

    switch(pItemNotify->rowIndex)
    {
        // 改变smv1或smv2控制块
    case 0:
        _adjustCtrlBlk(ORDER_PHASE_A, nSelChan);
        break;

    case 5:
        _adjustCtrlBlk(ORDER_PHASE_B, nSelChan);
        break;

    case 1:
        // A相通道改变，可能引起BC相通道的一起改变（查询一组ABC通道）
        _adjustABC(ORDER_PHASE_A, m_pCtrlBlkA, nSelChan);

    case 2:
    case 3:

        // 单独改变ABC通道
        m_pPageDataExtraDual->bChannelValidA[pItemNotify->rowIndex - 1] = true;
        m_pPageDataExtraDual->nChannelA[pItemNotify->rowIndex - 1]      = nSelChan;

        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidA[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelA[i];
            }
        }

        m_pDataLayer->SetSmv1ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
        _updateUnit(m_pCtrlBlkA, pItemNotify->rowIndex, nSelChan);

        break;

    case 6:

        // A相通道改变，可能引起BC相通道的一起改变（查询一组ABC通道）
        _adjustABC(ORDER_PHASE_B, m_pCtrlBlkB, nSelChan);

    case 7:
    case 8:
        // 单独改变ABC通道
        m_pPageDataExtraDual->bChannelValidB[pItemNotify->rowIndex - 6] = true;
        m_pPageDataExtraDual->nChannelB[pItemNotify->rowIndex - 6]      = nSelChan;

        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidB[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelB[i];
            }
        }

        m_pDataLayer->SetSmv2ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
        _updateUnit(m_pCtrlBlkB, pItemNotify->rowIndex, nSelChan);

        break;

    default:
        break;
    }

    *pResult = TRUE;
}


void CSMVDualphaseDlg::OnMenuF2()
{

}


void CSMVDualphaseDlg::OnMenuF4()
{
    if(m_bChannelChange)
    {
        return;
    }

    m_bGram = m_bGram ? false : true;

    // 刷新菜单
    _updateMenuNavi();

    // 切换界面显示
    _switchF4();
}

void CSMVDualphaseDlg::OnMenuF5()
{
    if(m_bChannelChange)
    {
        return;
    }

    if(!m_bGram)
    {
        m_bSamePhase = m_bSamePhase ? false : true;

        _updateMenuNavi();

        // 切换报表显示同相或相间数据
        _switchF5();
    }
}

//void CSMVDualphaseDlg::OnMenuF3()
//{
//    if(m_bChannelChange)
//    {
//        return;
//    }
//
//#if SU_FEI
//    // 启动定时器
//    if(0 == m_nTimerID)
//    {
//        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
//        m_pNaviBar->UpdateNaviMenu();
//        _beginTimer();
//    }
//    // 停止定时器
//    else
//    {
//        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_START));
//        m_pNaviBar->UpdateNaviMenu();
//        _stopTimer();
//
//        // 暂停时，报表和图形的数据要保持一致
//
//        // 图形模式时，需要刷新报表的数据
//        if(m_bGram)
//        {
//            _updateDataReport();
//        }
//        // 报表模式时，需要刷新图形的数据
//        else
//        {
//            _updateDataGram();
//        }
//    }
//#endif
//}

void CSMVDualphaseDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	//m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // 设置F4表格图形图标
    //m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_TABLEGRAPH_GRAPH : DSM_BMP_M_TABLEGRAPH_TABLE), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_SMV_SCAN_REPORT_SEL : DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL), NULL, NULL);
    // 修改F5字符串为『』
    m_menuMain.SetMenuItemCaption(4, L"");
}


/**
 *      F4切换功能，表格与相量图的切换显示
 */
void CSMVDualphaseDlg::_switchF4()
{
    // 切换显示有效值报表与相量图

    // 图形/表格模式切换
    if(m_bGram)
    {
        m_reportValue.ShowWindow(SW_HIDE);
        m_ctrlPhaseGram.ShowWindow(SW_SHOW);
    }
    else
    {
        m_ctrlPhaseGram.ShowWindow(SW_HIDE);
        m_reportValue.ShowWindow(SW_SHOW);
    }
}

/**
 *      F5切换功能，在表格模式下，切换显示同相相角差或相间相量差
 */
void CSMVDualphaseDlg::_switchF5()
{
    // 切换同相相间功能，主要设置标题

    // 同相  设置同相标题
    if(m_bSamePhase)
    {
        m_reportValue.SetRowItemText(0, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEANGDIFF));
        m_reportValue.SetRowItemText(5, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEDIFF));
    }
    // 相间  设置相间标题
    else
    {
        m_reportValue.SetRowItemText(0, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_BETWDIFF));
        m_reportValue.SetRowItemText(5, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_BETWDIFF));
    }

    // 立即更新报表数据
    _updateData();
}


/**
 *      更新菜单标题
 */
void CSMVDualphaseDlg::_updateMenuNavi()
{
    m_menuMain.SetMenuItemCaption(1, L"");

    // 图形模式
    if(m_bGram)
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL), NULL, NULL);

        // 隐藏F5标题
        m_menuMain.SetMenuItemBitmaps(4, NULL, NULL, NULL);
    }
    // 表格模式
    else
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_REPORT_SEL), NULL, NULL);

        // 同相
        if(m_bSamePhase)
        {
            m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_PHASE_SEL), NULL, NULL);
        }
        // 相间
        else
        {
            m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ALTERNATE_SEL), NULL, NULL);
        }
    }

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

    m_pNaviBar->UpdateNaviMenu();
}

/**
 *      第一个选中通道若是A相类型则自动为2，3通道选择紧接的BC相类型的通道
 *
 *  \param[in]
 *      eOrder          smv组序，smv1或smv2
 *  \param[in]
 *      pCtrlBlk        选中的控制块对象
 *  \param[in]
 *      nSel            控制块下选中的A相通道索引
 */
void CSMVDualphaseDlg::_adjustABC( ePhaseOrder eOrder, CBaseDetectItem* pCtrlBlk, uint32_t nSel )
{
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSel);

    if(!pInfo)
    {
        ASSERT(pInfo);
        return;
    }

    // 若选中的是A相类型，则尽量选中相邻的BC相类型通道
    if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
    {
        int nB = _findChannel(pCtrlBlk, pInfo->enChannelType, CHANNEL_PHASE_B, nSel + 1);
        int nC = _findChannel(pCtrlBlk, pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

        if(nB != UINT_MAX && nC != UINT_MAX)
        {
            ASSERT(m_pPageDataExtraDual);

            int nRedrawA = 1;

            if(eOrder == ORDER_PHASE_A)
            {
                m_pPageDataExtraDual->nChannelA[0] = nSel;
                m_pPageDataExtraDual->nChannelA[1] = nB;
                m_pPageDataExtraDual->nChannelA[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pPageDataExtraDual->bChannelValidA[k] = true;
                }
            }
            else
            {
                m_pPageDataExtraDual->nChannelB[0] = nSel;
                m_pPageDataExtraDual->nChannelB[1] = nB;
                m_pPageDataExtraDual->nChannelB[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pPageDataExtraDual->bChannelValidB[k] = true;
                }

                nRedrawA = 6;
            }

            m_reportPhase.SetRowItemText(nRedrawA + 1, PHASOR_COL1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
            m_reportPhase.SetRowItemText(nRedrawA + 2, PHASOR_COL1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());

            // 更新BC通道的单位
            _updateUnit(pCtrlBlk, nRedrawA + 1, nB);
            _updateUnit(pCtrlBlk, nRedrawA + 2, nC);
        }
    }
}

/**
 *      选中新控制块后，调整该控制块下ABC通道
 *
 *  \param[in]
 *      eOrder          smv组序，smv1或smv2
 *  \param[in]
 *      nSel            控制块索引，将会引起ABC三相通道的重新选择
 */
void CSMVDualphaseDlg::_adjustCtrlBlk( ePhaseOrder eOrder, uint32_t nSel )
{
    ASSERT(m_pDataLayer);

    CBaseDetectItem* pCtrlBlk = m_pDataLayer->GetSelectCtrlBlkByID(nSel);
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    int nRow            = 1;            // ABC通道起始行号
    uint32_t* pChannels = NULL;

    // 通用变量，方便设置
    if(eOrder == ORDER_PHASE_A)
    {
        m_pCtrlBlkA = pCtrlBlk;
        pChannels   = &m_pPageDataExtraDual->nChannelA[0];
    }
    else
    {
        m_pCtrlBlkB = pCtrlBlk;
        pChannels   = &m_pPageDataExtraDual->nChannelB[0];
        nRow        = 6;
    }

    CDsmSmvDualPhaseChannelRecord* pRecord = NULL;

    // 根据新的控制块为ABC三个列表加载通道
    for(int i = 0; i < 3; i++)
    {
        pRecord = dynamic_cast<CDsmSmvDualPhaseChannelRecord*>(m_reportPhase.GetRecordRow(nRow + i));
        ASSERT(pRecord);

        pRecord->UpdateChannels(pCtrlBlk);
    }

    // 先判断旧控制块下的ABC通道是否有效
    // 无效则为新控制块选中默认的ABC
    // 有效则不改动

    bool bEffect                = true;
    CSmvRecvChannelInfo* pInfo  = NULL;

    for(int i = 0; i < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(pChannels[i]);

        if(!pInfo ||
            pInfo->enChannelType != CHANNEL_TYPE_SMV_CURRENT || 
            pInfo->enChannelType != CHANNEL_TYPE_SMV_VOLTAGE)
        {
            bEffect = false;
            break;
        }
    }

    // 三个通道均有效
    if(!bEffect)
    {
        // 为新控制块选定默认通道
        _calcChannel(pCtrlBlk, eOrder);

        // 设置到报表
        _setChannel(eOrder);
    }

    uint32_t nChannel[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    // 更新设置到数据层
    if(eOrder == ORDER_PHASE_A)
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidA[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelA[i];
            }
        }

        m_pDataLayer->SetPhaseCheckSmv1CtrBlk(m_pCtrlBlkA);
        m_pDataLayer->SetSmv1ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
    }
    else
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidB[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelB[i];
            }
        }

        m_pDataLayer->SetPhaseCheckSmv2CtrBlk(m_pCtrlBlkB);
        m_pDataLayer->SetSmv2ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
    }

    // 更新单位
    for(int i = 0; i < 3; i++)
    {
        _updateUnit(pCtrlBlk, nRow + i, pChannels[i]);
    }
}


void CSMVDualphaseDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        // 定时刷新核相页面数据
        _updateData();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


/**
 *      刷新页面数据
 */
void CSMVDualphaseDlg::_updateData()
{
    ASSERT(m_pDataLayer);

    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        if(!m_pDataLayer->GetPhaseCheckData(&m_data))
        {
            // 清空相量图
            if(m_bGram)
            {
                m_ctrlPhaseGram.ErasePhasor();
                m_ctrlPhaseGram.Invalidate(TRUE);
            }
            // 清空报表
            else
            {
                _clearReport();
            }

            return;
        }
    }

    // 更新相量图
    if(m_bGram)
    {
        _updateDataGram();
    }
    // 更新报表数据
    else
    {
        _updateDataReport();
    }
}


/**
 *      刷新相量图
 */
void CSMVDualphaseDlg::_updateDataGram()
{
    // 频率
    CString csFreq1, csFreq2;
    CString csAa, csBb, csCc;
    csFreq1.Format(L"f1=%0.3f Hz", m_data.GetSmv1Freq());
    csFreq2.Format(L"f2=%0.3f Hz", m_data.GetSmv2Freq());

    // 获取数据
    if(!m_data.GetSmv1VaulePhase(&m_stPhaseVal1[0], &m_stPhaseVal1[1], &m_stPhaseVal1[2]) || 
        !m_data.GetSmv2VaulePhase(&m_stPhaseVal2[0], &m_stPhaseVal2[1], &m_stPhaseVal2[2]) || 
        !m_data.GetInPhaseAngleDiff(&m_fInPhaseAngleDiff[0], &m_fInPhaseAngleDiff[1], &m_fInPhaseAngleDiff[2]))
    {
        return;
    }

    float fEffect[6] = {
        m_pPageDataExtraDual->bChannelValidA[0] ? m_stPhaseVal1[0].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[1] ? m_stPhaseVal1[1].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[2] ? m_stPhaseVal1[2].m_fValue : 0.0f,
        m_pPageDataExtraDual->bChannelValidB[0] ? m_stPhaseVal2[0].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[1] ? m_stPhaseVal2[1].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[2] ? m_stPhaseVal2[2].m_fValue : 0.0f};

    // 以第一个A相通道相位角为基准
    float fPhaseA = m_stPhaseVal1[0].m_fPhase;

    float fAngel[6] = {
        m_pPageDataExtraDual->bChannelValidA[0] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[0].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[1] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[1].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[2] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[2].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[0] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[0].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[1] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[1].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[2] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[2].m_fPhase) : 0.0f};

    float fMaxLen = fEffect[0];

    for(int i = 1; i < 6; i++)
    {
        fMaxLen = max(fMaxLen, fEffect[i]);
    }

    csAa.Format(L"Aa∠%0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[0]));
    csBb.Format(L"Bb∠%0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[1]));
    csCc.Format(L"Cc∠%0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[2]));

    // 减少频繁设置
    float fMaxTemp  = float(fMaxLen / 4.0);
    float fMargin   = fMaxTemp * 0.1f;
    float fDiff     = m_fScaleInterval - fMaxTemp;

    if(fDiff < 0.0f || fDiff > fMargin)
    {
        m_fScaleInterval = fMaxTemp;
        m_ctrlPhaseGram.SetLenScaleInterval(m_fScaleInterval);
    }

#if !CHUN_HUA
	for (int i =1; i<4;i++)
	{
		CELRecord*  record = m_reportPhase.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportPhase.GetRecordRow(i+5);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+5,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+5,1);
	}

#endif

    m_ctrlPhaseGram.ErasePhasor();

    m_ctrlPhaseGram.DrawExtDesc(csAa, m_rcAa, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
    m_ctrlPhaseGram.DrawExtDesc(csBb, m_rcBb, GRAM_TIPS_COLOR);
    m_ctrlPhaseGram.DrawExtDesc(csCc, m_rcCc, GRAM_TIPS_COLOR);

    m_ctrlPhaseGram.DrawExtDesc(csFreq1, m_rcF1, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
    m_ctrlPhaseGram.DrawExtDesc(csFreq2, m_rcF2, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

    m_ctrlPhaseGram.DrawPhasor(fEffect, fAngel, m_csDesc, 6);
    m_ctrlPhaseGram.Invalidate(TRUE);
}

void CSMVDualphaseDlg::_updateDataReport()
{
    using namespace SMV_DUALPHASOR;

    // 显示smv1、smv2频率
//     m_reportValue.SetRowItemText(PHASOR_FREQ1, PHASOR_COL0, g_GetResultString(m_csFmtFreq, m_data.GetSmv1Freq()));
//     m_reportValue.SetRowItemText(PHASOR_FREQ2, PHASOR_COL0, g_GetResultString(m_csFmtFreq, m_data.GetSmv2Freq()));
    m_arrayItem[0]->SetCaption(g_GetResultString(m_csFmtFreq, m_data.GetSmv1Freq()));
    m_arrayItem[1]->SetCaption(g_GetResultString(m_csFmtFreq, m_data.GetSmv2Freq()));


    bool bSmv1 = false;

    /// smv1有效值
    if(m_data.GetSmv1VaulePhase(&m_stPhaseVal1[0], &m_stPhaseVal1[1], &m_stPhaseVal1[2]))
    {
//         m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[0], m_stPhaseVal1[0].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[0].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[1], m_stPhaseVal1[1].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[1].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[2], m_stPhaseVal1[2].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[2].m_fPhase)));

        m_arrayItem[2]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[0], m_stPhaseVal1[0].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[0].m_fPhase)));
        m_arrayItem[3]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[1], m_stPhaseVal1[1].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[1].m_fPhase)));
        m_arrayItem[4]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[2], m_stPhaseVal1[2].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[2].m_fPhase)));

        bSmv1 = true;
    }

    // smv2有效值
    if(m_data.GetSmv2VaulePhase(&m_stPhaseVal2[0], &m_stPhaseVal2[1], &m_stPhaseVal2[2]))
    {
        float fBase = m_stPhaseVal1[0].m_fPhase;

        if(!bSmv1)
        {
            fBase = m_stPhaseVal2[0].m_fPhase;
        }

//         m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[3], m_stPhaseVal2[0].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[0].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[4], m_stPhaseVal2[1].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[1].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[5], m_stPhaseVal2[2].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[2].m_fPhase)));

        m_arrayItem[5]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[3], m_stPhaseVal2[0].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[0].m_fPhase)));
        m_arrayItem[6]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[4], m_stPhaseVal2[1].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[1].m_fPhase)));
        m_arrayItem[7]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[5], m_stPhaseVal2[2].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[2].m_fPhase)));
    }

    // 同相
    if(m_bSamePhase)
    {
        // 获取同相相角差
        if(m_data.GetInPhaseAngleDiff(&m_fInPhaseAngleDiff[0], &m_fInPhaseAngleDiff[1], &m_fInPhaseAngleDiff[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_GetResultString(L"Aa: %0.3f°", m_fInPhaseAngleDiff[0]));
//             m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_GetResultString(L"Bb: %0.3f°", m_fInPhaseAngleDiff[1]));
//             m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_GetResultString(L"Cc: %0.3f°", m_fInPhaseAngleDiff[2]));

            m_arrayItem[8]->SetCaption(g_GetResultString(L"Aa: %0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[0])));
            m_arrayItem[9]->SetCaption(g_GetResultString(L"Bb: %0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[1])));
            m_arrayItem[10]->SetCaption(g_GetResultString(L"Cc: %0.3f°", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[2])));
        }

        // 获取同相相量差
        if(m_data.GetInPhasePhasorDiff(&m_stPhaseData2[0], &m_stPhaseData2[1], &m_stPhaseData2[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_GetResultString(L"Aa: %0.3f %s ∠%0.3f°", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_GetResultString(L"Bb: %0.3f %s ∠%0.3f°", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_GetResultString(L"Cc: %0.3f %s ∠%0.3f°", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));

            m_arrayItem[11]->SetCaption(g_GetResultString(L"Aa: %0.3f%s/%0.3f°", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
            m_arrayItem[12]->SetCaption(g_GetResultString(L"Bb: %0.3f%s/%0.3f°", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
            m_arrayItem[13]->SetCaption(g_GetResultString(L"Cc: %0.3f%s/%0.3f°", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));
        }
    }
    // 相间
    else
    {
        // 获取相间相量差
        if(m_data.GetSmv1BetweenPhasePhasorDiff(&m_stPhaseData1[0], &m_stPhaseData1[1], &m_stPhaseData1[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_GetResultString(L"AB: %0.3f %s ∠%0.3f°", m_csUnit[0], m_stPhaseData1[0].m_fValue, m_stPhaseData1[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_GetResultString(L"BC: %0.3f %s ∠%0.3f°", m_csUnit[1], m_stPhaseData1[1].m_fValue, m_stPhaseData1[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_GetResultString(L"CA: %0.3f %s ∠%0.3f°", m_csUnit[2], m_stPhaseData1[2].m_fValue, m_stPhaseData1[2].m_fPhase));

            m_arrayItem[8]->SetCaption(g_GetResultString(L"AB: %0.3f%s/%0.3f°", m_csUnit[0], m_stPhaseData1[0].m_fValue, m_stPhaseData1[0].m_fPhase));
            m_arrayItem[9]->SetCaption(g_GetResultString(L"BC: %0.3f%s/%0.3f°", m_csUnit[1], m_stPhaseData1[1].m_fValue, m_stPhaseData1[1].m_fPhase));
            m_arrayItem[10]->SetCaption(g_GetResultString(L"CA: %0.3f%s/%0.3f°", m_csUnit[2], m_stPhaseData1[2].m_fValue, m_stPhaseData1[2].m_fPhase));
        }
        // 获取相间相量差
        if(m_data.GetSmv2BetweenPhasePhasorDiff(&m_stPhaseData2[0], &m_stPhaseData2[1], &m_stPhaseData2[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_GetResultString(L"ab: %0.3f %s ∠%0.3f°", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_GetResultString(L"bc: %0.3f %s ∠%0.3f°", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_GetResultString(L"ca: %0.3f %s ∠%0.3f°", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));

            m_arrayItem[11]->SetCaption(g_GetResultString(L"ab: %0.3f%s/%0.3f°", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
            m_arrayItem[12]->SetCaption(g_GetResultString(L"bc: %0.3f%s/%0.3f°", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
            m_arrayItem[13]->SetCaption(g_GetResultString(L"ca: %0.3f%s/%0.3f°", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));
        }
    }

    // 不刷新report，由调用者刷新
    m_reportValue.Invalidate(TRUE);
}


/**
 *      更新选中通道的单位
 *
 *  \param[in]
 *      pCtrlBlk        通道关联的控制块对象
 *  \param[in]
 *      nRow            通道所在报表行号
 *  \param[in]
 *      nChannel        通道号
 */
void CSMVDualphaseDlg::_updateUnit( CBaseDetectItem* pCtrlBlk, int nRow, uint32_t nChannel )
{
    ASSERT(pCtrlBlk);

    enumSmvValueType eDispType      = CDsmDataLayer::getInstance()->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
    enumSmvValueType eFrameType     = CDsmDataLayer::getInstance()->GetParamConfigBasic()->GetFrameValueType();     // 获取一次二次额定值类型
    CSmvRecvConfig* pCfg            = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo      = pCfg->GetOneChannelInfo(nChannel);

    // 设置指定通道类型的单位
    m_csUnit[nRow > 3 ? nRow - 3 : nRow - 1] = pInfo ? DM_GetUnitFromType(pInfo->enChannelType, eDispType, eFrameType) : L"";
}



/**
 *      初始化当前核相的页面数据
 */
void CSMVDualphaseDlg::_initCurrentCtrlBlk()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    bool bInited = true;

    // 新进页面，创建页面数据
    if(pData->m_eAction == PageEnter)
    {
        if(!pData->m_pPageDataExtra)
        {
            bInited = false;
            m_pPageDataExtraDual = new CPageDataExtraDualPhase(pData);
            pData->m_pPageDataExtra = m_pPageDataExtraDual;
        }
    }

    // 返回页面，取出旧的页面数据
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtraDual = dynamic_cast<CPageDataExtraDualPhase*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtraDual);
    }

    // 初始化两个控制块
    _initPageStatus();

    // 设置核相模式
    m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_PHASE_CHECK);
}

/**
 *      清空表格数据
 */
void CSMVDualphaseDlg::_clearReport()
{
    m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL0, g_csBlankValue);

    m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_csBlankValue);

    m_reportValue.SetRowItemText(PHASOR_FREQ1, PHASOR_COL0, L"Freq = -- Hz");
    m_reportValue.SetRowItemText(PHASOR_FREQ2, PHASOR_COL0, L"Freq = -- Hz");
}

/**
 *      检查通道是否发生变化
 */
bool CSMVDualphaseDlg::_checkChannel()
{
    uint32_t nCtrlBlkA, nCtrlBlkB;

    // 通道数发生变化，
    if(!m_pDataLayer->GetPhaseCheckRealTimeChannelCounts(nCtrlBlkA, nCtrlBlkB) || 
        m_pCtrlBlkA->nChannelCount != nCtrlBlkA ||
        m_pCtrlBlkB->nChannelCount != nCtrlBlkB)
    {
        if(m_bChannelChange && CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // 界面tips提示
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;

            // 清空相量图
            if(m_bGram)
            {
                m_ctrlPhaseGram.ErasePhasor();
                m_ctrlPhaseGram.Invalidate(TRUE);
            }
            else
            {
                _clearReport();
            }

            // 禁止通道选择
            for(int i = 0; i < 9; i++)
            {
                if(i == 4)
                {
                    continue;
                }

                m_reportPhase.GetRecordRow(i)->GetItem(1)->SetEditable(FALSE);
            }

            if(m_pNaviBar->GetCommandTarget() == this)
            {
                // 隐藏部分菜单标题
#if SU_FEI
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
#endif
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(4, NULL);
                m_pNaviBar->UpdateNaviMenu();
            }
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);
        }

        // 恢复通道选择
        for(int i = 0; i < 9; i++)
        {
            if(i == 4)
            {
                continue;
            }

            m_reportPhase.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
        }

        if(m_pNaviBar->GetCommandTarget() == this)
        {
            // 恢复菜单功能
#if SU_FEI
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
#endif
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL : DSM_BMP_M_SMV_SCAN_REPORT_SEL));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(4, m_bGram ? NULL : dsm_bmpmgr::GetCB(m_bSamePhase ? DSM_BMP_M_SMV_SCAN_PHASE_SEL : DSM_BMP_M_SMV_SCAN_ALTERNATE_SEL));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVDualphaseDlg::OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    if(!m_bEnableTimer)
    {
        ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
        ASSERT(pItemNotify);

        switch(pItemNotify->rowIndex)
        {
        case 0:
        case 5:
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SWITCH_CTRL_TIPS));
            break;

        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
            break;

        default:
            break;
        }

        *pResult = TRUE;
    }
}

