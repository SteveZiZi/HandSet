/// @file
///     iec61850_data_struct.h
///
/// @brief
///     定义IEC61850协议的常用数据类型/结构的编解码接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.13
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef IEC61850_DATA_STRUCT_H_

#define IEC61850_DATA_STRUCT_H_

#include "src\include\stdint.h"

//
//=============================================================================
//    FT3帧中的状态字的编解码接口
//=============================================================================
//

///
/// @brief
///     定义FT3帧的类型
///
typedef enum{
	FT3_FRAME_TYPE_MU_STD = 0,       ///< 合并单元输出的标准FT3帧，主要指南瑞FT3、52字节标准FT3、与IEC60044-8兼容的IEC61850-9-1
	FT3_FRAME_TYPE_MU_EX,            ///< 合并单元输出的扩展FT3帧，主要指国网FT3
	FT3_FRAME_TYPE_CTORVT            ///< 电流/电压互感器输出的FT3帧
}FT3_FRAME_TYPE;

///
/// @brief
///     定义电流/电压互感器的激光电源的动作/状态类型
///
typedef enum{
	FT3CLPS_UNKNOWN = 0,                 ///< 未知
	FT3CLPS_MAINTAIN_OUTPUT,             ///< 激光器应“保持当前输出功率”
	FT3CLPS_DOWN_TO_MAINTAIN_POWER,      ///< 激光器应“下调至维持功率”
	FT3CLPS_DECREASE_OUTPUT_POWER,       ///< 激光器应“下调输出功率”
	FT3CLPS_INCREASE_OUTPUT_POWER,       ///< 激光器应“上调输出功率”
	FT3CLPS_SPEED_INCREASE_OUTPUT_POWER, ///< 激光器应“速上调输出功率”
	FT3CLPS_FAILURE,                     ///< 激光器故障
}FT3_CTORVT_LASER_POWER_STATUS;

