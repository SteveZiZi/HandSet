/// @file
///     SmvScaler.h
///
/// @brief
///     采样值传输报文个别字段的换算器，主要是各采样值通道和FT3、9-1的额定相电压
///
/// @note
///     公式：
///     FT3、9-1通道(自然值->报文值)： 报文值 = 自然值*一次二次值变比/参考额定值*协议规定的比例因子
///     9-2通道(自然值->报文值)：      报文值 = 自然值*一次二次值变比/协议规定的比例因子
///     FT3额定相电压(自然值->报文值)：报文值 = 自然值*根号3*10
///     9-1额定相电压(自然值->报文值)：报文值 = 自然值*10
///     其中，
///     通道换算的一次二次值变比由当前自然值类型和换算后的报文值的类型决定
///     FT3、9-1通道的参考额定值由换算后的报文值的类型决定
///     报文值->自然值换算则是逆反过程
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.5.20
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef SMV_SCALER_H_

#define SMV_SCALER_H_

#include "src/include/stdint.h"
#include "src/include/pub_datatypes.h"
#include <math.h>

class CSmvScaler
{
public:
	CSmvScaler();

	///
	/// @brief
	///     设置通道的报文值类型
	///
	/// @param[in]
	///     type - 报文值类型，1次or2次
	///
	void SetChannelMsgValType(enumSmvValueType type);

	///
	/// @brief
	///     设置通道的自然值类型
	///
	/// @param[in]
	///     type - 自然值类型，1次or2次
	///
	void SetChannelNtrValType(enumSmvValueType type);
	
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
	uint16_t ChannelNtrVal2MsgVal_FT3(
		float ntrVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType,
		enumChannelValueType channelValueType,
		bool rangeFlag = 0
		);

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
	float    ChannelMsgVal2NtrVal_FT3(
		int16_t msgVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType,
		enumChannelValueType channelValueType,
		bool rangeFlag = 0
		);

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
	uint16_t ChannelNtrVal2MsgVal_Sav91(
		float ntrVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType,
		enumChannelValueType channelValueType,
		bool rangeFlag = 0
		);

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
	float    ChannelMsgVal2NtrVal_Sav91(
		int16_t msgVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType,
		enumChannelValueType channelValueType,
		bool rangeFlag = 0
		);

	///
	/// @brief
	///     通道的自然值->报文值换算（IEC61850-9-2）
	///     注意，需先设置报文值、自然值类型
	///
	/// @param[in]
	///     ntrVal      - 自然值
	/// @param[in]
	///     ratedVal1   - 配置中设置的额定1次值，单位为V/A
	/// @param[in]
	///     ratedVal2   - 配置中设置的额定2次值，单位为V/A
	/// @param[in]
	///     channelType - 通道类型
	///
	/// @return
	///     报文值，单位为V/A
	///
	uint32_t ChannelNtrVal2MsgVal_Sav92(
		float ntrVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType
		);

	///
	/// @brief
	///     通道的报文值->自然值换算（IEC61850-9-2）
	///     注意，需先设置报文值、自然值类型
	///
	/// @param[in]
	///     msgVal      - 报文值
	/// @param[in]
	///     ratedVal1   - 配置中设置的额定1次值，单位为V/A
	/// @param[in]
	///     ratedVal2   - 配置中设置的额定2次值，单位为V/A
	/// @param[in]
	///     channelType - 通道类型
	///
	/// @return
	///     自然值，单位为V/A
	///
	float    ChannelMsgVal2NtrVal_Sav92(
		int32_t msgVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType
		);

	///
	/// @brief
	///     额定相电压：自然值->报文值换算（FT3）
	///
	/// @param[in]
	///     ntrVal    - 自然值，单位为KV
	///
	/// @return
	///     报文值
	///
	uint16_t PhsVrtgNtrVal2MsgVal_FT3(float ntrVal);

	///
	/// @brief
	///     额定相电压：报文值->自然值换算（FT3）
	///
	/// @param[in]
	///     msgVal    - 报文值
	///
	/// @return
	///     自然值，单位为KV
	///
	float    PhsVrtgMsgVal2NtrVal_FT3(uint16_t msgVal);

