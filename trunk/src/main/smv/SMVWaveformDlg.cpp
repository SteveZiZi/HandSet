/**
 * \file
 *      SMVWaveformDlg.h
 *
 * \brief
 *      波形图页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/19
 */


// SMVWaveformDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVWaveformDlg.h"

#include <algorithm>

#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/main/resourcebmp.h"

#define DSM_SMV_WAVEFORM_ZOOM           0x01        ///< 缩放模式
#define DSM_SMV_WAVEFORM_WATCH          0x02        ///< 观察模式

#define DSM_SMV_WAVEFORM_MAX_TIMES      10          ///< 最大波形放大倍数
#define DSM_SMV_WAVEFORM_MIN_TIMES      -10
#define DSM_SMV_TIMELINE_MAX_TIMES      10          ///< 最大时间轴放大倍数
#define DSM_SMV_TIMELINE_MIN_TIMES      -10

#define WAVEFORM_DISP_FONT_SZIE         18          ///< 波形图内字体大小

#define DSM_WFG_FONT_COLOR              RGB(128,128,135)      ///< 暖灰色



namespace SMVWaveform
{
    void conversion(float& val)
    {
        val = val / 1000;
    }
}


/**
 *      波形页面控制块相关数据
 */
class CCtrlBlkDataWaveform : public ICtrlBlkData
{
public:
    CCtrlBlkDataWaveform()
        : m_nPage(0)
        , m_nTimelineT(0)
        , m_nWaveformT(0)
    {

    }

    virtual ~CCtrlBlkDataWaveform() {}

public:
    int         m_nPage;                                ///< 当前页面
    int         m_nWaveformT;                           ///< Y轴放大倍数
    int         m_nTimelineT;                           ///< X轴放大倍数
};



// CSMVWaveformDlg 对话框

IMPLEMENT_DYNAMIC(CSMVWaveformDlg, DSM_SMVBASEDLG)

CSMVWaveformDlg::CSMVWaveformDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVWaveformDlg::IDD, DSM_STR_SMV_WAVEFORM_CAPTION, pParent)
    , m_nPage(0)
    , m_dwWaveMode(DSM_SMV_WAVEFORM_WATCH)
    , m_nSamplePer(0)
    , m_nSamplePer1(0)
    , m_nSample(0)
    , m_fAmperage(0.0f)
    , m_fVoltage(0.0)
    , m_nWaveformT(0)
    , m_nTimelineT(0)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
}

CSMVWaveformDlg::~CSMVWaveformDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVWaveformDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVWaveformDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVWaveformDlg::OnMenuF4)
END_MESSAGE_MAP()


// CSMVWaveformDlg 消息处理程序

BOOL CSMVWaveformDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 还原页面状态
    _restorePage();

    // 创建波形控件
    _createWaveform();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSMVWaveformDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if(nIDEvent == m_nTimerID)
    {
        // 刷新波形控件数据
        _updateWaveform();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

void CSMVWaveformDlg::_createWaveform()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(CRect(10, 10, 10, 10));

    // 创建波形控件
    m_ctrlWaveform.Create(NULL, WS_CHILD | WS_VISIBLE, rcClient, this, IDC_SMV_WAVFORM_CTRL);

    // 设置波形控件属性
    m_ctrlWaveform.ModifyStyle(0, SS_OWNERDRAW | WS_TABSTOP);
    m_ctrlWaveform.SetXScaleValVisible(TRUE);
    m_ctrlWaveform.SetTitleVisible(TRUE);
    m_ctrlWaveform.SetGramCount(DSM_SMV_WAVEFORM_GRAM_X, DSM_SMV_WAVEFORM_GRAM_Y);
    m_ctrlWaveform.SetHorGridVisible(FALSE);
	m_ctrlWaveform.SetBkgndColor(DSM_COLOR_BK);

    m_ctrlWaveform.SetWaveAreaGrid(3, 2);
    m_ctrlWaveform.SetTitleColor(DSM_WFG_FONT_COLOR);
	m_ctrlWaveform.SetYIntervalMaxCharNum(10);

#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
    m_ctrlWaveform.SetWaveDescVisible(TRUE);
#else
    m_ctrlWaveform.SetWaveDescVisible(FALSE);
#endif

    for(int i = 0; i < 3; i++)
    {
        m_ctrlWaveform.SetWaveDescColor(i, 0, DSM_WFG_FONT_COLOR);
    }

    // 设置波形空间内需要的字体
    CFont* pFont = dsm_fontmgr::GetInstance()->GetFont(WAVEFORM_DISP_FONT_SZIE);
    m_ctrlWaveform.SetScaleValFont(pFont);
    m_ctrlWaveform.SetTitleFont(pFont);
    m_ctrlWaveform.SetWaveDescFont(pFont);

    // 获取时间轴的默认刻度    X轴
    _setXScale();

    // 设置波形图的属性        Y轴
    _setWaveform();

    m_ctrlWaveform.SetFocus();
}


