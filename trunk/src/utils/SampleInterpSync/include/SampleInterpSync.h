/// @file
///     SampleInterpSync.h
///
/// @brief
///     ������ֵͬ��ģ��
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
///    |               |           |                                                          |
///
#ifndef SAMPLE_INTERP_SYNC_H_

#define SAMPLE_INTERP_SYNC_H_

#include "src\include\stdint.h"
#include <vector>
#include "src/service/datalayer/recv/FrameRecvStruct.h"

///
/// @brief
///     �������ƿ���Ҫ���в���ͬ�������ͨ����
///
#define MAX_SYNC_CHANNELS_NUM_PER_CTRL  3

///
/// @brief
///     ��Ҫ���в���ͬ���������ƿ���
///
#define MAX_SYNC_SMVCTRLS_NUM 2

///
/// @brief
///     ÿ�����ڵ�����ز�������
///
#define MAX_RESAMPLES_NUM_PER_CYCLE 256

///
/// @brief
///     ��ֵ�㷨������
///
typedef enum {
	INTERP_TYPE_LINEAR = 0,     ///< �ֶ����Բ�ֵ��ִ���ٶ���죬���㹻�ľ��ȣ�������ƽ���������
	INTERP_TYPE_QUADRATE,       ///< �ֶ�2�ζ���ʽ��ֵ���Ͽ죬���ȸߣ�ƽ���Ⱥ�
	INTERP_TYPE_THRICE,         ///< �ֶ�3�ζ���ʽ��ֵ�����������ȸߣ�ƽ���Ⱥ�
	INTERP_TYPE_SPLINE          ///< 3��������ֵ��ִ���ٶ����������ȸߣ���ƽ��
}INTERP_TYPE;

