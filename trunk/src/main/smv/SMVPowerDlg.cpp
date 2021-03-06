/**
 * \file
 *      SMVPowerDlg.h
 *
 * \brief
 *      功率页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */

// SMVPowerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVPowerDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"

//////////////////////////////////////////////////////////////////////////
// smv - 功率
#if SU_FEI

#define DSM_SMV_POWER_HEAD_ITEM             130         ///< 标题栏
#define DSM_SMV_POWER_HEAD_PHASEA           120         ///< A相
#define DSM_SMV_POWER_HEAD_PHASEB           120         ///< B相
#define DSM_SMV_POWER_HEAD_PHASEC           120         ///< C相
#define DSM_SMV_POWER_HEAD_POWER            110         ///< 总功率

#else
#if SHENG_ZE
#define DSM_SMV_POWER_HEAD_ITEM             100         ///< 标题栏
#define DSM_SMV_POWER_HEAD_PHASEA           115         ///< A相
#define DSM_SMV_POWER_HEAD_PHASEB           115         ///< B相
#define DSM_SMV_POWER_HEAD_PHASEC           115         ///< C相
#define DSM_SMV_POWER_HEAD_POWER            84         ///< 总功率
#else

#define DSM_SMV_POWER_HEAD_ITEM             90         ///< 标题栏
#define DSM_SMV_POWER_HEAD_PHASEA           118         ///< A相
#define DSM_SMV_POWER_HEAD_PHASEB           118         ///< B相
#define DSM_SMV_POWER_HEAD_PHASEC           118         ///< C相
#define DSM_SMV_POWER_HEAD_POWER            85         ///< 总功率
#endif
#endif

/**
 *      SMV接收 - 功率
 *
 *  \note
 *      描述、A相、B相、C相、总功率
 */
class CDsmSmvPowerRecord : public CELRecord
{
public:

    /**
     *      功率记录
     *
     *  \param[in]
     *      csSubCaption        子标题
     */
    CDsmSmvPowerRecord(int nRow, CString csSubCaption);

    virtual ~CDsmSmvPowerRecord() {}

public:

    /**
     *      更新记录
     *
     *  \param[in]
     *      nRow        待更新记录的行号
     *  \param[in]
     *      fVal        待更新数据的数组
     *  \param[in]
     *      nSize       更新数据的数组大小
     */
    void Update(int nRow, float* fVal, int nSize);
};

/**
 *      功率记录
 *
 *  \param[in]
 *      csSubCaption        子标题
 */
CDsmSmvPowerRecord::CDsmSmvPowerRecord( int nRow, CString csSubCaption )
{

    AddItem(new CDsmRecordItem(csSubCaption));

    if(nRow >= 2)
    {
        // 默认显示 L"--"  电流电压的相位有效值item
        AddItem(new CDsmRecordItem(g_csBlankValue));
        AddItem(new CDsmRecordItem(g_csBlankValue));
        AddItem(new CDsmRecordItem(g_csBlankValue));
    }
    else
    {
        // 默认显示 空白，电流电压通道的item
        AddItem(new CDsmRecordItem);
        AddItem(new CDsmRecordItem);
        AddItem(new CDsmRecordItem);
    }

    if(nRow >= 6)
    {
        // 功率信息item默认显示 L"--"
        AddItem(new CDsmRecordItem(g_csBlankValue))->SetFocusable(FALSE);
    }
    else
    {
        // 其他item显示 空白
        AddItem(new CDsmRecordItem)->SetFocusable(FALSE);
    }
}

/**
 *      更新记录
 *
 *  \param[in]
 *      nRow        待更新记录的行号
 *  \param[in]
 *      fVal        待更新数据的数组
 *  \param[in]
 *      nSize       更新数据的数组大小
 */
void CDsmSmvPowerRecord::Update( int nRow, float* fVal, int nSize )
{
    CString csV(L"");

    for(int i = 0, j = 1; i < nSize && j < 5; i++, j++)
    {
        // 无效值显示L"--"
        if(0 == _finite(fVal[i]))
        {
            csV = g_csBlankValue;
        }
        else
        {
            csV.Format(L"%0.3f", fVal[i]);
        }

        // 行4与行6是相位角信息，将添加角度符号
        if(3 == nRow || 5 == nRow)
        {
            csV = L"/" + csV + L"°";
        }

        // 设置到item中
        GetItem(j)->SetCaption(csV);
    }
}


