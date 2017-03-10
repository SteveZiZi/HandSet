/// @file
///
/// @brief
///     定义序量数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataXuliang.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\DsmDataLayer.h"


CDataXuliang::CDataXuliang(void) : m_enCurrentType(CHANNEL_TYPE_SMV_VOLTAGE)
{
    memset(m_fPhaseSeqContent, 0, sizeof(m_fPhaseSeqContent));
    memset(m_fPhaseSeqAngle, 0, sizeof(m_fPhaseSeqAngle));
}

CDataXuliang::~CDataXuliang(void)
{
}

///
/// @brief
///    获取零序、正序、负序数值
///
/// @param[out] fZeroPhaseSeqContent   获取零序数值
/// @param[out] fPosPhaseSeqContent    获取正序数值
/// @param[out] fNegPhaseSeqContent    获取负序数值
///
void CDataXuliang::GetPhaseSeqContent( float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
    {
        fZeroPhaseSeqContent = m_fPhaseSeqContent[0][0];
        fPosPhaseSeqContent  = m_fPhaseSeqContent[0][1];
        fNegPhaseSeqContent  = m_fPhaseSeqContent[0][2];
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
    {
        fZeroPhaseSeqContent = m_fPhaseSeqContent[1][0];
        fPosPhaseSeqContent  = m_fPhaseSeqContent[1][1];
        fNegPhaseSeqContent  = m_fPhaseSeqContent[1][2];
    }
}

///
/// @brief
///    获取零序、正序、负序角度
///
/// @param[out] fZeroPhaseSeqAngle   获取零序角度
/// @param[out] fPosPhaseSeqAngle    获取正序角度
/// @param[out] fNegPhaseSeqAngle    获取负序角度
///
void CDataXuliang::GetPhaseSeqAngle( float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
    {
        fZeroPhaseSeqAngle = m_fPhaseSeqAngle[0][0];
        fPosPhaseSeqAngle  = m_fPhaseSeqAngle[0][1];
        fNegPhaseSeqAngle  = m_fPhaseSeqAngle[0][2];
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
    {
        fZeroPhaseSeqAngle = m_fPhaseSeqAngle[1][0];
        fPosPhaseSeqAngle  = m_fPhaseSeqAngle[1][1];
        fNegPhaseSeqAngle  = m_fPhaseSeqAngle[1][2];
    }
}

///
/// @brief
///    获取三相电流通道
///
/// @param[out]  nPhaseA 保存A相通道号
/// @param[out]  nPhaseB 保存B相通道号
/// @param[out]  nPhaseC 保存C相通道号
///
/// return
///     bool    获取成功返回true
///
bool CDataXuliang::GetABCChannles( uint32_t& nPhaseA, uint32_t& nPhaseB, uint32_t& nPhaseC)
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvConfig* pSmvRecvConfig = m_pCtrlBlk->pRecvConfig;
        if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
        {
            return pSmvRecvConfig->GetDspSendVolInfo(nPhaseA, nPhaseB, nPhaseC);
        }
        else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
        {
            return pSmvRecvConfig->GetDspSendCurInfo(nPhaseA, nPhaseB, nPhaseC);
        }
    }
    return false;
}


///
/// @brief
///    设置三相电压/电流通道
///
/// @param[in]  nPhaseA A相通道号
/// @param[in]  nPhaseB B相通道号
/// @param[in]  nPhaseC C相通道号
///
/// return
///     bool    设置成功返回true
///
bool CDataXuliang::SetABCChannles( uint32_t nPhaseA, uint32_t nPhaseB, uint32_t nPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        CSmvRecvConfig* pSmvRecvConfig = m_pCtrlBlk->pRecvConfig;
        if (CHANNEL_TYPE_SMV_VOLTAGE == m_enCurrentType)
        {
            pSmvRecvConfig->SetDspSendCurInfo(0xFF, 0xFF, 0xFF);
            return pSmvRecvConfig->SetDspSendVolInfo(nPhaseA, nPhaseB, nPhaseC);
        }
        else if (CHANNEL_TYPE_SMV_CURRENT == m_enCurrentType)
        {
            pSmvRecvConfig->SetDspSendVolInfo(0xFF, 0xFF, 0xFF);
            return pSmvRecvConfig->SetDspSendCurInfo(nPhaseA, nPhaseB, nPhaseC);
        }
    }
    return false;
}


///
/// @brief
///    设置零序、正序、负序数值
///
/// @param[in] enChannelType          指出设置的是电压还是电流的序量值
/// @param[in] fZeroPhaseSeqContent   设置零序数值
/// @param[in] fPosPhaseSeqContent    设置正序数值
/// @param[in] fNegPhaseSeqContent    设置负序数值
///
void CDataXuliang::SetPhaseSeqContent( enumChannelType enChannelType, float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == enChannelType)
    {
        m_fPhaseSeqContent[0][0] = fZeroPhaseSeqContent;
        m_fPhaseSeqContent[0][1] = fPosPhaseSeqContent;
        m_fPhaseSeqContent[0][2] = fNegPhaseSeqContent;
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == enChannelType)
    {
        m_fPhaseSeqContent[1][0] = fZeroPhaseSeqContent;
        m_fPhaseSeqContent[1][1] = fPosPhaseSeqContent;
        m_fPhaseSeqContent[1][2] = fNegPhaseSeqContent;
    }
}

///
/// @brief
///    设置零序、正序、负序角度
///
/// @param[in] enChannelType        指出设置的是电压还是电流的序量角度
/// @param[in] fZeroPhaseSeqAngle   设置零序角度
/// @param[in] fPosPhaseSeqAngle    设置正序角度
/// @param[in] fNegPhaseSeqAngle    设置负序角度
///
void CDataXuliang::SetPhaseSeqAngle( enumChannelType enChannelType, float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle )
{
    if (CHANNEL_TYPE_SMV_VOLTAGE == enChannelType)
    {
        m_fPhaseSeqAngle[0][0] = fZeroPhaseSeqAngle;
        m_fPhaseSeqAngle[0][1] = fPosPhaseSeqAngle;
        m_fPhaseSeqAngle[0][2] = fNegPhaseSeqAngle;
    }
    else if (CHANNEL_TYPE_SMV_CURRENT == enChannelType)
    {
        m_fPhaseSeqAngle[1][0] = fZeroPhaseSeqAngle;
        m_fPhaseSeqAngle[1][1] = fPosPhaseSeqAngle;
        m_fPhaseSeqAngle[1][2] = fNegPhaseSeqAngle;
    }
}

///
/// @brief
///    设置当前序量电压电流模式
///
void CDataXuliang::SetXLMode( enumChannelType enChannelType )
{
    m_enCurrentType = enChannelType;
}

///
/// @brief
///    获取当前序量的电压电流模式
///
enumChannelType CDataXuliang::GetXLMode()
{
    return m_enCurrentType;
}

///
/// @brief
///    清空序量信息
///
void CDataXuliang::ClearXLData()
{
    memset(m_fPhaseSeqContent, 0, sizeof(m_fPhaseSeqContent));
    memset(m_fPhaseSeqAngle, 0, sizeof(m_fPhaseSeqAngle));
}
