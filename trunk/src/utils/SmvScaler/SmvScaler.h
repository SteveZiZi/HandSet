/// @file
///     SmvScaler.h
///
/// @brief
///     ����ֵ���䱨�ĸ����ֶεĻ���������Ҫ�Ǹ�����ֵͨ����FT3��9-1�Ķ���ѹ
///
/// @note
///     ��ʽ��
///     FT3��9-1ͨ��(��Ȼֵ->����ֵ)�� ����ֵ = ��Ȼֵ*һ�ζ���ֵ���/�ο��ֵ*Э��涨�ı�������
///     9-2ͨ��(��Ȼֵ->����ֵ)��      ����ֵ = ��Ȼֵ*һ�ζ���ֵ���/Э��涨�ı�������
///     FT3����ѹ(��Ȼֵ->����ֵ)������ֵ = ��Ȼֵ*����3*10
///     9-1����ѹ(��Ȼֵ->����ֵ)������ֵ = ��Ȼֵ*10
///     ���У�
///     ͨ�������һ�ζ���ֵ����ɵ�ǰ��Ȼֵ���ͺͻ����ı���ֵ�����;���
///     FT3��9-1ͨ���Ĳο��ֵ�ɻ����ı���ֵ�����;���
///     ����ֵ->��Ȼֵ���������淴����
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.5.20
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
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
	///     ����ͨ���ı���ֵ����
	///
	/// @param[in]
	///     type - ����ֵ���ͣ�1��or2��
	///
	void SetChannelMsgValType(enumSmvValueType type);

	///
	/// @brief
	///     ����ͨ������Ȼֵ����
	///
	/// @param[in]
	///     type - ��Ȼֵ���ͣ�1��or2��
	///
	void SetChannelNtrValType(enumSmvValueType type);
	
	///
	/// @brief
	///     ͨ������Ȼֵ->����ֵ���㣨FT3��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     ntrVal         - ��Ȼֵ����λΪV/A
	/// @param[in]
	///     ratedVal1      - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2      - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType    - ͨ������
	/// @param[in]
	///     channelValType - ͨ���������ͣ�����/���������ڵ���ͨ����
	/// @param[in]
	///     rangeFlag      - �ֵ��Χ��չ��־����ҪӰ�챣����ECT�ı�������
	///
	/// @return
	///     ����ֵ
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
	///     ͨ���ı���ֵ->��Ȼֵ���㣨FT3��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     msgVal         - ����ֵ
	/// @param[in]
	///     ratedVal1      - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2      - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType    - ͨ������
	/// @param[in]
	///     channelValType - ͨ���������ͣ�����/���������ڵ���ͨ����
	/// @param[in]
	///     rangeFlag      - �ֵ��Χ��չ��־����ҪӰ�챣����ECT�ı�������
	///
	/// @return
	///     ��Ȼֵ����λΪV/A
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
	///     ͨ������Ȼֵ->����ֵ���㣨IEC61850-9-1��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     ntrVal         - ��Ȼֵ����λΪV/A
	/// @param[in]
	///     ratedVal1      - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2      - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType    - ͨ������
	/// @param[in]
	///     channelValType - ͨ���������ͣ�����/���������ڵ���ͨ����
	/// @param[in]
	///     rangeFlag      - �ֵ��Χ��չ��־����ҪӰ�챣����ECT�ı�������
	///
	/// @return
	///     ����ֵ
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
	///     ͨ���ı���ֵ->��Ȼֵ���㣨IEC61850-9-1��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     msgVal         - ����ֵ
	/// @param[in]
	///     ratedVal1      - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2      - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType    - ͨ������
	/// @param[in]
	///     channelValType - ͨ���������ͣ�����/���������ڵ���ͨ����
	/// @param[in]
	///     rangeFlag      - �ֵ��Χ��չ��־����ҪӰ�챣����ECT�ı�������
	///
	/// @return
	///     ��Ȼֵ����λΪV/A
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
	///     ͨ������Ȼֵ->����ֵ���㣨IEC61850-9-2��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     ntrVal      - ��Ȼֵ
	/// @param[in]
	///     ratedVal1   - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2   - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType - ͨ������
	///
	/// @return
	///     ����ֵ����λΪV/A
	///
	uint32_t ChannelNtrVal2MsgVal_Sav92(
		float ntrVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType
		);

	///
	/// @brief
	///     ͨ���ı���ֵ->��Ȼֵ���㣨IEC61850-9-2��
	///     ע�⣬�������ñ���ֵ����Ȼֵ����
	///
	/// @param[in]
	///     msgVal      - ����ֵ
	/// @param[in]
	///     ratedVal1   - ���������õĶ1��ֵ����λΪV/A
	/// @param[in]
	///     ratedVal2   - ���������õĶ2��ֵ����λΪV/A
	/// @param[in]
	///     channelType - ͨ������
	///
	/// @return
	///     ��Ȼֵ����λΪV/A
	///
	float    ChannelMsgVal2NtrVal_Sav92(
		int32_t msgVal,
		float ratedVal1,
		float ratedVal2,
		enumChannelType channelType
		);

	///
	/// @brief
	///     ����ѹ����Ȼֵ->����ֵ���㣨FT3��
	///
	/// @param[in]
	///     ntrVal    - ��Ȼֵ����λΪKV
	///
	/// @return
	///     ����ֵ
	///
	uint16_t PhsVrtgNtrVal2MsgVal_FT3(float ntrVal);

	///
	/// @brief
	///     ����ѹ������ֵ->��Ȼֵ���㣨FT3��
	///
	/// @param[in]
	///     msgVal    - ����ֵ
	///
	/// @return
	///     ��Ȼֵ����λΪKV
	///
	float    PhsVrtgMsgVal2NtrVal_FT3(uint16_t msgVal);

	///
	/// @brief
	///     ����ѹ����Ȼֵ->����ֵ���㣨IEC61850-9-1��
	///
	/// @param[in]
	///     ntrVal    - ��Ȼֵ����λΪKV
	///
	/// @return
	///     ����ֵ
	///
	uint16_t PhsVrtgNtrVal2MsgVal_Sav91(float ntrVal);

	///
	/// @brief
	///     ����ѹ������ֵ->��Ȼֵ���㣨IEC61850-9-1��
	///
	/// @param[in]
	///     msgVal    - ����ֵ
	///
	/// @return
	///     ��Ȼֵ����λΪKV
	///
	float    PhsVrtgMsgVal2NtrVal_Sav91(uint16_t msgVal);

	///
	/// @brief
	///     ��Ȼֵ��һ��ֵ->����ֵ���㣬ע�Ᵽ�ֵ�λһ��
	///
	/// @param[in]
	///     ntrVal    - һ��ֵ
	/// @param[in]
	///     ratedVal1 - ���������õĶ1��ֵ
	/// @param[in]
	///     ratedVal2 - ���������õĶ2��ֵ
	///
	/// @return
	///     ����ֵ
	///
	float ChannelNtr1st2Ntr2nd(
		float ntrVal,
		float ratedVal1,
		float ratedVal2
		);

	///
	/// @brief
	///     ��Ȼֵ������ֵ->һ��ֵ���㣬ע�Ᵽ�ֵ�λһ��
	///
	/// @param[in]
	///     ntrVal    - ����ֵ
	/// @param[in]
	///     ratedVal1 - ���������õĶ1��ֵ
	/// @param[in]
	///     ratedVal2 - ���������õĶ2��ֵ
	///
	/// @return
	///     һ��ֵ
	///
	float ChannelNtr2nd2Ntr1st(
		float ntrVal,
		float ratedVal1,
		float ratedVal2
		);