class Ft3FrameStatusWord
{
public:
	Ft3FrameStatusWord();
	Ft3FrameStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2, FT3_FRAME_TYPE eFrameType);

	///
	/// @brief
	///     设置该状态字所属FT3帧的类型
	///
	/// @param[in]
	///     eFrameType - FT3帧的类型
	///
	void SetFt3FrameType(FT3_FRAME_TYPE eFrameType);

	///
	/// @brief
	///     获取状态字
	///
	/// @param[in]
	///     nStatusWord1 - 用于保存状态字1
	/// @param[in]
	///     nStatusWord2 - 用于保存状态字2
	///
	void GetStatusWord(uint16_t &nStatusWord1, uint16_t &nStatusWord2);

	///
	/// @brief
	///     设置状态字
	///
	/// @param[in]
	///     nStatusWord1 - 状态字1
	/// @param[in]
	///     nStatusWord2 - 状态字2
	///
	void SetStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2);
	
	///
	/// @brief
	///     获取设备（MU或互感器）的故障状态信息（是否要求维修）
	///
	/// @return
	///     0    - 良好（不要求维修）
	///     1    - 警告或报警（要求维修）
	///     其他 - 获取失败
	///
	int  IsMaintenanceRequired();

	///
	/// @brief
	///     设置设备（MU或互感器）的故障状态（是否要求维修）
	///
	/// @param[in]
	///     bRequired - true则要求维修,否则相反
	///
	void SetMaintenanceRequired(bool bRequired);

	///
	/// @brief
	///     获取设备（MU或互感器）的工作状态信息（是否处于试验状态）
	///
	/// @return
	///     0    - 接通（正常运行）
	///     1    - 试验
	///     其他 - 获取失败
	///
	int  IsTestMode();

	///
	/// @brief
	///     设置设备（MU或互感器）的工作状态信息（是否处于试验状态）
	///
	/// @param[in]
	///     bTest - true则为试验,否则为接通
	///
	void SetTestMode(bool bTest);

	///
	/// @brief
	///     获取FT3帧中CT的输出类型
	///
	/// @return
	///     0    - 输出为i(t)
	///     1    - 输出为d(i(t))/dt
	///     其他 - 获取失败
	///
	int  GetCtOutputType();

	///
	/// @brief
	///     设置FT3帧中CT的输出类型
	///
	/// @param[in]
	///     bOutputIt - true则CT输出为i(t),否则为d(i(t))/dt
	///
	void SetCtOutputType(bool bOutputIt);

	///
	/// @brief
	///     获取比例因子SCP的RangeFlag信息
	///
	/// @return
	///     0    - 比例因子SCP=0x01CF
	///     1    - 比例因子SCP=0x00E7
	///     其他 - 获取失败
	///
	int  GetRangeFlag();

	///
	/// @brief
	///     设置比例因子SCP的RangeFlag
	///
	/// @param[in]
	///     bRangeFlag0 - true则RangeFlag=0,即SCP=0x01CF,否则RangeFlag=1
	///
	void SetRangeFlag(bool bRangeFlag0);

	///
	/// @brief
	///     获取MU的唤醒时间指示信息
	///
	/// @return
	///     0    - 接通（正常运行），此时数据有效
	///     1    - 唤醒期间，此时数据无效
	///     其他 - 获取失败
	///
	int  IsMuWakingUp();

	///
	/// @brief
	///     设置MU的唤醒时间指示
	///
	/// @param[in]
	///     bWakingUp - true则为唤醒期间,否则MU正常运行
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetMuWakingUp(bool bWakingUp);

	///
	/// @brief
	///     获取MU的同步方法信息
	///
	/// @return
	///     0    - 数据集不采用插值法
	///     1    - 数据集适用于插值法
	///     其他 - 获取失败
	///
	int  GetMuSyncMethod();

	///
	/// @brief
	///     设置MU的同步方法
	///
	/// @param[in]
	///     bUseInterpolation - true则采用插值法,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetMuSyncMethod(bool bUseInterpolation);

	///
	/// @brief
	///     获取MU的同步状态信息（是否同步）
	///
	/// @return
	///     0    - 样本同步
	///     1    - 时间同步消逝/无效
	///     其他 - 获取失败
	///
	int  IsMuNotSync();

	///
	/// @brief
	///     设置MU的同步状态
	///
	/// @param[in]
	///     bSync - true则样本同步,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetMuSync(bool bSync);

	///
	/// @brief
	///     获取互感器的激发时间指示信息
	///
	/// @return
	///     0    - 接通（正常运行），此时数据有效
	///     1    - 激发期间，此时数据无效
	///     其他 - 获取失败
	///
	int  IsCtorVtFiring();

	///
	/// @brief
	///     设置互感器的激发时间指示
	///
	/// @param[in]
	///     bFiring - true则为激发期间,否则互感器正常运行
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtFiring(bool bFiring);

	///
	/// @brief
	///     获取互感器的同步方法信息
	///
	/// @return
	///     0    - 数据集不采用插值法
	///     1    - 数据集适用于插值法
	///     其他 - 获取失败
	///
	int  GetCtorVtSyncMethod();

	///
	/// @brief
	///     设置互感器的同步方法
	///
	/// @param[in]
	///     bUseInterpolation - true则采用插值法,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtSyncMethod(bool bUseInterpolation);

	///
	/// @brief
	///     获取互感器的同步状态信息
	///
	/// @return
	///     0    - 样本同步
	///     1    - 时间同步消逝/无效
	///     其他 - 获取失败
	///
	int  IsCtorVtNotSync();

	///
	/// @brief
	///     设置互感器的同步状态
	///
	/// @param[in]
	///     bSync - true则样本同步,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtSync(bool bSync);

	///
	/// @brief
	///     获取互感器的A/D转换的状态信息
	///
	/// @return
	///     0    - A/D转换正确
	///     1    - A/D转换错误
	///     其他 - 获取失败
	///
	int  IsCtorVtAdError();

	///
	/// @brief
	///     设置互感器的A/D转换的状态
	///
	/// @param[in]
	///     bError - true则A/D转换错误,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtAdError(bool bError);

	///
	/// @brief
	///     获取互感器的1号电源的状态信息
	///
	/// @return
	///     0    - 正常
	///     1    - 异常
	///     其他 - 获取失败
	///
	int  IsCtorVtPower1stAbnor();

	///
	/// @brief
	///     设置互感器的1号电源的状态
	///
	/// @param[in]
	///     bAbnor - true则异常,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtPower1stAbnor(bool bAbnor);

	///
	/// @brief
	///     获取互感器的2号电源的状态信息
	///
	/// @return
	///     0    - 正常
	///     1    - 异常
	///     其他 - 获取失败
	///
	int  IsCtorVtPower2ndAbnor();

	///
	/// @brief
	///     设置互感器的2号电源的状态
	///
	/// @param[in]
	///     bAbnor - true则异常,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtPower2ndAbnor(bool bAbnor);

	///
	/// @brief
	///     获取互感器的高压电源的有效性信息
	///
	/// @return
	///     0    - 有效
	///     1    - 无效
	///     其他 - 获取失败
	///
	int  IsCtorVtHvpsInvalid();

	///
	/// @brief
	///     设置互感器的高压电源的有效性
	///
	/// @param[in]
	///     bValid - true则有效,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtHvpsValidity(bool bValid);

	///
	/// @brief
	///     获取互感器的激光电源的动作/状态信息
	///
	/// @return
	///     激光电源的动作/状态
	///
	FT3_CTORVT_LASER_POWER_STATUS GetCtorVtLaserPowerStatus();

	///
	/// @brief
	///     设置互感器的激光电源的动作/状态
	///
	/// @param[in]
	///     eStatus -  激光电源的动作/状态
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetCtorVtLaserPowerStatus(FT3_CTORVT_LASER_POWER_STATUS eStatus);

	///
	/// @brief
	///     获取FT3帧中各通道数据的有效性信息
	///
	/// @param[in]
	///     nChannelIdx - 通道的顺序号，从0开始
	///
	/// @return
	///     0    - 有效
	///     1    - 无效
	///     其他 - 获取失败
	///
	int  IsChannelInvalid(uint32_t nChannelIdx);

	///
	/// @brief
	///     设置FT3帧中各通道数据的有效性
	///
	/// @param[in]
	///     nChannelIdx - 通道的顺序号，从0开始
	/// @param[in]
	///     bValid      - true则有效,否则相反
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetChannelValidity(uint32_t nChannelIdx, bool bValid);

	///
	/// @brief
	///     获取状态字中各备用位的值
	///
	/// @param[in]
	///     nBitIdx - 备用位的顺序号，从0开始
	///
	/// @return
	///     该位的值 - 0或1
	///     其他     - 获取失败
	///
	int  GetFutureUsedBitVal(uint32_t nBitIdx);

	///
	/// @brief
	///     设置状态字中各备用位的值
	///
	/// @param[in]
	///     nBitIdx - 通道的顺序号，从0开始
	/// @param[in]
	///     bBitVal - true则为1,否则为0
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetFutureUsedBitVal(uint32_t nBitIdx, bool bBitVal);

