#include "stdafx.h"
#include "../include/iec61850_data_struct.h"

//
//=============================================================================
//    FT3֡�е�״̬�ֵı����ӿڵ�ʵ��
//=============================================================================
//

///
/// @brief
///     ��ȡ״̬��ĳλ��ʹ�õ�����
///
#define FT3SW_BIT_MASK 0x0001

///
/// @brief
///     FT3֡����׼/��չ/���������г������Ե�bit��������
///
#define FT3SW_MAINTENANCE_REQUIRED 0             ///< �豸Ҫ��ά�ޣ�����/Ҫ��ά�ޣ�
#define FT3SW_TEST_MODE            1             ///< �豸����״̬����������/���飩
#define FT3SW_CT_OUTPUT_TYPE       12            ///< CT������ͣ�i(t) ���� d(i(t))/dt��
#define FT3SW_RANGE_FLAG           13            ///< RangeFlag��SCP=0x01CF����0x00E7��

///
/// @brief
///     ��׼/��չFT3֡�й���MU���Ե�bit��������
///
#define FT3SW_MU_WAKE_UP_TIME      2             ///< MU�Ļ���ʱ��ָʾ
#define FT3SW_MU_SYNC_METHOD       3             ///< MU��ͬ������
#define FT3SW_MU_SYNC_FLAG         4             ///< MU��ͬ��״̬


///
/// @brief
///     ��׼FT3֡�д���ͨ����Ч�Ե�bit��������
///
static uint8_t g_MuFt3StdFrameChannelBit[] = {
	// ״̬��1�е�bit_n
	5,6,7,8,9,10,11,
	// ״̬��2�е�bit_n
	0,1,2,3,4
};

///
/// @brief
///     ��׼FT3֡�б���λ��������
///
static uint8_t g_MuFt3StdFrameFutureUseBit[] = {
	// ״̬��1�е�bit_n
	14,15,
	// ״̬��2�е�bit_n
	5,6,7,8,9,10,11,12,13,14,15
};

///
/// @brief
///     ��չFT3֡�д���ͨ����Ч�Ե�bit��������
///
static uint8_t g_MuFt3ExFrameChannelBit[] = {
	// ״̬��1�е�bit_n
	5,6,7,8,9,10,11,
	// ״̬��2�е�bit_n
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14
};

///
/// @brief
///     ��չFT3֡�б���λ��������
///
static uint8_t g_MuFt3ExFrameFutureUseBit[] = {
	// ״̬��1�е�bit_n
	14,15,
	// ״̬��2�е�bit_n
	15
};

///
/// @brief
///     ����/��ѹ������FT3֡�й��ڻ��������Ե�bit��������
///
#define FT3SW_CTORVT_FIRING_TIME        FT3SW_MU_WAKE_UP_TIME    ///< �������ļ���ʱ��ָʾ
#define FT3SW_CTORVT_SYNC_METHOD        FT3SW_MU_SYNC_METHOD     ///< ��������ͬ������
#define FT3SW_CTORVT_SYNC_FLAG          FT3SW_MU_SYNC_FLAG       ///< ��������ͬ��״̬
#define FT3SW_CTORVT_AD_ERROR           8                        ///< A/D����
#define FT3SW_CTORVT_POWER1ST_ABNOR     9                        ///< 1�ŵ�Դ�쳣
#define FT3SW_CTORVT_POWER2ND_ABNOR     10                       ///< 2�ŵ�Դ�쳣
#define FT3SW_CTORVT_HVPS_VALIDITY      11                       ///< ��ѹ��Դ��Ч
#define FT3SW_CTORVT_LASER_POWER_STATUS 12                       ///< �����Դ

///
/// @brief
///     ����/��ѹ������FT3֡�д���ͨ����Ч�Ե�bit��������
///
static uint8_t g_CtorVtFt3FrameChannelBit[] = {
	// ״̬��1�е�bit_n
	5,6,7,8,9,10,11,
	// ״̬��2�е�bit_n
	0,1,2,3,4,5,6,7
};

