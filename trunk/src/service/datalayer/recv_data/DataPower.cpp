/// @file
///
/// @brief
///     定义功率数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.23
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataPower.h"
#include "src\service\datalayer\include\config.h"
#include "src\service\datalayer\DsmDataLayer.h"

CDataPower::CDataPower(void): m_fTotalActivePower(0)
                            , m_fTotalApparentPower(0)
                            , m_fTotalReactivePower(0)
                            , m_fTotalPowerFactor(0)
{
    memset(m_fActivePower, 0, sizeof(m_fActivePower));
    memset(m_fReactivePower, 0, sizeof(m_fReactivePower));
    memset(m_fApparentPower, 0, sizeof(m_fApparentPower));
    memset(m_fPowerFactor, 0, sizeof(m_fPowerFactor));
    init();
}

CDataPower::~CDataPower(void)
{
}

///
/// @brief
///    获取有功功率
///
/// @param[out]  fPhaseAData        保存A相有功功率
/// @param[out]  fPhaseBData        保存B相有功功率
/// @param[out]  fPhaseCData        保存C相有功功率
/// @param[out]  fTotalData         保存总有功功率
///
/// @return
///
void CDataPower::GetActivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fActivePower[0];
    fPhaseBData = m_fActivePower[1];
    fPhaseCData = m_fActivePower[2];
    fTotalData  = m_fTotalActivePower;
}

///
/// @brief
///    获取无功功率
///
/// @param[out]  fPhaseAData        保存A相无功功率
/// @param[out]  fPhaseBData        保存B相无功功率
/// @param[out]  fPhaseCData        保存C相无功功率
/// @param[out]  fTotalData         保存总无功功率
///
/// @return
///
void CDataPower::GetReactivePower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fReactivePower[0];
    fPhaseBData = m_fReactivePower[1];
    fPhaseCData = m_fReactivePower[2];
    fTotalData  = m_fTotalReactivePower;
}

///
/// @brief
///    获取视在功率
///
/// @param[out]  fPhaseAData        保存A相视在功率
/// @param[out]  fPhaseBData        保存B相视在功率
/// @param[out]  fPhaseCData        保存C相视在功率
/// @param[out]  fTotalData         保存总视在功率
///
/// @return
///
void CDataPower::GetApparentPower( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fApparentPower[0];
    fPhaseBData = m_fApparentPower[1];
    fPhaseCData = m_fApparentPower[2];
    fTotalData  = m_fTotalApparentPower;
}

///
/// @brief
///    获取功率因数
///
/// @param[out]  fPhaseAData        保存A相功率因数
/// @param[out]  fPhaseBData        保存B相功率因数
/// @param[out]  fPhaseCData        保存C相功率因数
/// @param[out]  fTotalData         保存总功率因数
///
/// @return
///
void CDataPower::GetPowerFactor( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData, float& fTotalData)
{
    fPhaseAData = m_fPowerFactor[0];
    fPhaseBData = m_fPowerFactor[1];
    fPhaseCData = m_fPowerFactor[2];
    fTotalData  = m_fTotalPowerFactor;
}

///
/// @brief
///    设置三相电压通道
///
/// @param[in]  nVolPhaseA A相电压通道号
/// @param[in]  nVolPhaseB B相电压通道号
/// @param[in]  nVolPhaseC C相电压通道号
///
bool CDataPower::SetVoltageChannles( uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->SetDspSendVolInfo(nVolPhaseA, nVolPhaseB, nVolPhaseC);
    }
    return false;
}

///
/// @brief
///    获取三相电压通道
///
/// @param[out]  nVolPhaseA 保存A相电压通道号
/// @param[out]  nVolPhaseB 保存B相电压通道号
/// @param[out]  nVolPhaseC 保存C相电压通道号
///
bool CDataPower::GetVoltageChannles( uint32_t& nVolPhaseA, uint32_t& nVolPhaseB, uint32_t& nVolPhaseC )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->GetDspSendVolInfo(nVolPhaseA, nVolPhaseB, nVolPhaseC);
    }
    return false;
}

