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
///    ��ȡSmv1Ƶ����Ϣ
///
/// @return
///     float ����smv���ƿ�Ƶ��
///
float CDataPhaseCheck::GetSmv1Freq()
{
    return m_fFreq[0];
}

///
/// @brief
///    ��ȡSmv2Ƶ����Ϣ
///
/// @return
///     float ����smv���ƿ�Ƶ��
///
float CDataPhaseCheck::GetSmv2Freq()
{
    return m_fFreq[1];
}

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
///    ��ȡsmv2 ABC��ͨ������Чֵ����λ��Ϣ
///
/// @param[out]     pPhaseA     ����Smv2 A����Чֵ����λ��Ϣ
/// @param[out]     pPhaseB     ����Smv2 B����Чֵ����λ��Ϣ
/// @param[out]     pPhaseC     ����Smv2 C����Чֵ����λ��Ϣ
///
/// @return
///     bool    ��ȡ�ɹ�����true
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
///    ��ȡͬ����ǲ�
///
/// @param[out]     pPhaseAa     ����Smv1 A����Smv2 a����ǲ�
/// @param[out]     pPhaseBb     ����Smv1 B����Smv2 b����ǲ�
/// @param[out]     pPhaseCc     ����Smv1 C����Smv2 c����ǲ�
///
/// @return
///     bool    ��ȡ�ɹ�����true
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
///    ��ȡͬ��������
///
/// @param[out]     pPhaseAa     ����Smv1 A����Smv2 a��������
/// @param[out]     pPhaseBb     ����Smv1 B����Smv2 b��������
/// @param[out]     pPhaseCc     ����Smv1 C����Smv2 c��������
///
/// @return
///     bool    ��ȡ�ɹ�����true
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
///    ��ȡSmv1���������
///
/// @param[out]     pPhaseAB     ����Smv1 A��B��������
/// @param[out]     pPhaseBC     ����Smv1 B��C��������
/// @param[out]     pPhaseCA     ����Smv1 C��A��������
///
/// @return
///     bool    ��ȡ�ɹ�����true
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
///    ��ȡSmv2���������
///
/// @param[out]     pPhaseab     ����Smv2 A��B��������
/// @param[out]     pPhasebc     ����Smv2 B��C��������
/// @param[out]     pPhaseca     ����Smv2 C��A��������
///
/// @return
///     bool    ��ȡ�ɹ�����true
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
///    ��������
///
void CDataPhaseCheck::ResetData()
{
    memset(m_fFreq, 0, sizeof(m_fFreq));
    memset(m_stPhaseABCData, 0, sizeof(m_stPhaseABCData));
    memset(m_fInPhaseAngleDiff, 0, sizeof(m_fInPhaseAngleDiff));
    memset(m_stInPhasePhasorDiff, 0, sizeof(m_stInPhasePhasorDiff));
    memset(m_stBetweenPhasePhasorDiff, 0, sizeof(m_stBetweenPhasePhasorDiff));
}