///
/// @brief
///     ����/��ѹ������FT3֡�б���λ��������
///
static uint8_t g_CtorVtFt3FrameFutureUseBit[] = {
	// ״̬��1�е�bit_n
	14,15,
	// ״̬��2�е�bit_n
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
///     ���ø�״̬������FT3֡������
///
/// @param[in]
///     eFrameType - FT3֡������
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
///     ��ȡ״̬��
///
/// @param[in]
///     nStatusWord1 - ���ڱ���״̬��1
/// @param[in]
///     nStatusWord2 - ���ڱ���״̬��2
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
///     ����״̬��
///
/// @param[in]
///     nStatusWord1 - ״̬��1
/// @param[in]
///     nStatusWord2 - ״̬��2
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
///     ��ȡ�豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�Ҫ��ά�ޣ�
///
/// @return
///     0    - ���ã���Ҫ��ά�ޣ�
///     1    - ����򱨾���Ҫ��ά�ޣ�
///     ���� - ��ȡʧ��
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
///     �����豸��MU�򻥸������Ĺ���״̬���Ƿ�Ҫ��ά�ޣ�
///
/// @param[in]
///     bRequired - true��Ҫ��ά��,�����෴
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
///     ��ȡ�豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�������״̬��
///
/// @return
///     0    - ��ͨ���������У�
///     1    - ����
///     ���� - ��ȡʧ��
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
///     �����豸��MU�򻥸������Ĺ���״̬��Ϣ���Ƿ�������״̬��
///
/// @param[in]
///     bTest - true��Ϊ����,����Ϊ��ͨ
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
///     ��ȡFT3֡��CT���������
///
/// @return
///     0    - ���Ϊi(t)
///     1    - ���Ϊd(i(t))/dt
///     ���� - ��ȡʧ��
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
///     ����FT3֡��CT���������
///
/// @param[in]
///     bOutputIt - true��CT���Ϊi(t),����Ϊd(i(t))/dt
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
///     ��ȡ��������SCP��RangeFlag��Ϣ
///
/// @return
///     0    - ��������SCP=0x01CF
///     1    - ��������SCP=0x00E7
///     ���� - ��ȡʧ��
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
///     ���ñ�������SCP��RangeFlag
///
/// @param[in]
///     bRangeFlag0 - true��RangeFlag=0,��SCP=0x01CF,����RangeFlag=1
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
///     ��ȡMU�Ļ���ʱ��ָʾ��Ϣ
///
/// @return
///     0    - ��ͨ���������У�����ʱ������Ч
///     1    - �����ڼ䣬��ʱ������Ч
///     ���� - ��ȡʧ��
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
///     ����MU�Ļ���ʱ��ָʾ
///
/// @param[in]
///     bWakingUp - true��Ϊ�����ڼ�,����MU��������
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡMU��ͬ��������Ϣ
///
/// @return
///     0    - ���ݼ������ò�ֵ��
///     1    - ���ݼ������ڲ�ֵ��
///     ���� - ��ȡʧ��
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
///     ����MU��ͬ������
///
/// @param[in]
///     bUseInterpolation - true����ò�ֵ��,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡMU��ͬ��״̬��Ϣ���Ƿ�ͬ����
///
/// @return
///     0    - ����ͬ��
///     1    - ʱ��ͬ������/��Ч
///     ���� - ��ȡʧ��
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
///     ����MU��ͬ��״̬
///
/// @param[in]
///     bSync - true������ͬ��,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ�������ļ���ʱ��ָʾ��Ϣ
///
/// @return
///     0    - ��ͨ���������У�����ʱ������Ч
///     1    - �����ڼ䣬��ʱ������Ч
///     ���� - ��ȡʧ��
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
///     ���û������ļ���ʱ��ָʾ
///
/// @param[in]
///     bFiring - true��Ϊ�����ڼ�,���򻥸�����������
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ��������ͬ��������Ϣ
///
/// @return
///     0    - ���ݼ������ò�ֵ��
///     1    - ���ݼ������ڲ�ֵ��
///     ���� - ��ȡʧ��
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
///     ���û�������ͬ������
///
/// @param[in]
///     bUseInterpolation - true����ò�ֵ��,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ��������ͬ��״̬��Ϣ
///
/// @return
///     0    - ����ͬ��
///     1    - ʱ��ͬ������/��Ч
///     ���� - ��ȡʧ��
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
///     ���û�������ͬ��״̬
///
/// @param[in]
///     bSync - true������ͬ��,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ��������A/Dת����״̬��Ϣ
///
/// @return
///     0    - A/Dת����ȷ
///     1    - A/Dת������
///     ���� - ��ȡʧ��
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
///     ���û�������A/Dת����״̬
///
/// @param[in]
///     bError - true��A/Dת������,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ��������1�ŵ�Դ��״̬��Ϣ
///
/// @return
///     0    - ����
///     1    - �쳣
///     ���� - ��ȡʧ��
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
///     ���û�������1�ŵ�Դ��״̬
///
/// @param[in]
///     bAbnor - true���쳣,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ��������2�ŵ�Դ��״̬��Ϣ
///
/// @return
///     0    - ����
///     1    - �쳣
///     ���� - ��ȡʧ��
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
///     ���û�������2�ŵ�Դ��״̬
///
/// @param[in]
///     bAbnor - true���쳣,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ�������ĸ�ѹ��Դ����Ч����Ϣ
///
/// @return
///     0    - ��Ч
///     1    - ��Ч
///     ���� - ��ȡʧ��
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
///     ���û������ĸ�ѹ��Դ����Ч��
///
/// @param[in]
///     bValid - true����Ч,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ�������ļ����Դ�Ķ���/״̬��Ϣ
///
/// @return
///     �����Դ�Ķ���/״̬
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
///     ���û������ļ����Դ�Ķ���/״̬
///
/// @param[in]
///     eStatus -  �����Դ�Ķ���/״̬
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡFT3֡�и�ͨ�����ݵ���Ч����Ϣ
///
/// @param[in]
///     nChannelIdx - ͨ����˳��ţ���0��ʼ
///
/// @return
///     0    - ��Ч
///     1    - ��Ч
///     ���� - ��ȡʧ��
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
///     ����FT3֡�и�ͨ�����ݵ���Ч��
///
/// @param[in]
///     nChannelIdx - ͨ����˳��ţ���0��ʼ
/// @param[in]
///     bValid      - true����Ч,�����෴
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
///     ��ȡ״̬���и�����λ��ֵ
///
/// @param[in]
///     nBitIdx - ����λ��˳��ţ���0��ʼ
///
/// @return
///     ��λ��ֵ - 0��1
///     ����     - ��ȡʧ��
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
///     ����״̬���и�����λ��ֵ
///
/// @param[in]
///     nBitIdx - ͨ����˳��ţ���0��ʼ
/// @param[in]
///     bBitVal - true��Ϊ1,����Ϊ0
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
//    IEC61850�����TimeStamp�����е�TimeQuality�ı����ӿڵ�ʵ��
//=============================================================================
//

///
/// @brief
///     ��ȡTimeQuality��ĳλ��ʹ�õ�����
///
#define IEC61850_TQ_BIT_MASK 0x01

///
/// @brief
///     TimeQuality�и����Ե�bit��������
///
#define IEC61850_TQ_LEAP_SECOND_KNOWN 7          ///< ������֪
#define IEC61850_TQ_CLOCK_FAILURE 6              ///< ʱ�ӹ���
#define IEC61850_TQ_CLOCK_NOT_SYNC 5             ///< ʱ��δͬ��
#define IEC61850_TQ_SECOND_ACCURACY 0            ///< TimeStamp�����С�����ֵ�ʱ�侫�ȣ�bit4~0��

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
///     ��ȡTimeQuality
///
/// @return
///     TimeQualityֵ
///
uint8_t Iec61850TimeQuality::GetTimeQuality()
{
	return m_nTimeQuality;
}

///
/// @brief
///     ����TimeQuality
///
/// @param[in]
///     tq - TimeQualityֵ
///
void Iec61850TimeQuality::SetTimeQuality(uint8_t tq)
{
	m_nTimeQuality = tq;
}

///
/// @brief
///     �ж������Ƿ���֪
///
/// @return
///     true  - ��֪
///     false - δ֪
///
bool Iec61850TimeQuality::IsLeapSecondKnown()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_LEAP_SECOND_KNOWN)) != 0 );
}

