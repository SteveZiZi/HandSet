/// @file
///
/// @brief
///     核相需要的数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.31
///
/// 修改历史：
///

#include "StdAfx.h"
#include "DataPhaseCheck.h"

CDataPhaseCheck::CDataPhaseCheck(void)
{
    memset(m_fFreq, 0, sizeof(m_fFreq));
    memset(m_stPhaseABCData, 0, sizeof(m_stPhaseABCData));
    memset(m_fInPhaseAngleDiff, 0, sizeof(m_fInPhaseAngleDiff));
    memset(m_stInPhasePhasorDiff, 0, sizeof(m_stInPhasePhasorDiff));
    memset(m_stBetweenPhasePhasorDiff, 0, sizeof(m_stBetweenPhasePhasorDiff));
}

CDataPhaseCheck::~CDataPhaseCheck(void)
{
}

///
/// @brief
///    获取Smv1频率信息
///
/// @return
///     float 返回smv控制块频率
///
float CDataPhaseCheck::GetSmv1Freq()
{
    return m_fFreq[0];
}

///
/// @brief
///    获取Smv2频率信息
///
/// @return
///     float 返回smv控制块频率
///
float CDataPhaseCheck::GetSmv2Freq()
{
    return m_fFreq[1];
}

///
/// @brief
///    获取smv1 ABC三通道的有效值，相位信息
///
/// @param[out]     pPhaseA     保存Smv1 A相有效值，相位信息
/// @param[out]     pPhaseB     保存Smv1 B相有效值，相位信息
/// @param[out]     pPhaseC     保存Smv1 C相有效值，相位信息
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetSmv1VaulePhase( CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC )
{
    if (NULL != pPhaseC && NULL != pPhaseB && NULL != pPhaseC)
    {
        *pPhaseA = m_stPhaseABCData[0][0];
        *pPhaseB = m_stPhaseABCData[0][1];
        *pPhaseC = m_stPhaseABCData[0][2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取smv2 ABC三通道的有效值，相位信息
///
/// @param[out]     pPhaseA     保存Smv2 A相有效值，相位信息
/// @param[out]     pPhaseB     保存Smv2 B相有效值，相位信息
/// @param[out]     pPhaseC     保存Smv2 C相有效值，相位信息
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetSmv2VaulePhase( CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC )
{
    if (NULL != pPhaseC && NULL != pPhaseB && NULL != pPhaseC)
    {
        *pPhaseA = m_stPhaseABCData[1][0];
        *pPhaseB = m_stPhaseABCData[1][1];
        *pPhaseC = m_stPhaseABCData[1][2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取同相相角差
///
/// @param[out]     pPhaseAa     保存Smv1 A相与Smv2 a相相角差
/// @param[out]     pPhaseBb     保存Smv1 B相与Smv2 b相相角差
/// @param[out]     pPhaseCc     保存Smv1 C相与Smv2 c相相角差
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetInPhaseAngleDiff( float* pPhaseAa, float* pPhaseBb, float* pPhaseCc )
{
    if (NULL != pPhaseAa && NULL != pPhaseBb && NULL != pPhaseCc)
    {
        *pPhaseAa = m_fInPhaseAngleDiff[0];
        *pPhaseBb = m_fInPhaseAngleDiff[1];
        *pPhaseCc = m_fInPhaseAngleDiff[2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取同相相量差
///
/// @param[out]     pPhaseAa     保存Smv1 A相与Smv2 a相相量差
/// @param[out]     pPhaseBb     保存Smv1 B相与Smv2 b相相量差
/// @param[out]     pPhaseCc     保存Smv1 C相与Smv2 c相相量差
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetInPhasePhasorDiff( CStructPhasor* pPhaseAa, CStructPhasor* pPhaseBb, CStructPhasor* pPhaseCc )
{
    if (NULL != pPhaseAa && NULL != pPhaseBb && NULL != pPhaseCc)
    {
        *pPhaseAa = m_stInPhasePhasorDiff[0];
        *pPhaseBb = m_stInPhasePhasorDiff[1];
        *pPhaseCc = m_stInPhasePhasorDiff[2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取Smv1相间相量差
///
/// @param[out]     pPhaseAB     保存Smv1 A相B相相量差
/// @param[out]     pPhaseBC     保存Smv1 B相C相相量差
/// @param[out]     pPhaseCA     保存Smv1 C相A相相量差
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetSmv1BetweenPhasePhasorDiff( CStructPhasor* pPhaseAB, CStructPhasor* pPhaseBC, CStructPhasor* pPhaseCA )
{
    if (NULL != pPhaseAB && NULL != pPhaseBC && NULL != pPhaseCA)
    {
        *pPhaseAB = m_stBetweenPhasePhasorDiff[0][0];
        *pPhaseBC = m_stBetweenPhasePhasorDiff[0][1];
        *pPhaseCA = m_stBetweenPhasePhasorDiff[0][2];
        return true;
    }
    return false;
}

///
/// @brief
///    获取Smv2相间相量差
///
/// @param[out]     pPhaseab     保存Smv2 A相B相相量差
/// @param[out]     pPhasebc     保存Smv2 B相C相相量差
/// @param[out]     pPhaseca     保存Smv2 C相A相相量差
///
/// @return
///     bool    获取成功返回true
///
bool CDataPhaseCheck::GetSmv2BetweenPhasePhasorDiff( CStructPhasor* pPhaseab, CStructPhasor* pPhasebc, CStructPhasor* pPhaseca )
{
    if (NULL != pPhaseab && NULL != pPhasebc && NULL != pPhaseca)
    {
        *pPhaseab = m_stBetweenPhasePhasorDiff[1][0];
        *pPhasebc = m_stBetweenPhasePhasorDiff[1][1];
        *pPhaseca = m_stBetweenPhasePhasorDiff[1][2];
        return true;
    }
    return false;
}

///
/// @brief
///    重置数据
///
void CDataPhaseCheck::ResetData()
{
    memset(m_fFreq, 0, sizeof(m_fFreq));
    memset(m_stPhaseABCData, 0, sizeof(m_stPhaseABCData));
    memset(m_fInPhaseAngleDiff, 0, sizeof(m_fInPhaseAngleDiff));
    memset(m_stInPhasePhasorDiff, 0, sizeof(m_stInPhasePhasorDiff));
    memset(m_stBetweenPhasePhasorDiff, 0, sizeof(m_stBetweenPhasePhasorDiff));
}