/**
 *      功率页面控制块管理的数据类
 */
class CCtrlBlkDataPower : public ICtrlBlkData
{
public:

    CCtrlBlkDataPower()
        : m_nFocusRow(0)
        , m_nFocusCol(1)
    {
        for(int i = 0; i < 3; i++)
        {
            m_arrVolts[i]       = UINT_MAX;
            m_arrAmpers[i]      = UINT_MAX;

            m_bVoltValid[i]     = false;
            m_bAmperValid[i]    = false;
        }
    }

    virtual ~CCtrlBlkDataPower()
    {
    }

public:

    int         m_nFocusRow;                ///< 焦点行
    int         m_nFocusCol;                ///< 焦点列

    uint32_t    m_arrVolts[3];              ///< 电压通道索引的数组
    uint32_t    m_arrAmpers[3];             ///< 电流通道索引的数组

    bool        m_bVoltValid[3];            ///< 电压通道有效性
    bool        m_bAmperValid[3];           ///< 电流通道有效性
};


// CSMVPowerDlg 对话框

IMPLEMENT_DYNAMIC(CSMVPowerDlg, DSM_SMVBASEDLG)

CSMVPowerDlg::CSMVPowerDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVPowerDlg::IDD, DSM_STR_SMV_POWER_CAPTION, pParent)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
    , m_pCtrlBlkData(NULL)
{

}

CSMVPowerDlg::~CSMVPowerDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVPowerDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVPowerDlg, DSM_SMVBASEDLG)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_POWER_REPORT, &CSMVPowerDlg::OnItemSelectChange)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_POWER_REPORT, &CSMVPowerDlg::OnInplaceButtonDown)
END_MESSAGE_MAP()

// CSMVPowerDlg 消息处理程序

BOOL CSMVPowerDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
    m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // 获取通道额定值类型

    // 创建报表
    _createReport();

    // 根据控制块的页面状态数据重设通道信息
    _resetChannel();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSMVPowerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        // 更新功率页面数据
        _updateReport();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

/**
 *      创建报表
 */
void CSMVPowerDlg::_createReport()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, 0);
#if SHENG_ZE
    rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * (10 - 1);
#else
	rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * (11 - 1);
#endif
    m_reportPower.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_POWER_REPORT);

    // 设置字体
    m_reportPower.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportPower.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置竖向报表样式
    m_reportPower.PostInitTemplateStyle();

    // 插入列元素
    m_reportPower.InsertColumn(0, L"描述", EL_LEFT, DSM_SMV_POWER_HEAD_ITEM);
    m_reportPower.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_PHASEA), EL_LEFT, DSM_SMV_POWER_HEAD_PHASEA);
    m_reportPower.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_PHASEB), EL_LEFT, DSM_SMV_POWER_HEAD_PHASEB);
    m_reportPower.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_PHASEC), EL_LEFT, DSM_SMV_POWER_HEAD_PHASEC);
    m_reportPower.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_POWER), EL_LEFT, DSM_SMV_POWER_HEAD_POWER);

    // 设置选择焦点样式
    m_reportPower.SetRowSelectionVisible(FALSE);
    m_reportPower.SetRowFocusEnable(TRUE);
    m_reportPower.SetSubItemFocusEnable(TRUE);
    m_reportPower.SetFreezeSelectionVisible(TRUE);
    m_reportPower.SetFreezeColumnsCount(1);
    m_reportPower.SetFocusedColIndex(1);

    _initReport();

    m_reportPower.SetFocus();
}

/**
 *      初始化报表
 */