void CSMVWaveformDlg::_updateWaveform()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pDataLayer);

    if(m_bEnableTimer)
    {
        // 通道发生变化，
        if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
        {
            if(CStatusMgr::ShowTipsStyle() == 0x00)
            {
                // 界面tips提示通道数量变化
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
            }

            if(!m_bChannelChange)
            {
                m_bChannelChange = true;

                // 取消部分菜单标题
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
                m_pNaviBar->UpdateNaviMenu();

                m_ctrlWaveform.EraseWave();
                m_ctrlWaveform.Invalidate(TRUE);
            }

            return;
        }
        else
        {
            if(m_bChannelChange)
            {
                m_bChannelChange = false;
                CStatusMgr::SetTips(L"", 0);

                // 恢复部分菜单标题
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
                m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
                m_pNaviBar->UpdateNaviMenu();
            }
        }

        // 获取波形数据
        if(!m_pDataLayer->GetOscillogramData(&m_data))
        {
            // 擦除波形
            m_ctrlWaveform.EraseWave();
            m_ctrlWaveform.Invalidate(TRUE);
            return;
        }
    }

    CString csEffect(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_EFFECT));   // 获取有效值描述
    CString csAngle(dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_ANGLE));     // 获取相位角描述

    CString csValue;
    COscillogramChannelInfo* pInfo = NULL;

    // 擦除波形
    m_ctrlWaveform.EraseWave();

    // 填充波形数据
    for(uint32_t nGram = 0, i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++, nGram++)
    {
        // 获取指定通道的波形数据
        pInfo = m_data.GetOneChannelInfo(m_arrChannels[i]);

        if(pInfo)
        {

#if DSM_OSCILLOGRAM_SHOW_EFFECT_PHASE
            // 电流类型通道的描述信息
            if(CHANNEL_TYPE_SMV_CURRENT == m_arrChannelType[i])
            {
                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f°", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            // 电压类型通道的描述信息
            else if(CHANNEL_TYPE_SMV_VOLTAGE == m_arrChannelType[i])
            {
                // 一次二次值单位数据换算
                if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eDispType == SMV_VALUE_TYPE_ORIGINANL && m_eFrameType == SMV_VALUE_TYPE_PRIMARY))
                {
                    pInfo->m_fEffectiveValue = pInfo->m_fEffectiveValue / 1000;

                    for_each(pInfo->m_vecChannelData.begin(), pInfo->m_vecChannelData.end(), SMVWaveform::conversion);
                }

                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f°", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            else if(CHANNEL_TYPE_SMV_TIME == m_arrChannelType[i])
            {
                csValue.Format(L"%s: %0.3f %s\r\n%s: %0.3f°", csEffect, pInfo->m_fEffectiveValue, m_arrUnits[i], csAngle, pInfo->m_fPhase);
            }
            // 其他类型的通道描述信息，没有单位
            else
            {
                csValue.Format(L"%s: %0.3f\r\n%s: %0.3f°", csEffect, pInfo->m_fEffectiveValue, csAngle, pInfo->m_fPhase);
            }
#endif
            // 把通道的波形信息设置到波形控件
            m_ctrlWaveform.DrawWave(nGram, &pInfo->m_vecChannelData[0], pInfo->m_vecChannelData.size(), csValue);
        }
    }

    m_ctrlWaveform.Invalidate(TRUE);
}

