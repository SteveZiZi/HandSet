/// @file
///     SampleInterpSync.cpp
///
/// @brief
///     ������ֵͬ��ģ��ʵ��
///
/// @note
///
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.10.21
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.10.22     |1.0.1.1022 |AddASample()�У���Ӷ��ز�����ʼʱ�̵��жϣ���ֵ�ľ���ֵ����������ڣ������¿�ʼͬ��|
///    |2013.10.22     |1.0.1.1022 |AddASample()�У��޸Ķ����õ��ɾ���жϣ�����ʱ��С�ڵ��ڵ�����һ�����ִ����´��ز����Ŀ�ʼʱ�̵��ٽ����Ϊɾ���ֽ�|
///    |2013.10.23     |1.0.1.1023 |AddASample()�У�����������������ʱ��ʱû�����������еĹ�ʱ�������������Bug|
///    |2013.11.05     |1.0.1.1105 |�޸Ĳ��������ݵĳ�ʼ��������                             |
///    |2013.11.05     |1.0.1.1105 |�޸��ز�����ʼ���и����ƿ��ѽ��ղ����������ʱ�̵Ļ�ȡ   |
///    |2013.11.05     |1.0.1.1105 |�޸����¿�ʼ����ͬ�����ж��������ѽ��ղ���������1���ڸ�Ϊ12����|
///
#include "stdafx.h"
#include "include\SampleInterpSync.h"
#include "interp.h"

///
/// @brief
///     ԭ���β��������ݵĳ�ʼ��������
///
#define SAMPLES_CACHE_CYCLES_NUM 20

CSampleInterpSync::CSampleInterpSync()
: m_nResmpsNumPerCycle(80)
, m_dResmpInterval(250.0)
, m_nSrcSmpsNumPerCycle(80)
, m_dSrcSmpInterval(250.0)
, m_nSyncCtrlNum(2)
, m_bDelayCompensateEnable(false)
, m_bIsSampleSync(false)
, m_bStartSync(false)
, m_pInterpFunc(Interp_Linear)
{
	m_stTimeThreshold.ts_sec = m_stTimeThreshold.ts_usec = 0;
	memset(m_nCtrlRatedDelay,0,sizeof(uint32_t)*MAX_SYNC_SMVCTRLS_NUM);
	memset(m_nCtrlReceiveSmpsNum,0,MAX_SYNC_SMVCTRLS_NUM*sizeof(uint32_t));

	m_vRelativeTime.reserve(m_nSrcSmpsNumPerCycle+3);
	for ( uint32_t i = 0; i < m_nSyncCtrlNum; i++ )
	{
		m_vSmpsTimes[i].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		for ( int j = 0; j < MAX_SYNC_CHANNELS_NUM_PER_CTRL; j++ )
		{
			m_vSmpsVals[i*MAX_SYNC_CHANNELS_NUM_PER_CTRL+j].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		}
		m_vCtrlChannelsIdx[i].reserve(MAX_SYNC_CHANNELS_NUM_PER_CTRL);
	}
}

CSampleInterpSync::~CSampleInterpSync()
{

}

///
/// @brief
///     ������ʹ�õĲ�ֵ�㷨�����ͣ�Ĭ��Ϊ���Բ�ֵ
///
/// @param[in]
///     eInterpType - ��ֵ�㷨������
///
void CSampleInterpSync::SetInterpType(INTERP_TYPE eInterpType /* = INTERP_TYPE_LINEAR */)
{
	switch ( eInterpType )
	{
	case INTERP_TYPE_LINEAR:
		m_pInterpFunc = Interp_Linear;
		break;
	case INTERP_TYPE_QUADRATE:
		m_pInterpFunc = Interp_Quadrate;
		break;
	case INTERP_TYPE_THRICE:
		m_pInterpFunc = Interp_Thrice;
		break;
	case INTERP_TYPE_SPLINE:
		m_pInterpFunc = Interp_Spline;
		break;
	default:
		m_pInterpFunc = Interp_Linear;
	}
}

