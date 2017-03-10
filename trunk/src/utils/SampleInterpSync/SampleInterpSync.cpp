/// @file
///     SampleInterpSync.cpp
///
/// @brief
///     采样插值同步模块实现
///
/// @note
///
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.10.21
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.10.22     |1.0.1.1022 |AddASample()中，添加对重采样开始时刻的判断，其值的绝对值超过半个周期，则重新开始同步|
///    |2013.10.22     |1.0.1.1022 |AddASample()中，修改对无用点的删除判断，将其时刻小于等于但其下一个点又大于下次重采样的开始时刻的临界点作为删除分界|
///    |2013.10.23     |1.0.1.1023 |AddASample()中，修正计算采样点相对时间时没有清理容器中的过时数据引起崩溃的Bug|
///    |2013.11.05     |1.0.1.1105 |修改采样点数据的初始缓存容量                             |
///    |2013.11.05     |1.0.1.1105 |修改重采样初始化中各控制块已接收采样点的最晚时刻的获取   |
///    |2013.11.05     |1.0.1.1105 |修改重新开始采样同步的判断条件，已接收采样点从相差1周期改为12周期|
///
#include "stdafx.h"
#include "include\SampleInterpSync.h"
#include "interp.h"

///
/// @brief
///     原波形采样点数据的初始缓存容量
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
///     设置所使用的插值算法的类型，默认为线性插值
///
/// @param[in]
///     eInterpType - 插值算法的类型
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
///     设置需要进行插值同步的控制块数，默认为2
///
/// @param[in]
///     nCtrlNum - 需要进行插值同步的控制块数
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
///     设置需要进行插值同步的通道
///
/// @param[in]
///     ctrlIdx     - 通道所在的控制块的索引号，从0开始
/// @param[in]
///     pChannelIdx - 保存通道索引号（从0开始）的数组
/// @param[in]
///     channelNum  - 通道数目
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
///     设置重采样配置
///
/// @param[in]
///     nSignalFrequency  - 原始波形（信号）的频率
/// @param[in]
///     nSampleFrequency  - 原波形的采样频率
/// @param[in]
///     nReampleFrequency - 重采样频率
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
///     设置控制块的额定延时
///
/// @param[in]
///     ctrlIdx  - 控制块的索引号（从0开始）
/// @param[in]
///     nDelay   - 控制块的额定延时时间
///
void CSampleInterpSync::SetRatedDelay(uint32_t ctrlIdx, uint32_t nDelay)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;
	m_nCtrlRatedDelay[ctrlIdx] = nDelay;
}

///
/// @brief
///     设置延时补偿使能，默认为禁止
///
/// @param[in]
///     bEnable - 延时补偿使能，true为使能，false为禁止
///
void CSampleInterpSync::SetDelayCompensateEnable(bool bEnable /* = false */)
{
	m_bDelayCompensateEnable = bEnable;
}

