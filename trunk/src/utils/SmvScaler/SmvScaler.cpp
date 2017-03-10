#include "stdafx.h"
#include "SmvScaler.h"

///
/// @brief
///     FT3/IEC61850-9-1Э��涨ͨ���ı�������
///
float k_ECT_SCM   = 11585.0f;            ///< ������ECT�ı�������
float k_ECT_SCP_0 = 463.0f;              ///< ������ECT�ı�������,��rangeflag = 0
float k_ECT_SCP_1 = 231.0f;              ///< ������ECT�ı�������,��rangeflag = 1
float k_EVT_SV    = 11585.0f;            ///< EVT�ı�������

///
/// @brief
///     IEC61850-9-2Э��涨ͨ���ı�������
///
float k_Sav92_SC  = 0.001f;              ///< �����ı�������
float k_Sav92_SV  = 0.01f;               ///< ��ѹ�ı�������

CSmvScaler::CSmvScaler()
: m_eChannelMsgValType(SMV_VALUE_TYPE_PRIMARY)
, m_eChannelNtrValType(SMV_VALUE_TYPE_PRIMARY)
{

}

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
uint16_t CSmvScaler::ChannelNtrVal2MsgVal_FT3( float ntrVal, float ratedVal1, float ratedVal2,
											   enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// Э��涨�ı�������
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// �ο��ֵ
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// �ߵ�ѹ->���ѹ
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	uint16_t nt = (uint16_t)((int16_t)(ntrVal*ntrValScale/ratedVal*protocolScale));
	return nt;
}

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
float    CSmvScaler::ChannelMsgVal2NtrVal_FT3( int16_t msgVal, float ratedVal1, float ratedVal2,
											   enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// Э��涨�ı�������
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// �ο��ֵ
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// �ߵ�ѹ->���ѹ
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	return (msgVal/ntrValScale*ratedVal/protocolScale);
}

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
uint16_t CSmvScaler::ChannelNtrVal2MsgVal_Sav91( float ntrVal, float ratedVal1, float ratedVal2,
												 enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// Э��涨�ı�������
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// �ο��ֵ
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// �ߵ�ѹ->���ѹ
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	uint16_t nt = (uint16_t)((int16_t)(ntrVal*ntrValScale/ratedVal*protocolScale));
	return nt;
}

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
float    CSmvScaler::ChannelMsgVal2NtrVal_Sav91( int16_t msgVal, float ratedVal1, float ratedVal2,
												 enumChannelType channelType, enumChannelValueType channelValueType, bool rangeFlag /* = 0 */ )
{
	// Э��涨�ı�������
	float protocolScale = GetFT3orSav91SacleFactor(channelType,channelValueType,rangeFlag);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	// �ο��ֵ
	float ratedVal;
	if ( m_eChannelMsgValType == SMV_VALUE_TYPE_PRIMARY )
		ratedVal = ratedVal1;
	else if ( m_eChannelMsgValType == SMV_VALUE_TYPE_SECONDARY )
		ratedVal = ratedVal2;
	else
		ratedVal = 1.0f;
	// �ߵ�ѹ->���ѹ
	if ( channelType == CHANNEL_TYPE_SMV_VOLTAGE )
		ratedVal = (float)(ratedVal/sqrt(3.0));
	return (msgVal/ntrValScale*ratedVal/protocolScale);
}

///
/// @brief
///     ͨ������Ȼֵ->����ֵ���㣨IEC61850-9-2��
///     ע�⣬�������ñ���ֵ����Ȼֵ����
///
/// @param[in]
///     ntrVal    - ��Ȼֵ
/// @param[in]
///     ratedVal1 - ���������õĶ1��ֵ
/// @param[in]
///     ratedVal2 - ���������õĶ2��ֵ
/// @param[in]
///     cType     - ͨ������
///
/// @return
///     ����ֵ
///
uint32_t CSmvScaler::ChannelNtrVal2MsgVal_Sav92(
									float ntrVal,
									float ratedVal1,
									float ratedVal2,
									enumChannelType cType
									)
{
	// Э��涨�ı�������
	float protocolScale = GetSav92SacleFactor(cType);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	uint32_t nt = (uint32_t)((int32_t)(ntrVal*ntrValScale/protocolScale));
	return nt;
}

///
/// @brief
///     ͨ���ı���ֵ->��Ȼֵ���㣨IEC61850-9-2��
///     ע�⣬�������ñ���ֵ����Ȼֵ����
///
/// @param[in]
///     msgVal    - ����ֵ
/// @param[in]
///     ratedVal1 - ���������õĶ1��ֵ
/// @param[in]
///     ratedVal2 - ���������õĶ2��ֵ
/// @param[in]
///     cType     - ͨ������
///
/// @return
///     ��Ȼֵ
///
float    CSmvScaler::ChannelMsgVal2NtrVal_Sav92(
									int32_t msgVal,
									float ratedVal1,
									float ratedVal2,
									enumChannelType cType
									)
{
	// Э��涨�ı�������
	float protocolScale = GetSav92SacleFactor(cType);
	// һ�ζ���ֵ���
	float ntrValScale   = GetNtrValSacleFactor(ratedVal1,ratedVal2);
	return (float(msgVal)/ntrValScale*protocolScale);
}

///
/// @brief
///     �������õı���ֵ����Ȼֵ���ͣ���ȡͨ������Ȼֵ��������
///     ���Ƿ���Ҫ����Ȼֵ�����һ��or����ֵ
///
/// @param[in]
///     ratedVal1 - ���������õĶ1��ֵ
/// @param[in]
///     ratedVal2 - ���������õĶ2��ֵ
///
/// @return
///     ��Ȼֵ��������
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
///     ��ȡFT3��IEC61850-9-1Э��涨��ͨ���ı�������
///
/// @param[in]
///     channelType    - ͨ������
/// @param[in]
///     channelValType - ͨ���������ͣ�����/���������ڵ���ͨ����
/// @param[in]
///     rangeFlag      - �ֵ��Χ��չ��־����ҪӰ�챣����ECT�ı�������
///
/// @return
///     Э��涨��ͨ���ı�������
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
///     ��ȡIEC61850-9-2Э��涨��ͨ���ı�������
///
/// @param[in]
///     cType     - ͨ������
///
/// @return
///     Э��涨��ͨ���ı�������
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