///
/// @brief
///     ������Ҫ���в�ֵͬ���Ŀ��ƿ�����Ĭ��Ϊ2
///
/// @param[in]
///     nCtrlNum - ��Ҫ���в�ֵͬ���Ŀ��ƿ���
///
void CSampleInterpSync::SetSyncCtrlNum(uint32_t nCtrlNum /* = 2 */)
{
	if ( nCtrlNum > MAX_SYNC_SMVCTRLS_NUM )
		nCtrlNum = MAX_SYNC_SMVCTRLS_NUM;

	for ( uint32_t i = m_nSyncCtrlNum; i < nCtrlNum; i++ )
	{
		m_vSmpsTimes[i].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		for ( int j = 0; j < MAX_SYNC_CHANNELS_NUM_PER_CTRL; j++ )
		{
			m_vSmpsVals[i*MAX_SYNC_CHANNELS_NUM_PER_CTRL+j].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		}
		m_vCtrlChannelsIdx[i].reserve(MAX_SYNC_CHANNELS_NUM_PER_CTRL);
	}

	m_nSyncCtrlNum = nCtrlNum;
}

///
/// @brief
///     ������Ҫ���в�ֵͬ����ͨ��
///
/// @param[in]
///     ctrlIdx     - ͨ�����ڵĿ��ƿ�������ţ���0��ʼ
/// @param[in]
///     pChannelIdx - ����ͨ�������ţ���0��ʼ��������
/// @param[in]
///     channelNum  - ͨ����Ŀ
///
void CSampleInterpSync::SetSyncChannelIdx(uint32_t ctrlIdx, uint32_t *pChannelIdx, uint32_t channelNum)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;

	uint32_t num = channelNum;
	if ( num > MAX_SYNC_CHANNELS_NUM_PER_CTRL )
		num = MAX_SYNC_CHANNELS_NUM_PER_CTRL;

	m_vCtrlChannelsIdx[ctrlIdx].clear();
	for ( uint32_t i = 0; i < num; i++ )
	{
		m_vCtrlChannelsIdx[ctrlIdx].push_back(pChannelIdx[i]);
	}
}

///
/// @brief
///     �����ز�������
///
/// @param[in]
///     nSignalFrequency  - ԭʼ���Σ��źţ���Ƶ��
/// @param[in]
///     nSampleFrequency  - ԭ���εĲ���Ƶ��
/// @param[in]
///     nReampleFrequency - �ز���Ƶ��
///
void CSampleInterpSync::SetResampleConfig(uint32_t nSignalFrequency, uint32_t nSampleFrequency, uint32_t nReampleFrequency)
{
	m_nSrcSmpsNumPerCycle = nSampleFrequency/nSignalFrequency;
	m_dSrcSmpInterval     = 1000000.0/nSampleFrequency;
	m_nResmpsNumPerCycle  = nReampleFrequency/nSignalFrequency;
	m_dResmpInterval      = 1000000.0/nReampleFrequency;
	m_bStartSync = false;

	m_vRelativeTime.reserve(m_nSrcSmpsNumPerCycle+3);
	for ( uint32_t i = 0; i < m_nSyncCtrlNum; i++ )
	{
		m_vSmpsTimes[i].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		for ( int j = 0; j < MAX_SYNC_CHANNELS_NUM_PER_CTRL; j++ )
		{
			m_vSmpsVals[i*MAX_SYNC_CHANNELS_NUM_PER_CTRL+j].reserve(m_nSrcSmpsNumPerCycle*SAMPLES_CACHE_CYCLES_NUM);
		}
		m_vCtrlChannelsIdx[i].reserve(MAX_SYNC_CHANNELS_NUM_PER_CTRL);
	}

	ResetSampleSync();
}