BOOL CSMVWaveformDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:

            if(!m_bChannelChange)
            {
                // 观察模式 翻页
                if(DSM_SMV_WAVEFORM_WATCH == m_dwWaveMode)
                {
                    _pageupChannel();
                }
                // 缩放模式 幅值
                else
                {
                    _zoominWaveform();
                }

                // 更新波形数据
                _updateWaveform();
            }

            return TRUE;

        case VK_DOWN:
            if(!m_bChannelChange)
            {
                // 观察模式 翻页
                if(DSM_SMV_WAVEFORM_WATCH == m_dwWaveMode)
                {
                    _pagedownChannel();
                }
                // 缩放模式 幅值
                else
                {
                    _zoomoutWaveform();
                }

                // 更新波形数据
                _updateWaveform();
            }

            return TRUE;

        case VK_LEFT:
            // 缩放模式 时间轴
            if(!m_bChannelChange && DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
            {
                _zoomoutTimeline();

                // 更新波形数据
                _updateWaveform();
            }

            return TRUE;

        case VK_RIGHT:
            // 缩放模式 时间轴
            if(!m_bChannelChange && DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
            {
                _zoominTimeline();

                // 更新波形数据
                _updateWaveform();
            }

            return TRUE;

        default:
            break;
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      上翻通道
 */
void CSMVWaveformDlg::_pageupChannel()
{
    if(0 == m_nPage)
    {
        return;
    }

    // 页面索引减1
    m_nPage--;

    // 设置波形图控件的属性
    _setWaveform();
}

/**
 *      下翻通道
 */
void CSMVWaveformDlg::_pagedownChannel()
{
    ASSERT(m_pCurrentCtrlBlk);

    // 如果下一页通道索引大于通道总数，则返回
    if((m_nPage + 1) * DSM_SMV_WAVEFORM_NUMS >= m_pCurrentCtrlBlk->nChannelCount)
    {
        return;
    }

    // 页面索引加1
    m_nPage++;

    // 更新显示通道的默认幅值
    _setWaveform();
}

/**
 *      修改底部菜单
 */
void CSMVWaveformDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_WAVEFORM_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // 设置F4缩放选择图标
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);

	
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
 *      nCmdID          菜单ID
 *  \return
 *      bool            切换控制块成功返回true
 */
bool CSMVWaveformDlg::_switchCtrlBlk( UINT nCmdID )
{
    // 保存原控制块对象
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

    // 获取原控制块关联的数据
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // 获取数据为空则创建新的控制块关联数据
    if(!pData)
    {
        pData = new CCtrlBlkDataWaveform;
        m_pPageDataExtra->Set(pPreCtrlBlk->nID, pData);
    }

    // 保存放大倍数
    pData->m_nTimelineT = m_nTimelineT;
    pData->m_nWaveformT = m_nWaveformT;


    // 切换控制块后对控件进行设置

    // 根据新的控制块信息设置页面状态
    _restorePage();

    // 根据新的控制块信息设置波形控件
    _setWaveform();

    return true;
}

/**
 *      响应『F4』缩放/选择功能模式的切换，并更新菜单图标
 */
void CSMVWaveformDlg::OnMenuF4()
{
    if(m_bChannelChange)
    {
        return;
    }

    m_dwWaveMode = m_dwWaveMode == DSM_SMV_WAVEFORM_ZOOM ? DSM_SMV_WAVEFORM_WATCH : DSM_SMV_WAVEFORM_ZOOM;

    // 切换波形的模式为『缩放』
    if(DSM_SMV_WAVEFORM_ZOOM == m_dwWaveMode)
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STRETCH_SEL), NULL, NULL);
    }
    // 切换波形的模式为『选择』
    else
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
    }

    m_pNaviBar->UpdateNaviMenu();
}

/**
 *      放大波形
 *
 *  \note
 *      减少Y轴幅值，放大波形
 */
