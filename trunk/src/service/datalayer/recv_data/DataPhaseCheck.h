/// @file
///
/// @brief
///     ������Ҫ��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.31
///
/// �޸���ʷ��
///

#pragma once
#include "DataRecvBase.h"

///
/// @brief
///    ���������ṹ
///
class CStructPhasor
{
public:
    float               m_fValue;                                       ///< ������Чֵ����
    float               m_fPhase;                                       ///< �����ǶȲ���

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
// ��ȡ��������
//=============================================================================
//
    ///
    /// @brief
    ///    ��ȡSmv1Ƶ����Ϣ
    ///
    /// @return
    ///     float ����smv���ƿ�Ƶ��
    ///
    float GetSmv1Freq();

    ///
    /// @brief
    ///    ��ȡSmv2Ƶ����Ϣ
    ///
    /// @return
    ///     float ����smv���ƿ�Ƶ��
    ///
    float GetSmv2Freq();

    ///
    /// @brief
    ///    ��ȡsmv1 ABC��ͨ������Чֵ����λ��Ϣ
    ///
    /// @param[out]     pPhaseA     ����Smv1 A����Чֵ����λ��Ϣ
    /// @param[out]     pPhaseB     ����Smv1 B����Чֵ����λ��Ϣ
    /// @param[out]     pPhaseC     ����Smv1 C����Чֵ����λ��Ϣ
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv1VaulePhase(CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC);

    ///
    /// @brief
    ///    ��ȡsmv2 ABC��ͨ������Чֵ����λ��Ϣ
    ///
    /// @param[out]     pPhaseA     ����Smv2 A����Чֵ����λ��Ϣ
    /// @param[out]     pPhaseB     ����Smv2 B����Чֵ����λ��Ϣ
    /// @param[out]     pPhaseC     ����Smv2 C����Чֵ����λ��Ϣ
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv2VaulePhase(CStructPhasor* pPhaseA, CStructPhasor* pPhaseB, CStructPhasor* pPhaseC);

    ///
    /// @brief
    ///    ��ȡͬ����ǲ�
    ///
    /// @param[out]     pPhaseAa     ����Smv1 A����Smv2 a����ǲ�
    /// @param[out]     pPhaseBb     ����Smv1 B����Smv2 b����ǲ�
    /// @param[out]     pPhaseCc     ����Smv1 C����Smv2 c����ǲ�
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetInPhaseAngleDiff(float* pPhaseAa, float* pPhaseBb, float* pPhaseCc);

    ///
    /// @brief
    ///    ��ȡͬ��������
    ///
    /// @param[out]     pPhaseAa     ����Smv1 A����Smv2 a��������
    /// @param[out]     pPhaseBb     ����Smv1 B����Smv2 b��������
    /// @param[out]     pPhaseCc     ����Smv1 C����Smv2 c��������
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetInPhasePhasorDiff(CStructPhasor* pPhaseAa, CStructPhasor* pPhaseBb, CStructPhasor* pPhaseCc);

    ///
    /// @brief
    ///    ��ȡSmv1���������
    ///
    /// @param[out]     pPhaseAB     ����Smv1 A��B��������
    /// @param[out]     pPhaseBC     ����Smv1 B��C��������
    /// @param[out]     pPhaseCA     ����Smv1 C��A��������
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv1BetweenPhasePhasorDiff(CStructPhasor* pPhaseAB, CStructPhasor* pPhaseBC, CStructPhasor* pPhaseCA);

    ///
    /// @brief
    ///    ��ȡSmv2���������
    ///
    /// @param[out]     pPhaseab     ����Smv2 A��B��������
    /// @param[out]     pPhasebc     ����Smv2 B��C��������
    /// @param[out]     pPhaseca     ����Smv2 C��A��������
    ///
    /// @return
    ///     bool    ��ȡ�ɹ�����true
    ///
    bool GetSmv2BetweenPhasePhasorDiff(CStructPhasor* pPhaseab, CStructPhasor* pPhasebc, CStructPhasor* pPhaseca);

    ///
    /// @brief
    ///    ��������
    ///
    void ResetData();

private:
    friend class CFrameRecv;
    float               m_fFreq[2];                                     ///< �������ƿ��Ƶ��
    CStructPhasor       m_stPhaseABCData[2][3];                         ///< �������ƿ�ABC�����Ӧ����Чֵ����λ��
                                                                        ///< m_stPhaseABCData[0][3]��ʾ��һ�����ƿ��ͨ������
    float               m_fInPhaseAngleDiff[3];                         ///< ͬ����ǲ�ֱ��ӦAa,Bb,Cc
    CStructPhasor       m_stInPhasePhasorDiff[3];                       ///< ͬ��������ֱ��ӦAa,Bb,Cc
    CStructPhasor       m_stBetweenPhasePhasorDiff[2][3];               ///< ��������[0][3]�ֱ��ӦAB,BC,CA,[1][3]�ֱ��Ӧab,bc,ca������
};
