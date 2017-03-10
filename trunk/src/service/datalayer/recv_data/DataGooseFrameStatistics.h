/// @file
///
/// @brief
///     定义goose数据统计信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.7
///
/// 修改历史：
///

#pragma once

#include "src\utils\MessageCodec\include\iec61850_data_struct.h"
#include "src\service\datalayer\recv_data\DataRecvBase.h"

//定义Goose异常统计结构
struct GooseFrameStatistics{
    uint32_t    m_nTotalFrameCount;                                             ///< 报文总计数
    uint32_t    m_nLostFrameCount;                                              ///< 从开始统计到当前，记录SqNum丢帧的次数。
    uint32_t    m_nMisOrderFrameCount;                                          ///< 从开始统计到当前，记录SqNum错序的次数。
    uint32_t    m_nRetryFrameCount;                                             ///< 从开始统计到当前，记录SqNum重发的次数。
    uint32_t    m_nStLostFrameCount;                                            ///< 从开始统计到当前，记录StNum丢帧的次数。
    uint32_t    m_nStMisOrderFrameCount;                                        ///< 从开始统计到当前，记录StNum错序的次数。
    uint32_t    m_nLostDataGramCount;                                           ///< 从开始统计到当前，报文丢失的次数，两帧报文的间隔超过 TimeAllowedToLive=T0 *2 ，则判断为一次报文丢失。
    uint32_t    m_nCommBreakCount;                                              ///< 从开始统计到当前，通信中断的次数，两帧报文的间隔超过 TimeAllowedToLive * 2 ，则判断为一次通信中断。
    uint32_t    m_nInvalidTTLCount;                                             ///< 从开始统计到当前，时间失效的次数，报文中的TTL(TimeAllowedToLive) = 0，则判断为一次存活时间无效。
    uint32_t    m_nTimeQualityNotSyncCount;                                     ///< 从开始统计到当前，时间品质时钟未同步的次数，报文中的Event TimeStamp变位时间中时间质量的字段时钟未同步标志为1时判断为一次时钟未同步。
    uint32_t    m_nTimeQualityFaultCount;                                            ///< 从开始统计到当前，时间品质时钟故障的次数，报文中的Event TimeStamp变位时间中时间质量的字段时钟故障标志为1时判断为一次时钟未同步。
};

//与界面交互goose数据统计类
class CGooseFrameStatistics
{
public:
    CGooseFrameStatistics()
    {
        memset(&m_stGooseFrameStatistics, 0, sizeof(m_stGooseFrameStatistics));
    }

    ~CGooseFrameStatistics()
    {
    }

    ///
    /// @brief
    ///    获取统计统计的所有结果项
    ///
    GooseFrameStatistics GetGooseFrameStatistics()
    {
        return m_stGooseFrameStatistics;
    }

    ///
    /// @brief
    ///    获取总报文数量
    ///
    uint32_t GetTotalFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nTotalFrameCount;
    }

    ///
    /// @brief
    ///    获取sqNum丢帧帧数
    ///
    uint32_t GetLostFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nLostFrameCount;
    }

    ///
    /// @brief
    ///    获取sqNum错误帧数
    ///
    uint32_t GetMisOrderFrameCount()
    {
        return m_stGooseFrameStatistics.m_nMisOrderFrameCount;
    }

    ///
    /// @brief
    ///    获取sqNum重发帧数
    ///
    uint32_t GetRetryFrameCount()
    {
        return m_stGooseFrameStatistics.m_nRetryFrameCount;
    }

    ///
    /// @brief
    ///    获取stNum丢帧帧数
    ///
    uint32_t GetStLostFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nStLostFrameCount;
    }

    ///
    /// @brief
    ///    获取stNum错序帧数
    ///
    uint32_t GetStMisOrderFrameCount()
    {
        return m_stGooseFrameStatistics.m_nStMisOrderFrameCount;
    }

    ///
    /// @brief
    ///    获取丢失报文次数
    ///
    uint32_t GetLostDataGramCount()
    {
        return m_stGooseFrameStatistics.m_nLostDataGramCount;
    }

    ///
    /// @brief
    ///    获取通信中断次数
    ///
    uint32_t GetCommBreakCount()
    {
        return m_stGooseFrameStatistics.m_nCommBreakCount;
    }

    ///
    /// @brief
    ///    获取报文ttl时间失效次数
    ///
    uint32_t GetInvalidTTLCount()
    {
        return m_stGooseFrameStatistics.m_nInvalidTTLCount;
    }

    ///
    /// @brief
    ///    获取时钟未同步次数
    ///
    uint32_t GetTimeQualityNotSyncCount()
    {
        return m_stGooseFrameStatistics.m_nTimeQualityNotSyncCount;
    }

    ///
    /// @brief
    ///    获取时钟故障次数
    ///
    uint32_t GetTimeQualityFaultCount()
    {
        return m_stGooseFrameStatistics.m_nTimeQualityFaultCount;
    }

    uint32_t GetStNumReversCounts()
    {
        return 0;
    }

    uint32_t GetSqNumReversCounts()
    {
        return 0;
    }

    friend class CFrameRecv;
    friend class CPcapAnalysis;
private:
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< 数据统计结构
};

