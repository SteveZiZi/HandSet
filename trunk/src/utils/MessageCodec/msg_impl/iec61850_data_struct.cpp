#include "stdafx.h"
#include "../include/iec61850_data_struct.h"

//
//=============================================================================
//    FT3帧中的状态字的编解码接口的实现
//=============================================================================
//

///
/// @brief
///     获取状态字某位所使用的掩码
///
#define FT3SW_BIT_MASK 0x0001

///
/// @brief
///     FT3帧（标准/扩展/互感器）中常规属性的bit的索引号
///
#define FT3SW_MAINTENANCE_REQUIRED 0             ///< 设备要求维修（良好/要求维修）
#define FT3SW_TEST_MODE            1             ///< 设备工作状态（正常运行/试验）
#define FT3SW_CT_OUTPUT_TYPE       12            ///< CT输出类型（i(t) 或者 d(i(t))/dt）
#define FT3SW_RANGE_FLAG           13            ///< RangeFlag（SCP=0x01CF或者0x00E7）

///
/// @brief
///     标准/扩展FT3帧中关于MU属性的bit的索引号
///
#define FT3SW_MU_WAKE_UP_TIME      2             ///< MU的唤醒时间指示
#define FT3SW_MU_SYNC_METHOD       3             ///< MU的同步方法
#define FT3SW_MU_SYNC_FLAG         4             ///< MU的同步状态


///
/// @brief
///     标准FT3帧中代表通道有效性的bit的索引号
///
static uint8_t g_MuFt3StdFrameChannelBit[] = {
	// 状态字1中的bit_n
	5,6,7,8,9,10,11,
	// 状态字2中的bit_n
	0,1,2,3,4
};

///
/// @brief
///     标准FT3帧中备用位的索引号
///
static uint8_t g_MuFt3StdFrameFutureUseBit[] = {
	// 状态字1中的bit_n
	14,15,
	// 状态字2中的bit_n
	5,6,7,8,9,10,11,12,13,14,15
};

///
/// @brief
///     扩展FT3帧中代表通道有效性的bit的索引号
///
static uint8_t g_MuFt3ExFrameChannelBit[] = {
	// 状态字1中的bit_n
	5,6,7,8,9,10,11,
	// 状态字2中的bit_n
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
};

///
/// @brief
///     扩展FT3帧中备用位的索引号
///
static uint8_t g_MuFt3ExFrameFutureUseBit[] = {
	// 状态字1中的bit_n
	14,15,
	// 状态字2中的bit_n
	15
};

///
/// @brief
///     电流/电压互感器FT3帧中关于互感器属性的bit的索引号
///
#define FT3SW_CTORVT_FIRING_TIME        FT3SW_MU_WAKE_UP_TIME    ///< 互感器的激发时间指示
#define FT3SW_CTORVT_SYNC_METHOD        FT3SW_MU_SYNC_METHOD     ///< 互感器的同步方法
#define FT3SW_CTORVT_SYNC_FLAG          FT3SW_MU_SYNC_FLAG       ///< 互感器的同步状态
#define FT3SW_CTORVT_AD_ERROR           8                        ///< A/D错误
#define FT3SW_CTORVT_POWER1ST_ABNOR     9                        ///< 1号电源异常
#define FT3SW_CTORVT_POWER2ND_ABNOR     10                       ///< 2号电源异常
#define FT3SW_CTORVT_HVPS_VALIDITY      11                       ///< 高压电源无效
#define FT3SW_CTORVT_LASER_POWER_STATUS 12                       ///< 激光电源

///
/// @brief
///     电流/电压互感器FT3帧中代表通道有效性的bit的索引号
///
static uint8_t g_CtorVtFt3FrameChannelBit[] = {
	// 状态字1中的bit_n
	5,6,7,8,9,10,11,
	// 状态字2中的bit_n
	0,1,2,3,4,5,6,7
};

///
/// @brief
///     电流/电压互感器FT3帧中备用位的索引号
///
static uint8_t g_CtorVtFt3FrameFutureUseBit[] = {
	// 状态字1中的bit_n
	14,15,
	// 状态字2中的bit_n
	15
};

Ft3FrameStatusWord::Ft3FrameStatusWord()
: m_nStatusWord1(0)
, m_nStatusWord2(0)
, m_eFrameType(FT3_FRAME_TYPE_MU_STD)
, m_nChannelBitsNum(sizeof(g_MuFt3StdFrameChannelBit))
, m_nFutureUseBitsNum(sizeof(g_MuFt3StdFrameFutureUseBit))
, m_pChannelBit(g_MuFt3StdFrameChannelBit)
, m_pFutureUseBit(g_MuFt3StdFrameFutureUseBit)
{

}

