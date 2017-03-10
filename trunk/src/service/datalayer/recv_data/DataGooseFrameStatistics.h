/// @file
///
/// @brief
///     ����goose����ͳ����Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.7
///
/// �޸���ʷ��
///

#pragma once

#include "src\utils\MessageCodec\include\iec61850_data_struct.h"
#include "src\service\datalayer\recv_data\DataRecvBase.h"

//����Goose�쳣ͳ�ƽṹ
struct GooseFrameStatistics{
    uint32_t    m_nTotalFrameCount;                                             ///< �����ܼ���
    uint32_t    m_nLostFrameCount;                                              ///< �ӿ�ʼͳ�Ƶ���ǰ����¼SqNum��֡�Ĵ�����
    uint32_t    m_nMisOrderFrameCount;                                          ///< �ӿ�ʼͳ�Ƶ���ǰ����¼SqNum����Ĵ�����
    uint32_t    m_nRetryFrameCount;                                             ///< �ӿ�ʼͳ�Ƶ���ǰ����¼SqNum�ط��Ĵ�����
    uint32_t    m_nStLostFrameCount;                                            ///< �ӿ�ʼͳ�Ƶ���ǰ����¼StNum��֡�Ĵ�����
    uint32_t    m_nStMisOrderFrameCount;                                        ///< �ӿ�ʼͳ�Ƶ���ǰ����¼StNum����Ĵ�����
    uint32_t    m_nLostDataGramCount;                                           ///< �ӿ�ʼͳ�Ƶ���ǰ�����Ķ�ʧ�Ĵ�������֡���ĵļ������ TimeAllowedToLive=T0 *2 �����ж�Ϊһ�α��Ķ�ʧ��
    uint32_t    m_nCommBreakCount;                                              ///< �ӿ�ʼͳ�Ƶ���ǰ��ͨ���жϵĴ�������֡���ĵļ������ TimeAllowedToLive * 2 �����ж�Ϊһ��ͨ���жϡ�
    uint32_t    m_nInvalidTTLCount;                                             ///< �ӿ�ʼͳ�Ƶ���ǰ��ʱ��ʧЧ�Ĵ����������е�TTL(TimeAllowedToLive) = 0�����ж�Ϊһ�δ��ʱ����Ч��
    uint32_t    m_nTimeQualityNotSyncCount;                                     ///< �ӿ�ʼͳ�Ƶ���ǰ��ʱ��Ʒ��ʱ��δͬ���Ĵ����������е�Event TimeStamp��λʱ����ʱ���������ֶ�ʱ��δͬ����־Ϊ1ʱ�ж�Ϊһ��ʱ��δͬ����
    uint32_t    m_nTimeQualityFaultCount;                                            ///< �ӿ�ʼͳ�Ƶ���ǰ��ʱ��Ʒ��ʱ�ӹ��ϵĴ����������е�Event TimeStamp��λʱ����ʱ���������ֶ�ʱ�ӹ��ϱ�־Ϊ1ʱ�ж�Ϊһ��ʱ��δͬ����
};

