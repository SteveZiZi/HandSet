/// @file
///     triphase_ct_ft3_asdu_seg.cpp
///
/// @brief
///     �������������FT3��ASDU��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.05
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
#include "triphase_ct_ft3_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     �������������FT3��ASDU�ĸ�ʽ
///
typedef enum {
	TRIPHASE_CT_FT3_MSG_TYPE = 0,
	TRIPHASE_CT_FT3_TEMPERATURE,
	TRIPHASE_CT_FT3_DELAY_TIME,
	TRIPHASE_CT_FT3_DATA_CHANNEL_1,
	TRIPHASE_CT_FT3_DATA_CHANNEL_2,
	TRIPHASE_CT_FT3_DATA_CHANNEL_3,
	TRIPHASE_CT_FT3_DATA_CHANNEL_4,
	TRIPHASE_CT_FT3_DATA_CHANNEL_5,
	TRIPHASE_CT_FT3_DATA_CHANNEL_6,
	TRIPHASE_CT_FT3_CRC1,
	TRIPHASE_CT_FT3_DATA_CHANNEL_7,
	TRIPHASE_CT_FT3_DATA_CHANNEL_8,
	TRIPHASE_CT_FT3_DATA_CHANNEL_9,
	TRIPHASE_CT_FT3_STATUS_WORD_1,
	TRIPHASE_CT_FT3_STATUS_WORD_2,
	TRIPHASE_CT_FT3_SMPCNT,
	TRIPHASE_CT_FT3_CRC2
}TRIPHASE_CT_FT3_ASDU_FORMAT;

///
/// @brief
///     ��Ӧ�����ASDU��ʽ���������������FT3��ASDU���ֶ�ռ�õ��ֽ�
///
static uint8_t g_Triphase_CT_FT3_Asdu_Size_List[] = {
	1,1,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2
};

TriphaseCTFT3AsduSeg::TriphaseCTFT3AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_AsduLen(0)
, m_AsduOff(0)
{
	m_AsduData->reserve(sizeof(g_Triphase_CT_FT3_Asdu_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Triphase_CT_FT3_Asdu_Size_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

TriphaseCTFT3AsduSeg::~TriphaseCTFT3AsduSeg()
{
	if ( m_AsduData != NULL )
	{
		std::vector<MsgDataElem *>::iterator it;
		for ( it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
		{
			ReleaseMsgDataElemImpl(*it);
		}
		delete m_AsduData;
		m_AsduData = NULL;
	}
}

///
/// @brief
///     �Ա���֡�е�ASDU���ֽ��н���
///
/// @param[in]
///     offset      - �������ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     asduDataBuf - ����������ASDU���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t TriphaseCTFT3AsduSeg::Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize)
{
	if ( bufSize < TRIPHASE_CT_FT3_MSG_WORD-1 || asduDataBuf == NULL )
		return 0;

	size_t dSize  = 0;
	size_t offTmp = 0;
	uint8_t *pBuf = (uint8_t *)asduDataBuf;

	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	int i = TRIPHASE_CT_FT3_MSG_TYPE;
	for ( ; i <= TRIPHASE_CT_FT3_CRC2; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= bufSize )
			break;

		if ( i == TRIPHASE_CT_FT3_CRC1 || i == TRIPHASE_CT_FT3_CRC2 )
		{
			// ����CRCУ��
			offTmp += g_Triphase_CT_FT3_Asdu_Size_List[i];
		}
		else
		{
			dSize = m_AsduData->at(i)->Decode(offset+offTmp,&pBuf[offTmp],g_Triphase_CT_FT3_Asdu_Size_List[i]);
			if ( dSize == 0 )
				return 0;
			offTmp += dSize;
		}
	}

	m_AsduLen = offTmp;
	m_AsduOff = offset;
	return (offTmp/sizeof(uint16_t));
}

///
/// @brief
///     �����õĲ������б��룬���ASDU��������������CRCУ��
///
/// @param[in]
///     asduDataArr - ����ASDU������������
/// @param[in]
///     arrSize     - �����С
///
/// @return
///     ����ASDU�������������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t TriphaseCTFT3AsduSeg::Code(uint16_t *asduDataArr, const size_t arrSize)
{
	if ( arrSize < TRIPHASE_CT_FT3_MSG_WORD-1 || asduDataArr == NULL )
		return 0;

	uint16_t bufTmp[TRIPHASE_CT_FT3_MSG_WORD-1];
	size_t   cSize  = 0;
	size_t   offTmp = 0;
	uint8_t *pBuf   = (uint8_t *)bufTmp;

	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	int i = TRIPHASE_CT_FT3_MSG_TYPE;
	for ( ; i <= TRIPHASE_CT_FT3_CRC2; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= arrSize )
			break;

		if ( i == TRIPHASE_CT_FT3_CRC1 || i == TRIPHASE_CT_FT3_CRC2 )
		{
			// ����CRCУ��
			continue;
		}
		else
		{
			cSize = m_AsduData->at(i)->Code(&pBuf[offTmp],arrSize*sizeof(uint16_t)-offTmp);
			offTmp += cSize;
		}
	}

	// �Ա������ݽ��д�С��ת��
	for ( size_t i = 0; i < offTmp/sizeof(uint16_t); i++ )
		asduDataArr[i] = BSWAP_16(bufTmp[i]);

	m_AsduLen = offTmp;
	return (offTmp/sizeof(uint16_t));
}

///
/// @brief
///     ��ȡASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool TriphaseCTFT3AsduSeg::GetASDUPublicInfo(const uint16_t *msgDataBuf, CTORVT_FT3_ASDU_PUBLIC &asduPublic)
{
	if ( msgDataBuf == NULL )
		return false;

	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_MSG_TYPE)->GetValueOffset()];
	asduPublic.msgType = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_TEMPERATURE)->GetValueOffset()];
	asduPublic.temperature = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_DELAY_TIME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.delayTime = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_1)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_2)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord2 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(TRIPHASE_CT_FT3_SMPCNT)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	return true;
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t TriphaseCTFT3AsduSeg::GetASDUSmpsNum()
{
	return TRIPHASE_CT_FT3_ASDU_CHANNEL_NUM;
}