private:
	///
	/// @brief
	///     ����
	///
	float GetNtrValSacleFactor(float ratedVal1, float ratedVal2);
	float GetFT3orSav91SacleFactor(enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag);
	float GetSav92SacleFactor(enumChannelType cType);

	enumSmvValueType m_eChannelMsgValType;         ///< ͨ���ı���ֵ����
	enumSmvValueType m_eChannelNtrValType;         ///< ͨ������Ȼֵ����
};

///
/// @brief
///     ����ͨ���ı���ֵ����
///
inline void CSmvScaler::SetChannelMsgValType(enumSmvValueType type)
{
	m_eChannelMsgValType = type;
}

///
/// @brief
///     ����ͨ������Ȼֵ����
///
inline void CSmvScaler::SetChannelNtrValType(enumSmvValueType type)
{
	m_eChannelNtrValType = type;
}

///
/// @brief
///     ����ѹ����Ȼֵ->����ֵ���㣨FT3��
///
inline uint16_t CSmvScaler::PhsVrtgNtrVal2MsgVal_FT3(float ntrVal)
{
	return (uint16_t)(ntrVal*sqrt(3.0)*10.0f);
}

///
/// @brief
///     ����ѹ������ֵ->��Ȼֵ���㣨FT3��
///
inline float    CSmvScaler::PhsVrtgMsgVal2NtrVal_FT3(uint16_t msgVal)
{
	return (float)(msgVal/sqrt(3.0)/10.0f);
}

///
/// @brief
///     ����ѹ����Ȼֵ->����ֵ���㣨IEC61850-9-1��
///
inline uint16_t CSmvScaler::PhsVrtgNtrVal2MsgVal_Sav91(float ntrVal)
{
	return (uint16_t)(ntrVal*10.0f);
}

///
/// @brief
///     ����ѹ������ֵ->��Ȼֵ���㣨IEC61850-9-1��
///
inline float    CSmvScaler::PhsVrtgMsgVal2NtrVal_Sav91(uint16_t msgVal)
{
	return (float)(msgVal/10.0f);
}

///
/// @brief
///     ��Ȼֵ��һ��ֵ->����ֵ����
///
inline float CSmvScaler::ChannelNtr1st2Ntr2nd(float ntrVal,
									          float ratedVal1,
									          float ratedVal2)
{
	return (ntrVal*ratedVal2/ratedVal1);
}

///
/// @brief
///     ��Ȼֵ������ֵ->һ��ֵ����
///
inline float CSmvScaler::ChannelNtr2nd2Ntr1st(float ntrVal,
									          float ratedVal1,
									          float ratedVal2)
{
	return (ntrVal*ratedVal1/ratedVal2);
}

#endif
