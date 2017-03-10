#include "stdafx.h"
#include "SmvScaler.h"

///
/// @brief
///     FT3/IEC61850-9-1协议规定通道的比例因子
///
float k_ECT_SCM   = 11585.0f;            ///< 测量用ECT的比例因子
float k_ECT_SCP_0 = 463.0f;              ///< 保护用ECT的比例因子,且rangeflag = 0
float k_ECT_SCP_1 = 231.0f;              ///< 保护用ECT的比例因子,且rangeflag = 1
float k_EVT_SV    = 11585.0f;            ///< EVT的比例因子

///
/// @brief
///     IEC61850-9-2协议规定通道的比例因子
///
float k_Sav92_SC  = 0.001f;              ///< 电流的比例因子
float k_Sav92_SV  = 0.01f;               ///< 电压的比例因子

CSmvScaler::CSmvScaler()
: m_eChannelMsgValType(SMV_VALUE_TYPE_PRIMARY)
, m_eChannelNtrValType(SMV_VALUE_TYPE_PRIMARY)
{

}

///
/// @brief
///     通道的自然值->报文值换算（FT3）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     ntrVal         - 自然值，单位为V/A
/// @param[in]
///     ratedVal1      - 配置中设置的额定1次值，单位为V/A
/// @param[in]
///     ratedVal2      - 配置中设置的额定2次值，单位为V/A
/// @param[in]
///     channelType    - 通道类型
/// @param[in]
///     channelValType - 通道数据类型（测量/保护，用于电流通道）
/// @param[in]
///     rangeFlag      - 额定值范围扩展标志，主要影响保护用ECT的比例因子
///
/// @return
///     报文值
///
uint16_t CSmvScaler::ChannelNtrVal2MsgVal_FT3( float ntrVal, float ratedVal1, float ratedVal2,
											   enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// 协议规定的比例因子
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// 参考额定值
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// 线电压->相电压
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	uint16_t nt = (uint16_t)((int16_t)(ntrVal*ntrValScale/ratedVal*protocolScale));
	return nt;
}

///
/// @brief
///     通道的报文值->自然值换算（FT3）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     msgVal         - 报文值
/// @param[in]
///     ratedVal1      - 配置中设置的额定1次值，单位为V/A
/// @param[in]
///     ratedVal2      - 配置中设置的额定2次值，单位为V/A
/// @param[in]
///     channelType    - 通道类型
/// @param[in]
///     channelValType - 通道数据类型（测量/保护，用于电流通道）
/// @param[in]
///     rangeFlag      - 额定值范围扩展标志，主要影响保护用ECT的比例因子
///
/// @return
///     自然值，单位为V/A
///
float    CSmvScaler::ChannelMsgVal2NtrVal_FT3( int16_t msgVal, float ratedVal1, float ratedVal2,
											   enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// 协议规定的比例因子
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// 参考额定值
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// 线电压->相电压
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	return (msgVal/ntrValScale*ratedVal/protocolScale);
}

///
/// @brief
///     通道的自然值->报文值换算（IEC61850-9-1）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     ntrVal         - 自然值，单位为V/A
/// @param[in]
///     ratedVal1      - 配置中设置的额定1次值，单位为V/A
/// @param[in]
///     ratedVal2      - 配置中设置的额定2次值，单位为V/A
/// @param[in]
///     channelType    - 通道类型
/// @param[in]
///     channelValType - 通道数据类型（测量/保护，用于电流通道）
/// @param[in]
///     rangeFlag      - 额定值范围扩展标志，主要影响保护用ECT的比例因子
///
/// @return
///     报文值
///
uint16_t CSmvScaler::ChannelNtrVal2MsgVal_Sav91( float ntrVal, float ratedVal1, float ratedVal2,
												 enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// 协议规定的比例因子
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// 参考额定值
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// 线电压->相电压
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	uint16_t nt = (uint16_t)((int16_t)(ntrVal*ntrValScale/ratedVal*protocolScale));
	return nt;
}

///
/// @brief
///     通道的报文值->自然值换算（IEC61850-9-1）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     msgVal         - 报文值
/// @param[in]
///     ratedVal1      - 配置中设置的额定1次值，单位为V/A
/// @param[in]
///     ratedVal2      - 配置中设置的额定2次值，单位为V/A
/// @param[in]
///     channelType    - 通道类型
/// @param[in]
///     channelValType - 通道数据类型（测量/保护，用于电流通道）
/// @param[in]
///     rangeFlag      - 额定值范围扩展标志，主要影响保护用ECT的比例因子
///
/// @return
///     自然值，单位为V/A
///
float    CSmvScaler::ChannelMsgVal2NtrVal_Sav91( int16_t msgVal, float ratedVal1, float ratedVal2,
												 enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// 协议规定的比例因子
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// 参考额定值
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// 线电压->相电压
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	return (msgVal/ntrValScale*ratedVal/protocolScale);
}