///
/// @brief
///     ���ÿ��ƿ�Ķ��ʱ
///
/// @param[in]
///     ctrlIdx  - ���ƿ�������ţ���0��ʼ��
/// @param[in]
///     nDelay   - ���ƿ�Ķ��ʱʱ��
///
void CSampleInterpSync::SetRatedDelay(uint32_t ctrlIdx, uint32_t nDelay)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;
	m_nCtrlRatedDelay[ctrlIdx] = nDelay;
}

///
/// @brief
///     ������ʱ����ʹ�ܣ�Ĭ��Ϊ��ֹ
///
/// @param[in]
///     bEnable - ��ʱ����ʹ�ܣ�trueΪʹ�ܣ�falseΪ��ֹ
///
void CSampleInterpSync::SetDelayCompensateEnable(bool bEnable /* = false */)
{
	m_bDelayCompensateEnable = bEnable;
}

///
/// @brief
///     ���ò���ͬ��
///
void CSampleInterpSync::ResetSampleSync()
{
	m_bIsSampleSync = false;
	m_bStartSync  = false;

	for ( int i=0; i<MAX_SYNC_SMVCTRLS_NUM; i++ )
	{
		m_vSmpsTimes[i].clear();
	}
	
	for ( int i=0; i<MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL; i++ )
	{
		m_vSmpsVals[i].clear();
	}

	memset(m_stCtrlResmpTime,0,MAX_SYNC_SMVCTRLS_NUM*sizeof(FrameTimeStamp));
	memset(m_nCtrlReceiveSmpsNum,0,MAX_SYNC_SMVCTRLS_NUM*sizeof(uint32_t));
}