///
/// @brief
///    记录运行时goose统计信息数据,并根据新的数据进行更新统计信息
///
class CGooseRTStatistics
{
public:
    CGooseRTStatistics(): m_nStNum(-1)
                        , m_nSqNum(-1)
    {
        memset(&m_stGooseFrameStatistics, 0, sizeof(m_stGooseFrameStatistics));
        memset(&m_stLastFrameTimeStamp, 0, sizeof(m_stLastFrameTimeStamp));
    }

    ~CGooseRTStatistics()
    {
    }

public:
    ///
    /// @brief
    ///    清空数据
    ///
    void ResetData()
    {
        m_nSqNum = -1;
        m_nSqNum = -1;
        memset(&m_stGooseFrameStatistics, 0, sizeof(m_stGooseFrameStatistics));
        memset(&m_stLastFrameTimeStamp, 0, sizeof(m_stLastFrameTimeStamp));
    }

    ///
    /// @brief
    ///    处理一帧数据的st，sq Number
    ///
    void ParsingStSqNum(int32_t nStNum, int32_t nSqNum)
    {
        if (m_nSqNum == -1 || m_nStNum == -1)
        {
            m_nStNum = nStNum;
            m_nSqNum  = nSqNum;
            return ;
        }
        if (m_nStNum == nStNum)
        {
            //sq丢帧
            if(nSqNum > m_nSqNum + 1)
            {
                m_stGooseFrameStatistics.m_nLostFrameCount++;
            }
            //错序
            else if (nSqNum !=0 && m_nSqNum > nSqNum)
            {
                m_stGooseFrameStatistics.m_nMisOrderFrameCount++;
            }
            //重发
            else if (nSqNum == m_nSqNum)
            {
                m_stGooseFrameStatistics.m_nRetryFrameCount++;
            }
        }
        //st丢帧
        else if (nStNum > m_nStNum + 1)
        {
            m_stGooseFrameStatistics.m_nStLostFrameCount++;
        }
        else if (nStNum == m_nSqNum + 1)
        {
            //st正常，此时要判断sq,如果此时sq不是从0或者1开始，则认为有丢帧
            if (!(nSqNum == 0 || nSqNum == 1))
            {
                m_stGooseFrameStatistics.m_nLostFrameCount++;
            }
        }
        //st错序
        else if (nStNum != 0 && m_nStNum > nStNum)
        {
            m_stGooseFrameStatistics.m_nStMisOrderFrameCount++;
        }

        //处理完毕，保存最后一次的st，sq数据
        m_nStNum = nStNum;
        m_nSqNum  = nSqNum;
    }

    ///
    /// @brief
    ///    处理一帧报文的goose变位时间，获取时钟同步/异常信息
    ///
    void ParsingGooseUtcTime(uint8_t nGooseUtcTimeQuality)
    {
        Iec61850TimeQuality tmQuality(nGooseUtcTimeQuality);
        //时钟故障
        if (tmQuality.IsClockFailure())
        {
            m_stGooseFrameStatistics.m_nTimeQualityFaultCount++;
        }
        //时钟未同步
        if (tmQuality.IsClockNotSync())
        {
            m_stGooseFrameStatistics.m_nTimeQualityNotSyncCount++;
        }
    }

    ///
    /// @brief
    ///    处理一帧的接收时间，判断通信中断，超时信息,统计总帧数信息
    ///
    void ParsingFrameTimeStamp(uint32_t nTTL, FrameTimeStamp& stFrameTimeStamp)
    {
        //统计总帧数
        m_stGooseFrameStatistics.m_nTotalFrameCount++;
        //统计ttl无效情况
        if (nTTL == 0)
        {
            m_stGooseFrameStatistics.m_nInvalidTTLCount++;
        }
        //统计通信超时/中断
        if (m_stLastFrameTimeStamp.ts_sec == 0 && m_stLastFrameTimeStamp.ts_usec == 0)
        {
            m_stLastFrameTimeStamp = stFrameTimeStamp;
            return;
        }

        int64_t ntmLast = m_stLastFrameTimeStamp.ts_sec;
        ntmLast = ntmLast * 1000000 + m_stLastFrameTimeStamp.ts_usec;
        int64_t ntmThis = stFrameTimeStamp.ts_sec;
        ntmThis = ntmThis * 1000000 + stFrameTimeStamp.ts_usec;
        int64_t nTTLms  = nTTL * 1000;
        int64_t nTimeDiff = ntmThis - ntmLast;
        if (nTimeDiff > nTTLms && nTimeDiff <= nTTLms * 2)
        {
            m_stGooseFrameStatistics.m_nLostDataGramCount++;
        }
        else if (nTimeDiff > nTTLms * 2)
        {
            m_stGooseFrameStatistics.m_nCommBreakCount++;
        }
        //处理完毕，保留最后一次时间戳
        m_stLastFrameTimeStamp = stFrameTimeStamp;
    }

    friend class CFrameRecv;

private:
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< 数据统计信息
    int32_t                             m_nStNum;                       ///< 记录goose控制块上一帧的stNumber，每次数据帧更新此数据
    int32_t                             m_nSqNum;                       ///< 记录goose控制块上一帧的sqNumber，每次数据帧更新此数据
    FrameTimeStamp                      m_stLastFrameTimeStamp;         ///< 上一数据帧的接收时间，用于判断超时/断线
};

///
/// @brief
///    定义和一个goose控制块相关的统计信息
///
typedef std::map<CBaseDetectItem*, CGooseRTStatistics>   MapDetectItemGooseFrameStatistics;