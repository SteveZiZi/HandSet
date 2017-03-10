/// @file
///     ieee1588_types.cpp
///
/// @brief
///     用于IEEE1588报文帧编解码的自定义类型数据的处理函数
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.9.29
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "../include/ieee1588_types.h"

//
//=============================================================================
//    IEEE1588报文的类型信息字段
//=============================================================================
//

///
/// @brief
///     获取当前报文所属的类型
///
/// @return
///     当前报文所属的类型
///
IEEE1588_MESSAGE_TYPE IEEE1588_TYPE_FIELD::GetMessageType()
{
	if ( messageType == 0 )
		return IEEE1588_TYPE_SYNC;
	else if ( messageType == 1 )
		return IEEE1588_TYPE_DELAY_REQUEST;
	else if ( messageType == 2 )
		return IEEE1588_TYPE_PDELAY_REQUEST;
	else if ( messageType == 3 )
		return IEEE1588_TYPE_PDELAY_RESPONSE;
	else if ( messageType == 8 )
		return IEEE1588_TYPE_FOLLOW_UP;
	else if ( messageType == 9 )
		return IEEE1588_TYPE_DELAY_RESPONSE;
	else if ( messageType == 0xA )
		return IEEE1588_TYPE_PDELAY_RESPONSE_FOLLOW_UP;
	else if ( messageType == 0xB )
		return IEEE1588_TYPE_ANNOUNCE;
	else if ( messageType == 0xC )
		return IEEE1588_TYPE_SIGNALING;
	else if ( messageType == 0xD )
		return IEEE1588_TYPE_MANAGEMENT;
	else
		return IEEE1588_TYPE_UNKNOWN;
}

///
/// @brief
///     设置当前报文所属的类型
///
/// @param[in]
///     eMessageType - 当前报文所属的类型
///
void IEEE1588_TYPE_FIELD::SetMessageType(IEEE1588_MESSAGE_TYPE eMessageType)
{
	switch ( eMessageType )
	{
	case IEEE1588_TYPE_SYNC:
		messageType = 0;
		break;
	case IEEE1588_TYPE_DELAY_REQUEST:
		messageType = 1;
		break;
	case IEEE1588_TYPE_PDELAY_REQUEST:
		messageType = 2;
		break;
	case IEEE1588_TYPE_PDELAY_RESPONSE:
		messageType = 3;
		break;
	case IEEE1588_TYPE_FOLLOW_UP:
		messageType = 8;
		break;
	case IEEE1588_TYPE_DELAY_RESPONSE:
		messageType = 9;
		break;
	case IEEE1588_TYPE_PDELAY_RESPONSE_FOLLOW_UP:
		messageType = 0xA;
		break;
	case IEEE1588_TYPE_ANNOUNCE:
		messageType = 0xB;
		break;
	case IEEE1588_TYPE_SIGNALING:
		messageType = 0xC;
		break;
	case IEEE1588_TYPE_MANAGEMENT:
		messageType = 0xD;
		break;
	default:
		messageType = 4;
	}
}

//
//=============================================================================
//    IEEE1588报文的报文头中的标志位集合
//=============================================================================
//

///
/// @brief
///     获取IEEE1588报文的报文头中的标志位集合中某位所使用的掩码
///
#define IEEE1588_HEADER_FLAGS_BIT_MASK 0x0001

///
/// @brief
///     IEEE1588报文的报文头中的标志位集合中各个标志所在的bit的索引号
///
#define IEEE1588_HEADER_FLAGS_PTP_SECURITY          15
#define IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC2 14
#define IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC1 13
#define IEEE1588_HEADER_FLAGS_PTP_UNICAST           10
#define IEEE1588_HEADER_FLAGS_PTP_TWO_STEP          9
#define IEEE1588_HEADER_FLAGS_PTP_ALTERNATE_MASTER  8
#define IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE   5
#define IEEE1588_HEADER_FLAGS_TIME_TRACEABLE        4
#define IEEE1588_HEADER_FLAGS_PTP_TIMESCALE         3
#define IEEE1588_HEADER_FLAGS_PTP_UTC_REASONABLE    2
#define IEEE1588_HEADER_FLAGS_PTP_LI_59             1
#define IEEE1588_HEADER_FLAGS_PTP_LI_61             0

///
/// @brief
///     判断发出端口是否为非Master状态
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpAlternateMaster()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_ALTERNATE_MASTER)) != 0 );
}

///
/// @brief
///     设置报文发出端口是否为非Master状态
///
/// @param[in]
///     bAlternateMaster - 为非Master状态则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpAlternateMaster(bool bAlternateMaster)
{
	if ( bAlternateMaster )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_ALTERNATE_MASTER);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_ALTERNATE_MASTER);
}

///
/// @brief
///     判断同步时钟是否为两步法时钟
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpTwoStep()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TWO_STEP)) != 0 );
}