Ft3FrameStatusWord::Ft3FrameStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2, FT3_FRAME_TYPE eFrameType)
: m_nStatusWord1(nStatusWord1)
, m_nStatusWord2(nStatusWord2)
, m_eFrameType(eFrameType)
{
	switch ( eFrameType )
	{
	case FT3_FRAME_TYPE_MU_STD:
		{
			m_nChannelBitsNum   = sizeof(g_MuFt3StdFrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_MuFt3StdFrameFutureUseBit);
			m_pChannelBit       = g_MuFt3StdFrameChannelBit;
			m_pFutureUseBit     = g_MuFt3StdFrameFutureUseBit;
		}
		break;
	case FT3_FRAME_TYPE_MU_EX:
		{
			m_nChannelBitsNum   = sizeof(g_MuFt3ExFrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_MuFt3ExFrameFutureUseBit);
			m_pChannelBit       = g_MuFt3ExFrameChannelBit;
			m_pFutureUseBit     = g_MuFt3ExFrameFutureUseBit;
		}
		break;
	case FT3_FRAME_TYPE_CTORVT:
		{
			m_nChannelBitsNum   = sizeof(g_CtorVtFt3FrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_CtorVtFt3FrameFutureUseBit);
			m_pChannelBit       = g_CtorVtFt3FrameChannelBit;
			m_pFutureUseBit     = g_CtorVtFt3FrameFutureUseBit;
		}
		break;
	default:
		{
			m_nChannelBitsNum   = 0;
			m_nFutureUseBitsNum = 0;
			m_pChannelBit       = NULL;
			m_pFutureUseBit     = NULL;
		}
	}
}

///
/// @brief
///     设置该状态字所属FT3帧的类型
///
/// @param[in]
///     eFrameType - FT3帧的类型
///
/// @return
///     
///
void Ft3FrameStatusWord::SetFt3FrameType(FT3_FRAME_TYPE eFrameType)
{
	m_eFrameType = eFrameType;

	switch ( eFrameType )
	{
	case FT3_FRAME_TYPE_MU_STD:
		{
			m_nChannelBitsNum   = sizeof(g_MuFt3StdFrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_MuFt3StdFrameFutureUseBit);
			m_pChannelBit       = g_MuFt3StdFrameChannelBit;
			m_pFutureUseBit     = g_MuFt3StdFrameFutureUseBit;
		}
		break;
	case FT3_FRAME_TYPE_MU_EX:
		{
			m_nChannelBitsNum   = sizeof(g_MuFt3ExFrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_MuFt3ExFrameFutureUseBit);
			m_pChannelBit       = g_MuFt3ExFrameChannelBit;
			m_pFutureUseBit     = g_MuFt3ExFrameFutureUseBit;
		}
		break;
	case FT3_FRAME_TYPE_CTORVT:
		{
			m_nChannelBitsNum   = sizeof(g_CtorVtFt3FrameChannelBit);
			m_nFutureUseBitsNum = sizeof(g_CtorVtFt3FrameFutureUseBit);
			m_pChannelBit       = g_CtorVtFt3FrameChannelBit;
			m_pFutureUseBit     = g_CtorVtFt3FrameFutureUseBit;
		}
		break;
	default:
		{
			m_nChannelBitsNum   = 0;
			m_nFutureUseBitsNum = 0;
			m_pChannelBit       = NULL;
			m_pFutureUseBit     = NULL;
		}
	}
}

///
/// @brief
///     获取状态字
///
/// @param[in]
///     nStatusWord1 - 用于保存状态字1
/// @param[in]
///     nStatusWord2 - 用于保存状态字2
///
/// @return
///     
///
void Ft3FrameStatusWord::GetStatusWord(uint16_t &nStatusWord1, uint16_t &nStatusWord2)
{
	nStatusWord1 = m_nStatusWord1;
	nStatusWord2 = m_nStatusWord2;
}

///
/// @brief
///     设置状态字
///
/// @param[in]
///     nStatusWord1 - 状态字1
/// @param[in]
///     nStatusWord2 - 状态字2
///
/// @return
///     
///
void Ft3FrameStatusWord::SetStatusWord(uint16_t nStatusWord1, uint16_t nStatusWord2)
{
	m_nStatusWord1 = nStatusWord1;
	m_nStatusWord2 = nStatusWord2;
}

///
/// @brief
///     获取设备（MU或互感器）的故障状态信息（是否要求维修）
///
/// @return
///     0    - 良好（不要求维修）
///     1    - 警告或报警（要求维修）
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsMaintenanceRequired()
{
	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_MAINTENANCE_REQUIRED)) != 0 )
		return 1;
	else
		return 0;
}

///
/// @brief
///     设置设备（MU或互感器）的故障状态（是否要求维修）
///
/// @param[in]
///     bRequired - true则要求维修,否则相反
///
/// @return
///
void Ft3FrameStatusWord::SetMaintenanceRequired(bool bRequired)
{
	if ( bRequired )
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_MAINTENANCE_REQUIRED);
	else
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_MAINTENANCE_REQUIRED);
}

///
/// @brief
///     获取设备（MU或互感器）的工作状态信息（是否处于试验状态）
///
/// @return
///     0    - 接通（正常运行）
///     1    - 试验
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsTestMode()
{
	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_TEST_MODE)) != 0 )
		return 1;
	else
		return 0;
}

///
/// @brief
///     设置设备（MU或互感器）的工作状态信息（是否处于试验状态）
///
/// @param[in]
///     bTest - true则为试验,否则为接通
///
/// @return
///
void Ft3FrameStatusWord::SetTestMode(bool bTest)
{
	if ( bTest )
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_TEST_MODE);
	else
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_TEST_MODE);
}