private:
	uint16_t m_nStatusWord1;
	uint16_t m_nStatusWord2;
	FT3_FRAME_TYPE m_eFrameType;
	uint16_t m_nChannelBitsNum;
	uint16_t m_nFutureUseBitsNum;
	uint8_t *m_pChannelBit;
	uint8_t *m_pFutureUseBit;
};

//
//=============================================================================
//    IEC61850定义的TimeStamp类型中的TimeQuality的编解码接口
//=============================================================================
//
class Iec61850TimeQuality
{
public:
	Iec61850TimeQuality();
	Iec61850TimeQuality(uint8_t tq);

	///
	/// @brief
	///     获取TimeQuality
	///
	/// @return
	///     TimeQuality值
	///
	uint8_t GetTimeQuality();

	///
	/// @brief
	///     设置TimeQuality
	///
	/// @param[in]
	///     tq - TimeQuality值
	///
	void SetTimeQuality(uint8_t tq);

	///
	/// @brief
	///     判断闰秒是否已知
	///
	/// @return
	///     true  - 已知
	///     false - 未知
	///
	bool IsLeapSecondKnown();

	///
	/// @brief
	///     设置闰秒是否已知
	///
	/// @param[in]
	///     bKnown - true则已知,否则相反
	///
	void SetLeapSecondKnown(bool bKnown);