///
/// @brief
///     ���������Ƿ���֪
///
/// @param[in]
///     bKnown - true����֪,�����෴
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
///     �ж�ʱ���Ƿ����
///
/// @return
///     true  - ����
///     false - ����
///
bool Iec61850TimeQuality::IsClockFailure()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_FAILURE)) != 0 );
}

///
/// @brief
///     ����ʱ�ӹ���״̬
///
/// @param[in]
///     bFail - true��ʱ�ӹ���,�����෴
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
///     �ж�ʱ���Ƿ�δͬ��
///
/// @return
///     true  - δͬ��
///     false - ͬ��
///
bool Iec61850TimeQuality::IsClockNotSync()
{
	return ( (m_nTimeQuality & (IEC61850_TQ_BIT_MASK << IEC61850_TQ_CLOCK_NOT_SYNC)) != 0 );
}

///
/// @brief
///     ����ʱ��ͬ��״̬
///
/// @param[in]
///     bSync - true��ʱ��ͬ��,�����෴
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
///     ��ȡIEC61850ʱ��������С�����ֵ�ʱ�侫��
///
/// @return
///     -1   - �Ƿ�
///     -2   - δ�涨
///     ���� - ��ֵ��Ӧ����λ���ȣ�0~23λ���ȣ�
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
///     ����IEC61850ʱ��������С�����ֵ�ʱ�侫��
///
/// @param[in]
///     nAccuracy - ��Ӧ����λ���ȵ���ֵ��0~23λ���ȣ�
///
/// @return
///     true  - �ɹ�
///     false - ʧ��
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
//    IEC61850����Ļ�������Quality�ı����ӿ�
//    ע�⣺
//    1��Iec61850SmvQuality������SMV����Iec61850GooseQuality������Goose��
//       �������Ի���һ�£�����SMV����Ϊuint32����Goose������Ϊbit-string��
//    2��IEC61850-9-2LE ��ʹ��validity��test��derived�������ԣ���
//       derived����ΪIEC61850-9-2LE ��Quality���͵����䶨�塣
//=============================================================================
//