///
/// @brief
///     获取FT3帧中CT的输出类型
///
/// @return
///     0    - 输出为i(t)
///     1    - 输出为d(i(t))/dt
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::GetCtOutputType()
{
	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_CT_OUTPUT_TYPE)) != 0 )
		return 1;
	else
		return 0;
}

///
/// @brief
///     设置FT3帧中CT的输出类型
///
/// @param[in]
///     bOutputIt - true则CT输出为i(t),否则为d(i(t))/dt
///
/// @return
///
void Ft3FrameStatusWord::SetCtOutputType(bool bOutputIt)
{
	if ( bOutputIt )
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_CT_OUTPUT_TYPE);
	else
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_CT_OUTPUT_TYPE);
}

///
/// @brief
///     获取比例因子SCP的RangeFlag信息
///
/// @return
///     0    - 比例因子SCP=0x01CF
///     1    - 比例因子SCP=0x00E7
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::GetRangeFlag()
{
	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_RANGE_FLAG)) != 0 )
		return 1;
	else
		return 0;
}

///
/// @brief
///     设置比例因子SCP的RangeFlag
///
/// @param[in]
///     bRangeFlag0 - true则RangeFlag=0,即SCP=0x01CF,否则RangeFlag=1
///
/// @return
///
void Ft3FrameStatusWord::SetRangeFlag(bool bRangeFlag0)
{
	if ( bRangeFlag0 )
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_RANGE_FLAG);
	else
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_RANGE_FLAG);
}

///
/// @brief
///     获取MU的唤醒时间指示信息
///
/// @return
///     0    - 接通（正常运行），此时数据有效
///     1    - 唤醒期间，此时数据无效
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsMuWakingUp()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return -1;

	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_MU_WAKE_UP_TIME)) != 0 )
		return 1;
	else
		return 0;
}

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
bool Ft3FrameStatusWord::SetMuWakingUp(bool bWakingUp)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return false;

	if ( bWakingUp )
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_MU_WAKE_UP_TIME);
	else
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_MU_WAKE_UP_TIME);
	return true;
}

///
/// @brief
///     获取MU的同步方法信息
///
/// @return
///     0    - 数据集不采用插值法
///     1    - 数据集适用于插值法
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::GetMuSyncMethod()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return -1;

	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_MU_SYNC_METHOD)) != 0 )
		return 1;
	else
		return 0;
}

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
bool Ft3FrameStatusWord::SetMuSyncMethod(bool bUseInterpolation)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return false;

	if ( bUseInterpolation )
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_MU_SYNC_METHOD);
	else
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_MU_SYNC_METHOD);
	return true;
}

///
/// @brief
///     获取MU的同步状态信息（是否同步）
///
/// @return
///     0    - 样本同步
///     1    - 时间同步消逝/无效
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsMuNotSync()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return -1;

	if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_MU_SYNC_FLAG)) != 0 )
		return 1;
	else
		return 0;
}

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
bool Ft3FrameStatusWord::SetMuSync(bool bSync)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
		return false;

	if ( !bSync )
		m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_MU_SYNC_FLAG);
	else
		m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_MU_SYNC_FLAG);
	return true;
}

///
/// @brief
///     获取互感器的激发时间指示信息
///
/// @return
///     0    - 接通（正常运行），此时数据有效
///     1    - 激发期间，此时数据无效
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtFiring()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_CTORVT_FIRING_TIME)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtFiring(bool bFiring)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( bFiring )
			m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_FIRING_TIME);
		else
			m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_FIRING_TIME);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的同步方法信息
///
/// @return
///     0    - 数据集不采用插值法
///     1    - 数据集适用于插值法
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::GetCtorVtSyncMethod()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_METHOD)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtSyncMethod(bool bUseInterpolation)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( bUseInterpolation )
			m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_METHOD);
		else
			m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_METHOD);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的同步状态信息
///
/// @return
///     0    - 样本同步
///     1    - 时间同步消逝/无效
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtNotSync()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord1 & (FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_FLAG)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtSync(bool bSync)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( !bSync )
			m_nStatusWord1 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_FLAG);
		else
			m_nStatusWord1 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_SYNC_FLAG);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的A/D转换的状态信息
///
/// @return
///     0    - A/D转换正确
///     1    - A/D转换错误
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtAdError()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord2 & (FT3SW_BIT_MASK << FT3SW_CTORVT_AD_ERROR)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtAdError(bool bError)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( bError )
			m_nStatusWord2 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_AD_ERROR);
		else
			m_nStatusWord2 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_AD_ERROR);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的1号电源的状态信息
///
/// @return
///     0    - 正常
///     1    - 异常
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtPower1stAbnor()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord2 & (FT3SW_BIT_MASK << FT3SW_CTORVT_POWER1ST_ABNOR)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtPower1stAbnor(bool bAbnor)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( bAbnor )
			m_nStatusWord2 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_POWER1ST_ABNOR);
		else
			m_nStatusWord2 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_POWER1ST_ABNOR);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的2号电源的状态信息
