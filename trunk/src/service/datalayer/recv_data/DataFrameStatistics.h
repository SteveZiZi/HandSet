/// @file
///
/// @brief
///     定义报文统计数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.15
///
/// 修改历史：
///

#pragma once
#include "DataRecvBase.h"

//定义保存报文离散度统计结果数组大小
#define   MSG_DISPERSION_ARRAY_SIZE     11

enum MSG_DISP_PERIOD
{
    MSG_DISP_PERIOD_UNKOWN,                     ///< 采样点的间隔时间离散度未定义
    MSG_DISP_PERIOD_0,                          ///< 采样点的间隔时间离散度为0us
    MSG_DISP_PERIOD_1,                          ///< 采样点的间隔时间离散度为1us
    MSG_DISP_PERIOD_2_TO_9,                     ///< 采样点的间隔时间离散度在2us到9us内
    MSG_DISP_PERIOD_10_TO_50_ASDU,              ///< 采样点的间隔时间离散度在10us到50*nAsdu -1us (nAsdu = 1时为49)
    MSG_DISP_PERIOD_50_ASDU_TO_250,             ///< 采样点的间隔时间离散度在(50*nAsdu)us到250us内
    MSG_DISP_PERIOD_OVER_250                    ///< 采样点的间隔时间离散度在超过250us
};

//定义采样值异常统计结构
struct SmvFrameStatistics{
    uint32_t    m_nTotalFrameCount;                                             ///< 报文总计数
    uint32_t    m_nLostFrameCount;                                              ///< 丢点计数
    uint32_t    m_nMisOrderFrameCount;                                          ///< 错序计数
    uint32_t    m_nRetryFrameCount;                                             ///< 重发计数
    uint32_t    m_nLostSyncFrameCount;                                          ///< 失步计数
    uint32_t    m_nQualityInvalidFrameCount;                                    ///< 品质无效计数
    uint32_t    m_nTestFrameCount;                                              ///< 检修计数
    uint32_t    m_nJitterFrameCount;                                            ///< 抖动计数
    uint32_t    m_nTimeOutFrameCount;                                           ///< 发送超时计数
};

//与界面交互异常统计信息使用的数据类
class CDataFrameStatistics
{
public:
    CDataFrameStatistics(void);
    ~CDataFrameStatistics(void);

public:
    ///
    /// @brief
    ///    获取控制块离散度信息
    ///
    float GetDispPercent(MSG_DISP_PERIOD enMsgDispPeriod);

    ///
    /// @brief
    ///    获取报文总计数
    ///
    uint32_t GetTotalFrameCount();

    ///
    /// @brief
    ///    获取丢点计数
    ///
    uint32_t GetLostFrameCount();

    ///
    /// @brief
    ///    获取错序计数
    ///
    uint32_t GetMisOrderFrameCount();

    ///
    /// @brief
    ///    获取重发数据计数
    ///
    uint32_t GetRetryFrameCount();

    ///
    /// @brief
    ///    获取失步统计计数
    ///
    uint32_t GetLostSyncFrameCount();

    ///
    /// @brief
    ///    获取品质无效数据帧计数
    ///
    uint32_t GetQualityInvalidFrameCount();

    ///
    /// @brief
    ///    获取检修数据帧数
    ///
    uint32_t GetTestFrameCount();

    ///
    /// @brief
    ///    获取抖动计数
    ///
    uint32_t GetJitterFrameCount();

    ///
    /// @brief
    ///    获取发送超时计数
    ///
    uint32_t GetTimeOutFrameCount();

    ///
    /// @brief
    ///    一次获取所有的统计结果
    ///
    SmvFrameStatistics GetAllFrameStatistics();

    ///
    /// @brief
    ///    清空数据
    ///
    void ClearData();

private:
    friend class CFrameRecv;
    friend class CPcapAnalysis;
    ///
    /// @note
    ///     数组元素代表的意义
    ///m_fMsgDispersion[0]:<=-250
    ///m_fMsgDispersion[1]:-249~-50
    ///m_fMsgDispersion[2]:-49~-10
    ///m_fMsgDispersion[3]:-9~-2
    ///m_fMsgDispersion[4]:-1
    ///m_fMsgDispersion[5]:0
    ///m_fMsgDispersion[6]:1
    ///m_fMsgDispersion[7]:2-9
    ///m_fMsgDispersion[8]:10-49
    ///m_fMsgDispersion[9]:50-249
    ///m_fMsgDispersion[10]:>=250
    float               m_fMsgDispersion[MSG_DISPERSION_ARRAY_SIZE];       ///< 报文离散度信息
    SmvFrameStatistics  m_stSmvFrameStatistics;                            ///< 一个控制块的报文统计信息
};