///
/// @brief
///     ��ȡIEC61850�ж���Ļ�������Quality��ĳλ��ʹ�õ�����
///
#define IEC61850_QUALITY_BIT_MASK 0x00000001

///
/// @brief
///     IEC61850�ж���Ļ�������Quality�ĸ����������ڵ�bit��������
///
#define IEC61850_QUALITY_VALIDITY         0    ///< �Ϸ���
#define IEC61850_QUALITY_OVERFLOW         2    ///< ���
#define IEC61850_QUALITY_OUTOFRANGE       3    ///< ������
#define IEC61850_QUALITY_BADREFERENCE     4    ///< ����׼ֵ
#define IEC61850_QUALITY_OSCILLATORY      5    ///< �񵴣�������
#define IEC61850_QUALITY_FAILURE          6    ///< ����
#define IEC61850_QUALITY_OLDDATA          7    ///< ������
#define IEC61850_QUALITY_INCONSISTENT     8    ///< ��һ��
#define IEC61850_QUALITY_INACCURATE       9    ///< ��׼ȷ
#define IEC61850_QUALITY_SOURCE           10   ///< Դ
#define IEC61850_QUALITY_TEST             11   ///< ����/����
#define IEC61850_QUALITY_OPERATOR_BLOCKED 12   ///< ����Ա����
#define IEC61850_QUALITY_DERIVED          13   ///< ��������ʾ���ֵ�Ƿ�ͨ�������������λΪIEC61850-9-2LE�����䶨��

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
///     ��ȡQuality��Ʒ��������ֵ
///
/// @return
///     Quality��Ʒ��������ֵ
///
uint32_t Iec61850SmvQuality::GetQuality()
{
	return m_nQuality;
}

///
/// @brief
///     ����Quality��Ʒ��������ֵ
///
/// @param[in]
///     q - Quality��Ʒ��������ֵ
///
void Iec61850SmvQuality::SetQuality(uint32_t q)
{
	m_nQuality = q;
}

///
/// @brief
///     ��ȡ��Ч����Ϣ
///
/// @return
///     ��Ч����Ϣ
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
///     ������Ч��
///
/// @param[in]
///     eValidity - ��Ч��ֵ
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
///     �ж��й�ֵ�Ƿ����
///
/// @return
///     true  - ���
///     false - δ���
///
bool Iec61850SmvQuality::IsOverflow()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����
///
/// @param[in]
///     bOverflow - true�����,�����෴
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
///     �ж��й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsOutofRange()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
///
/// @param[in]
///     bOutofRange - true������,�����෴
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
///     �ж��й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsBadReference()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
///
/// @param[in]
///     bBadReference - true�򻵻�׼ֵ,�����෴
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
///     �ж��й�ֵ�Ƿ��񵴣�������
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsOscillatory()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ��񵴣�������
///
/// @param[in]
///     bOscillatory - true�򶶶�,�����෴
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
///     �ж��й�ֵ�Ƿ�Ϊ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsFailure()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
///
/// @param[in]
///     bFail - true�����,�����෴
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
///     �ж��й�ֵ�Ƿ�Ϊ�����ݣ��ڸ���ʱ�����ڲ����ã�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsOldData()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ�����ݣ��ڸ���ʱ�����ڲ����ã�
///
/// @param[in]
///     bOldData - true��Ϊ������,�����෴
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
///     �ж��й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsInconsistent()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
///
/// @param[in]
///     bInconsistent - true��Ϊ��һ��,�����෴
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
///     �ж��й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsInaccurate()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
///
/// @param[in]
///     bInaccurate - true��Ϊ����ȷ,�����෴
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
///     ��ȡ�й�ֵ����Դ��Ϣ
///
/// @return
///     ��Դ������
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
///     �����й�ֵ����Դ����
///
/// @param[in]
///     eSourceType - ��Դ������
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
///     �ж��Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsTest()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST)) != 0 );
}

