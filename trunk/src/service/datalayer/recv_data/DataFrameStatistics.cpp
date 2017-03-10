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

#include "StdAfx.h"
#include "DataFrameStatistics.h"

CDataFrameStatistics::CDataFrameStatistics(void)
{
    memset(&m_stSmvFrameStatistics, 0, sizeof(m_stSmvFrameStatistics));
    memset(m_fMsgDispersion, 0, sizeof(m_fMsgDispersion));
}

CDataFrameStatistics::~CDataFrameStatistics(void)
{
}

///
/// @brief
///    ��ȡ���ƿ���ɢ����Ϣ
///
float CDataFrameStatistics::GetDispPercent( MSG_DISP_PERIOD enMsgDispPeriod )
{
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
    float fResult = 0.0f;
    switch(enMsgDispPeriod)
    {
    case MSG_DISP_PERIOD_0:
        fResult = m_fMsgDispersion[5];
        break;
    case MSG_DISP_PERIOD_1:
        fResult = m_fMsgDispersion[4] + m_fMsgDispersion[6];
        break;
    case MSG_DISP_PERIOD_2_TO_9:
        fResult = m_fMsgDispersion[3] + m_fMsgDispersion[7];
        break;
    case MSG_DISP_PERIOD_10_TO_50_ASDU:
        fResult = m_fMsgDispersion[2] + m_fMsgDispersion[8];
        break;
    case MSG_DISP_PERIOD_50_ASDU_TO_250:
        fResult = m_fMsgDispersion[1] + m_fMsgDispersion[9];
        break;
    case MSG_DISP_PERIOD_OVER_250:
        fResult = m_fMsgDispersion[0] + m_fMsgDispersion[10];
        break;
    }
    return fResult;
}

///
/// @brief
///    ��ȡ�����ܼ���
///
uint32_t CDataFrameStatistics::GetTotalFrameCount()
{
    return m_stSmvFrameStatistics.m_nTotalFrameCount;
}

///
/// @brief
///    ��ȡ�������
///
uint32_t CDataFrameStatistics::GetLostFrameCount()
{
    return m_stSmvFrameStatistics.m_nLostFrameCount;
}

///
/// @brief
///    ��ȡ�������
///
uint32_t CDataFrameStatistics::GetMisOrderFrameCount()
{
    return m_stSmvFrameStatistics.m_nMisOrderFrameCount;
}

///
/// @brief
///    ��ȡ���ͳ�ʱ����
///
uint32_t CDataFrameStatistics::GetTimeOutFrameCount()
{
    return m_stSmvFrameStatistics.m_nTimeOutFrameCount;
}

///
/// @brief
///    ��ȡ��������
///
uint32_t CDataFrameStatistics::GetJitterFrameCount()
{
    return m_stSmvFrameStatistics.m_nJitterFrameCount;
}

void CDataFrameStatistics::ClearData()
{
    memset(&m_stSmvFrameStatistics, 0, sizeof(m_stSmvFrameStatistics));
    memset(m_fMsgDispersion, 0, sizeof(m_fMsgDispersion));
 }

///
/// @brief
///    ��ȡ�ط����ݼ���
///
uint32_t CDataFrameStatistics::GetRetryFrameCount()
{
    return m_stSmvFrameStatistics.m_nRetryFrameCount;
}

///
/// @brief
///    ��ȡʧ��ͳ�Ƽ���
///
uint32_t CDataFrameStatistics::GetLostSyncFrameCount()
{
    return m_stSmvFrameStatistics.m_nLostSyncFrameCount;
}

///
/// @brief
///    ��ȡƷ����Ч����֡����
///
uint32_t CDataFrameStatistics::GetQualityInvalidFrameCount()
{
    return m_stSmvFrameStatistics.m_nQualityInvalidFrameCount;
}

///
/// @brief
///    ��ȡ��������֡��
///
uint32_t CDataFrameStatistics::GetTestFrameCount()
{
    return m_stSmvFrameStatistics.m_nTestFrameCount;
}

///
/// @brief
///    һ�λ�ȡ���е�ͳ�ƽ��
///
SmvFrameStatistics CDataFrameStatistics::GetAllFrameStatistics()
{
    return m_stSmvFrameStatistics;
}

///
/// @brief
///    ��������ʱ���ʱ���ж�ʱ���Ƿ񶶶���ʱ������ƫ�������������10us��
///
bool CSmvFrameStatistics::_IsTimeStampJitter( CBaseDetectItem* pDetectItem, const FrameTimeStamp& tm1, const FrameTimeStamp& tm2 )
{
    //����pDetectItem������������ʱ����
    uint32_t nNormalTime = (pDetectItem->nAsdu * 1000000)/pDetectItem->nSampleRate;

    int64_t ntm1 = tm1.ts_sec;
    ntm1 = ntm1 * 1000000 + tm1.ts_usec;
    int64_t ntm2 = tm2.ts_sec;
    ntm2 = ntm2 * 1000000 + tm2.ts_usec;

    int64_t ndiff = _abs64(ntm2 - ntm1);
#ifdef SU_FEI
    if (ndiff > nNormalTime + 10)
    {
        return true;
    }
#else
	if ((ndiff > nNormalTime + 10) || (ndiff < nNormalTime - 10))
	{
		return true;
	}

#endif
    return false;
}