void CSMVPowerDlg::_initReport()
{
    int nIndex = 0;

    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLT)));           // 电压通道列表
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(1, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPER)));          // 电流通道列表
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(2, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVAL)));        // 电压有效值
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(3, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTPHASE)));      // 电压相位
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(4, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPERVAL)));       // 电流有效值
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(5, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPERPHASE)));     // 电流相位
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(6, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_ACTIVE)));         // 有功功率
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(7, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_IDLE)));           // 无功功率
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(8, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_APPARENT)));       // 视在功率
    m_reportPower.InsertRecordRow(nIndex++, new CDsmSmvPowerRecord(9, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_FACTOR)));         // 功率因数

    // 禁止选中除了电流电压通道的行
    for(int i = 2; i < 10; i++)
    {
        m_reportPower.GetRecordRow(i)->SetFocusable(FALSE);
    }

    if(m_reportPower.GetRecordRowCount() > 0)
    {
        m_reportPower.SetFocusedSelectionRowIndex(0);
    }
}

/**
 *      根据控制块关联的页面状态数据重设电流电压通道信息
 */
void CSMVPowerDlg::_resetChannel()
{
    // 为电流电压通道行的item加载通道
    _loadItemChannel(0, CHANNEL_TYPE_SMV_VOLTAGE);
    _loadItemChannel(1, CHANNEL_TYPE_SMV_CURRENT);

    // 根据控制块关联的页面状态数据设置所选通道
    if(!_restorePage())
    {
        // 当前控制块没有关联页面状态数据则自动适配合适的通道
        _adjustChannel();
    }
}


/**
 *      为指定行号加载指定通道类型的通道信息
 *
 *  \param[in]
 *      nRow        指定的行号
 *  \param[in]
 *      eType       指定的通道类型
 */
void CSMVPowerDlg::_loadItemChannel( int nRow, enumChannelType eType )
{
    ASSERT(m_pCurrentCtrlBlk);

    CELRecord* pRecord = m_reportPower.GetRecordRow(nRow);

    if(!pRecord)
    {
        ASSERT(pRecord);
        return;
    }

    CELRecordItem* pItem                = NULL;
    CELRecordItemEditOptions* pOption   = NULL;

    // 设置电流/电压通道列表item的属性
    for(int i = 1; i < 4; i++)
    {
        pItem = pRecord->GetItem(i);
        pItem->SetEditable(TRUE);
        pOption = pItem->GetEditOptions(NULL);
        pOption->RemoveButtons();
        pOption->GetConstraints()->RemoveAll();
        pOption->AddComboButton();
    }

    // 获取ABC三相通道item的指针
    CELRecordItemEditOptions* pItemA = pRecord->GetItem(1)->GetEditOptions(NULL);
    CELRecordItemEditOptions* pItemB = pRecord->GetItem(2)->GetEditOptions(NULL);
    CELRecordItemEditOptions* pItemC = pRecord->GetItem(3)->GetEditOptions(NULL);

    CSmvRecvChannelInfo*    pInfo   = NULL;
    CSmvRecvConfig*         pCfg    = m_pCurrentCtrlBlk->pRecvConfig;
    size_t                  nCounts = pCfg->GetChannelCount();

    // 枚举指定通道类型，并添加到ABC三相通道item中
    for(size_t i = 0; i < nCounts; i++)
    {
       pInfo = pCfg->GetOneChannelInfo(i);
       ASSERT(pInfo);

       // 类型匹配
       if(pInfo && eType == pInfo->enChannelType)
       {
           pItemA->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
           pItemB->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
           pItemC->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
       }
    }
}

/**
 *      响应定时器消息，更新报表数据
 */