///
/// @return
///     0    - 正常
///     1    - 异常
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtPower2ndAbnor()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord2 & (FT3SW_BIT_MASK << FT3SW_CTORVT_POWER2ND_ABNOR)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtPower2ndAbnor(bool bAbnor)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( bAbnor )
			m_nStatusWord2 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_POWER2ND_ABNOR);
		else
			m_nStatusWord2 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_POWER2ND_ABNOR);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的高压电源的有效性信息
///
/// @return
///     0    - 有效
///     1    - 无效
///     其他 - 获取失败
///
int  Ft3FrameStatusWord::IsCtorVtHvpsInvalid()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( (m_nStatusWord2 & (FT3SW_BIT_MASK << FT3SW_CTORVT_HVPS_VALIDITY)) != 0 )
			return 1;
		else
			return 0;
	}
	return -1;
}

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
bool Ft3FrameStatusWord::SetCtorVtHvpsValidity(bool bValid)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		if ( !bValid )
			m_nStatusWord2 |= (FT3SW_BIT_MASK << FT3SW_CTORVT_HVPS_VALIDITY);
		else
			m_nStatusWord2 &= ~(FT3SW_BIT_MASK << FT3SW_CTORVT_HVPS_VALIDITY);
		return true;
	}
	return false;
}

///
/// @brief
///     获取互感器的激光电源的动作/状态信息
///
/// @return
///     激光电源的动作/状态
///
FT3_CTORVT_LASER_POWER_STATUS Ft3FrameStatusWord::GetCtorVtLaserPowerStatus()
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		uint16_t nStatus = m_nStatusWord2 >> FT3SW_CTORVT_LASER_POWER_STATUS;
		nStatus &= 0x0007;
		if ( nStatus == 0 )
			return FT3CLPS_MAINTAIN_OUTPUT;
		else if ( nStatus == 1 )
			return FT3CLPS_DOWN_TO_MAINTAIN_POWER;
		else if ( nStatus == 2 )
			return FT3CLPS_DECREASE_OUTPUT_POWER;
		else if ( nStatus == 3 )
			return FT3CLPS_INCREASE_OUTPUT_POWER;
		else if ( nStatus == 5 )
			return FT3CLPS_SPEED_INCREASE_OUTPUT_POWER;
		else if ( nStatus == 7 )
			return FT3CLPS_FAILURE;
	}
	return FT3CLPS_UNKNOWN;
}

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
bool Ft3FrameStatusWord::SetCtorVtLaserPowerStatus(FT3_CTORVT_LASER_POWER_STATUS eStatus)
{
	if ( m_eFrameType == FT3_FRAME_TYPE_CTORVT )
	{
		uint8_t nStatus = 0;
		switch (eStatus)
		{
		case FT3CLPS_MAINTAIN_OUTPUT:
			nStatus = 0;
			break;
		case FT3CLPS_DOWN_TO_MAINTAIN_POWER:
			nStatus = 1;
			break;
		case FT3CLPS_DECREASE_OUTPUT_POWER:
			nStatus = 2;
			break;
		case FT3CLPS_INCREASE_OUTPUT_POWER:
			nStatus = 3;
			break;
		case FT3CLPS_SPEED_INCREASE_OUTPUT_POWER:
			nStatus = 5;
			break;
		case FT3CLPS_FAILURE:
			nStatus = 7;
			break;
		default:
			return false;
		}
		m_nStatusWord2 &= 0x8FFF;
		m_nStatusWord2 |= (nStatus<<FT3SW_CTORVT_LASER_POWER_STATUS);
		return true;
	}
	return false;
}

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
int  Ft3FrameStatusWord::IsChannelInvalid(uint32_t nChannelIdx)
{
	if ( nChannelIdx >= m_nChannelBitsNum )
		return -1;

	uint16_t *pStatusWord = &m_nStatusWord2;
	if ( nChannelIdx < 7 )
		pStatusWord = &m_nStatusWord1;

	if ( ((*pStatusWord) & (FT3SW_BIT_MASK << m_pChannelBit[nChannelIdx])) != 0 )
		return 1;
	else
		return 0;
}

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
bool Ft3FrameStatusWord::SetChannelValidity(uint32_t nChannelIdx, bool bValid)
{
	if ( nChannelIdx >= m_nChannelBitsNum )
		return false;

	uint16_t *pStatusWord = &m_nStatusWord2;
	if ( nChannelIdx < 7 )
		pStatusWord = &m_nStatusWord1;

	if ( !bValid )
		*pStatusWord |= (FT3SW_BIT_MASK << m_pChannelBit[nChannelIdx]);
	else
		*pStatusWord &= ~(FT3SW_BIT_MASK << m_pChannelBit[nChannelIdx]);
	
	return true;
}

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
int  Ft3FrameStatusWord::GetFutureUsedBitVal(uint32_t nBitIdx)
{
	if ( nBitIdx >= m_nFutureUseBitsNum )
		return -1;

	uint16_t *pStatusWord = &m_nStatusWord2;
	if ( nBitIdx < 2 )
		pStatusWord = &m_nStatusWord1;

	if ( ((*pStatusWord) & (FT3SW_BIT_MASK << m_pFutureUseBit[nBitIdx])) == 0 )
		return 0;
	else
		return 1;
}

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
bool Ft3FrameStatusWord::SetFutureUsedBitVal(uint32_t nBitIdx, bool bBitVal)
{
	if ( nBitIdx >= m_nFutureUseBitsNum )
		return false;

	uint16_t *pStatusWord = &m_nStatusWord2;
	if ( nBitIdx < 2 )
		pStatusWord = &m_nStatusWord1;

	if ( bBitVal )
		*pStatusWord |= (FT3SW_BIT_MASK << m_pFutureUseBit[nBitIdx]);
	else
		*pStatusWord &= ~(FT3SW_BIT_MASK << m_pFutureUseBit[nBitIdx]);

	return true;
}