//����潻��goose����ͳ����
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
    ///    ��ȡͳ��ͳ�Ƶ����н����
    ///
    GooseFrameStatistics GetGooseFrameStatistics()
    {
        return m_stGooseFrameStatistics;
    }

    ///
    /// @brief
    ///    ��ȡ�ܱ�������
    ///
    uint32_t GetTotalFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nTotalFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡsqNum��֡֡��
    ///
    uint32_t GetLostFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nLostFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡsqNum����֡��
    ///
    uint32_t GetMisOrderFrameCount()
    {
        return m_stGooseFrameStatistics.m_nMisOrderFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡsqNum�ط�֡��
    ///
    uint32_t GetRetryFrameCount()
    {
        return m_stGooseFrameStatistics.m_nRetryFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡstNum��֡֡��
    ///
    uint32_t GetStLostFrameCounts()
    {
        return m_stGooseFrameStatistics.m_nStLostFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡstNum����֡��
    ///
    uint32_t GetStMisOrderFrameCount()
    {
        return m_stGooseFrameStatistics.m_nStMisOrderFrameCount;
    }

    ///
    /// @brief
    ///    ��ȡ��ʧ���Ĵ���
    ///
    uint32_t GetLostDataGramCount()
    {
        return m_stGooseFrameStatistics.m_nLostDataGramCount;
    }

    ///
    /// @brief
    ///    ��ȡͨ���жϴ���
    ///
    uint32_t GetCommBreakCount()
    {
        return m_stGooseFrameStatistics.m_nCommBreakCount;
    }

    ///
    /// @brief
    ///    ��ȡ����ttlʱ��ʧЧ����
    ///
    uint32_t GetInvalidTTLCount()
    {
        return m_stGooseFrameStatistics.m_nInvalidTTLCount;
    }

    ///
    /// @brief
    ///    ��ȡʱ��δͬ������
    ///
    uint32_t GetTimeQualityNotSyncCount()
    {
        return m_stGooseFrameStatistics.m_nTimeQualityNotSyncCount;
    }

    ///
    /// @brief
    ///    ��ȡʱ�ӹ��ϴ���
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
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< ����ͳ�ƽṹ
};

///
/// @brief
///    ��¼����ʱgooseͳ����Ϣ����,�������µ����ݽ��и���ͳ����Ϣ
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
    ///    �������
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
    ///    ����һ֡���ݵ�st��sq Number
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
            //sq��֡
            if(nSqNum > m_nSqNum + 1)
            {
                m_stGooseFrameStatistics.m_nLostFrameCount++;
            }
            //����
            else if (nSqNum !=0 && m_nSqNum > nSqNum)
            {
                m_stGooseFrameStatistics.m_nMisOrderFrameCount++;
            }
            //�ط�
            else if (nSqNum == m_nSqNum)
            {
                m_stGooseFrameStatistics.m_nRetryFrameCount++;
            }
        }
        //st��֡
        else if (nStNum > m_nStNum + 1)
        {
            m_stGooseFrameStatistics.m_nStLostFrameCount++;
        }
        else if (nStNum == m_nSqNum + 1)
        {
            //st��������ʱҪ�ж�sq,�����ʱsq���Ǵ�0����1��ʼ������Ϊ�ж�֡
            if (!(nSqNum == 0 || nSqNum == 1))
            {
                m_stGooseFrameStatistics.m_nLostFrameCount++;
            }
        }
        //st����
        else if (nStNum != 0 && m_nStNum > nStNum)
        {
            m_stGooseFrameStatistics.m_nStMisOrderFrameCount++;
        }

        //������ϣ��������һ�ε�st��sq����
        m_nStNum = nStNum;
        m_nSqNum  = nSqNum;
    }

    ///
    /// @brief
    ///    ����һ֡���ĵ�goose��λʱ�䣬��ȡʱ��ͬ��/�쳣��Ϣ
    ///
    void ParsingGooseUtcTime(uint8_t nGooseUtcTimeQuality)
    {
        Iec61850TimeQuality tmQuality(nGooseUtcTimeQuality);
        //ʱ�ӹ���
        if (tmQuality.IsClockFailure())
        {
            m_stGooseFrameStatistics.m_nTimeQualityFaultCount++;
        }
        //ʱ��δͬ��
        if (tmQuality.IsClockNotSync())
        {
            m_stGooseFrameStatistics.m_nTimeQualityNotSyncCount++;
        }
    }

    ///
    /// @brief
    ///    ����һ֡�Ľ���ʱ�䣬�ж�ͨ���жϣ���ʱ��Ϣ,ͳ����֡����Ϣ
    ///
    void ParsingFrameTimeStamp(uint32_t nTTL, FrameTimeStamp& stFrameTimeStamp)
    {
        //ͳ����֡��
        m_stGooseFrameStatistics.m_nTotalFrameCount++;
        //ͳ��ttl��Ч���
        if (nTTL == 0)
        {
            m_stGooseFrameStatistics.m_nInvalidTTLCount++;
        }
        //ͳ��ͨ�ų�ʱ/�ж�
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
        //������ϣ��������һ��ʱ���
        m_stLastFrameTimeStamp = stFrameTimeStamp;
    }

    friend class CFrameRecv;

private:
    GooseFrameStatistics                m_stGooseFrameStatistics;       ///< ����ͳ����Ϣ
    int32_t                             m_nStNum;                       ///< ��¼goose���ƿ���һ֡��stNumber��ÿ������֡���´�����
    int32_t                             m_nSqNum;                       ///< ��¼goose���ƿ���һ֡��sqNumber��ÿ������֡���´�����
    FrameTimeStamp                      m_stLastFrameTimeStamp;         ///< ��һ����֡�Ľ���ʱ�䣬�����жϳ�ʱ/����
};

///
/// @brief
///    �����һ��goose���ƿ���ص�ͳ����Ϣ
///
typedef std::map<CBaseDetectItem*, CGooseRTStatistics>   MapDetectItemGooseFrameStatistics;