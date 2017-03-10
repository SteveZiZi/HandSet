/// @file
///     SampleInterpSync.h
///
/// @brief
///     采样插值同步模块
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
///    |               |           |                                                          |
///
#ifndef SAMPLE_INTERP_SYNC_H_

#define SAMPLE_INTERP_SYNC_H_

#include "src\include\stdint.h"
#include <vector>
#include "src/service/datalayer/recv/FrameRecvStruct.h"

///
/// @brief
///     单个控制块需要进行采样同步的最大通道数
///
#define MAX_SYNC_CHANNELS_NUM_PER_CTRL  3

///
/// @brief
///     需要进行采样同步的最大控制块数
///
#define MAX_SYNC_SMVCTRLS_NUM 2

///
/// @brief
///     每个周期的最大重采样点数
///
#define MAX_RESAMPLES_NUM_PER_CYCLE 256

///
/// @brief
///     插值算法的类型
///
typedef enum {
	INTERP_TYPE_LINEAR = 0,     ///< 分段线性插值，执行速度最快，有足够的精度，但数据平滑方面最差
	INTERP_TYPE_QUADRATE,       ///< 分段2次多项式插值，较快，精度高，平滑度好
	INTERP_TYPE_THRICE,         ///< 分段3次多项式插值，较慢，精度高，平滑度好
	INTERP_TYPE_SPLINE          ///< 3次样条插值，执行速度最慢，精度高，最平滑
}INTERP_TYPE;

///
/// @brief
///     定义插值算法的函数指针
///
typedef int (*interp_func)(double *pd_X, double *pd_Y, int dataNum, double x0, double dx, float *pd_resultY ,int resultYSize);

///
/// @brief
///     采样插值同步模块
///
class CSampleInterpSync
{
public:
	CSampleInterpSync();
	~CSampleInterpSync();

	///
	/// @brief
	///     设置所使用的插值算法的类型，默认为线性插值
	///
	/// @param[in]
	///     eInterpType - 插值算法的类型
	///
	void SetInterpType(INTERP_TYPE eInterpType = INTERP_TYPE_LINEAR);

	///
	/// @brief
	///     设置需要进行插值同步的控制块数，默认为2
	///
	/// @param[in]
	///     nCtrlNum - 需要进行插值同步的控制块数
	///
	void SetSyncCtrlNum(uint32_t nCtrlNum = 2);

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
	void SetSyncChannelIdx(uint32_t ctrlIdx, uint32_t *pChannelIdx, uint32_t channelNum);
	
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
	void SetResampleConfig(uint32_t nSignalFrequency, uint32_t nSampleFrequency, uint32_t nReampleFrequency);

	///
	/// @brief
	///     设置控制块的额定延时
	///
	/// @param[in]
	///     ctrlIdx  - 控制块的索引号（从0开始）
	/// @param[in]
	///     nDelay   - 控制块的额定延时时间
	///
	void SetRatedDelay(uint32_t ctrlIdx, uint32_t nDelay);

	///
	/// @brief
	///     设置延时补偿使能，默认为禁止
	///
	/// @param[in]
	///     bEnable - 延时补偿使能，true为使能，false为禁止
	///
	void SetDelayCompensateEnable(bool bEnable = false);

	///
	/// @brief
	///     重置采样同步
	///
	void ResetSampleSync();

	///
	/// @brief
	///     添加一个采样点信息
	///
	/// @param[in]
	///     ctrlIdx  - 该采样点所属的控制块的索引号（从0开始）
	/// @param[in]
	///     smpVal   - 采样点信息
	///
	void AddASample(uint32_t ctrlIdx, const SMV_VALUE &smpVal);

	///
	/// @brief
	///     判断是否已完成一周期的插值同步
	///     注意，完成一周期的同步后，若再次添加采样点，则会置为未完成状态
	///
	/// @return
	///     true - 已完成； false - 未完成
	///
	bool IsSampleSync();

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
	void GetSyncSamples(float *pAlignSmpsBuf, uint32_t rowsNum, uint32_t columnsNum);

protected:
	///
	/// @brief
	///     延时补偿
	///
	/// @param[in]
	///     ctrlIdx  - 时标所属的控制块的索引号（从0开始）
	/// @param[in]
	///     ts       - 时标信息
	///
	void DelayCompensate(uint32_t ctrlIdx, FrameTimeStamp &ts);

	///
	/// @brief
	///     判断是否启动采样同步
	///
	/// @return
	///     true - 是； false - 否
	///
	bool IsStartSync();

	///
	/// @brief
	///     重采样初始化（初始化时间阀值、重采样时刻等）
	///
	void InitResample();

	///
	/// @brief
	///     判断是否可以进行插值同步
	///
	/// @return
	///     true - 是； false - 否
	///
	bool IsReadyToInterpSync();

	///
	/// @brief
	///     判断是否重新开始采样同步
	///
	/// @return
	///     true - 是； false - 否
	///
	bool IsRestartSync();

private:
	std::vector<FrameTimeStamp> m_vSmpsTimes[MAX_SYNC_SMVCTRLS_NUM];                        ///< 各个控制块采样点的时标，单位：us
	std::vector<double>   m_vSmpsVals[MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL];///< 各个通道与m_vSmpsTimes对应的采样值
	std::vector<double>   m_vRelativeTime;                                                  ///< 用于插值计算时的相对时间缓存
	std::vector<uint32_t> m_vCtrlChannelsIdx[MAX_SYNC_SMVCTRLS_NUM];                        ///< 记录各控制块要进行采样同步（时标对齐）的通道的其控制块中的索引（从0开始）
	float                 m_vResmpsVals[MAX_SYNC_SMVCTRLS_NUM*MAX_SYNC_CHANNELS_NUM_PER_CTRL][MAX_RESAMPLES_NUM_PER_CYCLE+10];
	                                                                                        ///< 各个通道重采样后的对齐波形（1周波）
	uint32_t              m_nCtrlRatedDelay[MAX_SYNC_SMVCTRLS_NUM];                         ///< 各个控制块的额定延时
	FrameTimeStamp        m_stCtrlResmpTime[MAX_SYNC_SMVCTRLS_NUM];                         ///< 各个控制块下一次的重采样时刻
	uint32_t              m_nCtrlReceiveSmpsNum[MAX_SYNC_SMVCTRLS_NUM];                     ///< 各个控制块已接收到的采样点的数目
	uint32_t              m_nResmpsNumPerCycle;                                             ///< 重采样后波形每周期的采样点数
	double                m_dResmpInterval;                                                 ///< 重采样后波形的采样点时间间隔，单位：us
	uint32_t              m_nSrcSmpsNumPerCycle;                                            ///< 原波形每周期的采样点数
	double                m_dSrcSmpInterval;                                                ///< 原波形波形的采样点时间间隔，单位：us
	uint32_t              m_nSyncCtrlNum;                                                   ///< 当前要进行采样同步的控制块数
	bool                  m_bDelayCompensateEnable;                                         ///< 延时补偿使能
	bool                  m_bStartSync;                                                     ///< 标志是否启动采样同步
	bool                  m_bIsSampleSync;                                                  ///< 标志各个通道是否已经完成同步（时标对齐）
	FrameTimeStamp        m_stTimeThreshold;                                                ///< 时间阀值，只处理大于该时刻的采样点
	interp_func           m_pInterpFunc;                                                    ///< 插值同步算法的函数指针
};

#endif
