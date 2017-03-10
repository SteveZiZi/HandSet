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

#pragma once
#include "DataRecvBase.h"

///
/// @brief
///    定义相量结构
///
class CStructPhasor
{
public:
    float               m_fValue;                                       ///< 相量有效值部分
    float               m_fPhase;                                       ///< 相量角度部分

    CStructPhasor(): m_fPhase(0.0)
                   , m_fValue(0.0)
    {
    }
};

class CDataPhaseCheck : public CDataRecvBase
{
public:
    CDataPhaseCheck(void);
    ~CDataPhaseCheck(void);

public:

//
//=============================================================================
// 获取核相数据
//=============================================================================
//
    ///
    /// @brief
    ///    获取Smv1频率信息
    ///
    /// @return
    ///     float 返回smv控制块频率
    ///
    float GetSmv1Freq();

    ///
    /// @brief
    ///    获取Smv2频率信息
    ///
    /// @return
    ///     float 返回smv控制块频率
    ///
    float GetSmv2Freq();

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
    bool GetSmv1VaulePhase(CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC);

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
    bool GetSmv2VaulePhase(CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC);

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
    bool GetInPhaseAngleDiff(float* pPhaseAa, float* pPhaseBb, float* pPhaseCc);

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
    bool GetInPhasePhasorDiff(CStructPhasor* pPhaseAa, CStructPhasor* pPhaseBb, CStructPhasor* pPhaseCc);

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
    bool GetSmv1BetweenPhasePhasorDiff(CStructPhasor* pPhaseAB, CStructPhasor* pPhaseBC, CStructPhasor* pPhaseCA);

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
    bool GetSmv2BetweenPhasePhasorDiff(CStructPhasor* pPhaseab, CStructPhasor* pPhasebc, CStructPhasor* pPhaseca);

    ///
    /// @brief
    ///    重置数据
    ///
    void ResetData();

private:
    friend class CFrameRecv;
    float               m_fFreq[2];                                     ///< 两个控制块的频率
    CStructPhasor       m_stPhaseABCData[2][3];                         ///< 两个控制块ABC三相对应的有效值和相位角
                                                                        ///< m_stPhaseABCData[0][3]表示第一个控制块的通道数据
    float               m_fInPhaseAngleDiff[3];                         ///< 同相相角差，分别对应Aa,Bb,Cc
    CStructPhasor       m_stInPhasePhasorDiff[3];                       ///< 同相相量差，分别对应Aa,Bb,Cc
    CStructPhasor       m_stBetweenPhasePhasorDiff[2][3];               ///< 相间相量差，[0][3]分别对应AB,BC,CA,[1][3]分别对应ab,bc,ca相量差
};
