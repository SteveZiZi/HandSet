/// @file
///     ieee1588_types.cpp
///
/// @brief
///     ����IEEE1588����֡�������Զ����������ݵĴ�����
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.9.29
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "../include/ieee1588_types.h"

//
//=============================================================================
//    IEEE1588���ĵ�������Ϣ�ֶ�
//=============================================================================
//

///
/// @brief
///     ��ȡ��ǰ��������������
///
/// @return
///     ��ǰ��������������
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
///     ���õ�ǰ��������������
///
/// @param[in]
///     eMessageType - ��ǰ��������������
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
//    IEEE1588���ĵı���ͷ�еı�־λ����
//=============================================================================
//

///
/// @brief
///     ��ȡIEEE1588���ĵı���ͷ�еı�־λ������ĳλ��ʹ�õ�����
///
#define IEEE1588_HEADER_FLAGS_BIT_MASK 0x0001

///
/// @brief
///     IEEE1588���ĵı���ͷ�еı�־λ�����и�����־���ڵ�bit��������
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
///     �жϷ����˿��Ƿ�Ϊ��Master״̬
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpAlternateMaster()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_ALTERNATE_MASTER)) != 0 );
}

///
/// @brief
///     ���ñ��ķ����˿��Ƿ�Ϊ��Master״̬
///
/// @param[in]
///     bAlternateMaster - Ϊ��Master״̬��Ϊtrue�������෴
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
///     �ж�ͬ��ʱ���Ƿ�Ϊ������ʱ��
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpTwoStep()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TWO_STEP)) != 0 );
}

///
/// @brief
///     ����ͬ��ʱ���Ƿ�Ϊ������ʱ��
///
/// @param[in]
///     bTwoStep - ������ʱ����Ϊtrue��������ʱ����Ϊfalse
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
///     �жϸñ����Ƿ��Ե�����ʽ����
///
/// @return
///     �Ե�����ʽ���� - true
///     �Զಥ��ʽ���� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpUnicast()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UNICAST)) != 0 );
}

///
/// @brief
///     ���øñ����Ƿ��Ե�����ʽ����
///
/// @param[in]
///     bTwoStep - �Ե�����ʽ������Ϊtrue���Զಥ��ʽ������Ϊfalse
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
///     �ж�ʱ���Ƿ�alternate PTP�����ļ����壨��־1��
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpProfileSpecific1()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC1)) != 0 );
}

///
/// @brief
///     ����ʱ���Ƿ�alternate PTP�����ļ����壨��־1��
///
/// @param[in]
///     bSpecific - ����Ϊtrue�������෴
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
///     �ж�ʱ���Ƿ�alternate PTP�����ļ����壨��־2��
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpProfileSpecific2()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_PROFILE_SPECIFIC2)) != 0 );
}


///
/// @brief
///     ����ʱ���Ƿ�alternate PTP�����ļ����壨��־2��
///
/// @param[in]
///     bSpecific - ����Ϊtrue�������෴
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
///     �жϱ����Ƿ��������Ϣ��ȫ����
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpSecurity()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_SECURITY)) != 0 );
}

///
/// @brief
///     ���ñ����Ƿ��������Ϣ��ȫ����
///
/// @param[in]
///     bSecurity - ����Ϊtrue�������෴
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
///     �жϵ������1min�Ƿ�Ϊ61s
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpLI61()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_61)) != 0 );
}

///
/// @brief
///     ���õ������1min�Ƿ�Ϊ61s
///
/// @param[in]
///     bLI61 - ���ڲ���PTPʱԪ�Ķ�ʱϵͳ����ֵΪtrue���ʾ�������1minΪ61s��
///             ���ڲ�����PTPʱԪ�Ķ�ʱϵͳ����ֵΪfalse��
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
///     �жϵ������1min�Ƿ�Ϊ59s
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpLI59()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_LI_59)) != 0 );
}

///
/// @brief
///     ���õ������1min�Ƿ�Ϊ59s
///
/// @param[in]
///     bLI59 - ���ڲ���PTPʱԪ�Ķ�ʱϵͳ����ֵΪtrue���ʾ�������1minΪ59s��
///             ���ڲ�����PTPʱԪ�Ķ�ʱϵͳ����ֵΪfalse��
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
///     �ж��Ƿ����UTC��ʾʱ����Ϣ
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpUtcReasonable()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_UTC_REASONABLE)) != 0 );
}

///
/// @brief
///     �����Ƿ����UTC��ʾʱ����Ϣ
///
/// @param[in]
///     bUtcReasonable - ����Ϊtrue�������෴
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
///     �ж���߼�����ʱ���Ƿ����PTPʱ����
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsPtpTimescale()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_PTP_TIMESCALE)) != 0 );
}

///
/// @brief
///     ������߼�����ʱ���Ƿ����PTPʱ����
///
/// @param[in]
///     bTimescale - ����Ϊtrue�������෴
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
///     �ж�ʱ���ߵ�ʱ��ƫ���Ƿ��׷�ݵ���߼�����ʱ��
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsTimeTraceble()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_TIME_TRACEABLE)) != 0 );
}

///
/// @brief
///     ����ʱ���ߵ�ʱ��ƫ���Ƿ��׷�ݵ���߼�����ʱ��
///
/// @param[in]
///     bTimeTraceble - ����Ϊtrue�������෴
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
///     �ж�Ƶ�ʾ�������Ƿ����׷�ݵ���߼�����ʱ��
///
/// @return
///     �� - true
///     �� - false
///
bool IEEE1588_HEADER_FLAGS::IsFrequencyTraceable()
{
	return ( (iFlags & (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE)) != 0 );
}

///
/// @brief
///     ����Ƶ�ʾ�������Ƿ����׷�ݵ���߼�����ʱ��
///
/// @param[in]
///     bTraceable - ����Ϊtrue�������෴
///
void IEEE1588_HEADER_FLAGS::SetFrequencyTraceable(bool bTraceable)
{
	if ( bTraceable )
		iFlags |= (IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE);
	else
		iFlags &= ~(IEEE1588_HEADER_FLAGS_BIT_MASK << IEEE1588_HEADER_FLAGS_FREQUENCY_TRACEABLE);
}
