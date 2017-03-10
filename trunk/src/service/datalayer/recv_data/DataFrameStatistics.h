/// @file
///
/// @brief
///     ���屨��ͳ��������Ϣ
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.5.15
///
/// �޸���ʷ��
///

#pragma once
#include "DataRecvBase.h"

//���屣�汨����ɢ��ͳ�ƽ�������С
#define   MSG_DISPERSION_ARRAY_SIZE     11

enum MSG_DISP_PERIOD
{
    MSG_DISP_PERIOD_UNKOWN,                     ///< ������ļ��ʱ����ɢ��δ����
    MSG_DISP_PERIOD_0,                          ///< ������ļ��ʱ����ɢ��Ϊ0us
    MSG_DISP_PERIOD_1,                          ///< ������ļ��ʱ����ɢ��Ϊ1us
    MSG_DISP_PERIOD_2_TO_9,                     ///< ������ļ��ʱ����ɢ����2us��9us��
    MSG_DISP_PERIOD_10_TO_50_ASDU,              ///< ������ļ��ʱ����ɢ����10us��50*nAsdu -1us (nAsdu = 1ʱΪ49)
    MSG_DISP_PERIOD_50_ASDU_TO_250,             ///< ������ļ��ʱ����ɢ����(50*nAsdu)us��250us��
    MSG_DISP_PERIOD_OVER_250                    ///< ������ļ��ʱ����ɢ���ڳ���250us
};

//�������ֵ�쳣ͳ�ƽṹ
struct SmvFrameStatistics{
    uint32_t    m_nTotalFrameCount;                                             ///< �����ܼ���
    uint32_t    m_nLostFrameCount;                                              ///< �������
    uint32_t    m_nMisOrderFrameCount;                                          ///< �������
    uint32_t    m_nRetryFrameCount;                                             ///< �ط�����
    uint32_t    m_nLostSyncFrameCount;                                          ///< ʧ������
    uint32_t    m_nQualityInvalidFrameCount;                                    ///< Ʒ����Ч����
    uint32_t    m_nTestFrameCount;                                              ///< ���޼���
    uint32_t    m_nJitterFrameCount;                                            ///< ��������
    uint32_t    m_nTimeOutFrameCount;                                           ///< ���ͳ�ʱ����
};

//����潻���쳣ͳ����Ϣʹ�õ�������
class CDataFrameStatistics
{
public:
    CDataFrameStatistics(void);
    ~CDataFrameStatistics(void);

public:
    ///
    /// @brief
    ///    ��ȡ���ƿ���ɢ����Ϣ
    ///
    float GetDispPercent(MSG_DISP_PERIOD enMsgDispPeriod);

    ///
    /// @brief
    ///    ��ȡ�����ܼ���
    ///
    uint32_t GetTotalFrameCount();

    ///
    /// @brief
    ///    ��ȡ�������
    ///
    uint32_t GetLostFrameCount();

    ///
    /// @brief
    ///    ��ȡ�������
    ///
    uint32_t GetMisOrderFrameCount();

    ///
    /// @brief
    ///    ��ȡ�ط����ݼ���
    ///
    uint32_t GetRetryFrameCount();

    ///
    /// @brief
    ///    ��ȡʧ��ͳ�Ƽ���
    ///
    uint32_t GetLostSyncFrameCount();

    ///
    /// @brief
    ///    ��ȡƷ����Ч����֡����
    ///
    uint32_t GetQualityInvalidFrameCount();

    ///
    /// @brief
    ///    ��ȡ��������֡��
    ///
    uint32_t GetTestFrameCount();

    ///
    /// @brief
    ///    ��ȡ��������
    ///
    uint32_t GetJitterFrameCount();

    ///
    /// @brief
    ///    ��ȡ���ͳ�ʱ����
    ///
    uint32_t GetTimeOutFrameCount();

    ///
    /// @brief
    ///    һ�λ�ȡ���е�ͳ�ƽ��
    ///
    SmvFrameStatistics GetAllFrameStatistics();