///
/// @brief
///     重置采样同步
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
///     添加一个采样点信息
///
/// @param[in]
///     ctrlIdx  - 该采样点所属的控制块的索引号（从0开始）
/// @param[in]
///     smpVal   - 采样点信息
///
void CSampleInterpSync::AddASample(uint32_t ctrlIdx, const SMV_VALUE &smpVal)
{
	if ( ctrlIdx >= m_nSyncCtrlNum )
		return;

	// 时标处理（是否延时补偿）
	FrameTimeStamp ts = smpVal.stFrameTime;
	if ( m_bDelayCompensateEnable )
		DelayCompensate(ctrlIdx,ts);

	// 己接受采样点计数器
	m_nCtrlReceiveSmpsNum[ctrlIdx]++;

	// 采样点时标小于设定的时间阀值，则不作处理
	if ( ts.ts_sec < m_stTimeThreshold.ts_sec ||
		 ( ts.ts_sec == m_stTimeThreshold.ts_sec && ts.ts_usec < m_stTimeThreshold.ts_usec ) )
		return;

	// 保存控制块需要采样同步的通道的采样点数据
	m_vSmpsTimes[ctrlIdx].push_back(ts);
	std::vector<uint32_t> &syncChannels = m_vCtrlChannelsIdx[ctrlIdx];
	for ( size_t i = 0; i < syncChannels.size(); i++ )
	{
		int channelIdx = ctrlIdx * MAX_SYNC_CHANNELS_NUM_PER_CTRL + i;
		m_vSmpsVals[channelIdx].push_back(smpVal.fSmvValue[syncChannels.at(i)]);
	}

	// 若未开始同步，则判断是否开始
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

	// 启动同步后，存在有两个控制块的已接收的采样点数相差超过12个周期，则重新开始采样同步
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
			// 计算重采样的波形输出的开始时间（相对该控制块第1个采样点的时间）
			ts = m_vSmpsTimes[i].at(0);
			double dsec  = (double)m_stCtrlResmpTime[i].ts_sec - (double)ts.ts_sec;
			double t0 = (double)m_stCtrlResmpTime[i].ts_usec + dsec * 1000000.0 - (double)ts.ts_usec;;

			// 重采样的波形输出的开始时间的绝对值超过半个周期，则重新开始采样同步
			double halfCycleTime = (double)m_nSrcSmpsNumPerCycle*m_dSrcSmpInterval/2.0;
			if ( t0 > halfCycleTime || t0 < -halfCycleTime )
			{
				ResetSampleSync();
				return;
			}

			// 计算各个采样点相对时间（相对该控制块第1个采样点的时间）
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

			// 插值同步运算
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

			// 更新下次重采样的波形输出的开始时间（绝对时间）
			m_stCtrlResmpTime[i].ts_usec += (uint32_t)(m_dResmpInterval * (double)m_nResmpsNumPerCycle);
			m_stCtrlResmpTime[i].ts_sec  += m_stCtrlResmpTime[i].ts_usec/1000000;
			m_stCtrlResmpTime[i].ts_usec = m_stCtrlResmpTime[i].ts_usec%1000000;

			// 回溯查找其时刻小于等于但其下一个点又大于下一个重采样开始时刻的临界点，该点之前的采样点则为无用点
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
			
			// 删除无用点
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
///     判断是否已完成一周期的插值同步
///     注意，完成一周期的同步后，若再次添加采样点，则会置为未完成状态
///
/// @return
///     true - 已完成； false - 未完成
///
bool CSampleInterpSync::IsSampleSync()
{
	return m_bIsSampleSync;
}

///
/// @brief
///     获取同步后的采样点信息（一周期）
///     采样点的存放：控制块1的同步通道1~n（与SetSyncChannelIdx()的通道对齐）
///                   控制块2的同步通道1~n
///                   ...
///
/// @param[in]
///     pAlignSmpsBuf  - 保存采样点信息的缓存区
/// @param[in]
///     rowsNum        - 该缓存区的行数，需 >= SetSyncChannelIdx()设置的同步通道的总和
/// @param[in]
///     columnsNum     - 该缓存区的列数，需 >= 重采样后一周期的采样点数
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
///     延时补偿
///
/// @param[in]
///     ctrlIdx  - 时标所属的控制块的索引号（从0开始）
/// @param[in]
///     ts       - 时标信息
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
///     判断是否开始采样同步
///
/// @return
///     true - 是； false - 否
///
bool CSampleInterpSync::IsStartSync()
{
	// 各个控制块已接收的采样点均>=1，则开始采样同步
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
///     重采样初始化（初始化时间阀值、重采样时刻等）
///
void CSampleInterpSync::InitResample()
{
	// 对比各控制块，获取已接收采样点的最晚时刻
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

	// 初始化时间阀值
	m_stTimeThreshold.ts_sec  = ts.ts_sec;
	m_stTimeThreshold.ts_usec = ts.ts_usec;

	// 初始化重采样时刻
	m_stCtrlResmpTime[0].ts_usec = ts.ts_usec + (uint32_t)(1.5*m_dSrcSmpInterval);
	m_stCtrlResmpTime[0].ts_sec  = ts.ts_sec + m_stCtrlResmpTime[0].ts_usec/1000000;
	m_stCtrlResmpTime[0].ts_usec = m_stCtrlResmpTime[0].ts_usec % 1000000;
	for ( uint32_t i = 1; i < m_nSyncCtrlNum; i++ )
	{
		m_stCtrlResmpTime[i] = m_stCtrlResmpTime[0];
	}

	// 移除无用采样点
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
///     判断是否可以进行插值同步
///
/// @return
///     true - 是； false - 否
///
bool CSampleInterpSync::IsReadyToInterpSync()
{
	// 各个控制块已接收的采样点均 >= 原波形的一个周期的采样点数+3，则开始插值同步
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
///     判断是否重新开始采样同步
///
/// @return
///     true - 是； false - 否
///
bool CSampleInterpSync::IsRestartSync()
{
	// 对比各个控制块，获取已接受采样点数的最大值、最小值
	uint32_t minSmpsNum = m_nCtrlReceiveSmpsNum[0];
	uint32_t maxSmpsNum = m_nCtrlReceiveSmpsNum[0];
	for ( uint32_t i = 1; i < m_nSyncCtrlNum; i++ )
	{
		if ( minSmpsNum > m_nCtrlReceiveSmpsNum[i] )
			minSmpsNum = m_nCtrlReceiveSmpsNum[i];
		if ( maxSmpsNum < m_nCtrlReceiveSmpsNum[i] )
			maxSmpsNum = m_nCtrlReceiveSmpsNum[i];
	}

	// 最大采样点数-最小采样点数超过12个周期，则重新开始采样同步
	return ( maxSmpsNum - minSmpsNum > m_nSrcSmpsNumPerCycle*12 );
}
