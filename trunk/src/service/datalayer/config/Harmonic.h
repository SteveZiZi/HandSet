///
/// @brief
///     г����
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.14
///
/// �޸���ʷ��
///

#ifndef __HARMONIC_H__
#define __HARMONIC_H__
#include "paramconfigbase.h"

///
/// @brief
///    г������
///
enum enumHarmonicType
{
    HARMONIC_UNKNOWN = 0,
    HARMONIC_NONE,                                                      ///< ��г��
    HARMONIC_DC,                                                        ///< ֱ��
    HARMONIC_2_TIMES,                                                   ///< 2��г��
    HARMONIC_3_TIMES,                                                   ///< 3��г��
    HARMONIC_4_TIMES,                                                   ///< 4��г��
    HARMONIC_5_TIMES,                                                   ///< 5��г��
    HARMONIC_6_TIMES,                                                   ///< 6��г��
    HARMONIC_7_TIMES,                                                   ///< 7��г��
    HARMONIC_8_TIMES,                                                   ///< 8��г��
    HARMONIC_9_TIMES,                                                   ///< 9��г��
    HARMONIC_10_TIMES,                                                  ///< 10��г��
    HARMONIC_11_TIMES,                                                  ///< 11��г��
    HARMONIC_12_TIMES,                                                  ///< 12��г��
    HARMONIC_13_TIMES,                                                  ///< 13��г��
    HARMONIC_14_TIMES,                                                  ///< 14��г��
    HARMONIC_15_TIMES,                                                  ///< 15��г��
    HARMONIC_16_TIMES,                                                  ///< 16��г��
    HARMONIC_17_TIMES,                                                  ///< 17��г��
    HARMONIC_18_TIMES,                                                  ///< 18��г��
    HARMONIC_19_TIMES,                                                  ///< 19��г��
    HARMONIC_20_TIMES                                                   ///< 20��г��
};

///
/// @brief
///    г��
///
class CHarmonic
{
    friend class CSMVSendVirtualChannel;

protected:
    CHarmonic(void);
    virtual ~CHarmonic(void);

public:
    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  pParamConfigBase    �������ù�����
    ///
    CHarmonic(CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���캯��
    ///
    /// @param[in]  pSmvSendVirtualChannel  ָ�����ⷢ��ͨ�������ָ��
    /// @param[in]  pParamConfigBase    �������ù�����
    ///
    CHarmonic(CSMVSendVirtualChannel* pSmvSendVirtualChannel, CParamConfigBase* pParamConfigBase);

    ///
    /// @brief
    ///    ���г������
    ///
    /// @return
    ///     enumHarmonicType    г������
    ///
    enumHarmonicType GetHarmonicType();

    ///
    /// @brief
    ///    ����г������
    ///
    /// @param[in]  eNewType    г������
    ///
    void SetHarmonicType(enumHarmonicType eNewType);

    ///
    /// @brief
    ///    ��÷�ֵ
    ///
    /// @return
    ///     double    ��ֵ
    ///
    double GetAmplitude();

    ///
    /// @brief
    ///    ���÷�ֵ
    ///
    /// @param[in]  dAmplitude    �µķ�ֵ
    ///
    void SetAmplitude(double dAmplitude);

    ///
    /// @brief
    ///    ������
    ///
    /// @return
    ///     double    ���
    ///
    double GetPhaseAngle();

    ///
    /// @brief
    ///    �������
    ///
    /// @param[in]  dPhaseAngle    ���
    ///
    void SetPhaseAngle(double dPhaseAngle);

    ///
    /// @brief
    ///     ���г����������������ͨ��
    ///
    /// @return
    ///     CSMVSendVirtualChannel* ָ������ͨ�������ָ��
    ///
    CSMVSendVirtualChannel* GetSmvSendVirtualChannel() const;

    ///
    /// @brief
    ///     ����г����������������ͨ��
    ///
    /// @param[in]  val ָ������ͨ�������ָ��
    ///
    void SetSmvSendVirtualChannel(CSMVSendVirtualChannel* val);

    ///
    /// @brief
    ///     �Ƚ�г�����͡���ֵ�����
    ///
    /// @param[in]  oldHarmonic     ���Աȵ�г������
    ///
    /// @return
    ///     bool    ��ͬ����true
    ///
    bool Compare(CHarmonic& oldHarmonic);

private:
    CSMVSendVirtualChannel*             m_pSmvSendVirtualChannel;       ///< г����������������ͨ��
    CParamConfigBase*                   m_pParamConfigBase;             ///< �������ù�����

    enumHarmonicType                    m_eHarmonicType;                ///< г������
    double                              m_dAmplitude;                   ///< ��ֵ
    double                              m_dPhaseAngle;                  ///< ���
};
#endif