///
/// @brief
///     通道的自然值->报文值换算（IEC61850-9-2）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     ntrVal    - 自然值
/// @param[in]
///     ratedVal1 - 配置中设置的额定1次值
/// @param[in]
///     ratedVal2 - 配置中设置的额定2次值
/// @param[in]
///     cType     - 通道类型
///
/// @return
///     报文值
///
uint32_t CSmvScaler::ChannelNtrVal2MsgVal_Sav92(
									float ntrVal,
									float ratedVal1,
									float ratedVal2,
									enumChannelType cType
									)
{
	// 协议规定的比例因子
	float protocolScale = GetSav92SacleFactor(cType);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	uint32_t nt = (uint32_t)((int32_t)(ntrVal*ntrValScale/protocolScale));
	return nt;
}

///
/// @brief
///     通道的报文值->自然值换算（IEC61850-9-2）
///     注意，需先设置报文值、自然值类型
///
/// @param[in]
///     msgVal    - 报文值
/// @param[in]
///     ratedVal1 - 配置中设置的额定1次值
/// @param[in]
///     ratedVal2 - 配置中设置的额定2次值
/// @param[in]
///     cType     - 通道类型
///
/// @return
///     自然值
///
float    CSmvScaler::ChannelMsgVal2NtrVal_Sav92(
									int32_t msgVal,
									float ratedVal1,
									float ratedVal2,
									enumChannelType cType
									)
{
	// 协议规定的比例因子
	float protocolScale = GetSav92SacleFactor(cType);
	// 一次二次值变比
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	return (float(msgVal)/ntrValScale*protocolScale);
}

///
/// @brief
///     根据设置的报文值、自然值类型，获取通道的自然值换算因子
///     即是否需要将自然值换算成一次or二次值
///
/// @param[in]
///     ratedVal1 - 配置中设置的额定1次值
/// @param[in]
///     ratedVal2 - 配置中设置的额定2次值
///
/// @return
///     自然值换算因子
///
float CSmvScaler::GetNtrValSacleFactor(float ratedVal1, float ratedVal2)
{
	float ntrValScale = 1.0f;
	if ( m_eChannelNtrValType == SMV_VALUE_TYPE_PRIMARY && m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
	{
		ntrValScale = ratedVal2/ratedVal1;
	}
	else if ( m_eChannelNtrValType == SMV_VALUE_TYPE_SECONDARY && m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
	{
		ntrValScale = ratedVal1/ratedVal2;
	}
	return ntrValScale;
}

///
/// @brief
///     获取FT3或IEC61850-9-1协议规定的通道的比例因子
///
/// @param[in]
///     channelType    - 通道类型
/// @param[in]
///     channelValType - 通道数据类型（测量/保护，用于电流通道）
/// @param[in]
///     rangeFlag      - 额定值范围扩展标志，主要影响保护用ECT的比例因子
///
/// @return
///     协议规定的通道的比例因子
///
float CSmvScaler::GetFT3orSav91SacleFactor(enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag)
{
	float protocolScale = 1.0f;
	switch(channelType)
	{
	case CHANNEL_TYPE_SMV_CURRENT:
		if ( channelValueType == CHANNEL_VALUE_TYPE_MEASURE )
		{
			protocolScale = k_ECT_SCM;
		}
		else if ( channelValueType == CHANNEL_VALUE_TYPE_PROTECT )
		{
			if ( rangeFlag )
				protocolScale = k_ECT_SCP_1;
			else
				protocolScale = k_ECT_SCP_0;
		}
		break;
	case CHANNEL_TYPE_SMV_VOLTAGE:
		protocolScale = k_EVT_SV;
		break;
	default:
		protocolScale = 1.0f;
		break;
	}
	return protocolScale;
}

///
/// @brief
///     获取IEC61850-9-2协议规定的通道的比例因子
///
/// @param[in]
///     cType     - 通道类型
///
/// @return
///     协议规定的通道的比例因子
///
float CSmvScaler::GetSav92SacleFactor(enumChannelType cType)
{
	switch(cType)
	{
	case CHANNEL_TYPE_SMV_CURRENT:
		return k_Sav92_SC;
		break;
	case CHANNEL_TYPE_SMV_VOLTAGE:
		return k_Sav92_SV;
		break;
	default:
		return 1.0f;
	}
}
