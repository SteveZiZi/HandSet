/// @file
///
/// @brief
///     离散度计算
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.11.11
///
/// 修改历史：
///

#include "stdafx.h"
#include "msgdispersioncalc.h"

///
/// @brief
///    hash数组，用于通过报文时间差映射报文时间差区间，初始化为1个ASDU
///
static uint8_t timeMsgHash1Asdu[MAX_HASH_COUNT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,5,6,7,7,7,7,7,7,7,7,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
9,9,9,9};


CMsgDisperisonCalc::CMsgDisperisonCalc(): m_ndispSize(0)
{
    memset(m_ndispArr, 0, sizeof(m_ndispArr));
    memcpy_s(m_nTimeMsgHash, MAX_HASH_COUNT, timeMsgHash1Asdu, sizeof(timeMsgHash1Asdu));
    _dispHashCreate(m_cConfig.nASDUCount);
}

CMsgDisperisonCalc::~CMsgDisperisonCalc()
{
}

///
/// @brief
///    对一个时间差值进行哈希，判断属于哪个范围内
///
uint8_t CMsgDisperisonCalc::_dispHash( int64_t timeDif)
{
    int64_t nMaxTime = 250* m_cConfig.nASDUCount;
    if(timeDif >= nMaxTime)
    {
        return 10;
    }
    else if(timeDif <= -nMaxTime)
    {
        return 0;
    }
    else
    {
        uint64_t nIndex = timeDif + nMaxTime -1;
        if (nIndex < MAX_HASH_COUNT)
        {
            return m_nTimeMsgHash[nIndex];
        }
        else
        {
            return 10;
        }
    }
}

///
/// @brief
///     生成报文时间差与报文区间的映射hash数组
/// @param[in] 
///    nASDUCount           ASDU的个数（目前最多支持8个）
///    
/// @return
///    int32_t              0：成功
///                         -1：ASDU个数超过限制
///
int32_t CMsgDisperisonCalc::_dispHashCreate( uint16_t nASDUCount )
{
    if(nASDUCount > MAX_ASDU_COUNT || nASDUCount == 0)
    {
        return -1;
    }
    memset(&m_nTimeMsgHash[0], 1, 200*nASDUCount);
    memset(&m_nTimeMsgHash[200*nASDUCount], 2, 50*nASDUCount-10);
    memset(&m_nTimeMsgHash[250*nASDUCount-10], 3, 8);
    memset(&m_nTimeMsgHash[250*nASDUCount-2], 4, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount-1], 5, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount], 6, 1);
    memset(&m_nTimeMsgHash[250*nASDUCount+1], 7, 8);
    memset(&m_nTimeMsgHash[250*nASDUCount+9], 8, 50*nASDUCount-10);
    memset(&m_nTimeMsgHash[300*nASDUCount-1], 9, 200*nASDUCount);
    return 0;
}

///
/// @brief
///     重置报文统计数据
///
void CMsgDisperisonCalc::ResetData()
{
    memset(m_ndispArr, 0, sizeof(m_ndispArr));
    m_ndispSize = 0;
}


///
/// @brief
///    获取离散度统计结果
///
bool CMsgDisperisonCalc::GetDispersion(float* dispArrRate, uint32_t nArraySize)
{
    if (nArraySize != DISPERISON_ARRAY_SIZE)
    {
        return false;
    }

    // 计算概率百分比
    for(uint32_t i=0; i<DISPERISON_ARRAY_SIZE; i++)
    {
        if(m_ndispArr[i] == 0)
        {
            dispArrRate[i] = 0.0f;
        }
        else
        {
            dispArrRate[i] = mpysp_i(divsp_i((float)m_ndispArr[i],(float)m_ndispSize),100);
        }
    }
    return true;
}

///
/// @brief
///    处理一个时间戳信息
///
void CMsgDisperisonCalc::ParsingFrameTimeStamp( FrameTimeStamp& stFrameTimeStamp )
{
    if (m_stLastFrameTimeStamp.ts_sec == 0 && m_stLastFrameTimeStamp.ts_usec == 0)
    {
        m_stLastFrameTimeStamp = stFrameTimeStamp;
        return;
    }

    int64_t nTimeInterval = stFrameTimeStamp - m_stLastFrameTimeStamp;
    int64_t defaultTimeDif = m_cConfig.nASDUCount* 1000000 / m_cConfig.nSamplingFreq;

    // 计算时间差
    int64_t difTime = 0;
    if(nTimeInterval > defaultTimeDif && nTimeInterval - defaultTimeDif > 2147483647)
    {
        difTime = 2147483647;
    }
    else if(nTimeInterval < defaultTimeDif && defaultTimeDif - nTimeInterval > 2147483648)
    {
        difTime = -2147483647;
    }
    else
    {
        difTime = nTimeInterval - defaultTimeDif;
    }

    // 计算时间差级别的hash下标
    int8_t hash = _dispHash(difTime);

    // 相应级别次数+1
    m_ndispArr[hash]++;

    //总离散度数据计数
    m_ndispSize++;

    //保存最后一次时间戳
    m_stLastFrameTimeStamp = stFrameTimeStamp;
}