void CSMVPowerDlg::_updateReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(_checkChannel())
    {
        return;
    }

    // 获取功率数据
    if(!m_pDataLayer->GetPowerData(&m_data))
    {
        // 清空数据
        for(int i = 2; i < 10; i++)
        {
            m_reportPower.SetRowItemText(i, 1, g_csBlankValue);
            m_reportPower.SetRowItemText(i, 2, g_csBlankValue);
            m_reportPower.SetRowItemText(i, 3, g_csBlankValue);

            if(i >= 6)
            {
                m_reportPower.SetRowItemText(i, 4, g_csBlankValue);
            }
        }

        return;
    }

    m_data.GetVotageEffectValue(m_fVolt[0], m_fVolt[1], m_fVolt[2]);                            // 获取电压有效值
    m_data.GetVoltageAngle(m_fVoltAngle[0], m_fVoltAngle[1], m_fVoltAngle[2]);                  // 获取电压相位角
    m_data.GetCurrentEffectValue(m_fAmper[0], m_fAmper[1], m_fAmper[2]);                        // 获取电流有效值
    m_data.GetCurrentAngle(m_fAmperAngle[0], m_fAmperAngle[1], m_fAmperAngle[2]);               // 获取电压相位角
    m_data.GetActivePower(m_fActive[0], m_fActive[1], m_fActive[2], m_fActive[3]);              // 获取有功功率
    m_data.GetReactivePower(m_fReactive[0], m_fReactive[1], m_fReactive[2], m_fReactive[3]);    // 获取无功功率
    m_data.GetApparentPower(m_fApparent[0], m_fApparent[1], m_fApparent[2], m_fApparent[3]);    // 获取视在功率
    m_data.GetPowerFactor(m_fFactor[0], m_fFactor[1], m_fFactor[2], m_fFactor[3]);              // 功率因数

#if !CHUN_HUA
	m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
	m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // 获取通道额定值类型
#endif
	
	// 调整电压一次/二次值的数值换算
    if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
    {
        m_fVolt[0] /= 1000.0f;
        m_fVolt[1] /= 1000.0f;
        m_fVolt[2] /= 1000.0f;
    }

#if !CHUN_HUA
	if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
	{
		m_reportPower.SetRowItemText(2,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVALK));
		m_reportPower.SetRowItemText(6,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_ACTIVE));
		m_reportPower.SetRowItemText(7,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_IDLE));
		m_reportPower.SetRowItemText(8,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_APPARENT));	
	}
	else  
	{
		m_reportPower.SetRowItemText(2,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVAL));
		m_reportPower.SetRowItemText(6,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_ACTIVE_S));
		m_reportPower.SetRowItemText(7,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_IDLE_S));
		m_reportPower.SetRowItemText(8,0,dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_APPARENT_S));
	}	
#endif

    float fBasePhase = _getBasePhaseValue();
    bool bPowers[3]  = {false, false, false};
    CString csEffect, csAngel, csActive, csReactive, csApparent, csFactor, csTotal;

    for(int i = 0; i < 3; i++)
    {
        // 电压数据
        if(m_pCtrlBlkData->m_bVoltValid[i])
        {
            csEffect.Format(L"%.3f", m_fVolt[i]);
            csAngel.Format(L"%.3f°", DSM_GetRelativePhase(fBasePhase, m_fVoltAngle[i]));

            m_reportPower.SetRowItemText(2, i + 1, csEffect);
            m_reportPower.SetRowItemText(3, i + 1, csAngel);
        }
        else
        {
            m_reportPower.SetRowItemText(2, i + 1, g_csBlankValue);
            m_reportPower.SetRowItemText(3, i + 1, g_csBlankValue);
        }

        // 电流数据
        if(m_pCtrlBlkData->m_bAmperValid[i])
        {
            csEffect.Format(L"%.3f", m_fAmper[i]);
            csAngel.Format(L"%.3f°", DSM_GetRelativePhase(fBasePhase, m_fAmperAngle[i]));

            m_reportPower.SetRowItemText(4, i + 1, csEffect);
            m_reportPower.SetRowItemText(5, i + 1, csAngel);
        }

        // 功率数据
        if(m_pCtrlBlkData->m_bVoltValid[i] && m_pCtrlBlkData->m_bAmperValid[i])
        {
            bPowers[i] = true;
			
#if SU_FEI
            csActive.Format(L"%.3f", m_fActive[i]);             // 有功
            csReactive.Format(L"%.3f", m_fReactive[i]);         // 无功
            csApparent.Format(L"%.3f", m_fApparent[i]);         // 视在
            csFactor.Format(L"%.3f", m_fFactor[i]);             // 因数
 
			if (m_fActive[i] > 1000.000)
			{
				csActive.Format(L"%.3fK", m_fActive[i]/1000);
			}
			else
			{
				csActive.Format(L"%.3f", m_fActive[i]);             // 有功
			}

			if (m_fActive[i] > 1000.000)
			{
				csReactive.Format(L"%.3fK", m_fReactive[i]/1000);
			}
			else
			{
				csReactive.Format(L"%.3f", m_fReactive[i]);         // 无功
			}

			if (m_fActive[i] > 1000.000)
			{
				csApparent.Format(L"%.3fK", m_fApparent[i]/1000);
			}
			else
			{
				csApparent.Format(L"%.3f", m_fApparent[i]);         // 视在
			}
			
			csFactor.Format(L"%.3f", m_fFactor[i]); 		 
#endif
#if !CHUN_HUA
			if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
			{
				m_fActive[i] /= 1000000.0f;
				m_fReactive[i] /= 1000000.0f;
				m_fApparent[i] /= 1000000.0f;
			}	

			csActive.Format(L"%.3f", m_fActive[i]);             // 有功
			csReactive.Format(L"%.3f", m_fReactive[i]);         // 无功
			csApparent.Format(L"%.3f", m_fApparent[i]);         // 视在
			csFactor.Format(L"%.3f", m_fFactor[i]);             // 因数
#endif
            m_reportPower.SetRowItemText(6, i + 1, csActive);
            m_reportPower.SetRowItemText(7, i + 1, csReactive);
            m_reportPower.SetRowItemText(8, i + 1, csApparent);
            m_reportPower.SetRowItemText(9, i + 1, csFactor);
        }
        else
        {
            bPowers[i] = false;

            m_reportPower.SetRowItemText(6, i + 1, g_csBlankValue);
            m_reportPower.SetRowItemText(7, i + 1, g_csBlankValue);
            m_reportPower.SetRowItemText(8, i + 1, g_csBlankValue);
            m_reportPower.SetRowItemText(9, i + 1, g_csBlankValue);
        }
    }