void CSMVWaveformDlg::_zoominWaveform()
{
    // 判断放大倍数是否超过限制
    if(DSM_SMV_WAVEFORM_MIN_TIMES >= m_nWaveformT)
    {
        return;
    }

    // 减少幅值的缩放倍数
    m_nWaveformT--;

    // 循环设置每个波形的刻度信息，主要是改变Y轴刻度（幅值）
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      缩小波形
 *
 *  \note
 *      增大Y轴幅值，缩小波形
 */
void CSMVWaveformDlg::_zoomoutWaveform()
{
     // 判断缩放倍数是否超过限制
    if(DSM_SMV_WAVEFORM_MAX_TIMES <= m_nWaveformT)
    {
        return;
    }

    // 增大幅值缩放倍数
    m_nWaveformT++;

    // 循环设置每个波形的的刻度信息，主要是改变Y轴刻度（幅值）
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      放大时间轴（X轴）
 *
 *  \note
 *      减少X轴每个格子的采样点数，拉伸波形
 */
void CSMVWaveformDlg::_zoominTimeline()
{
    // 判断缩放倍数是否超过限制
    if(DSM_SMV_TIMELINE_MIN_TIMES >= m_nTimelineT)
    {
        return;
    }

    // 减少倍数因子
    m_nTimelineT--;

    // 循环设置每个波形的刻度信息，主要是改变X轴刻度（每个格子采样点数）
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // 设置波形下方每个格子的采样点数的显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      减小时间轴（X轴）
 *
 *  \note
 *      增大X轴每个格子的采样点数，压缩波形
 */
void CSMVWaveformDlg::_zoomoutTimeline()
{
    // 判断缩放倍数是否超过限制
    if(DSM_SMV_WAVEFORM_MAX_TIMES <= m_nTimelineT)
    {
        return;
    }

    // 增大倍数因子
    m_nTimelineT++;

    // 循环设置每个波形的刻度信息，主要是改变X轴刻度（每个格子采样点数）
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // 设置波形下方每个格子的采样点数的显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));
    }

    // 重置波形空间使设置生效
    m_ctrlWaveform.Init();
}

/**
 *      根据通道的类型设置波形图的属性
 *
 *  \note
 *      根据通道信息设置波形上显示的单位信息，
 *      以及波形的基本幅值与时间轴的默认每个格子默认采样点数
 */
void CSMVWaveformDlg::_setWaveform()
{
    ASSERT(m_pCurrentCtrlBlk);

    CSmvRecvConfig* pConfig = m_pCurrentCtrlBlk->pRecvConfig;

    if(!pConfig)
    {
        ASSERT(pConfig);
        return;
    }

    COLORREF color;
    CString csDesc;
    CSmvRecvChannelInfo* pInfo  = NULL;
    m_eDispType  = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // 获取通道值的显示类型
    m_eFrameType = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // 获取额定通道类型的显示类型

    // 获取当前显示的通道序号
    _calcChannel();

    // 循环设置与通道关联的每个波形的信息
    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(m_arrChannels[i]);

        if(!pInfo)
        {
            // 设置空的title
            m_ctrlWaveform.SetTitle(i, L"");

            continue;
        }

        // 设置该通道对应波形的显示信息 - 单位与默认幅值
        // 根据设置里面显示的一次二次值信息来设置相应的默认幅值

        // 电流通道的单位与默认幅值
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            m_arrUnits[i] = L"A";

            switch(m_eDispType)
            {
                // 设置显示一次值数据的默认幅值
            case SMV_VALUE_TYPE_PRIMARY:
                m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2;
                break;

                // 设置显示二次值数据的默认幅值
            case SMV_VALUE_TYPE_SECONDARY:
                m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                break;

                // 显示原始报文数据的默认幅值
            case SMV_VALUE_TYPE_ORIGINANL:
                {
                    // 没有单位
                    m_arrUnits[i]               = L"";

                    // 根据报文参数设置页面的『额定通道类型』信息设置默认复制
                    if(SMV_VALUE_TYPE_PRIMARY == m_eFrameType)
                    {
                        m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2;
                    }
                    else
                    {
                        m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                    }
                }
                break;

            default:
                ASSERT(FALSE);
                break;
            }
        }
        // 电压通道的单位与默认幅值
        else if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            switch(m_eDispType)
            {
                // 设置显示一次值数据的默认幅值
            case SMV_VALUE_TYPE_PRIMARY:
                m_arrUnits[i]               = L"kV";
                m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2000;
                break;

                // 设置显示二次值数据的默认幅值
            case SMV_VALUE_TYPE_SECONDARY:
                m_arrUnits[i]               = L"V";
                m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                break;

                // 显示原始报文数据的默认幅值
            case SMV_VALUE_TYPE_ORIGINANL:
                m_arrUnits[i]               = L"";

                // 根据报文参数设置页面的『额定通道类型』信息设置默认复制
                if(SMV_VALUE_TYPE_PRIMARY == m_eFrameType)
                {
                    m_arrChannelAmplitude[i]    = pInfo->fPrimaryRatedValue / 2000;
                }
                else
                {
                    m_arrChannelAmplitude[i]    = pInfo->fSecondaryRatedValue / 2;
                }
                break;

            default:
                ASSERT(FALSE);
                break;
            }
        }
        // 时间通道，默认幅值为400.0
        else if(CHANNEL_TYPE_SMV_TIME == pInfo->enChannelType)
        {
            m_arrUnits[i]               = L"μs";
            m_arrChannelAmplitude[i]    = 400.0f;
        }
        // 其他类型，默认幅值为400.0
        else
        {
            m_arrUnits[i]               = L"";
            m_arrChannelAmplitude[i]    = 400.0f;
        }

        // 设置通道类型到与波形控件相关联的变量
        m_arrChannelType[i] = pInfo->enChannelType;

        csDesc.Format(L"%d: %s", pInfo->nIndex + 1, pInfo->strChannelDesc.c_str());

        // 设置通道关联的波形图标题
        m_ctrlWaveform.SetTitle(i, csDesc);

        // 设置通道关联的XY轴刻度信息
        m_ctrlWaveform.SetScaleInterval(i, 
            (float)(m_nSamplePer * pow(2, m_nTimelineT)), 
            m_arrChannelAmplitude[i] * (float)pow(2, m_nWaveformT), 
            m_arrUnits[i]);

        // 设置波形下方的X轴显示信息
        m_ctrlWaveform.SetXOneGridPointCount(i, (int)(m_nSamplePer1 * (float)pow(2, m_nTimelineT)));

        // 电流电压通道类型需要设置3相类型的颜色
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            // 根据通道的相别获取相应的颜色
            switch(pInfo->enChannelPhase)
            {
                // A相
            case CHANNEL_PHASE_A:
                color = DSM_PHASE_COLOR_A;
                break;

                // B相
            case CHANNEL_PHASE_B:
                color = DSM_PHASE_COLOR_B;
                break;

                // C相
            case CHANNEL_PHASE_C:
                color = DSM_PHASE_COLOR_C;
                break;

                // N相
            case CHANNEL_PHASE_N:
                color = DSM_PHASE_COLOR_N;
                break;

                // X相
            case CHANNEL_PHASE_X:
                color = DSM_PHASE_COLOR_X;
                break;

                // 未知
            default:
                color = DSM_PHASE_COLOR_UN;
                break;
            }
        }
        else
        {
            // 其他类型通道设置未知颜色
            color = DSM_PHASE_COLOR_UN;
        }

        // 把颜色信息设置到波形控件
        m_ctrlWaveform.SetWaveColor(i, 0, color);
    }

    // 使上面设置生效
    m_ctrlWaveform.Init();
}