///
/// @brief
///     ���һ����������Ϣ
///
/// @param[in]
///     ctrlIdx  - �ò����������Ŀ��ƿ�������ţ���0��ʼ��
/// @param[in]
///     smpVal   - ��������Ϣ
///
void CSampleInterpSync::AddASample(uint32_t ctrlIdx, const SMV_VALUE &smpVal)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;

	// ʱ�괦���Ƿ���ʱ������
	FrameTimeStamp ts = smpVal.stFrameTime;
	if ( m_bDelayCompensateEnable )
		DelayCompensate(ctrlIdx,ts);

	// �����ܲ����������
	m_nCtrlReceiveSmpsNum[ctrlIdx]++;

	// ������ʱ��С���趨��ʱ�䷧ֵ����������
	if ( ts.ts_sec < m_stTimeThreshold.ts_sec ||
		 ( ts.ts_sec == m_stTimeThreshold.ts_sec && ts.ts_usec < m_stTimeThreshold.ts_usec ) )
		return;

	// ������ƿ���Ҫ����ͬ����ͨ���Ĳ���������
	m_vSmpsTimes[ctrlIdx].push_back(ts);
	std::vector<uint32_t> &syncChannels = m_vCtrlChannelsIdx[ctrlIdx];
	for ( size_t i = 0; i < syncChannels.size(); i++ )
	{
		int channelIdx = ctrlIdx * MAX_SYNC_CHANNELS_NUM_PER_CTRL + i;
		m_vSmpsVals[channelIdx].push_back(smpVal.fSmvValue[syncChannels.at(i)]);
	}

	// ��δ��ʼͬ�������ж��Ƿ�ʼ
	if ( !m_bStartSync )
	{
		if ( IsStartSync() )
		{
			InitResample();
			m_bStartSync = true;
		}
		else
			return;
	}

	// ����ͬ���󣬴������������ƿ���ѽ��յĲ�����������12�����ڣ������¿�ʼ����ͬ��
	if ( IsRestartSync() )
	{
		ResetSampleSync();
		return;
	}

	m_bIsSampleSync = false;
	if ( IsReadyToInterpSync() )
	{
		for ( uint32_t i=0; i<m_nSyncCtrlNum; i++ )
		{
			// �����ز����Ĳ�������Ŀ�ʼʱ�䣨��Ըÿ��ƿ��1���������ʱ�䣩
			ts = m_vSmpsTimes[i].at(0);
			double dsec  = (double)m_stCtrlResmpTime[i].ts_sec - (double)ts.ts_sec;
			double t0 = (double)m_stCtrlResmpTime[i].ts_usec + dsec * 1000000.0 - (double)ts.ts_usec;;

			// �ز����Ĳ�������Ŀ�ʼʱ��ľ���ֵ����������ڣ������¿�ʼ����ͬ��
			double halfCycleTime = (double)m_nSrcSmpsNumPerCycle*m_dSrcSmpInterval/2.0;
			if ( t0 > halfCycleTime || t0 < -halfCycleTime )
			{
				ResetSampleSync();
				return;
			}

			// ����������������ʱ�䣨��Ըÿ��ƿ��1���������ʱ�䣩
			m_vRelativeTime.clear();
			m_vRelativeTime.push_back(0.0);
			for ( uint32_t j=1; j<m_nSrcSmpsNumPerCycle+3; j++ )
			{
				FrameTimeStamp ts1 = m_vSmpsTimes[i].front();
				FrameTimeStamp ts2 = m_vSmpsTimes[i].at(j);
				if ( ts2.ts_usec < ts1.ts_usec )
					m_vRelativeTime.push_back(ts2.ts_usec + 1000000 - ts1.ts_usec);
				else
					m_vRelativeTime.push_back(ts2.ts_usec - ts1.ts_usec);
			}

			// ��ֵͬ������
			int resmpsNum = 0;
			for ( size_t j=0; j<m_vCtrlChannelsIdx[i].size(); j++ )
			{
				int channelIdx = i * MAX_SYNC_CHANNELS_NUM_PER_CTRL + j;
				resmpsNum = (*m_pInterpFunc)(&m_vRelativeTime[0],&m_vSmpsVals[channelIdx][0],m_nSrcSmpsNumPerCycle+3,
					        t0,m_dResmpInterval,&m_vResmpsVals[channelIdx][0],m_nSrcSmpsNumPerCycle+3);
				
				#if _DEBUG
				if ( resmpsNum == 0 )
				{
					TRACE(_T("resmpsNum = 0\r\n"));
				}
				#endif
			}

			// �����´��ز����Ĳ�������Ŀ�ʼʱ�䣨����ʱ�䣩
			m_stCtrlResmpTime[i].ts_usec += (uint32_t)(m_dResmpInterval * (double)m_nResmpsNumPerCycle);
			m_stCtrlResmpTime[i].ts_sec  += m_stCtrlResmpTime[i].ts_usec/1000000;
			m_stCtrlResmpTime[i].ts_usec = m_stCtrlResmpTime[i].ts_usec%1000000;

			// ���ݲ�����ʱ��С�ڵ��ڵ�����һ�����ִ�����һ���ز�����ʼʱ�̵��ٽ�㣬�õ�֮ǰ�Ĳ�������Ϊ���õ�
			uint32_t nDeleteNum = m_nSrcSmpsNumPerCycle;
			while ( true )
			{
				FrameTimeStamp ts1 = m_vSmpsTimes[i].at(nDeleteNum);
				FrameTimeStamp ts2 = m_stCtrlResmpTime[i];
				if ( ts1.ts_sec > ts2.ts_sec || ( ts1.ts_sec == ts2.ts_sec && ts1.ts_usec > ts2.ts_usec) )
					nDeleteNum--;
				else
					break;
			}
			
			// ɾ�����õ�
			m_vSmpsTimes[i].erase(m_vSmpsTimes[i].begin(),m_vSmpsTimes[i].begin()+nDeleteNum);
			for ( int j = 0; j < MAX_SYNC_CHANNELS_NUM_PER_CTRL; j++ )
			{
				int channelIdx = i*MAX_SYNC_CHANNELS_NUM_PER_CTRL+j;
				m_vSmpsVals[channelIdx].erase(m_vSmpsVals[channelIdx].begin(),m_vSmpsVals[channelIdx].begin()+nDeleteNum);
			}
		}
		
		#ifdef _DEBUG
// 		for ( int i = 0; i < m_nSrcSmpsNumPerCycle+3; i++ )
// 		{
// 			TRACE(_T("%d : %6f\r\n"),i,m_vResmpsVals[0][i]);
// 		}
		TRACE(_T("%d : %6f\r\n"),0,m_vResmpsVals[0][0]);
		TRACE(_T("%d : %6f\r\n"),1,m_vResmpsVals[0][1]);
		TRACE(_T("%d : %6f\r\n"),2,m_vResmpsVals[0][2]);
		TRACE(_T("%d : %6f\r\n"),78,m_vResmpsVals[0][78]);
		TRACE(_T("%d : %6f\r\n"),79,m_vResmpsVals[0][79]);
		TRACE(_T("%d : %6f\r\n\r\n"),80,m_vResmpsVals[0][80]);
		#endif

		m_bIsSampleSync = true;
	}
}