	///
	/// @brief
	///     判断时钟是否故障
	///
	/// @return
	///     true  - 故障
	///     false - 正常
	///
	bool IsClockFailure();

	///
	/// @brief
	///     设置时钟故障状态
	///
	/// @param[in]
	///     bFail - true则时钟故障,否则相反
	///
	void SetClockFailure(bool bFail);

	///
	/// @brief
	///     判断时钟是否未同步
	///
	/// @return
	///     true  - 未同步
	///     false - 同步
	///
	bool IsClockNotSync();

	///
	/// @brief
	///     设置时钟同步状态
	///
	/// @param[in]
	///     bSync - true则时钟同步,否则相反
	///
	void SetClockSync(bool bSync);

	///
	/// @brief
	///     获取IEC61850时间戳的秒的小数部分的时间精度
	///
	/// @return
	///     -1   - 非法
	///     -2   - 未规定
	///     其他 - 数值对应整数位精度（0~23位精度）
	///
	int  GetSecondAccuracy();

	///
	/// @brief
	///     设置IEC61850时间戳的秒的小数部分的时间精度
	///
	/// @param[in]
	///     nAccuracy - 对应整数位精度的数值（0~23位精度）
	///
	/// @return
	///     true  - 成功
	///     false - 失败
	///
	bool SetSecondAccuracy(int nAccuracy);

private:
	uint8_t m_nTimeQuality;
};

//
//=============================================================================
//    IEC61850定义的基本类型Quality的编解码接口
//    注意：
//    1、Iec61850SmvQuality适用于SMV，而Iec61850GooseQuality适用于Goose，
//       两者属性基本一致，但在SMV传输为uint32，在Goose传输中为bit-string；
//    2、IEC61850-9-2LE 仅使用validity、test和derived三个属性，且
//       derived属性为IEC61850-9-2LE 对Quality类型的扩充定义。
//=============================================================================
//

///
/// @brief
///     定义有关值的的合法性类型
///
typedef enum{
	IEC61850_QV_GOOD = 0,             ///< 好
	IEC61850_QV_INVALID,              ///< 非法/无效
	IEC61850_QV_RESERVED,             ///< 保留
	IEC61850_QV_QUESTIONABLE          ///< 可疑
}IEC61850_QUALITY_VALIDITY_TYPE;

///
/// @brief
///     定义有关值的来源类型
///
typedef enum{
	IEC61850_QST_PROCESS = 0,         ///< 过程，表示相关值由过程I/O输入所提供或者有某些应用功能计算出来
	IEC61850_QST_SUBSTITUTED,         ///< 被取代，表示相关值有操作员输入或者由自动源提供
}IEC61850_QUALITY_SOURCE_TYPE;

class Iec61850SmvQuality
{
public:
	Iec61850SmvQuality();
	Iec61850SmvQuality(uint32_t q);

	///
	/// @brief
	///     获取Quality（品质因数）值
	///
	/// @return
	///     Quality（品质因数）值
	///
	uint32_t GetQuality();

	///
	/// @brief
	///     设置Quality（品质因数）值
	///
	/// @param[in]
	///     q - Quality（品质因数）值
	///
	void SetQuality(uint32_t q);

	///
	/// @brief
	///     获取有效性信息
	///
	/// @return
	///     有效性信息
	///
	IEC61850_QUALITY_VALIDITY_TYPE GetValidity();