	///
	/// @brief
	///     额定相电压：自然值->报文值换算（IEC61850-9-1）
	///
	/// @param[in]
	///     ntrVal    - 自然值，单位为KV
	///
	/// @return
	///     报文值
	///
	uint16_t PhsVrtgNtrVal2MsgVal_Sav91(float ntrVal);

	///
	/// @brief
	///     额定相电压：报文值->自然值换算（IEC61850-9-1）
	///
	/// @param[in]
	///     msgVal    - 报文值
	///
	/// @return
	///     自然值，单位为KV
	///
	float    PhsVrtgMsgVal2NtrVal_Sav91(uint16_t msgVal);

	///
	/// @brief
	///     自然值：一次值->二次值换算，注意保持单位一致
	///
	/// @param[in]
	///     ntrVal    - 一次值
	/// @param[in]
	///     ratedVal1 - 配置中设置的额定1次值
	/// @param[in]
	///     ratedVal2 - 配置中设置的额定2次值
	///
	/// @return
	///     二次值
	///
	float ChannelNtr1st2Ntr2nd(
		float ntrVal,
		float ratedVal1,
		float ratedVal2
		);

	///
	/// @brief
	///     自然值：二次值->一次值换算，注意保持单位一致
	///
	/// @param[in]
	///     ntrVal    - 二次值
	/// @param[in]
	///     ratedVal1 - 配置中设置的额定1次值
	/// @param[in]
	///     ratedVal2 - 配置中设置的额定2次值
	///
	/// @return
	///     一次值
	///
	float ChannelNtr2nd2Ntr1st(
		float ntrVal,
		float ratedVal1,
		float ratedVal2
		);

private:
	///
	/// @brief
	///     辅助
	///
	float GetNtrValSacleFactor(float ratedVal1, float ratedVal2);
	float GetFT3orSav91SacleFactor(enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag);
	float GetSav92SacleFactor(enumChannelType cType);

	enumSmvValueType m_eChannelMsgValType;         ///< 通道的报文值类型
	enumSmvValueType m_eChannelNtrValType;         ///< 通道的自然值类型
};

///
/// @brief
///     设置通道的报文值类型
///
inline void CSmvScaler::SetChannelMsgValType(enumSmvValueType type)
{
	m_eChannelMsgValType = type;
}

///
/// @brief
///     设置通道的自然值类型
///
inline void CSmvScaler::SetChannelNtrValType(enumSmvValueType type)
{
	m_eChannelNtrValType = type;
}

///
/// @brief
///     额定相电压：自然值->报文值换算（FT3）
///
inline uint16_t CSmvScaler::PhsVrtgNtrVal2MsgVal_FT3(float ntrVal)
{
	return (uint16_t)(ntrVal*sqrt(3.0)*10.0f);
}

///
/// @brief
///     额定相电压：报文值->自然值换算（FT3）
///
inline float    CSmvScaler::PhsVrtgMsgVal2NtrVal_FT3(uint16_t msgVal)
{
	return (float)(msgVal/sqrt(3.0)/10.0f);
}

///
/// @brief
///     额定相电压：自然值->报文值换算（IEC61850-9-1）
///
inline uint16_t CSmvScaler::PhsVrtgNtrVal2MsgVal_Sav91(float ntrVal)
{
	return (uint16_t)(ntrVal*10.0f);
}

///
/// @brief
///     额定相电压：报文值->自然值换算（IEC61850-9-1）
///
inline float    CSmvScaler::PhsVrtgMsgVal2NtrVal_Sav91(uint16_t msgVal)
{
	return (float)(msgVal/10.0f);
}

///
/// @brief
///     自然值：一次值->二次值换算
///
inline float CSmvScaler::ChannelNtr1st2Ntr2nd(float ntrVal,
									          float ratedVal1,
									          float ratedVal2)
{
	return (ntrVal*ratedVal2/ratedVal1);
}

///
/// @brief
///     自然值：二次值->一次值换算
///
inline float CSmvScaler::ChannelNtr2nd2Ntr1st(float ntrVal,
									          float ratedVal1,
									          float ratedVal2)
{
	return (ntrVal*ratedVal1/ratedVal2);
}

#endif
