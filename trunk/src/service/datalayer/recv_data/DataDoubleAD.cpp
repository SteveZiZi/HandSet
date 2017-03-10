/// @file
///
/// @brief
///     ����˫ad������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.4.24
///
/// �޸���ʷ��
///


#include "StdAfx.h"
#include "DataDoubleAD.h"
#include "src\service\datalayer\include\config.h"

//������뾯��todo��Ϣ
//#pragma message(__FILE__ "(" STRING(__LINE__) "): ����Uninitʱ�Ƿ�Ҫ��ֹͣ����ӿ����ݽ���")
#define STRING2(x) #x
#define STRING(x) STRING2(x)

CDataDoubleAD::CDataDoubleAD(void)
{
    memset(m_nADGroupChannels, 0, sizeof(m_nADGroupChannels));
    memset(&m_stSmvValue, 0, sizeof(m_stSmvValue));
}

CDataDoubleAD::~CDataDoubleAD(void)
{
}

///
/// @brief
///    ����AD1��Ӧ��ͨ����
///
/// @param[in] nIndexADGroup ָ���ڼ���AD
/// @param[in] nIndexChannel ָ��Ҫ���õ�ͨ����
///
void CDataDoubleAD::SetAD1Channel( uint32_t nIndexADGroup, uint32_t nIndexChannel )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        m_nADGroupChannels[nIndexADGroup][0] = nIndexChannel;
    }
}

///
/// @brief
///    ����AD2��Ӧ��ͨ����
///
/// @param[in] nIndexADGroup ָ���ڼ���AD
/// @param[in] nIndexChannel ָ��Ҫ���õ�ͨ����
///
void CDataDoubleAD::SetAD2Channel( uint32_t nIndexADGroup, uint32_t nIndexChannel )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        m_nADGroupChannels[nIndexADGroup][1] = nIndexChannel;
    }
}

///
/// @brief
///    ��ȡAD1��Ӧ��ͨ����
///
/// @param[in] nIndexADGroup ָ���ڼ���AD
///
/// @return
///     uint32_t ���ػ�ȡ����ͨ���ţ�����ͨ����Ŀ������Ч
///
uint32_t CDataDoubleAD::GetAD1Channel( uint32_t nIndexADGroup )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        return m_nADGroupChannels[nIndexADGroup][0];
    }
    //����һ����ͨ����Ŀ�����ֵ������ͨ����Ч
    return NET_9_2_LE_MAX_CHANNELS_PER_ASDU + 1;
}

///
/// @brief
///    ��ȡAD2��Ӧ��ͨ����
///
/// @param[in] nIndexADGroup ָ���ڼ���AD
///
/// @return
///     uint32_t ���ػ�ȡ����ͨ���ţ�����ͨ����Ŀ������Ч
///
uint32_t CDataDoubleAD::GetAD2Channel( uint32_t nIndexADGroup )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        return m_nADGroupChannels[nIndexADGroup][1];
    }
    //����һ����ͨ����Ŀ�����ֵ������ͨ����Ч
    return NET_9_2_LE_MAX_CHANNELS_PER_ASDU + 1;
}

///
/// @brief
///    ��ȡĳһ��˫AD������
///
/// @param[in]  nIndexADGroup   Ҫ��ȡ��˫AD���
/// @param[out] fVal1           Ҫ��ȡ�Ľ��1
/// @param[out] fVAl2           Ҫ��ȡ�Ľ��2
///
/// @return
///     bool    ��ȡ�ɹ�����true
///
bool CDataDoubleAD::GetADGroupResult( uint32_t nIndexADGroup, float& fVal1, float& fVal2 )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        float fResult1 = 0.0f, fResult2 = 0.0f;
        uint32_t nChannel1 = NET_9_2_LE_MAX_CHANNELS_PER_ASDU, nChannel2 = NET_9_2_LE_MAX_CHANNELS_PER_ASDU;
        nChannel1 = GetAD1Channel(nIndexADGroup);
        nChannel2 = GetAD2Channel(nIndexADGroup);
        if (nChannel1 < m_stSmvValue.nChannelCount && nChannel2 < m_stSmvValue.nChannelCount)
        {
            fResult1 = m_stSmvValue.fSmvValue[nChannel1];
            fResult2 = m_stSmvValue.fSmvValue[nChannel2];

#pragma message(__FILE__ "(" STRING(__LINE__) "): ˫AD�������㷨��ȷ��")
            fVal1 = fabsf(fResult1 - fResult2);
            fVal2 = fabsf(fResult1) - fabsf(fResult2);
            return true;
        }
    }
    return false;
}

///
/// @brief
///    ���˫AD������Ϣ
///
void CDataDoubleAD::ClearDoubleADData()
{
    memset(m_nADGroupChannels, 0, sizeof(m_nADGroupChannels));
}

///
/// @brief
///    ����˫ad������
///
void CDataDoubleAD::SetDoubleAdSmvValue( const SMV_VALUE& stSmvValue )
{
    m_stSmvValue = stSmvValue;
}