///
/// @brief
///    设置三相电流通道
///
/// @param[in]  nCurPhaseA A相电流通道号
/// @param[in]  nCurPhaseB B相电流通道号
/// @param[in]  nCurPhaseC C相电流通道号
///
bool CDataPower::SetCurrentChannles( uint32_t nCurPhaseA, uint32_t nCurPhaseB, uint32_t nCurPhaseC )
{
    CBaseDetectItem* pCurrentItem = CDsmDataLayer::getInstance()->GetCurrentCtrlBlk();
    if (!m_pCtrlBlk || m_pCtrlBlk->nID != pCurrentItem->nID)
    {
        SetCtrlBlk(pCurrentItem);
    }
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->SetDspSendCurInfo(nCurPhaseA, nCurPhaseB, nCurPhaseC);
    }
    return false;
}

///
/// @brief
///    获取三相电流通道
///
/// @param[out]  nCurPhaseA 保存A相电流通道号
/// @param[out]  nCurPhaseB 保存B相电流通道号
/// @param[out]  nCurPhaseC 保存C相电流通道号
///
bool CDataPower::GetCurrentChannles( uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC )
{
    if (m_pCtrlBlk && m_pCtrlBlk->pRecvConfig)
    {
        return m_pCtrlBlk->pRecvConfig->GetDspSendCurInfo(nCurPhaseA, nCurPhaseB, nCurPhaseC);
    }
    return false;
}


///
/// @brief
///    初始化获取相别/类型信息，在添加完所有通道后进行调用
///
void CDataPower::init()
{
    //初始化时先设置当前控制块
    if (NULL == m_pCtrlBlk)
    {
        SetCtrlBlk(CDsmDataLayer::getInstance()->GetCurrentCtrlBlk());
    }
}

///
/// @brief
///    清空功率信息
///
void CDataPower::ClearPowerData()
{
    memset(m_fActivePower, 0, sizeof(m_fActivePower));
    memset(m_fReactivePower, 0, sizeof(m_fReactivePower));
    memset(m_fApparentPower, 0, sizeof(m_fApparentPower));
    memset(m_fPowerFactor, 0, sizeof(m_fPowerFactor));
    m_fTotalActivePower     = 0;
    m_fTotalReactivePower   = 0;
    m_fTotalApparentPower   = 0;
    m_fTotalPowerFactor     = 0;

}

///
/// @brief
///    获取电压有效值信息
///
/// @param[out]  fPhaseAData        保存A相电压有效值
/// @param[out]  fPhaseBData        保存B相电压有效值
/// @param[out]  fPhaseCData        保存C相电压有效值
///
/// @return
///     获取成功返回true
///
bool CDataPower::GetVotageEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetVoltageChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fEffectiveValue;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取电压相位信息
///
/// @param[out]  fPhaseAData        保存A相电压相位
/// @param[out]  fPhaseBData        保存B相电压相位
/// @param[out]  fPhaseCData        保存C相电压相位
///
/// @return
///     获取成功返回true
///
bool CDataPower::GetVoltageAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetVoltageChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fPhase;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取电流有效值信息
///
/// @param[out]  fPhaseAData        保存A相电流有效值
/// @param[out]  fPhaseBData        保存B相电流有效值
/// @param[out]  fPhaseCData        保存C相电流有效值
///
/// @return
///     获取成功返回true
///
bool CDataPower::GetCurrentEffectValue( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetCurrentChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fEffectiveValue;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取电流相位信息
///
/// @param[out]  fPhaseAData        保存A相电流相位
/// @param[out]  fPhaseBData        保存B相电流相位
/// @param[out]  fPhaseCData        保存C相电流相位
///
/// @return
///     获取成功返回true
///
bool CDataPower::GetCurrentAngle( float& fPhaseAData, float& fPhaseBData, float& fPhaseCData )
{
    uint32_t nPhaseABC[3] = {0};
    float    fPhaseABCData[3] = {0.0f};
    if( GetCurrentChannles(nPhaseABC[0], nPhaseABC[1], nPhaseABC[2]) )
    {
        CEffectiveChanelInfo* pChannelInfo = NULL;
        for (int i = 0; i < 3; ++i)
        {
            pChannelInfo = GetOneChannelInfo(nPhaseABC[i]);
            if (pChannelInfo)
            {
                fPhaseABCData[i] = pChannelInfo->m_fPhase;
            }
            else
            {
                return false;
            }
        }
        fPhaseAData = fPhaseABCData[0];
        fPhaseBData = fPhaseABCData[1];
        fPhaseCData = fPhaseABCData[2];
        return true;
    }
    return false;
}
