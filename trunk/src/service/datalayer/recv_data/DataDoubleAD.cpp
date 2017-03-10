/// @file
///
/// @brief
///     定义双ad数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///


#include "StdAfx.h"
#include "DataDoubleAD.h"
#include "src\service\datalayer\include\config.h"

//定义编译警告todo信息
//#pragma message(__FILE__ "(" STRING(__LINE__) "): 考虑Uninit时是否要先停止网络接口数据接收")
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
///    设置AD1对应的通道号
///
/// @param[in] nIndexADGroup 指出第几组AD
/// @param[in] nIndexChannel 指出要设置的通道号
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
///    设置AD2对应的通道号
///
/// @param[in] nIndexADGroup 指出第几组AD
/// @param[in] nIndexChannel 指出要设置的通道号
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
///    获取AD1对应的通道号
///
/// @param[in] nIndexADGroup 指出第几组AD
///
/// @return
///     uint32_t 返回获取到的通道号，超出通道数目代表无效
///
uint32_t CDataDoubleAD::GetAD1Channel( uint32_t nIndexADGroup )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        return m_nADGroupChannels[nIndexADGroup][0];
    }
    //返回一个比通道数目大的数值，代表通道无效
    return NET_9_2_LE_MAX_CHANNELS_PER_ASDU + 1;
}

///
/// @brief
///    获取AD2对应的通道号
///
/// @param[in] nIndexADGroup 指出第几组AD
///
/// @return
///     uint32_t 返回获取到的通道号，超出通道数目代表无效
///
uint32_t CDataDoubleAD::GetAD2Channel( uint32_t nIndexADGroup )
{
    if (nIndexADGroup < DOUBLE_AD_GROUP_COUNT)
    {
        return m_nADGroupChannels[nIndexADGroup][1];
    }
    //返回一个比通道数目大的数值，代表通道无效
    return NET_9_2_LE_MAX_CHANNELS_PER_ASDU + 1;
}

///
/// @brief
///    获取某一组双AD计算结果
///
/// @param[in]  nIndexADGroup   要获取的双AD组别
/// @param[out] fVal1           要获取的结果1
/// @param[out] fVAl2           要获取的结果2
///
/// @return
///     bool    获取成功返回true
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

#pragma message(__FILE__ "(" STRING(__LINE__) "): 双AD计算结果算法待确认")
            fVal1 = fabsf(fResult1 - fResult2);
            fVal2 = fabsf(fResult1) - fabsf(fResult2);
            return true;
        }
    }
    return false;
}

///
/// @brief
///    清空双AD保留信息
///
void CDataDoubleAD::ClearDoubleADData()
{
    memset(m_nADGroupChannels, 0, sizeof(m_nADGroupChannels));
}

///
/// @brief
///    设置双ad的数据
///
void CDataDoubleAD::SetDoubleAdSmvValue( const SMV_VALUE& stSmvValue )
{
    m_stSmvValue = stSmvValue;
}
