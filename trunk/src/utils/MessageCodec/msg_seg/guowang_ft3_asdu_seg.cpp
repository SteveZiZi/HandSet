/// @file
///     guowang_ft3_asdu_seg.cpp
///
/// @brief
///     ����FT3����ֵ���ĵ�ASDU��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
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
#include "GUOWANG_FT3_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     ����FT3����ֵ���ĵ�ASDU�ĸ�ʽ
typedef enum {
	GUOWANG_FT3_ASDU_LEN = 0,
	GUOWANG_FT3_LN_NAME,
	GUOWANG_FT3_DATASET_NAME,
	GUOWANG_FT3_LD_NAME,
	GUOWANG_FT3_PHS_ARTG,
	GUOWANG_FT3_NEUT_ARTG,
	GUOWANG_FT3_PHS_VRTG,
	GUOWANG_FT3_DELAY_TIME,
	GUOWANG_FT3_SMPCNT,
	GUOWANG_FT3_CRC1,
	GUOWANG_FT3_DATA_CHANNEL_1,
	GUOWANG_FT3_DATA_CHANNEL_2,
	GUOWANG_FT3_DATA_CHANNEL_3,
	GUOWANG_FT3_DATA_CHANNEL_4,
	GUOWANG_FT3_DATA_CHANNEL_5,
	GUOWANG_FT3_DATA_CHANNEL_6,
	GUOWANG_FT3_DATA_CHANNEL_7,
	GUOWANG_FT3_DATA_CHANNEL_8,
	GUOWANG_FT3_CRC2,
	GUOWANG_FT3_DATA_CHANNEL_9,
	GUOWANG_FT3_DATA_CHANNEL_10,
	GUOWANG_FT3_DATA_CHANNEL_11,
	GUOWANG_FT3_DATA_CHANNEL_12,
	GUOWANG_FT3_DATA_CHANNEL_13,
	GUOWANG_FT3_DATA_CHANNEL_14,
	GUOWANG_FT3_DATA_CHANNEL_15,
	GUOWANG_FT3_DATA_CHANNEL_16,
	GUOWANG_FT3_CRC3,
	GUOWANG_FT3_DATA_CHANNEL_17,
	GUOWANG_FT3_DATA_CHANNEL_18,
	GUOWANG_FT3_DATA_CHANNEL_19,
	GUOWANG_FT3_DATA_CHANNEL_20,
	GUOWANG_FT3_DATA_CHANNEL_21,
	GUOWANG_FT3_DATA_CHANNEL_22,
	GUOWANG_FT3_STATUS_WORD_1,
	GUOWANG_FT3_STATUS_WORD_2,
	GUOWANG_FT3_CRC4
}GUOWANG_FT3_ASDU_FORMAT;

///
/// @brief
///     ��Ӧ�����ASDU��ʽ������FT3����ֵ���ĵ�ASDU���ֶ�ռ�õ��ֽ�
///
static const uint8_t g_GuoWang_FT3_Asdu_Size_List[] = {
	2,1,1,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,2
};