///
/// @brief
///     �ж��Ƿ������һ���ڵĲ�ֵͬ��
///     ע�⣬���һ���ڵ�ͬ�������ٴ���Ӳ����㣬�����Ϊδ���״̬
///
/// @return
///     true - ����ɣ� false - δ���
///
bool CSampleInterpSync::IsSampleSync()
{
	return m_bIsSampleSync;
}

///
/// @brief
///     ��ȡͬ����Ĳ�������Ϣ��һ���ڣ�
///     ������Ĵ�ţ����ƿ�1��ͬ��ͨ��1~n����SetSyncChannelIdx()��ͨ�����룩
///                   ���ƿ�2��ͬ��ͨ��1~n
///                   ...
///
/// @param[in]
///     pAlignSmpsBuf  - �����������Ϣ�Ļ�����
/// @param[in]
///     rowsNum        - �û��������������� >= SetSyncChannelIdx()���õ�ͬ��ͨ�����ܺ�
/// @param[in]
///     columnsNum     - �û��������������� >= �ز�����һ���ڵĲ�������
///
void CSampleInterpSync::GetSyncSamples(float *pAlignSmpsBuf, uint32_t rowsNum, uint32_t columnsNum)
{
	if ( !m_bIsSampleSync || columnsNum < m_nResmpsNumPerCycle )
		return;

	uint32_t channelNum = 0;
	for ( uint32_t i = 0; i < m_nSyncCtrlNum; i++ )
	{
		if ( channelNum >= rowsNum )
			break;

		for ( size_t j = 0; j < m_vCtrlChannelsIdx[i].size(); j++ )
		{
			int channelIdx = i * MAX_SYNC_CHANNELS_NUM_PER_CTRL + j;
			memcpy_s(pAlignSmpsBuf+channelNum*columnsNum,sizeof(float)*columnsNum,
				     &m_vResmpsVals[channelIdx][0],sizeof(float)*m_nResmpsNumPerCycle);
			channelNum++;
		}
	}
}

///
/// @brief
///     ��ʱ����
///
/// @param[in]
///     ctrlIdx  - ʱ�������Ŀ��ƿ�������ţ���0��ʼ��
/// @param[in]
///     ts       - ʱ����Ϣ
///
void CSampleInterpSync::DelayCompensate(uint32_t ctrlIdx, FrameTimeStamp &ts)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;

	if ( ts.ts_usec < m_nCtrlRatedDelay[ctrlIdx] )
	{
		ts.ts_sec--;
		ts.ts_usec = ts.ts_usec + 1000000 - m_nCtrlRatedDelay[ctrlIdx];
	}
	else
	{
		ts.ts_usec = ts.ts_usec - m_nCtrlRatedDelay[ctrlIdx];
	}
}

///
/// @brief
///     �ж��Ƿ�ʼ����ͬ��
///
/// @return
///     true - �ǣ� false - ��
///
bool CSampleInterpSync::IsStartSync()
{
	// �������ƿ��ѽ��յĲ������>=1����ʼ����ͬ��
	bool bStartSync = true;
	for ( size_t i = 0; i < m_nSyncCtrlNum; i++ )
	{
		if ( m_vSmpsTimes[i].size() < 1 )
			bStartSync = false;
	}
	return bStartSync;
}