void CSMVWaveformDlg::_setXScale()
{
    ASSERT(m_pCurrentCtrlBlk);

    // 设置X轴的默认刻度值，默认是一周波的采样点数
    m_nSamplePer = m_pCurrentCtrlBlk->nSampleRate / DSM_WORK_HERTZ_50 / 2;
    m_nSamplePer1 = m_nSamplePer;
#if DSM_DSP_CALC_128K_RESAMPLE_TO_4K
    if (m_pCurrentCtrlBlk->nSampleRate == 12800)
    {
        m_nSamplePer1 = 4000 / DSM_WORK_HERTZ_50 / 2;
    }
#endif
}


/**
 *      保存当前波形页面的状态信息，以便切换回波形页面时能显示上一次离开时的状态
 */
void CSMVWaveformDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    if(pData)
    {
        pData->m_nTimelineT = m_nTimelineT;     // 时间轴缩放倍数
        pData->m_nWaveformT = m_nWaveformT;     // 波形幅值缩放倍数
    }
}

/**
 *      根据控制块关联的页面状态信息进行设置
 *
 *  \return
 *      bool        还原页面状态成功返回true，失败返回false
 *
 *  \note
 *      让波形显示与控制块相关联的信息，比如幅值（缩放倍数）、显示通道等信息
 */
bool CSMVWaveformDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVWaveformDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }

    m_nPage      = pData->m_nPage;              // 当前页面值
    m_nTimelineT = pData->m_nTimelineT;         // 时间轴缩放倍数
    m_nWaveformT = pData->m_nWaveformT;         // 幅值缩放倍数

    // 根据当前页面计算需要显示的通道索引
    _calcChannel();

    return true;
}

/**
 *      根据当前页面索引计算显示的通道索引
 */
void CSMVWaveformDlg::_calcChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // 计算当前显示通道索引
    uint32_t nTotals = DSM_SMV_WAVEFORM_NUMS;
    uint32_t nFirst  = m_nPage * nTotals;            // 计算第一个显示通道序号
    uint32_t nLast   = nFirst + nTotals;             // 最后一个显示通道序号

    // 通道数目减少，调整起始通道从0开始
    if(nFirst >= m_pCurrentCtrlBlk->nChannelCount)
    {
        nFirst  = 0;
        m_nPage = 0;
        nLast   = nFirst + nTotals;
    }

    nLast = nLast > m_pCurrentCtrlBlk->nChannelCount ? m_pCurrentCtrlBlk->nChannelCount : nLast;

    // 获取当前的控制块数据
    CCtrlBlkDataWaveform* pData = dynamic_cast<CCtrlBlkDataWaveform*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块的页面状态信息为空，则新建一个
    if(!pData)
    {
        pData = new CCtrlBlkDataWaveform;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    // 下面的设置信息为该通道新的页面状态信息

    // 设置控制块关联的页面状态信息
    pData->m_nPage = m_nPage;

    for(int i = 0; i < DSM_SMV_WAVEFORM_NUMS; i++)
    {
        uint32_t nChannel = nFirst + i;

        if(nChannel <= nLast)
        {
            m_arrChannels[i] = nChannel;
        }
        else
        {
            m_arrChannels[i] = UINT_MAX;
        }
    }
}