#if !CHUN_HUA
	if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
	{
		m_fActive[3] /= 1000000.0f;
		m_fReactive[3] /= 1000000.0f;
		m_fApparent[3] /= 1000000.0f;
	}	
#endif
	
    // 总功率，
    if(bPowers[0] && bPowers[1] && bPowers[2])
    {
        csTotal.Format(L"%.3f", m_fActive[3]);
        m_reportPower.SetRowItemText(6, 4, csTotal);

        csTotal.Format(L"%.3f", m_fReactive[3]);
        m_reportPower.SetRowItemText(7, 4, csTotal);

        csTotal.Format(L"%.3f", m_fApparent[3]);
        m_reportPower.SetRowItemText(8, 4, csTotal);

        csTotal.Format(L"%.3f", m_fFactor[3]);
        m_reportPower.SetRowItemText(9, 4, csTotal);
    }
    else
    {
        m_reportPower.SetRowItemText(6, 4, g_csBlankValue);
        m_reportPower.SetRowItemText(7, 4, g_csBlankValue);
        m_reportPower.SetRowItemText(8, 4, g_csBlankValue);
        m_reportPower.SetRowItemText(9, 4, g_csBlankValue);
    }
}

/**
 *      为ABC三相通道选中合适的默认通道
 */
void CSMVPowerDlg::_adjustChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // 获取控制块绑定数据，若为空，则新建一个数据
    m_pCtrlBlkData = dynamic_cast<CCtrlBlkDataPower*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!m_pCtrlBlkData)
    {
        m_pCtrlBlkData = new CCtrlBlkDataPower;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, m_pCtrlBlkData);
    }

    // 获取默认显示的通道
    _calcChannel(CHANNEL_TYPE_SMV_VOLTAGE, m_pCtrlBlkData);
    _calcChannel(CHANNEL_TYPE_SMV_CURRENT, m_pCtrlBlkData);

    // 根据上面新的数据设置页面
    _restorePage();
}

/**
 *      显示合适的默认显示通道名
 *
 *  \param[in]
 *      eType       通道类型
 *  \param[in]
 *      pData       当前控制块关联的数据
 */