///
/// @brief
///    记录一个控制块统计信息所需要的数据，运行过程中保存控制块的当前统计信息
///
class CSmvFrameStatistics
{
public:
    CSmvFrameStatistics()
    {
        ResetData();
    }
    ~CSmvFrameStatistics(){}

public:
    ///
    /// @brief
    ///    获取最后一帧的时间戳
    ///
    FrameTimeStamp GetLastFrameTimeStamp()
    {
        return m_stLastFrameTimeStamp;
    }

    ///
    /// @brief
    ///    获取最后一帧的采样计数器数值
    ///
    uint32_t GetLastSampleCountIndex()
    {
        return m_nLastSampleCountIndex;
    }

    ///
    /// @brief
    ///    重置数据
    ///
    void ResetData()
    {
        memset(&m_stLastFrameTimeStamp, 0, sizeof(m_stLastFrameTimeStamp));
        memset(&m_stSmvFrameStatistics, 0, sizeof(m_stSmvFrameStatistics));
        m_nLastSampleCountIndex = -1;
    }

    ///
    /// @brief
    ///    传入一个时间戳进行分析,一个时间戳处理表示有一个新的数据帧
    ///
    void ParsingFrameTimeStamp(CBaseDetectItem* pDetectItem, FrameTimeStamp& stFrameTimeStamp)
    {
        if (m_stSmvFrameStatistics.m_nTotalFrameCount != 0)
        {
            if(_IsTimeStampJitter(pDetectItem, m_stLastFrameTimeStamp, stFrameTimeStamp))
            {
                m_stSmvFrameStatistics.m_nJitterFrameCount++;
            }
        }
        m_stSmvFrameStatistics.m_nTotalFrameCount++;
        m_stLastFrameTimeStamp = stFrameTimeStamp;
    }

    ///
    /// @brief
    ///    传进一个新的采样计数器数值进行分析
    ///
    void ParsingSmpIndex(int32_t nSmpIndex)
    {
        if (m_nLastSampleCountIndex == -1)
        {
            m_nLastSampleCountIndex = nSmpIndex;
            return;
        }
        if (nSmpIndex == m_nLastSampleCountIndex + 1)
        //正常
        {
        }
        //重发
        else if (nSmpIndex == m_nLastSampleCountIndex)
        {
            m_stSmvFrameStatistics.m_nRetryFrameCount++;
        }
        //丢帧
        else if (nSmpIndex > m_nLastSampleCountIndex + 1 )
        {
            m_stSmvFrameStatistics.m_nLostFrameCount++;
        }
        //错序
        else if (nSmpIndex < m_nLastSampleCountIndex && nSmpIndex != 0)
        {
            m_stSmvFrameStatistics.m_nMisOrderFrameCount++;
        }
        m_nLastSampleCountIndex = nSmpIndex;
    }

    ///
    /// @brief
    ///    处理控制块状态字，通道品质信息
    ///
    void ParsingStatusQuality(CSmvCtrlBlkQuality& smvCtrlBlkQuality)
    {
        //失步
        if (!smvCtrlBlkQuality.IsMuSync())
        {
            m_stSmvFrameStatistics.m_nLostSyncFrameCount++;
        }

        //检修
        if (smvCtrlBlkQuality.GetCtrlBlk()->enFrameType == FRAME_TYPE_NET_9_2)
        {
            for (uint32_t i = 0; i < smvCtrlBlkQuality.GetCtrlBlk()->nChannelCount; ++i)
            {
                if (smvCtrlBlkQuality.IsChannelTest(i))
                {
                    m_stSmvFrameStatistics.m_nTestFrameCount++;
                    break;
                }
            }
            //品质无效计数
            for (uint32_t i = 0; i < smvCtrlBlkQuality.GetCtrlBlk()->nChannelCount; ++i)
            {
                if (!smvCtrlBlkQuality.IsChannelValid(i))
                {
                    m_stSmvFrameStatistics.m_nQualityInvalidFrameCount++;
                    break;
                }
            }
        }
        else
        {
            if (smvCtrlBlkQuality.IsChannelTest())
            {
                m_stSmvFrameStatistics.m_nTestFrameCount++;
            }
        }
    }

private:
    ///
    /// @brief
    ///    计算两个时间戳时间差，判断时间是否抖动（时间戳间隔偏离正常间隔超过10us）
    ///
    bool _IsTimeStampJitter(CBaseDetectItem* pDetectItem, const FrameTimeStamp& tm1, const FrameTimeStamp& tm2);

    friend class CFrameRecv;
private:
    int32_t             m_nLastSampleCountIndex;                            ///< 上次数据帧的采样计数器
    FrameTimeStamp      m_stLastFrameTimeStamp;                             ///< 上一数据帧的时间戳，统计抖动信息
    SmvFrameStatistics  m_stSmvFrameStatistics;                             ///< 控制块对应的统计信息
};

///
/// @brief
///    定义和一个采样值控制块相关的统计信息
///
typedef std::map<CBaseDetectItem*, CSmvFrameStatistics>   MapDetectItemSmvFrameStatistics;