///
/// @brief
///     �����Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
///
/// @param[in]
///     bTest - true���ڲ���/����״̬,�����෴
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
///     �ж�ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsOperatorBlocked()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED)) != 0 );
}

///
/// @brief
///     ����ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
///
/// @param[in]
///     bOperatorBlocked - true�򱻲���Ա����,�����෴
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
///     �ж��й�ֵ�Ƿ�������ͨ�����㣩����
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850SmvQuality::IsDerived()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_DERIVED)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ�������ͨ�����㣩����
///
/// @param[in]
///     bDerived - true��Ϊ����,�����෴
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
	// 16λ����
	m_nQuality = uint16_BitReverse(m_nQuality);
}

///
/// @brief
///     ��ȡQuality��Ʒ��������ֵ
///
/// @return
///     Quality��Ʒ��������ֵ
///
uint16_t Iec61850GooseQuality::GetQuality()
{
	return uint16_BitReverse(m_nQuality);
}

///
/// @brief
///     ����Quality��Ʒ��������ֵ
///
/// @param[in]
///     q - Quality��Ʒ��������ֵ
///
void Iec61850GooseQuality::SetQuality(uint16_t q)
{
	m_nQuality = uint16_BitReverse(q);
}

///
/// @brief
///     ��ȡ��Ч����Ϣ
///
/// @return
///     ��Ч����Ϣ
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
///     ������Ч��
///
/// @param[in]
///     eValidity - ��Ч��ֵ
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
///     �ж��й�ֵ�Ƿ����
///
/// @return
///     true  - ���
///     false - δ���
///
bool Iec61850GooseQuality::IsOverflow()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OVERFLOW)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����
///
/// @param[in]
///     bOverflow - true�����,�����෴
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
///     �ж��й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsOutofRange()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OUTOFRANGE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����̣�����Ԥ�ȶ�������̣����������͵���Ч��Χ��
///
/// @param[in]
///     bOutofRange - true������,�����෴
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
///     �ж��й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsBadReference()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_BADREFERENCE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ񻵻�׼ֵ����׼ֵ�޷�У׼��
///
/// @param[in]
///     bBadReference - true�򻵻�׼ֵ,�����෴
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
///     �ж��й�ֵ�Ƿ��񵴣�������
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsOscillatory()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OSCILLATORY)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ��񵴣�������
///
/// @param[in]
///     bOscillatory - true�򶶶�,�����෴
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
///     �ж��й�ֵ�Ƿ�Ϊ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsFailure()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_FAILURE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ����ֵ�����ӹ��ܼ��һ���ڲ����ⲿ���ϣ�
///
/// @param[in]
///     bFail - true�����,�����෴
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
///     �ж��й�ֵ�Ƿ�Ϊ�����ݣ��ڸ���ʱ�����ڲ����ã�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsOldData()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OLDDATA)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ�����ݣ��ڸ���ʱ�����ڲ����ã�
///
/// @param[in]
///     bOldData - true��Ϊ������,�����෴
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
///     �ж��й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsInconsistent()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INCONSISTENT)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ�һ�£����������Ѽ����һ�£�
///
/// @param[in]
///     bInconsistent - true��Ϊ��һ��,�����෴
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
///     �ж��й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsInaccurate()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_INACCURATE)) != 0 );
}

///
/// @brief
///     �����й�ֵ�Ƿ񲻾�ȷ����ֵ������Դ�������ľ��ȣ�
///
/// @param[in]
///     bInaccurate - true��Ϊ����ȷ,�����෴
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
///     ��ȡ�й�ֵ����Դ��Ϣ
///
/// @return
///     ��Դ������
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
///     �����й�ֵ����Դ����
///
/// @param[in]
///     eSourceType - ��Դ������
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
///     �ж��Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsTest()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_TEST)) != 0 );
}

///
/// @brief
///     �����Ƿ��ڲ���/����״̬���й�ֵΪ���ڲ��Ե�ֵ��������������Ŀ�ģ�
///
/// @param[in]
///     bTest - true���ڲ���/����״̬,�����෴
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
///     �ж�ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
///
/// @return
///     true  - ��
///     false - ��
///
bool Iec61850GooseQuality::IsOperatorBlocked()
{
	return ( (m_nQuality & (IEC61850_QUALITY_BIT_MASK << IEC61850_QUALITY_OPERATOR_BLOCKED)) != 0 );
}

///
/// @brief
///     ����ֵ��ˢ���Ƿ񱻲���Ա���������й�ֵΪ����ǰ��ֵ��
///
/// @param[in]
///     bOperatorBlocked - true�򱻲���Ա����,�����෴
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
