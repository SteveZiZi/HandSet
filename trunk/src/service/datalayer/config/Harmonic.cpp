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

#include "StdAfx.h"
#include "Harmonic.h"
#include "ParamConfigSMVSend.h"
#include "SMVSendVirtualChannel.h"

///
/// @brief
///    ���캯��
///
CHarmonic::CHarmonic(void)
:m_pSmvSendVirtualChannel(NULL)
,m_pParamConfigBase(NULL)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{
}

///
/// @brief
///    ���캯��
///
/// @param[in]  pParamConfigBase    �������ù�����
///
CHarmonic::CHarmonic( CParamConfigBase* pParamConfigBase )
:m_pSmvSendVirtualChannel(NULL)
,m_pParamConfigBase(pParamConfigBase)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{

}

///
/// @brief
///    ���캯��
///
/// @param[in]  pSmvSendVirtualChannel  ָ�����ⷢ��ͨ�������ָ��
/// @param[in]  pParamConfigBase    �������ù�����
///
CHarmonic::CHarmonic( CSMVSendVirtualChannel* pSmvSendVirtualChannel, CParamConfigBase* pParamConfigBase )
:m_pSmvSendVirtualChannel(pSmvSendVirtualChannel)
,m_pParamConfigBase(pParamConfigBase)
,m_eHarmonicType(HARMONIC_NONE)
,m_dAmplitude(0.0f)
,m_dPhaseAngle(0.0f)
{

}

///
/// @brief
///    ��������
///
CHarmonic::~CHarmonic(void)
{
}

///
/// @brief
///    ���г������
///
/// @return
///     enumHarmonicType    г������
///
enumHarmonicType CHarmonic::GetHarmonicType()
{
    return m_eHarmonicType;
}

///
/// @brief
///    ����г������
///
/// @param[in]  eNewType    г������
///
void CHarmonic::SetHarmonicType( enumHarmonicType eNewType )
{
    if (m_eHarmonicType != eNewType)
    {
        m_eHarmonicType = eNewType;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///    ��÷�ֵ
///
/// @return
///     double    ��ֵ
///
double CHarmonic::GetAmplitude()
{
    double                      dRet = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if ((NULL == pParamConfigSMVSend) || (NULL == m_pSmvSendVirtualChannel))
    {
        return dRet;
    }

    //
    // ���ݲ���ֵ��ʾ��ʽ��������յķ���ֵ
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                        ///< һ��ֵ
        {
            dRet =  m_dAmplitude;
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                      ///< ����ֵ
        {
            // ��õ�ǰѡ�е�ͨ��
            CChannel* pChannel = m_pSmvSendVirtualChannel->GetChannel(m_pSmvSendVirtualChannel->GetSelectedIndex());
            if (NULL != pChannel)
            {
                // ���ͨ����һ�ζֵ�����ζֵ
                double dOnceRatings         = pChannel->GetOnceRatings();
                double dSecondaryRatings    = pChannel->GetSecondaryRatings();

                // ���ݱ�����ϵ��������շ���ֵ
                if (fabs(dOnceRatings) >= FLOAT_PRECISION)
                {
                    dRet = m_dAmplitude * dSecondaryRatings / dOnceRatings;
                }
            }
        }
        break;

    default:
        break;
    }

    return dRet;
}

///
/// @brief
///    ���÷�ֵ
///
/// @param[in]  fAmplitude    �µķ�ֵ
///
void CHarmonic::SetAmplitude( double dAmplitude )
{
    double                          dData = 0.0f;


    // ���SMV���Ͳ������ù�����
    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if ((NULL == pParamConfigSMVSend) || (NULL == m_pSmvSendVirtualChannel))
    {
        return;
    }

    //
    // ���ݲ���ֵ��ʾ��ʽ��������յķ�ֵһ��ֵ
    //
    switch(pParamConfigSMVSend->GetSVDisplay())
    {
    case SMV_VALUE_TYPE_PRIMARY:                                        ///< һ��ֵ
        {
            if (fabs(m_dAmplitude - dAmplitude) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dAmplitude;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;

    case SMV_VALUE_TYPE_SECONDARY:                                      ///< ����ֵ
        {
            // ��õ�ǰѡ�е�ͨ��
            CChannel* pChannel = m_pSmvSendVirtualChannel->GetChannel(m_pSmvSendVirtualChannel->GetSelectedIndex());
            if (NULL == pChannel)
            {
                return;
            }

            // ���ͨ����һ�ζֵ�����ζֵ
            double      dOnceRatings          = pChannel->GetOnceRatings();
            double      dSecondaryRatings     = pChannel->GetSecondaryRatings();

            // ���ݱ�����ϵ��������շ���ֵ
            if (fabs(dSecondaryRatings) < FLOAT_PRECISION)
            {
                return;
            }

            // ���һ��ֵ
            dData = dAmplitude * dOnceRatings / dSecondaryRatings;
            if (fabs(m_dAmplitude - dData) >= FLOAT_PRECISION)
            {
                m_dAmplitude = dData;

                // ���Ϊ���޸�
                if (NULL != m_pParamConfigBase)
                {
                    m_pParamConfigBase->SetModified(true);
                }
            }
        }
        break;
    }
}

///
/// @brief
///    ������
///
/// @return
///     double    ���
///
double CHarmonic::GetPhaseAngle()
{
    return m_dPhaseAngle;
}

///
/// @brief
///    �������
///
/// @param[in]  fPhaseAngle    ���
///
void CHarmonic::SetPhaseAngle( double dPhaseAngle )
{
    if (m_dPhaseAngle != dPhaseAngle)
    {
        m_dPhaseAngle = dPhaseAngle;

        // ���Ϊ���޸�
        if (NULL != m_pParamConfigBase)
        {
            m_pParamConfigBase->SetModified(true);
        }
    }
}

///
/// @brief
///     ���г����������������ͨ��
///
/// @return
///     CSMVSendVirtualChannel* ָ������ͨ�������ָ��
///
CSMVSendVirtualChannel* CHarmonic::GetSmvSendVirtualChannel() const
{
    return m_pSmvSendVirtualChannel;
}

///
/// @brief
///     ����г����������������ͨ��
///
/// @param[in]  val ָ������ͨ�������ָ��
///
void CHarmonic::SetSmvSendVirtualChannel( CSMVSendVirtualChannel* val )
{
    m_pSmvSendVirtualChannel = val;
}

///
/// @brief
///     �Ƚ�г�����͡���ֵ�����
///
/// @param[in]  oldHarmonic     ���Աȵ�г������
///
/// @return
///     bool    ��ͬ����true
///
bool CHarmonic::Compare( CHarmonic& oldHarmonic )
{
    // �Ƚ�г������
    if (m_eHarmonicType != oldHarmonic.m_eHarmonicType)
    {
        return false;
    }
    // �����г������ô�����з�ֵ����ǱȽ�
    else if (HARMONIC_NONE == m_eHarmonicType)
    {
        return true;
    }
    // �ȽϷ�ֵ
    else if (fabs(m_dAmplitude - oldHarmonic.m_dAmplitude) >= FLOAT_PRECISION)
    {
        return false;
    }
    // �Ƚ����
    else if (fabs(m_dPhaseAngle - oldHarmonic.m_dPhaseAngle) >= FLOAT_PRECISION)
    {
        return false;
    }

    return true;
}