///
/// @brief
///     �����ֵ�㷨�ĺ���ָ��
///
typedef int (*interp_func)(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     ������ֵͬ��ģ��
///
class CSampleInterpSync
{
public:
	CSampleInterpSync();
	~CSampleInterpSync();

	///
	/// @brief
	///     ������ʹ�õĲ�ֵ�㷨�����ͣ�Ĭ��Ϊ���Բ�ֵ
	///
	/// @param[in]
	///     eInterpType - ��ֵ�㷨������
	///
	void SetInterpType(INTERP_TYPE eInterpType = INTERP_TYPE_LINEAR);

	///
	/// @brief
	///     ������Ҫ���в�ֵͬ���Ŀ��ƿ�����Ĭ��Ϊ2
	///
	/// @param[in]
	///     nCtrlNum - ��Ҫ���в�ֵͬ���Ŀ��ƿ���
	///
	void SetSyncCtrlNum(uint32_t nCtrlNum = 2);

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
	void SetSyncChannelIdx(uint32_t ctrlIdx, uint32_t *pChannelIdx, uint32_t channelNum);
	
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
	void SetResampleConfig(uint32_t nSignalFrequency, uint32_t nSampleFrequency, uint32_t nReampleFrequency);

	///
	/// @brief
	///     ���ÿ��ƿ�Ķ��ʱ
	///
	/// @param[in]
	///     ctrlIdx  - ���ƿ�������ţ���0��ʼ��
	/// @param[in]
	///     nDelay   - ���ƿ�Ķ��ʱʱ��
	///
	void SetRatedDelay(uint32_t ctrlIdx, uint32_t nDelay);

	///
	/// @brief
	///     ������ʱ����ʹ�ܣ�Ĭ��Ϊ��ֹ
	///
	/// @param[in]
	///     bEnable - ��ʱ����ʹ�ܣ�trueΪʹ�ܣ�falseΪ��ֹ
	///
	void SetDelayCompensateEnable(bool bEnable = false);

	///
	/// @brief
	///     ���ò���ͬ��
	///
	void ResetSampleSync();

	///
	/// @brief
	///     ���һ����������Ϣ
	///
	/// @param[in]
	///     ctrlIdx  - �ò����������Ŀ��ƿ�������ţ���0��ʼ��
	/// @param[in]
	///     smpVal   - ��������Ϣ
	///
	void AddASample(uint32_t ctrlIdx, const SMV_VALUE &smpVal);

	///
	/// @brief
	///     �ж��Ƿ������һ���ڵĲ�ֵͬ��
	///     ע�⣬���һ���ڵ�ͬ�������ٴ���Ӳ����㣬�����Ϊδ���״̬
	///
	/// @return
	///     true - ����ɣ� false - δ���
	///
	bool IsSampleSync();

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
	void GetSyncSamples(float *pAlignSmpsBuf, uint32_t rowsNum, uint32_t columnsNum);

protected:
	///
	/// @brief
	///     ��ʱ����
	///
	/// @param[in]
	///     ctrlIdx  - ʱ�������Ŀ��ƿ�������ţ���0��ʼ��
	/// @param[in]
	///     ts       - ʱ����Ϣ
	///
	void DelayCompensate(uint32_t ctrlIdx, FrameTimeStamp &ts);

	///
	/// @brief
	///     �ж��Ƿ���������ͬ��
	///
	/// @return
	///     true - �ǣ� false - ��
	///
	bool IsStartSync();

	///
	/// @brief
	///     �ز�����ʼ������ʼ��ʱ�䷧ֵ���ز���ʱ�̵ȣ�
	///
	void InitResample();

	///
	/// @brief
	///     �ж��Ƿ���Խ��в�ֵͬ��
	///
	/// @return
	///     true - �ǣ� false - ��
	///
	bool IsReadyToInterpSync();

	///
	/// @brief
	///     �ж��Ƿ����¿�ʼ����ͬ��
	///
	/// @return
	///     true - �ǣ� false - ��
	///
	bool IsRestartSync();

private:
	std::vector<FrameTimeStamp> m_vSmpsTimes[MAX_SYNC_SMVCTRLS_NUM];                        ///< �������ƿ�������ʱ�꣬��λ��us
	std::vector<double>   m_vSmpsVals[MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL];///< ����ͨ����m_vSmpsTimes��Ӧ�Ĳ���ֵ
	std::vector<double>   m_vRelativeTime;                                                  ///< ���ڲ�ֵ����ʱ�����ʱ�仺��
	std::vector<uint32_t> m_vCtrlChannelsIdx[MAX_SYNC_SMVCTRLS_NUM];                        ///< ��¼�����ƿ�Ҫ���в���ͬ����ʱ����룩��ͨ��������ƿ��е���������0��ʼ��
	float                 m_vResmpsVals[MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL][MAX_RESAMPLES_NUM_PER_CYCLE+10];
	                                                                                        ///< ����ͨ���ز�����Ķ��벨�Σ�1�ܲ���
	uint32_t              m_nCtrlRatedDelay[MAX_SYNC_SMVCTRLS_NUM];                         ///< �������ƿ�Ķ��ʱ
	FrameTimeStamp        m_stCtrlResmpTime[MAX_SYNC_SMVCTRLS_NUM];                         ///< �������ƿ���һ�ε��ز���ʱ��
	uint32_t              m_nCtrlReceiveSmpsNum[MAX_SYNC_SMVCTRLS_NUM];                     ///< �������ƿ��ѽ��յ��Ĳ��������Ŀ
	uint32_t              m_nResmpsNumPerCycle;                                             ///< �ز�������ÿ���ڵĲ�������
	double                m_dResmpInterval;                                                 ///< �ز������εĲ�����ʱ��������λ��us
	uint32_t              m_nSrcSmpsNumPerCycle;                                            ///< ԭ����ÿ���ڵĲ�������
	double                m_dSrcSmpInterval;                                                ///< ԭ���β��εĲ�����ʱ��������λ��us
	uint32_t              m_nSyncCtrlNum;                                                   ///< ��ǰҪ���в���ͬ���Ŀ��ƿ���
	bool                  m_bDelayCompensateEnable;                                         ///< ��ʱ����ʹ��
	bool                  m_bStartSync;                                                     ///< ��־�Ƿ���������ͬ��
	bool                  m_bIsSampleSync;                                                  ///< ��־����ͨ���Ƿ��Ѿ����ͬ����ʱ����룩
	FrameTimeStamp        m_stTimeThreshold;                                                ///< ʱ�䷧ֵ��ֻ������ڸ�ʱ�̵Ĳ�����
	interp_func           m_pInterpFunc;                                                    ///< ��ֵͬ���㷨�ĺ���ָ��
};

#endif