	///
	/// @brief
	///     设置有效性
	///
	/// @param[in]
	///     eValidity - 有效性值
	///
	void SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity);

	///
	/// @brief
	///     判断有关值是否溢出
	///
	/// @return
	///     true  - 溢出
	///     false - 未溢出
	///
	bool IsOverflow();

	///
	/// @brief
	///     设置有关值是否溢出
	///
	/// @param[in]
	///     bOverflow - true则溢出,否则相反
	///
	void SetOverflow( bool bOverflow);

	///
	/// @brief
	///     判断有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOutofRange();

	///
	/// @brief
	///     设置有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
	///
	/// @param[in]
	///     bOutofRange - true则超量程,否则相反
	///
	void SetOutofRange(bool bOutofRange);

	///
	/// @brief
	///     判断有关值是否坏基准值（基准值无法校准）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsBadReference();

	///
	/// @brief
	///     设置有关值是否坏基准值（基准值无法校准）
	///
	/// @param[in]
	///     bBadReference - true则坏基准值,否则相反
	///
	void SetBadReference(bool bBadReference);

	///
	/// @brief
	///     判断有关值是否振荡（抖动）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOscillatory();

	///
	/// @brief
	///     设置有关值是否振荡（抖动）
	///
	/// @param[in]
	///     bOscillatory - true则抖动,否则相反
	///
	void SetOscillatory(bool bOscillatory);

	///
	/// @brief
	///     判断有关值是否为故障值（监视功能检出一个内部或外部故障）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsFailure();

	///
	/// @brief
	///     设置有关值是否故障值（监视功能检出一个内部或外部故障）
	///
	/// @param[in]
	///     bFail - true则故障,否则相反
	///
	void SetFailure(bool bFail);

	///
	/// @brief
	///     判断有关值是否为旧数据（在给定时间间隔内不可用）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOldData();

	///
	/// @brief
	///     设置有关值是否旧数据（在给定时间间隔内不可用）
	///
	/// @param[in]
	///     bOldData - true则为旧数据,否则相反
	///
	void SetOldData(bool bOldData);

	///
	/// @brief
	///     判断有关值是否不一致（评估功能已检出不一致）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsInconsistent();

	///
	/// @brief
	///     设置有关值是否不一致（评估功能已检出不一致）
	///
	/// @param[in]
	///     bInconsistent - true则为不一致,否则相反
	///
	void SetInconsistent(bool bInconsistent);

	///
	/// @brief
	///     判断有关值是否不精确（该值不满足源所声明的精度）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsInaccurate();

	///
	/// @brief
	///     设置有关值是否不精确（该值不满足源所声明的精度）
	///
	/// @param[in]
	///     bInaccurate - true则为不精确,否则相反
	///
	void SetInaccurate(bool bInaccurate);

	///
	/// @brief
	///     获取有关值的来源信息
	///
	/// @return
	///     来源的类型
	///
	IEC61850_QUALITY_SOURCE_TYPE GetSourceType();

	///
	/// @brief
	///     设置有关值的来源类型
	///
	/// @param[in]
	///     eSourceType - 来源的类型
	///
	void SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType);

	///
	/// @brief
	///     判断是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsTest();

	///
	/// @brief
	///     设置是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
	///
	/// @param[in]
	///     bTest - true则处于测试/检修状态,否则相反
	///
	void SetTest(bool bTest);

	///
	/// @brief
	///     判断值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOperatorBlocked();

	///
	/// @brief
	///     设置值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
	///
	/// @param[in]
	///     bOperatorBlocked - true则被操作员闭锁,否则相反
	///
	void SetOperatorBlocked(bool bOperatorBlocked);

	///
	/// @brief
	///     判断有关值是否派生（通过计算）得来
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsDerived();

	///
	/// @brief
	///     设置有关值是否派生（通过计算）得来
	///
	/// @param[in]
	///     bDerived - true则为派生,否则相反
	///
	void SetDerived(bool bDerived);
private:
	uint32_t m_nQuality;
};

class Iec61850GooseQuality
{
public:
	Iec61850GooseQuality();
	Iec61850GooseQuality(uint16_t q);

	///
	/// @brief
	///     获取Quality（品质因数）值
	///
	/// @return
	///     Quality（品质因数）值
	///
	uint16_t GetQuality();

	///
	/// @brief
	///     设置Quality（品质因数）值
	///
	/// @param[in]
	///     q - Quality（品质因数）值
	///
	void SetQuality(uint16_t q);