void CSMVPowerDlg::_calcChannel( enumChannelType eType, CCtrlBlkDataPower* pData )
{
    ASSERT(pData);

    uint32_t nChann[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    // 按照ABC顺序查找
    nChann[0] = _findChannel(eType, CHANNEL_PHASE_A, 0);
    nChann[1] = _findChannel(eType, CHANNEL_PHASE_B, nChann[0] + 1);
    nChann[2] = _findChannel(eType, CHANNEL_PHASE_C, nChann[1] + 1);

    // 将刚才找到的通道号设置到控制块关联的页面状态数据中
    for(int i = 0; i < 3; i++)
    {
        // 找到无效的通道好，将随便找一个填充
        if(UINT_MAX == nChann[i])
        {
            nChann[i] = _findChannel(eType, CHANNEL_PHASE_UNKOWN, 0);
        }

        // 电压通道
        if(CHANNEL_TYPE_SMV_VOLTAGE == eType)
        {
            pData->m_arrVolts[i] = nChann[i];

            if(nChann[i] == UINT_MAX)
            {
                pData->m_bVoltValid[i] = false;
            }
            else
            {
                pData->m_bVoltValid[i] = true;
            }
        }
        // 电流通道
        else if(CHANNEL_TYPE_SMV_CURRENT == eType)
        {
            pData->m_arrAmpers[i] = nChann[i];

            if(nChann[i] == UINT_MAX)
            {
                pData->m_bAmperValid[i] = false;
            }
            else
            {
                pData->m_bAmperValid[i] = true;
            }
        }
    }
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
 *      uint32_t        返回符合条件的通道号，找不到则返回 UINT_MAX
 */
uint32_t CSMVPowerDlg::_findChannel( enumChannelType eType, ChannelPhase ePhase, int nStart )
{
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 选3个有效的通道号ABC
    for(size_t i = nStart; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        if(pInfo && eType == pInfo->enChannelType)
        {
            // 未知相位则返回当前通道号
            if(CHANNEL_PHASE_UNKOWN == ePhase)
            {
                return pInfo->nIndex;
            }

            // 找到合适的相位的通道号
            if(ePhase == pInfo->enChannelPhase)
            {
                return pInfo->nIndex;
            }
        }
    }

    // 找不到合适通道号，返回UINT_MAX
    return UINT_MAX;
}

/**
 *      保存当前控制块关联的页面状态数据
 */
void CSMVPowerDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pCtrlBlkData);

    // 保存焦点行列号
    if(m_pCtrlBlkData)
    {
        m_pCtrlBlkData->m_nFocusRow = m_reportPower.GetFocusedRowIndex();
        m_pCtrlBlkData->m_nFocusCol = m_reportPower.GetFocusedColIndex();
    }

    // 控制块关联的电流电压通道索引已经在平时操作中保存
}

/**
 *      根据菜单ID切换到指定的控制块
 *
 *  /param[in]
 *      nCmdID      控制块关联的菜单命令ID
 */
bool CSMVPowerDlg::_switchCtrlBlk( UINT nCmdID )
{
    // 保存上一个控制块对象
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
    CCtrlBlkDataPower* pData = dynamic_cast<CCtrlBlkDataPower*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // 保存上一个控制块的行列焦点
    if(pData)
    {
        pData->m_nFocusRow = m_reportPower.GetFocusedRowIndex();
        pData->m_nFocusCol = m_reportPower.GetFocusedColIndex();
    }

    m_pCtrlBlkData = dynamic_cast<CCtrlBlkDataPower*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 根据新控制块页面状态信息重设报表
    _resetChannel();

    return true;
}

/**
 *      使用控制块的页面状态数据设置报表
 */