    ///
    /// @brief
    ///    �������
    ///
    void ClearData();

private:
    friend class CFrameRecv;
    friend class CPcapAnalysis;
    ///
    /// @note
    ///     ����Ԫ�ش��������
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
    float               m_fMsgDispersion[MSG_DISPERSION_ARRAY_SIZE];       ///< ������ɢ����Ϣ
    SmvFrameStatistics  m_stSmvFrameStatistics;                            ///< һ�����ƿ�ı���ͳ����Ϣ
};

///
/// @brief
///    ��¼һ�����ƿ�ͳ����Ϣ����Ҫ�����ݣ����й����б�����ƿ�ĵ�ǰͳ����Ϣ
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
    ///    ��ȡ���һ֡��ʱ���
    ///
    FrameTimeStamp GetLastFrameTimeStamp()
    {
        return m_stLastFrameTimeStamp;
    }

    ///
    /// @brief
    ///    ��ȡ���һ֡�Ĳ�����������ֵ
    ///
    uint32_t GetLastSampleCountIndex()
    {
        return m_nLastSampleCountIndex;
    }

    ///
    /// @brief
    ///    ��������
    ///
    void ResetData()
    {
        memset(&m_stLastFrameTimeStamp, 0, sizeof(m_stLastFrameTimeStamp));
        memset(&m_stSmvFrameStatistics, 0, sizeof(m_stSmvFrameStatistics));
        m_nLastSampleCountIndex = -1;
    }

    ///
    /// @brief
    ///    ����һ��ʱ������з���,һ��ʱ��������ʾ��һ���µ�����֡
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
    ///    ����һ���µĲ�����������ֵ���з���
    ///
    void ParsingSmpIndex(int32_t nSmpIndex)
    {
        if (m_nLastSampleCountIndex == -1)
        {
            m_nLastSampleCountIndex = nSmpIndex;
            return;
        }
        if (nSmpIndex == m_nLastSampleCountIndex + 1)
        //����
        {
        }
        //�ط�
        else if (nSmpIndex == m_nLastSampleCountIndex)
        {
            m_stSmvFrameStatistics.m_nRetryFrameCount++;
        }
        //��֡
        else if (nSmpIndex > m_nLastSampleCountIndex + 1 )
        {
            m_stSmvFrameStatistics.m_nLostFrameCount++;
        }
        //����
        else if (nSmpIndex < m_nLastSampleCountIndex && nSmpIndex != 0)
        {
            m_stSmvFrameStatistics.m_nMisOrderFrameCount++;
        }
        m_nLastSampleCountIndex = nSmpIndex;
    }

    ///
    /// @brief
    ///    ������ƿ�״̬�֣�ͨ��Ʒ����Ϣ
    ///
    void ParsingStatusQuality(CSmvCtrlBlkQuality& smvCtrlBlkQuality)
    {
        //ʧ��
        if (!smvCtrlBlkQuality.IsMuSync())
        {
            m_stSmvFrameStatistics.m_nLostSyncFrameCount++;
        }

        //����
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
            //Ʒ����Ч����
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
    ///    ��������ʱ���ʱ���ж�ʱ���Ƿ񶶶���ʱ������ƫ�������������10us��
    ///
    bool _IsTimeStampJitter(CBaseDetectItem* pDetectItem, const FrameTimeStamp& tm1, const FrameTimeStamp& tm2);

    friend class CFrameRecv;
private:
    int32_t             m_nLastSampleCountIndex;                            ///< �ϴ�����֡�Ĳ���������
    FrameTimeStamp      m_stLastFrameTimeStamp;                             ///< ��һ����֡��ʱ�����ͳ�ƶ�����Ϣ
    SmvFrameStatistics  m_stSmvFrameStatistics;                             ///< ���ƿ��Ӧ��ͳ����Ϣ
};

///
/// @brief
///    �����һ������ֵ���ƿ���ص�ͳ����Ϣ
///
typedef std::map<CBaseDetectItem*, CSmvFrameStatistics>   MapDetectItemSmvFrameStatistics;