//
//=============================================================================
//    IEC61850定义的TimeStamp类型中的TimeQuality的编解码接口的实现
//=============================================================================
//

///
/// @brief
///     获取TimeQuality中某位所使用的掩码
///
#define IEC61850_TQ_BIT_MASK 0x01

///
/// @brief
///     TimeQuality中各属性的bit的索引号
///
#define IEC61850_TQ_LEAP_SECOND_KNOWN 7          ///< 闰秒已知
#define IEC61850_TQ_CLOCK_FAILURE 6              ///< 时钟故障
#define IEC61850_TQ_CLOCK_NOT_SYNC 5             ///< 时钟未同步
#define IEC61850_TQ_SECOND_ACCURACY 0            ///< TimeStamp中秒的小数部分的时间精度（bit4~0）

Iec61850TimeQuality::Iec61850TimeQuality()
: m_nTimeQuality(0)
{

}
Iec61850TimeQuality::Iec61850TimeQuality(uint8_t tq)
: m_nTimeQuality(tq)
{

}

///
/// @brief
///     获取TimeQuality
///
/// @return
///     TimeQuality值
///
uint8_t Iec61850TimeQuality::GetTimeQuality()
{
	return m_nTimeQuality;
}

///
/// @brief
///     设置TimeQuality
///
/// @param[in]
///     tq - TimeQuality值
///
void Iec61850TimeQuality::SetTimeQuality(uint8_t tq)
{
	m_nTimeQuality = tq;
}

///
/// @brief
///     判断闰秒是否已知
///
/// @return
///     true  - 已知
///     false - 未知
///
bool Iec61850TimeQuality::IsLeapSecondKnown()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_LEAP_SECOND_KNOWN)) != 0 );
}

///
/// @brief
///     设置闰秒是否已知
///
/// @param[in]
///     bKnown - true则已知,否则相反
///
void Iec61850TimeQuality::SetLeapSecondKnown(bool bKnown)
{
	if ( bKnown )
		m_nTimeQuality |= (IEC61850_TQ_BIT_MASK << IEC61850_TQ_LEAP_SECOND_KNOWN);
	else
		m_nTimeQuality &= ~(IEC61850_TQ_BIT_MASK << IEC61850_TQ_LEAP_SECOND_KNOWN);
}

///
/// @brief
///     判断时钟是否故障
///
/// @return
///     true  - 故障
///     false - 正常
///
bool Iec61850TimeQuality::IsClockFailure()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_FAILURE)) != 0 );
}

///
/// @brief
///     设置时钟故障状态
///
/// @param[in]
///     bFail - true则时钟故障,否则相反
///
/// @return
///
void Iec61850TimeQuality::SetClockFailure(bool bFail)
{
	if ( bFail )
		m_nTimeQuality |= (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_FAILURE);
	else
		m_nTimeQuality &= ~(IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_FAILURE);
}

///
/// @brief
///     判断时钟是否未同步
///
/// @return
///     true  - 未同步
///     false - 同步
///
bool Iec61850TimeQuality::IsClockNotSync()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_NOT_SYNC)) != 0 );
}

///
/// @brief
///     设置时钟同步状态
///
/// @param[in]
///     bSync - true则时钟同步,否则相反
///
/// @return
///
void Iec61850TimeQuality::SetClockSync(bool bSync)
{
	if ( !bSync )
		m_nTimeQuality |= (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_NOT_SYNC);
	else
		m_nTimeQuality &= ~(IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_NOT_SYNC);
}

///
/// @brief
///     获取IEC61850时间戳的秒的小数部分的时间精度
///
/// @return
///     -1   - 非法
///     -2   - 未规定
///     其他 - 数值对应整数位精度（0~23位精度）
///
int  Iec61850TimeQuality::GetSecondAccuracy()
{
	uint8_t nAccuracy = m_nTimeQuality >> IEC61850_TQ_SECOND_ACCURACY;
	nAccuracy &= 0x1F;
	if ( nAccuracy < 0x18 )
		return (int)nAccuracy;
	else if ( nAccuracy < 0x1E )
		return -1;
	else
		return -2;
}

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
bool Iec61850TimeQuality::SetSecondAccuracy(int nAccuracy)
{
	if ( nAccuracy < 0 || nAccuracy >= 0x18 )
		return false;

	m_nTimeQuality &= 0xE0;
	m_nTimeQuality += (uint8_t)nAccuracy;

	return true;
}

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
///     获取IEC61850中定义的基本类型Quality中某位所使用的掩码
///
#define IEC61850_QUALITY_BIT_MASK 0x00000001