bool CSMVPowerDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVPowerDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块页面状态数据
    m_pCtrlBlkData = dynamic_cast<CCtrlBlkDataPower*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!m_pCtrlBlkData)
    {
        return false;
    }

    CELRecordItem* pItemVolt        = NULL;
    CELRecordItem* pItemAmper       = NULL;
    CELRecordItemConstraint* pConVolt   = NULL;
    CELRecordItemConstraint* pConAmper  = NULL;

    // 在报表的第一列设置一次二次值的显示单位变化
    switch(m_eDispType)
    {
        // 显示一次值，电压单位kV，电流A
    case SMV_VALUE_TYPE_PRIMARY:
        m_reportPower.SetRowItemText(2, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVALK));
        m_reportPower.SetRowItemText(4, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPERVAL));
        break;

        // 显示二次值，电压单位V，电流A
    case SMV_VALUE_TYPE_SECONDARY:
        m_reportPower.SetRowItemText(2, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVAL));
        m_reportPower.SetRowItemText(4, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPERVAL));
        break;

        // 显示原始报文值，无单位
    case SMV_VALUE_TYPE_ORIGINANL:
        m_reportPower.SetRowItemText(2, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_VOLTVALORI));
        m_reportPower.SetRowItemText(4, 0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_HEAD_AMPERVALORI));
        break;

    default:
        ASSERT(FALSE);
        break;
    }

    uint32_t nVolts[3]  = {UINT_MAX, UINT_MAX, UINT_MAX};
    uint32_t nAmpers[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    // 根据当前控制块选中的电流电压通道索引，从报表的电流电压通道列表中选中相应的通道
    for(int i = 0, j = 1; i < 3 && j < 4; i++, j++)
    {
        pItemVolt       = dynamic_cast<CELRecordItem*>(m_reportPower.GetRecordRow(0)->GetItem(j));
        pItemAmper      = dynamic_cast<CELRecordItem*>(m_reportPower.GetRecordRow(1)->GetItem(j));

        // 设置电压通道
        if(pItemVolt)
        {
            pConVolt    = pItemVolt->GetEditOptions(NULL)->FindConstraint(m_pCtrlBlkData->m_arrVolts[i]);

            if(pConVolt)
            {
                nVolts[i] = m_pCtrlBlkData->m_arrVolts[i];

                m_reportPower.SetRowItemText(0, j, pConVolt->GetConstraintCaption());
            }
            else
            {
                m_pCtrlBlkData->m_bVoltValid[i] = false;
                m_reportPower.SetRowItemText(0, j, g_csBlankValue);
            }
        }

        // 设置电流通道
        if(pItemAmper)
        {
            pConAmper    = pItemAmper->GetEditOptions(NULL)->FindConstraint(m_pCtrlBlkData->m_arrAmpers[i]);

            if(pConAmper)
            {
                nAmpers[i] = m_pCtrlBlkData->m_arrAmpers[i];

                m_reportPower.SetRowItemText(1, j, pConAmper->GetConstraintCaption());
            }
            else
            {
                m_pCtrlBlkData->m_bAmperValid[i] = false;
                m_reportPower.SetRowItemText(1, j, g_csBlankValue);
            }
        }
    }

    // 向数据层设置相别信息
    m_data.SetCurrentChannles(nAmpers[0], nAmpers[1], nAmpers[2]);
    m_data.SetVoltageChannles(nVolts[0], nVolts[1], nVolts[2]);

    m_reportPower.Invalidate(TRUE);

    // 恢复报表行列焦点
    m_reportPower.SetFocusedSelectionRowIndex(m_pCtrlBlkData->m_nFocusRow);
    m_reportPower.SetFocusedColIndex(m_pCtrlBlkData->m_nFocusCol);

    return true;
}

void CSMVPowerDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);
    ASSERT(pItemNotify->rowIndex < 2 && pItemNotify->rowIndex >= 0);

    // 不是列表发生变化则发挥
    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        *pResult = FALSE;
        return;
    }

    *pResult = TRUE;

    // 获取选中的通道索引
    int nSelChan = (int)pItemNotify->pConstraint->GetConstraintData();

    // 设置新选择的通道信息

    // 如果第一列选中了A相类型的通道，则自动就近为下两列选中BC相通道
    if(1 == pItemNotify->colIndex)
    {
        CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
        ASSERT(pCfg);

        CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSelChan);
        ASSERT(pInfo);

        // 选中A相列，则自动调整BC相通道
        if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
        {
            uint32_t nB = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_B, nSelChan + 1);
            uint32_t nC = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

            int nRow = 0;

            if(nB != UINT_MAX && nC != UINT_MAX)
            {
                // 电压
                if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
                {
                    nRow                            = 0;
                    m_pCtrlBlkData->m_arrVolts[1]   = nB;
                    m_pCtrlBlkData->m_arrVolts[2]   = nC;

                    m_pCtrlBlkData->m_bVoltValid[1] = true;
                    m_pCtrlBlkData->m_bVoltValid[2] = true;
                }
                // 电流
                else if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
                {
                    nRow                            = 1;
                    m_pCtrlBlkData->m_arrAmpers[1]  = nB;
                    m_pCtrlBlkData->m_arrAmpers[2]  = nC;

                    m_pCtrlBlkData->m_bAmperValid[1] = true;
                    m_pCtrlBlkData->m_bAmperValid[2] = true;
                }

                // 为新值设置报表界面
                m_reportPower.SetRowItemText(nRow, 2, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
                m_reportPower.SetRowItemText(nRow, 3, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());
            }
        }
    }

    // 若是电压通道改变，则向数据层设置新的电压通道索引
    if(0 == pItemNotify->rowIndex)
    {
        m_pCtrlBlkData->m_arrVolts[pItemNotify->colIndex - 1]   = nSelChan;
        m_pCtrlBlkData->m_bVoltValid[pItemNotify->colIndex - 1] = true;

        // 向数据层设置相别信息
        m_data.SetVoltageChannles(m_pCtrlBlkData->m_arrVolts[0], m_pCtrlBlkData->m_arrVolts[1], m_pCtrlBlkData->m_arrVolts[2]);
    }
    // 若是电流通道改变，则向数据层设置新的电流通道索引
    else if(1 == pItemNotify->rowIndex)
    {
        m_pCtrlBlkData->m_arrAmpers[pItemNotify->colIndex - 1]   = nSelChan;
        m_pCtrlBlkData->m_bAmperValid[pItemNotify->colIndex - 1] = true;

        // 向数据层设置相别信息
        m_data.SetCurrentChannles(m_pCtrlBlkData->m_arrAmpers[0], m_pCtrlBlkData->m_arrAmpers[1], m_pCtrlBlkData->m_arrAmpers[2]);
    }
}

/**
 *      检查通道是否发生变化
 */
bool CSMVPowerDlg::_checkChannel()
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

            // 禁用通道选择
            for(int i = 0; i < 2; i++)
            {
                m_reportPower.GetRecordRow(i)->GetItem(1)->SetEditable(FALSE);
                m_reportPower.GetRecordRow(i)->GetItem(2)->SetEditable(FALSE);
                m_reportPower.GetRecordRow(i)->GetItem(3)->SetEditable(FALSE);
            }

            // 清空数据
            for(int i = 2; i < 10; i++)
            {
                m_reportPower.SetRowItemText(i, 1, g_csBlankValue);
                m_reportPower.SetRowItemText(i, 2, g_csBlankValue);
                m_reportPower.SetRowItemText(i, 3, g_csBlankValue);

                if(i >= 6)
                {
                    m_reportPower.SetRowItemText(i, 4, g_csBlankValue);
                }
            }

            // 隐藏部分菜单标题
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
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

            // 恢复通道选择功能
            for(int i = 0; i < 2; i++)
            {
                m_reportPower.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
                m_reportPower.GetRecordRow(i)->GetItem(2)->SetEditable(TRUE);
                m_reportPower.GetRecordRow(i)->GetItem(3)->SetEditable(TRUE);
            }

            // 恢复暂停开始菜单标题
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

float CSMVPowerDlg::_getBasePhaseValue()
{
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // 查找第一个电流或电压通道的索引
    for(uint32_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        if(pInfo->enChannelType == CHANNEL_TYPE_SMV_CURRENT || pInfo->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
        {
            return m_data.GetOneChannelInfo(i)->m_fPhase;
        }
    }

    return 0.0f;
}

void CSMVPowerDlg::OnInplaceButtonDown( NMHDR* , LRESULT *pResult )
{
    if(!m_bEnableTimer)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
        *pResult = TRUE;
    }
}

void CSMVPowerDlg::_modifyMenu()
{
	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_POWER_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


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