///
/// @brief
///     ��ȡASDU�еĲ���ֵ����
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     smpValArr  - �������˲ʱֵ������
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t TriphaseCTFT3AsduSeg::GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	if ( msgDataBuf == NULL || smpValArr == NULL )
		return 0;

	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;
	int i = TRIPHASE_CT_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= TRIPHASE_CT_FT3_DATA_CHANNEL_9; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == TRIPHASE_CT_FT3_CRC1 )
			continue;
		uint8Buf = &pMsgBuf[m_AsduData->at(i)->GetValueOffset()];
		memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
		smpValArr[smpNum] = BSWAP_16(uint16Tmp);
		smpNum++;
	}
	return smpNum;
}

///
/// @brief
///     ��ȡASDU�ĳ���
///
/// @return
///     ASDU�ĳ���
///
size_t TriphaseCTFT3AsduSeg::GetASDULen()
{
	return m_AsduLen;
}

///
/// @brief
///     ��ȡASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t TriphaseCTFT3AsduSeg::GetASDUOff()
{
	return m_AsduOff;
}

///
/// @brief
///     ��ȡASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     asduPublicOff - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     
///
void TriphaseCTFT3AsduSeg::GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.msgTypeOff     = m_AsduData->at(TRIPHASE_CT_FT3_MSG_TYPE)->GetElemOffset();
	asduPublicOff.temperatureOff = m_AsduData->at(TRIPHASE_CT_FT3_TEMPERATURE)->GetElemOffset();
	asduPublicOff.delayTimeOff   = m_AsduData->at(TRIPHASE_CT_FT3_DELAY_TIME)->GetElemOffset();
	asduPublicOff.statusWord1Off = m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_1)->GetElemOffset();
	asduPublicOff.statusWord2Off = m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_2)->GetElemOffset();
	asduPublicOff.smpCntOff      = m_AsduData->at(TRIPHASE_CT_FT3_SMPCNT)->GetElemOffset();
}

///
/// @brief
///     ��ȡASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     smpOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     arrSize   - �����С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t TriphaseCTFT3AsduSeg::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	if ( smpOffArr == NULL )
		return 0;

	int i = TRIPHASE_CT_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= TRIPHASE_CT_FT3_DATA_CHANNEL_9; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == TRIPHASE_CT_FT3_CRC1 )
			continue;
		smpOffArr[smpNum] = m_AsduData->at(i)->GetElemOffset();
		smpNum++;
	}
	return smpNum;
}

///
/// @brief
///     ����ASDU��Ϣ
///
/// @param[in]
///     asduPublic - ASDU�Ĺ�����Ϣ
/// @param[in]
///     smpValBuf  - �������˲ʱֵ�Ļ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     
///
void TriphaseCTFT3AsduSeg::SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	if ( smpValBuf == NULL )
		return;

	uint16_t uint16Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	// ���ù�����Ϣ
	uint8Tmp = 0x02;
	m_AsduData->at(TRIPHASE_CT_FT3_MSG_TYPE)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.temperature;
	m_AsduData->at(TRIPHASE_CT_FT3_TEMPERATURE)->SetValue(&uint8Tmp,1);
	uint16Tmp = BSWAP_16(asduPublic.delayTime);
	m_AsduData->at(TRIPHASE_CT_FT3_DELAY_TIME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord1);
	m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_1)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord2);
	m_AsduData->at(TRIPHASE_CT_FT3_STATUS_WORD_2)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(TRIPHASE_CT_FT3_SMPCNT)->SetValue((uint8_t *)&uint16Tmp,2);

	// ��������ͨ��
	int i = TRIPHASE_CT_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= TRIPHASE_CT_FT3_DATA_CHANNEL_9; i++ )
	{
		if ( i == TRIPHASE_CT_FT3_CRC1 )
			continue;
		// �����ͨ����ֵ��Ϊ0
		if ( smpNum >= bufSize )
			uint16Tmp = 0;
		else
			uint16Tmp = BSWAP_16(smpValBuf[smpNum]);
		m_AsduData->at(i)->SetValue((uint8_t *)&uint16Tmp,2);
		smpNum++;
	}
}