///
/// @brief
///     IEC61850中定义的基本类型Quality的各个属性所在的bit的索引号
///
#define IEC61850_QUALITY_VALIDITY         0    ///< 合法性
#define IEC61850_QUALITY_OVERFLOW         2    ///< 溢出
#define IEC61850_QUALITY_OUTOFRANGE       3    ///< 超量程
#define IEC61850_QUALITY_BADREFERENCE     4    ///< 坏基准值
#define IEC61850_QUALITY_OSCILLATORY      5    ///< 振荡（抖动）
#define IEC61850_QUALITY_FAILURE          6    ///< 故障
#define IEC61850_QUALITY_OLDDATA          7    ///< 旧数据
#define IEC61850_QUALITY_INCONSISTENT     8    ///< 不一致
#define IEC61850_QUALITY_INACCURATE       9    ///< 不准确
#define IEC61850_QUALITY_SOURCE           10   ///< 源
#define IEC61850_QUALITY_TEST             11   ///< 测试/检修
#define IEC61850_QUALITY_OPERATOR_BLOCKED 12   ///< 操作员闭锁
#define IEC61850_QUALITY_DERIVED          13   ///< 派生，表示相关值是否通过计算得来，该位为IEC61850-9-2LE的扩充定义

Iec61850SmvQuality::Iec61850SmvQuality()
: m_nQuality(0)
{

}
Iec61850SmvQuality::Iec61850SmvQuality(uint32_t q)
: m_nQuality(q)
{

}

///
/// @brief
///     获取Quality（品质因数）值
///
/// @return
///     Quality（品质因数）值
///
uint32_t Iec61850SmvQuality::GetQuality()
{
	return m_nQuality;
}

///
/// @brief
///     设置Quality（品质因数）值
///
/// @param[in]
///     q - Quality（品质因数）值
///
void Iec61850SmvQuality::SetQuality(uint32_t q)
{
	m_nQuality = q;
}

///
/// @brief
///     获取有效性信息
///
/// @return
///     有效性信息
///
IEC61850_QUALITY_VALIDITY_TYPE Iec61850SmvQuality::GetValidity()
{
	uint32_t nValidity = m_nQuality >> IEC61850_QUALITY_VALIDITY;
	nValidity &= 0x00000003;
	if ( nValidity == 0 )
		return IEC61850_QV_GOOD;
	else if ( nValidity == 1 )
		return IEC61850_QV_INVALID;
	else if ( nValidity == 2 )
		return IEC61850_QV_RESERVED;
	else
		return IEC61850_QV_QUESTIONABLE;
}

///
/// @brief
///     设置有效性
///
/// @param[in]
///     eValidity - 有效性值
///
/// @return
///     
///
void Iec61850SmvQuality::SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity)
{
	uint32_t nValidity = 0;
	switch ( eValidity )
	{
	case IEC61850_QV_GOOD:
		nValidity = 0;
		break;
	case IEC61850_QV_INVALID:
		nValidity = 1;
		break;
	case IEC61850_QV_RESERVED:
		nValidity = 2;
		break;
	case IEC61850_QV_QUESTIONABLE:
		nValidity = 3;
		break;
	default:
		nValidity = 0;
	}

	m_nQuality &= 0xFFFFFFFC;
	m_nQuality += (uint8_t)nValidity;
}

///
/// @brief
///     判断有关值是否溢出
///
/// @return
///     true  - 溢出
///     false - 未溢出
///
bool Iec61850SmvQuality::IsOverflow()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW)) != 0 );
}

///
/// @brief
///     设置有关值是否溢出
///
/// @param[in]
///     bOverflow - true则溢出,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetOverflow( bool bOverflow)
{
	if ( bOverflow )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW);
}

///
/// @brief
///     判断有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsOutofRange()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE)) != 0 );
}

///
/// @brief
///     设置有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
///
/// @param[in]
///     bOutofRange - true则超量程,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetOutofRange(bool bOutofRange)
{
	if ( bOutofRange )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE);
}

///
/// @brief
///     判断有关值是否坏基准值（基准值无法校准）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsBadReference()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE)) != 0 );
}

///
/// @brief
///     设置有关值是否坏基准值（基准值无法校准）
///
/// @param[in]
///     bBadReference - true则坏基准值,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetBadReference(bool bBadReference)
{
	if ( bBadReference )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE);
}

///
/// @brief
///     判断有关值是否振荡（抖动）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsOscillatory()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY)) != 0 );
}

///
/// @brief
///     设置有关值是否振荡（抖动）
///
/// @param[in]
///     bOscillatory - true则抖动,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetOscillatory(bool bOscillatory)
{
	if ( bOscillatory )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY);
}

///
/// @brief
///     判断有关值是否为故障值（监视功能检出一个内部或外部故障）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsFailure()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE)) != 0 );
}

///
/// @brief
///     设置有关值是否故障值（监视功能检出一个内部或外部故障）
///
/// @param[in]
///     bFail - true则故障,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetFailure(bool bFail)
{
	if ( bFail )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE);
}

