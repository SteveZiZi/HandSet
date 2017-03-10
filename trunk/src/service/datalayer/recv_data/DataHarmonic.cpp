/// @file
///
/// @brief
///     定义谐波数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.14
///
/// 修改历史：
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
///    获取通道DC有效值
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
///    获取通道总谐波含有率
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
///    获取一个通道某次谐波有效值和相位信息
///
/// @param[in]  nChannelIndex 通道索引，从0开始
///
/// @param[in]  nHarmType 谐波次数，基波为一次谐波，传递数值1表示一次谐波
///
/// @param[out] fHarmContent 保存对应次谐波的有效值
///
/// @param[out] fHarmAngle   保存对应次谐波的相位角
///
/// @return
///     bool 获取成功返回true
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
///    清空谐波信息
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