///
/// @brief
///     �ز�����ʼ������ʼ��ʱ�䷧ֵ���ز���ʱ�̵ȣ�
///
void CSampleInterpSync::InitResample()
{
	// �Աȸ����ƿ飬��ȡ�ѽ��ղ����������ʱ��
	FrameTimeStamp ts = m_vSmpsTimes[0].at(m_vSmpsTimes[0].size()-1);
	for ( uint32_t i = 1; i < m_nSyncCtrlNum; i++ )
	{
		size_t idx = m_vSmpsTimes[i].size()-1;
		if ( m_vSmpsTimes[i].at(idx).ts_sec > ts.ts_sec )
			ts = m_vSmpsTimes[i].at(idx);
		else
		if ( m_vSmpsTimes[i].at(idx).ts_sec == ts.ts_sec )
		{
			if ( m_vSmpsTimes[i].at(idx).ts_usec > ts.ts_usec )
				ts = m_vSmpsTimes[i].at(idx);
		}
	}

	// ��ʼ��ʱ�䷧ֵ
	m_stTimeThreshold.ts_sec  = ts.ts_sec;
	m_stTimeThreshold.ts_usec = ts.ts_usec;

	// ��ʼ���ز���ʱ��
	m_stCtrlResmpTime[0].ts_usec = ts.ts_usec + (uint32_t)(1.5*m_dSrcSmpInterval);
	m_stCtrlResmpTime[0].ts_sec  = ts.ts_sec + m_stCtrlResmpTime[0].ts_usec/1000000;
	m_stCtrlResmpTime[0].ts_usec = m_stCtrlResmpTime[0].ts_usec % 1000000;
	for ( uint32_t i = 1; i < m_nSyncCtrlNum; i++ )
	{
		m_stCtrlResmpTime[i] = m_stCtrlResmpTime[0];
	}

	// �Ƴ����ò�����
	for ( int i=0; i<MAX_SYNC_SMVCTRLS_NUM; i++ )
	{
		m_vSmpsTimes[i].clear();
	}

	for ( int i=0; i<MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL; i++ )
	{
		m_vSmpsVals[i].clear();
	}
}

///
/// @brief
///     �ж��Ƿ���Խ��в�ֵͬ��
///
/// @return
///     true - �ǣ� false - ��
///
bool CSampleInterpSync::IsReadyToInterpSync()
{
	// �������ƿ��ѽ��յĲ������ >= ԭ���ε�һ�����ڵĲ�������+3����ʼ��ֵͬ��
	bool bIsReadyToSync = true;
	for ( size_t i = 0; i < m_nSyncCtrlNum; i++ )
	{
		if ( m_vSmpsTimes[i].size() < m_nSrcSmpsNumPerCycle+3 )
			bIsReadyToSync = false;
	}
	return bIsReadyToSync;
}

///
/// @brief
///     �ж��Ƿ����¿�ʼ����ͬ��
///
/// @return
///     true - �ǣ� false - ��
///
bool CSampleInterpSync::IsRestartSync()
{
	// �Աȸ������ƿ飬��ȡ�ѽ��ܲ������������ֵ����Сֵ
	uint32_t minSmpsNum = m_nCtrlReceiveSmpsNum[0];
	uint32_t maxSmpsNum = m_nCtrlReceiveSmpsNum[0];
	for ( uint32_t i = 1; i < m_nSyncCtrlNum; i++ )
	{
		if ( minSmpsNum > m_nCtrlReceiveSmpsNum[i] )
			minSmpsNum = m_nCtrlReceiveSmpsNum[i];
		if ( maxSmpsNum < m_nCtrlReceiveSmpsNum[i] )
			maxSmpsNum = m_nCtrlReceiveSmpsNum[i];
	}

	// ����������-��С������������12�����ڣ������¿�ʼ����ͬ��
	return ( maxSmpsNum - minSmpsNum > m_nSrcSmpsNumPerCycle*12 );
}
