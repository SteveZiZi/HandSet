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

#ifndef _MSG_DISPERISON_CALC_H
#define _MSG_DISPERISON_CALC_H

#include "src\include\stdint.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define MAX_ASDU_COUNT          8
#define DISPERISON_ARRAY_SIZE   11

#define mpysp_i(a,b) ((a)*(b))
#define divsp_i(a,b) ((a)/(b))

///< 定义最大的时间差与离散度区间hash数组的大小
#define MAX_HASH_COUNT (500*MAX_ASDU_COUNT)

///
/// @brief
///    定义计算离散度时的配置信息结构体
///
struct DispersionCalcConfig
{
    uint16_t nASDUCount;                                            ///< ASDU的个数
    uint32_t nSamplingFreq;                                         ///< 采样频率
    uint16_t nRatedFreq;                                            ///< 额定频率

    DispersionCalcConfig(): nASDUCount(1)
                          , nSamplingFreq(4000)
                          , nRatedFreq(50)
    {
    }

    bool operator == (const DispersionCalcConfig& r)
    {
        return nASDUCount == r.nASDUCount && nSamplingFreq == r.nSamplingFreq && nRatedFreq == r.nRatedFreq;
    }

    bool operator!= (const DispersionCalcConfig& rhs)
    {
        return !(*this == rhs);
    }
};

class CMsgDisperisonCalc
{
public:
    CMsgDisperisonCalc();
    ~CMsgDisperisonCalc();

public:
    ///
    /// @brief
    ///    配置计算参数
    ///
    bool SetCalcConfig(const DispersionCalcConfig& stConfig)
    {
        bool bModified = false;
        if (stConfig.nASDUCount != 0 && stConfig.nASDUCount != m_cConfig.nASDUCount)
        {
            m_cConfig.nASDUCount = stConfig.nASDUCount;
            bModified = true;
        }
        if (stConfig.nRatedFreq != 0 && stConfig.nRatedFreq != m_cConfig.nRatedFreq)
        {
            m_cConfig.nRatedFreq = stConfig.nRatedFreq;
            bModified = true;
        }
        if (stConfig.nSamplingFreq != 0 && stConfig.nSamplingFreq != m_cConfig.nSamplingFreq)
        {
            m_cConfig.nSamplingFreq = stConfig.nSamplingFreq;
            bModified = true;
        }
        if (bModified)
        {
            ResetData();
            _dispHashCreate(m_cConfig.nASDUCount);
        }
        return true;
    }

    ///
    /// @brief
    ///    获取计算参数配置信息
    ///
    DispersionCalcConfig GetCalcConfig()
    {
        return m_cConfig;
    }

    ///
    /// @brief
    ///    处理一个时间戳信息
    ///
    void ParsingFrameTimeStamp(FrameTimeStamp& stFrameTimeStamp);

    ///
    /// @brief
    ///     重置报文统计数据
    ///
    void ResetData();

    ///
    /// @brief
    ///    获取离散度统计结果
    ///
    bool GetDispersion(float* dispArrRate, uint32_t nArraySize);

private:
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
    int32_t _dispHashCreate(uint16_t nASDUCount);

    ///
    /// @brief
    ///    对一个时间差值进行哈希，判断属于哪个范围内
    ///
    uint8_t _dispHash(int64_t timeDif);


private:
    DispersionCalcConfig        m_cConfig;                          ///< 计算离散度的配置信息

    uint64_t                    m_ndispArr[DISPERISON_ARRAY_SIZE];  ///< 各离散度区间数目
    uint64_t                    m_ndispSize;                        ///< 离散度数据总数，会比报文数目少1

    uint8_t                     m_nTimeMsgHash[MAX_HASH_COUNT];     ///< hash数组，用于通过报文时间差映射报文时间差区间，初始化为1个ASDU

    FrameTimeStamp              m_stLastFrameTimeStamp;             ///< 上一帧的报文时间戳
};

#endif //_MSG_DISPERISON_CALC_H