	///
	/// @brief
	///     获取有效性信息
	///
	/// @return
	///     有效性信息
	///
	IEC61850_QUALITY_VALIDITY_TYPE GetValidity();

	///
	/// @brief
	///     设置有效性
	///
	/// @param[in]
	///     eValidity - 有效性值
	///
	void SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity);

	///
	/// @brief
	///     判断有关值是否溢出
	///
	/// @return
	///     true  - 溢出
	///     false - 未溢出
	///
	bool IsOverflow();

	///
	/// @brief
	///     设置有关值是否溢出
	///
	/// @param[in]
	///     bOverflow - true则溢出,否则相反
	///
	void SetOverflow( bool bOverflow);

	///
	/// @brief
	///     判断有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOutofRange();

	///
	/// @brief
	///     设置有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
	///
	/// @param[in]
	///     bOutofRange - true则超量程,否则相反
	///
	void SetOutofRange(bool bOutofRange);

	///
	/// @brief
	///     判断有关值是否坏基准值（基准值无法校准）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsBadReference();

	///
	/// @brief
	///     设置有关值是否坏基准值（基准值无法校准）
	///
	/// @param[in]
	///     bBadReference - true则坏基准值,否则相反
	///
	void SetBadReference(bool bBadReference);

	///
	/// @brief
	///     判断有关值是否振荡（抖动）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOscillatory();

	///
	/// @brief
	///     设置有关值是否振荡（抖动）
	///
	/// @param[in]
	///     bOscillatory - true则抖动,否则相反
	///
	void SetOscillatory(bool bOscillatory);

	///
	/// @brief
	///     判断有关值是否为故障值（监视功能检出一个内部或外部故障）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsFailure();

	///
	/// @brief
	///     设置有关值是否故障值（监视功能检出一个内部或外部故障）
	///
	/// @param[in]
	///     bFail - true则故障,否则相反
	///
	void SetFailure(bool bFail);

	///
	/// @brief
	///     判断有关值是否为旧数据（在给定时间间隔内不可用）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOldData();

	///
	/// @brief
	///     设置有关值是否旧数据（在给定时间间隔内不可用）
	///
	/// @param[in]
	///     bOldData - true则为旧数据,否则相反
	///
	void SetOldData(bool bOldData);

	///
	/// @brief
	///     判断有关值是否不一致（评估功能已检出不一致）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsInconsistent();

	///
	/// @brief
	///     设置有关值是否不一致（评估功能已检出不一致）
	///
	/// @param[in]
	///     bInconsistent - true则为不一致,否则相反
	///
	void SetInconsistent(bool bInconsistent);

	///
	/// @brief
	///     判断有关值是否不精确（该值不满足源所声明的精度）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsInaccurate();

	///
	/// @brief
	///     设置有关值是否不精确（该值不满足源所声明的精度）
	///
	/// @param[in]
	///     bInaccurate - true则为不精确,否则相反
	///
	void SetInaccurate(bool bInaccurate);

	///
	/// @brief
	///     获取有关值的来源信息
	///
	/// @return
	///     来源的类型
	///
	IEC61850_QUALITY_SOURCE_TYPE GetSourceType();

	///
	/// @brief
	///     设置有关值的来源类型
	///
	/// @param[in]
	///     eSourceType - 来源的类型
	///
	void SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType);

	///
	/// @brief
	///     判断是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsTest();

	///
	/// @brief
	///     设置是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
	///
	/// @param[in]
	///     bTest - true则处于测试/检修状态,否则相反
	///
	void SetTest(bool bTest);

	///
	/// @brief
	///     判断值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
	///
	/// @return
	///     true  - 是
	///     false - 否
	///
	bool IsOperatorBlocked();

	///
	/// @brief
	///     设置值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
	///
	/// @param[in]
	///     bOperatorBlocked - true则被操作员闭锁,否则相反
	///
	void SetOperatorBlocked(bool bOperatorBlocked);

protected:
	uint16_t uint16_BitReverse(uint16_t inData);

private:
	uint16_t m_nQuality;
};

#endif
