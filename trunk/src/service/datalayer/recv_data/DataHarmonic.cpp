/// @file
///
/// @brief
///     ����г��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.14
///
/// �޸���ʷ��
///

#include "StdAfx.h"
#include "DataHarmonic.h"

CDataHarmonic::CDataHarmonic(void)
{

    memset(m_fDCValue, 0, sizeof(m_fDCValue));
    memset(m_fSumHarmTHD, 0 ,sizeof(m_fSumHarmTHD));
    memset(m_fHarmContent, 0, sizeof(m_fHarmContent));
    memset(m_fHarmAngle, 0, sizeof(m_fHarmAngle));
    memset(m_fHarmTHD, 0, sizeof(m_fHarmTHD));
}

CDataHarmonic::~CDataHarmonic(void)
{
}

///
/// @brief
///    ��ȡͨ��DC��Чֵ
///
float CDataHarmonic::GetChannelDCValue( uint32_t nIndex )
{
    if (nIndex < MAX_SMV_CHANNELS)
    {
        return m_fDCValue[nIndex];
    }
    return 0.0f;
}

///
/// @brief
///    ��ȡͨ����г��������
///
float CDataHarmonic::GetChannelHarmTHD( uint32_t nIndex , uint32_t nHarmType /*= 0*/)
{
    if (nIndex < MAX_SMV_CHANNELS)
    {
        if (0 == nHarmType)
        {
            return m_fSumHarmTHD[nIndex];
        }
        else if (nHarmType < MAX_HARM_COUNT && nHarmType > 0)
        {
            return m_fHarmTHD[nIndex][nHarmType - 1];
        }
    }
    return 0.0f;
}

///
/// @brief
///    ��ȡһ��ͨ��ĳ��г����Чֵ����λ��Ϣ
///
/// @param[in]  nChannelIndex ͨ����������0��ʼ
///
/// @param[in]  nHarmType г������������Ϊһ��г����������ֵ1��ʾһ��г��
///
/// @param[out] fHarmContent �����Ӧ��г������Чֵ
///
/// @param[out] fHarmAngle   �����Ӧ��г������λ��
///
/// @return
///     bool ��ȡ�ɹ�����true
///
bool CDataHarmonic::GetChannelHarmInfo( uint32_t nChannelIndex, uint32_t nHarmType, float& fHarmContent, float& fHarmAngle )
{
    if (nChannelIndex < MAX_SMV_CHANNELS && nHarmType < MAX_HARM_COUNT && nHarmType > 0)
    {
        fHarmContent = m_fHarmContent[nChannelIndex][nHarmType - 1];
        fHarmAngle   = m_fHarmAngle[nChannelIndex][nHarmType - 1];
        return true;
    }
    return false;
}

///
/// @brief
///    ���г����Ϣ
///
void CDataHarmonic::ClearHarmonicData()
{
    memset(m_fDCValue, 0, sizeof(m_fDCValue));
    memset(m_fSumHarmTHD, 0 ,sizeof(m_fSumHarmTHD));
    memset(m_fHarmContent, 0, sizeof(m_fHarmContent));
    memset(m_fHarmAngle, 0, sizeof(m_fHarmAngle));
    memset(m_fHarmTHD, 0, sizeof(m_fHarmTHD));
}

float CDataHarmonic::GetChannelDcTHD( uint32_t nIndex )
{
    if (nIndex < MAX_SMV_CHANNELS)
    {
        return m_fDCTHD[nIndex];
    }
    return 0.0f;
}