///
/// @brief
///     设置同步时钟是否为两步法时钟
///
/// @param[in]
///     bTwoStep - 两步法时钟则为true，两步法时钟则为false
///
void IEEE1588_HEADER_FLAGS::SetPtpTwoStep(bool bTwoStep)
{
	if ( bTwoStep )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TWO_STEP);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TWO_STEP);
}

///
/// @brief
///     判断该报文是否以单播方式发送
///
/// @return
///     以单播方式发送 - true
///     以多播方式发送 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpUnicast()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UNICAST)) != 0 );
}

///
/// @brief
///     设置该报文是否以单播方式发送
///
/// @param[in]
///     bTwoStep - 以单播方式发送则为true，以多播方式发送则为false
///
void IEEE1588_HEADER_FLAGS::SetPtpUnicast(bool bUnicast)
{
	if ( bUnicast )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UNICAST);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UNICAST);
}


///
/// @brief
///     判断时钟是否被alternate PTP配置文件定义（标志1）
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpProfileSpecific1()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC1)) != 0 );
}

///
/// @brief
///     设置时钟是否被alternate PTP配置文件定义（标志1）
///
/// @param[in]
///     bSpecific - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpProfileSpecific1(bool bSpecific)
{
	if ( bSpecific )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC1);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC1);
}

///
/// @brief
///     判断时钟是否被alternate PTP配置文件定义（标志2）
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpProfileSpecific2()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC2)) != 0 );
}


///
/// @brief
///     设置时钟是否被alternate PTP配置文件定义（标志2）
///
/// @param[in]
///     bSpecific - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpProfileSpecific2(bool bSpecific)
{
	if ( bSpecific )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC2);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC2);
}

///
/// @brief
///     判断报文是否采用了信息安全机制
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpSecurity()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_SECURITY)) != 0 );
}

///
/// @brief
///     设置报文是否采用了信息安全机制
///
/// @param[in]
///     bSecurity - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpSecurity(bool bSecurity)
{
	if ( bSecurity )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_SECURITY);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_SECURITY);
}

///
/// @brief
///     判断当天最后1min是否为61s
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpLI61()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_61)) != 0 );
}

///
/// @brief
///     设置当天最后1min是否为61s
///
/// @param[in]
///     bLI61 - 对于采用PTP时元的对时系统，其值为true则表示当天最后1min为61s；
///             对于不采用PTP时元的对时系统，其值为false。
///
void IEEE1588_HEADER_FLAGS::SetPtpLI61(bool bLI61)
{
	if ( bLI61 )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_61);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_61);
}

///
/// @brief
///     判断当天最后1min是否为59s
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpLI59()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_59)) != 0 );
}

///
/// @brief
///     设置当天最后1min是否为59s
///
/// @param[in]
///     bLI59 - 对于采用PTP时元的对时系统，其值为true则表示当天最后1min为59s；
///             对于不采用PTP时元的对时系统，其值为false。
///
void IEEE1588_HEADER_FLAGS::SetPtpLI59(bool bLI59)
{
	if ( bLI59 )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_59);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_59);
}


///
/// @brief
///     判断是否采用UTC显示时间信息
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpUtcReasonable()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UTC_REASONABLE)) != 0 );
}

///
/// @brief
///     设置是否采用UTC显示时间信息
///
/// @param[in]
///     bUtcReasonable - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpUtcReasonable(bool bUtcReasonable)
{
	if ( bUtcReasonable )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UTC_REASONABLE);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UTC_REASONABLE);
}

///
/// @brief
///     判断最高级超主时钟是否采用PTP时间标尺
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpTimescale()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TIMESCALE)) != 0 );
}

///
/// @brief
///     设置最高级超主时钟是否采用PTP时间标尺
///
/// @param[in]
///     bTimescale - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetPtpTimescale(bool bTimescale)
{
	if ( bTimescale )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TIMESCALE);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TIMESCALE);
}

///
/// @brief
///     判断时间标尺的时间偏移是否可追溯到最高级超主时钟
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsTimeTraceble()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_TIME_TRACEABLE)) != 0 );
}

///
/// @brief
///     设置时间标尺的时间偏移是否可追溯到最高级超主时钟
///
/// @param[in]
///     bTimeTraceble - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetTimeTraceble(bool bTimeTraceble)
{
	if ( bTimeTraceble )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_TIME_TRACEABLE);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_TIME_TRACEABLE);
}

///
/// @brief
///     判断频率决定标尺是否可以追溯到最高级超主时钟
///
/// @return
///     是 - true
///     否 - false
///
bool IEEE1588_HEADER_FLAGS::IsFrequencyTraceable()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE)) != 0 );
}

///
/// @brief
///     设置频率决定标尺是否可以追溯到最高级超主时钟
///
/// @param[in]
///     bTraceable - 是则为true，否则相反
///
void IEEE1588_HEADER_FLAGS::SetFrequencyTraceable(bool bTraceable)
{
	if ( bTraceable )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE);
}