///
/// @brief
///     判断有关值是否为旧数据（在给定时间间隔内不可用）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsOldData()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA)) != 0 );
}

///
/// @brief
///     设置有关值是否旧数据（在给定时间间隔内不可用）
///
/// @param[in]
///     bOldData - true则为旧数据,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetOldData(bool bOldData)
{
	if ( bOldData )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA);
}

///
/// @brief
///     判断有关值是否不一致（评估功能已检出不一致）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsInconsistent()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT)) != 0 );
}

///
/// @brief
///     设置有关值是否不一致（评估功能已检出不一致）
///
/// @param[in]
///     bInconsistent - true则为不一致,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetInconsistent(bool bInconsistent)
{
	if ( bInconsistent )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT);
}

///
/// @brief
///     判断有关值是否不精确（该值不满足源所声明的精度）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsInaccurate()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE)) != 0 );
}

///
/// @brief
///     设置有关值是否不精确（该值不满足源所声明的精度）
///
/// @param[in]
///     bInaccurate - true则为不精确,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetInaccurate(bool bInaccurate)
{
	if ( bInaccurate )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE);
}

///
/// @brief
///     获取有关值的来源信息
///
/// @return
///     来源的类型
///
IEC61850_QUALITY_SOURCE_TYPE Iec61850SmvQuality::GetSourceType()
{
	if ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE)) != 0 )
		return IEC61850_QST_SUBSTITUTED;
	else
		return IEC61850_QST_PROCESS;
}

///
/// @brief
///     设置有关值的来源类型
///
/// @param[in]
///     eSourceType - 来源的类型
///
/// @return
///
///
void Iec61850SmvQuality::SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType)
{
	if ( eSourceType == IEC61850_QST_SUBSTITUTED )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE);
}

///
/// @brief
///     判断是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsTest()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST)) != 0 );
}

///
/// @brief
///     设置是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
///
/// @param[in]
///     bTest - true则处于测试/检修状态,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetTest(bool bTest)
{
	if ( bTest )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST);
}

///
/// @brief
///     判断值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsOperatorBlocked()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED)) != 0 );
}

///
/// @brief
///     设置值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
///
/// @param[in]
///     bOperatorBlocked - true则被操作员闭锁,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetOperatorBlocked(bool bOperatorBlocked)
{
	if ( bOperatorBlocked )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED);
}

///
/// @brief
///     判断有关值是否派生（通过计算）得来
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850SmvQuality::IsDerived()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_DERIVED)) != 0 );
}

///
/// @brief
///     设置有关值是否派生（通过计算）得来
///
/// @param[in]
///     bDerived - true则为派生,否则相反
///
/// @return
///
///
void Iec61850SmvQuality::SetDerived(bool bDerived)
{
	if ( bDerived )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_DERIVED);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_DERIVED);
}

Iec61850GooseQuality::Iec61850GooseQuality()
: m_nQuality(0)
{

}
Iec61850GooseQuality::Iec61850GooseQuality(uint16_t q)
: m_nQuality(q)
{
	// 16位倒序
	m_nQuality = uint16_BitReverse(m_nQuality);
}

///
/// @brief
///     获取Quality（品质因数）值
///
/// @return
///     Quality（品质因数）值
///
uint16_t Iec61850GooseQuality::GetQuality()
{
	return uint16_BitReverse(m_nQuality);
}

///
/// @brief
///     设置Quality（品质因数）值
///
/// @param[in]
///     q - Quality（品质因数）值
///
void Iec61850GooseQuality::SetQuality(uint16_t q)
{
	m_nQuality = uint16_BitReverse(q);
}

///
/// @brief
///     获取有效性信息
///
/// @return
///     有效性信息
///
IEC61850_QUALITY_VALIDITY_TYPE Iec61850GooseQuality::GetValidity()
{
	uint32_t nValidity = m_nQuality >> IEC61850_QUALITY_VALIDITY;
	nValidity &= 0x00000003;
	if ( nValidity == 0 )
		return IEC61850_QV_GOOD;
	else if ( nValidity == 1 )
		return IEC61850_QV_INVALID;
	else if ( nValidity == 2 )
		return IEC61850_QV_RESERVED;
	else
		return IEC61850_QV_QUESTIONABLE;
}

///
/// @brief
///     设置有效性
///
/// @param[in]
///     eValidity - 有效性值
///
/// @return
///     
///
void Iec61850GooseQuality::SetValidity(IEC61850_QUALITY_VALIDITY_TYPE eValidity)
{
	uint32_t nValidity = 0;
	switch ( eValidity )
	{
	case IEC61850_QV_GOOD:
		nValidity = 0;
		break;
	case IEC61850_QV_INVALID:
		nValidity = 1;
		break;
	case IEC61850_QV_RESERVED:
		nValidity = 2;
		break;
	case IEC61850_QV_QUESTIONABLE:
		nValidity = 3;
		break;
	default:
		nValidity = 0;
	}

	m_nQuality &= 0xFFFFFFFC;
	m_nQuality += (uint8_t)nValidity;
}