GuoWangFT3AsduSeg::GuoWangFT3AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_AsduLen(0)
, m_AsduOff(0)
{
	m_AsduData->reserve(sizeof(g_GuoWang_FT3_Asdu_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_GuoWang_FT3_Asdu_Size_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

GuoWangFT3AsduSeg::~GuoWangFT3AsduSeg()
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
size_t GuoWangFT3AsduSeg::Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize)
{
	if ( bufSize < GUOWANG_FT3_MSG_WORD-1 )
		return 0;

	size_t dSize  = 0;
	size_t offTmp = 0;
	uint8_t *pBuf = (uint8_t *)asduDataBuf;
	
	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	int i = GUOWANG_FT3_ASDU_LEN;
	for ( ; i <= GUOWANG_FT3_CRC4; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= bufSize )
			break;

		if ( i == GUOWANG_FT3_CRC1 || i == GUOWANG_FT3_CRC2 || i == GUOWANG_FT3_CRC3 || i == GUOWANG_FT3_CRC4 )
		{
			// ����CRCУ��
			offTmp += g_GuoWang_FT3_Asdu_Size_List[i];
		}
		else
		{
			dSize = m_AsduData->at(i)->Decode(offset+offTmp,&pBuf[offTmp],g_GuoWang_FT3_Asdu_Size_List[i]);
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
size_t GuoWangFT3AsduSeg::Code(uint16_t *asduDataArr, const size_t arrSize)
{
	if ( arrSize < GUOWANG_FT3_MSG_WORD-1 )
		return 0;

	uint16_t bufTmp[GUOWANG_FT3_MSG_WORD-1];
	size_t   cSize  = 0;
	size_t   offTmp = 0;
	uint8_t *pBuf   = (uint8_t *)bufTmp;
	
	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	int i = GUOWANG_FT3_ASDU_LEN;
	for ( ; i <= GUOWANG_FT3_CRC4; i++ )
	{
		if ( (offTmp/sizeof(uint16_t)) >= arrSize )
			break;

		if ( i == GUOWANG_FT3_CRC1 || i == GUOWANG_FT3_CRC2 || i == GUOWANG_FT3_CRC3 || i == GUOWANG_FT3_CRC4 )
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
bool GuoWangFT3AsduSeg::GetASDUPublicInfo(const uint16_t *msgDataBuf, GUOWANG_FT3_ASDU_PUBLIC &asduPublic)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_ASDU_LEN)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.asduLen = BSWAP_16(uint16Tmp);
	
	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_LN_NAME)->GetValueOffset()];
	asduPublic.lnName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_DATASET_NAME)->GetValueOffset()];
	asduPublic.dataSetName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_LD_NAME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.ldName = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_PHS_ARTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.phsArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_NEUT_ARTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.neutArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_PHS_VRTG)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.phsVrtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_DELAY_TIME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.delayTime = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_STATUS_WORD_1)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_STATUS_WORD_2)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord2 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(GUOWANG_FT3_SMPCNT)->GetValueOffset()];
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
size_t GuoWangFT3AsduSeg::GetASDUSmpsNum()
{
	return GUOWANG_FT3_ASDU_CHANNEL_NUM;
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
size_t GuoWangFT3AsduSeg::GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;
	int i = GUOWANG_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= GUOWANG_FT3_DATA_CHANNEL_22; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == GUOWANG_FT3_CRC2 || i == GUOWANG_FT3_CRC3 )
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
size_t GuoWangFT3AsduSeg::GetASDULen()
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
size_t GuoWangFT3AsduSeg::GetASDUOff()
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
void GuoWangFT3AsduSeg::GetASDUPublicInfoOff(GUOWANG_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.asduLenOff     = m_AsduData->at(GUOWANG_FT3_ASDU_LEN)->GetElemOffset();
	asduPublicOff.lnNameOff      = m_AsduData->at(GUOWANG_FT3_LN_NAME)->GetElemOffset();
	asduPublicOff.dataSetNameOff = m_AsduData->at(GUOWANG_FT3_DATASET_NAME)->GetElemOffset();
	asduPublicOff.ldNameOff      = m_AsduData->at(GUOWANG_FT3_LD_NAME)->GetElemOffset();
	asduPublicOff.phsArtgOff     = m_AsduData->at(GUOWANG_FT3_PHS_ARTG)->GetElemOffset();
	asduPublicOff.neutArtgOff    = m_AsduData->at(GUOWANG_FT3_NEUT_ARTG)->GetElemOffset();
	asduPublicOff.phsVrtgOff     = m_AsduData->at(GUOWANG_FT3_PHS_VRTG)->GetElemOffset();
	asduPublicOff.delayTimeOff   = m_AsduData->at(GUOWANG_FT3_DELAY_TIME)->GetElemOffset();
	asduPublicOff.statusWord1Off = m_AsduData->at(GUOWANG_FT3_STATUS_WORD_1)->GetElemOffset();
	asduPublicOff.statusWord2Off = m_AsduData->at(GUOWANG_FT3_STATUS_WORD_2)->GetElemOffset();
	asduPublicOff.smpCntOff      = m_AsduData->at(GUOWANG_FT3_SMPCNT)->GetElemOffset();
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
size_t GuoWangFT3AsduSeg::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	int i = GUOWANG_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= GUOWANG_FT3_DATA_CHANNEL_22; i++ )
	{
		if ( smpNum == arrSize )
			break;
		if ( i == GUOWANG_FT3_CRC2 || i == GUOWANG_FT3_CRC3 )
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
void GuoWangFT3AsduSeg::SetASDU(const GUOWANG_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	uint16_t uint16Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	// ���ù�����Ϣ
	uint16Tmp = 0x3E;
	uint16Tmp = BSWAP_16(uint16Tmp);
	m_AsduData->at(GUOWANG_FT3_ASDU_LEN)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.lnName;
	m_AsduData->at(GUOWANG_FT3_LN_NAME)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.dataSetName;
	m_AsduData->at(GUOWANG_FT3_DATASET_NAME)->SetValue(&uint8Tmp,1);
	uint16Tmp = BSWAP_16(asduPublic.ldName);
	m_AsduData->at(GUOWANG_FT3_LD_NAME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsArtg);
	m_AsduData->at(GUOWANG_FT3_PHS_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.neutArtg);
	m_AsduData->at(GUOWANG_FT3_NEUT_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsVrtg);
	m_AsduData->at(GUOWANG_FT3_PHS_VRTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.delayTime);
	m_AsduData->at(GUOWANG_FT3_DELAY_TIME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord1);
	m_AsduData->at(GUOWANG_FT3_STATUS_WORD_1)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord2);
	m_AsduData->at(GUOWANG_FT3_STATUS_WORD_2)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(GUOWANG_FT3_SMPCNT)->SetValue((uint8_t *)&uint16Tmp,2);

	// ��������ͨ��
	int i = GUOWANG_FT3_DATA_CHANNEL_1;
	size_t smpNum = 0;
	for ( ; i <= GUOWANG_FT3_DATA_CHANNEL_22; i++ )
	{
		if ( i == GUOWANG_FT3_CRC2 || i == GUOWANG_FT3_CRC3 )
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