///
/// @brief
///     判断有关值是否溢出
///
/// @return
///     true  - 溢出
///     false - 未溢出
///
bool Iec61850GooseQuality::IsOverflow()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW)) != 0 );
}

///
/// @brief
///     设置有关值是否溢出
///
/// @param[in]
///     bOverflow - true则溢出,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetOverflow( bool bOverflow)
{
	if ( bOverflow )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW);
}

///
/// @brief
///     判断有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsOutofRange()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE)) != 0 );
}

///
/// @brief
///     设置有关值是否超量程（超出预先定义的量程，非数据类型的有效范围）
///
/// @param[in]
///     bOutofRange - true则超量程,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetOutofRange(bool bOutofRange)
{
	if ( bOutofRange )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE);
}

///
/// @brief
///     判断有关值是否坏基准值（基准值无法校准）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsBadReference()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE)) != 0 );
}

///
/// @brief
///     设置有关值是否坏基准值（基准值无法校准）
///
/// @param[in]
///     bBadReference - true则坏基准值,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetBadReference(bool bBadReference)
{
	if ( bBadReference )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE);
}

///
/// @brief
///     判断有关值是否振荡（抖动）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsOscillatory()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY)) != 0 );
}

///
/// @brief
///     设置有关值是否振荡（抖动）
///
/// @param[in]
///     bOscillatory - true则抖动,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetOscillatory(bool bOscillatory)
{
	if ( bOscillatory )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY);
}

///
/// @brief
///     判断有关值是否为故障值（监视功能检出一个内部或外部故障）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsFailure()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE)) != 0 );
}

///
/// @brief
///     设置有关值是否故障值（监视功能检出一个内部或外部故障）
///
/// @param[in]
///     bFail - true则故障,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetFailure(bool bFail)
{
	if ( bFail )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE);
}

///
/// @brief
///     判断有关值是否为旧数据（在给定时间间隔内不可用）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsOldData()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA)) != 0 );
}

///
/// @brief
///     设置有关值是否旧数据（在给定时间间隔内不可用）
///
/// @param[in]
///     bOldData - true则为旧数据,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetOldData(bool bOldData)
{
	if ( bOldData )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA);
}

///
/// @brief
///     判断有关值是否不一致（评估功能已检出不一致）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsInconsistent()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT)) != 0 );
}

///
/// @brief
///     设置有关值是否不一致（评估功能已检出不一致）
///
/// @param[in]
///     bInconsistent - true则为不一致,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetInconsistent(bool bInconsistent)
{
	if ( bInconsistent )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT);
}

///
/// @brief
///     判断有关值是否不精确（该值不满足源所声明的精度）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsInaccurate()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE)) != 0 );
}

///
/// @brief
///     设置有关值是否不精确（该值不满足源所声明的精度）
///
/// @param[in]
///     bInaccurate - true则为不精确,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetInaccurate(bool bInaccurate)
{
	if ( bInaccurate )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE);
}

///
/// @brief
///     获取有关值的来源信息
///
/// @return
///     来源的类型
///
IEC61850_QUALITY_SOURCE_TYPE Iec61850GooseQuality::GetSourceType()
{
	if ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE)) != 0 )
		return IEC61850_QST_SUBSTITUTED;
	else
		return IEC61850_QST_PROCESS;
}

///
/// @brief
///     设置有关值的来源类型
///
/// @param[in]
///     eSourceType - 来源的类型
///
/// @return
///
///
void Iec61850GooseQuality::SetSourceType(IEC61850_QUALITY_SOURCE_TYPE eSourceType)
{
	if ( eSourceType == IEC61850_QST_SUBSTITUTED )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_SOURCE);
}

///
/// @brief
///     判断是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsTest()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST)) != 0 );
}

///
/// @brief
///     设置是否处于测试/检修状态（有关值为正在测试的值，不能用于运行目的）
///
/// @param[in]
///     bTest - true则处于测试/检修状态,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetTest(bool bTest)
{
	if ( bTest )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST);
}

///
/// @brief
///     判断值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
///
/// @return
///     true  - 是
///     false - 否
///
bool Iec61850GooseQuality::IsOperatorBlocked()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED)) != 0 );
}

///
/// @brief
///     设置值的刷新是否被操作员闭锁（即有关值为闭锁前的值）
///
/// @param[in]
///     bOperatorBlocked - true则被操作员闭锁,否则相反
///
/// @return
///
///
void Iec61850GooseQuality::SetOperatorBlocked(bool bOperatorBlocked)
{
	if ( bOperatorBlocked )
		m_nQuality |= (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED);
	else
		m_nQuality &= ~(IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED);
}

uint16_t Iec61850GooseQuality::uint16_BitReverse(uint16_t inData)
{
	uint16_t outData = inData;
	outData = ((outData & 0xFF00) >> 8) | ((outData & 0x00FF) << 8);
	outData = ((outData & 0xF0F0) >> 4) | ((outData & 0x0F0F) << 4);
	outData = ((outData & 0xCCCC) >> 2) | ((outData & 0x3333) << 2);
	outData = ((outData & 0xAAAA) >> 1) | ((outData & 0x5555) << 1);
	return outData;
}
