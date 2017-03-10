/// @file
///     sav91_asdu_seg.cpp
///
/// @brief
///     Sav91��IEC91850-9-1������ֵ���ĵ�ASDU��
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
#include "sav91_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     Sav91����ֵ���ĵ�ASDU�ĸ�ʽ
///
typedef enum {
	SAV91_ASDU_LEN = 0,
	SAV91_LN_NAME,
	SAV91_DATASET_NAME,
	SAV91_LD_NAME,
	SAV91_PHS_ARTG,
	SAV91_NEUT_ARTG,
	SAV91_PHS_VRTG,
	SAV91_DELAY_TIME,
	SAV91_DATA_CHANNEL,
// 	SAV91_DATA_CHANNEL_2,
// 	SAV91_DATA_CHANNEL_3,
// 	SAV91_DATA_CHANNEL_4,
// 	SAV91_DATA_CHANNEL_5,
// 	SAV91_DATA_CHANNEL_6,
// 	SAV91_DATA_CHANNEL_7,
// 	SAV91_DATA_CHANNEL_8,
// 	SAV91_DATA_CHANNEL_9,
// 	SAV91_DATA_CHANNEL_10,
// 	SAV91_DATA_CHANNEL_11,
// 	SAV91_DATA_CHANNEL_12,
	SAV91_STATUS_WORD_1,
	SAV91_STATUS_WORD_2,
	SAV91_SMPCNT,
	SAV91_SMPRATE,
	SAV91_CONFREV
}SAV91_ASDU_FORMAT;

///
/// @brief
///     ��Ӧ�����ASDU��ʽ��Sav91����ֵ���ĵ�ASDU���ֶ�ռ�õ��ֽ�
///
static uint8_t g_Sav91_Asdu_Size_List[] = {
	2,1,1,2,2,2,2,2,2*12,/*2,*/
//	2,2,2,2,2,2,2,2,2,2,
	2,2,2,1,1
};

Sav91AsduSeg::Sav91AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
{
	m_AsduData->reserve(sizeof(g_Sav91_Asdu_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Sav91_Asdu_Size_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

Sav91AsduSeg::~Sav91AsduSeg()
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
size_t Sav91AsduSeg::Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize)
{
	if ( bufSize < SAV91_ASDU_DEFAULT_SIZE )
		return 0;

	int    decIdx = 0;
	size_t dSize  = 0;
	size_t offTmp = 0;
	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;
		dSize = (*it)->Decode(offset+offTmp,&asduDataBuf[offTmp],g_Sav91_Asdu_Size_List[decIdx]);
		if ( dSize == 0 )
			return 0;
		offTmp += dSize;
		decIdx++;
	}
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���ASDU������
///
/// @param[in]
///     asduDataArr - ����ASDU������������
/// @param[in]
///     arrSize     - �����С
///
/// @return
///     ����ASDU�������������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91AsduSeg::Code(uint8_t *asduDataArr, const size_t arrSize)
{
	if ( arrSize < SAV91_ASDU_DEFAULT_SIZE )
		return 0;

	size_t cSize  = 0;
	size_t offTmp = 0;
	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		if ( offTmp >= arrSize )
			break;
		cSize = (*it)->Code(&asduDataArr[offTmp],arrSize-offTmp);
		if ( cSize == 0 )
			return 0;
		offTmp += cSize;
	}
	return offTmp;
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
bool Sav91AsduSeg::GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV91_ASDU_PUBLIC &asduPublic)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	size_t   datSize = 2;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_ASDU_LEN)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_ASDU_LEN)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.asduLen = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_LN_NAME)->GetValueOffset()];
	asduPublic.lnName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_DATASET_NAME)->GetValueOffset()];
	asduPublic.dataSetName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_LD_NAME)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_LD_NAME)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.ldName = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_PHS_ARTG)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_PHS_ARTG)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.phsArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_NEUT_ARTG)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_NEUT_ARTG)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.neutArtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_PHS_VRTG)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_PHS_VRTG)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.phsVrtg = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_DELAY_TIME)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_DELAY_TIME)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.delayTime = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_STATUS_WORD_1)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_STATUS_WORD_1)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_STATUS_WORD_2)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_STATUS_WORD_2)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.statusWord2 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_SMPCNT)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_SMPCNT)->GetValueLen();
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,datSize);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_SMPRATE)->GetValueOffset()];
	asduPublic.smpRate = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_CONFREV)->GetValueOffset()];
	asduPublic.confRev = *uint8Buf;

	return true;
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ�����������Ϣ
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     smpCnt     - ���������������Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91AsduSeg::GetSmpCntInASDU(const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	size_t   datSize = 2;

	uint8_t *uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV91_SMPCNT)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_SMPCNT)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	smpCnt = BSWAP_16(uint16Tmp);

	return true;
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�е�״̬����Ϣ
///
/// @param[in]
///     msgDataBuf  - ���汨��֡���ݵĻ���
/// @param[in]
///     statusWord1 - ����״̬��1��Ϣ
/// @param[in]
///     statusWord2 - ����״̬��2��Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91AsduSeg::GetStatusWordsInASDU(const uint8_t *msgDataBuf, uint16_t &statusWord1, uint16_t &statusWord2)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	size_t   datSize = 2;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_STATUS_WORD_1)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_STATUS_WORD_1)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_STATUS_WORD_2)->GetValueOffset()];
//	datSize   = m_AsduData->at(SAV91_STATUS_WORD_2)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	statusWord2 = BSWAP_16(uint16Tmp);

	return true;
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
size_t Sav91AsduSeg::GetASDUSmps(const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	size_t   i = 0;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;
	uint8_t *uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_DATA_CHANNEL)->GetValueOffset()];
	for ( i = 0; i < SAV91_ASDU_CHANNEL_NUM; i++ )
	{
		if ( i == arrSize )
			break;
		memcpy((uint8_t *)&uint16Tmp,&uint8Buf[i*2],2);
		smpValArr[i] = BSWAP_16(uint16Tmp);
	}
	return i;
}

///
/// @brief
///     ��ȡASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     
///
void Sav91AsduSeg::GetASDUPublicInfoOff(SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.asduLenOff     = m_AsduData->at(SAV91_ASDU_LEN)->GetElemOffset();
	asduPublicOff.lnNameOff      = m_AsduData->at(SAV91_LN_NAME)->GetElemOffset();
	asduPublicOff.dataSetNameOff = m_AsduData->at(SAV91_DATASET_NAME)->GetElemOffset();
	asduPublicOff.ldNameOff      = m_AsduData->at(SAV91_LD_NAME)->GetElemOffset();
	asduPublicOff.phsArtgOff     = m_AsduData->at(SAV91_PHS_ARTG)->GetElemOffset();
	asduPublicOff.neutArtgOff    = m_AsduData->at(SAV91_NEUT_ARTG)->GetElemOffset();
	asduPublicOff.phsVrtgOff     = m_AsduData->at(SAV91_PHS_VRTG)->GetElemOffset();
	asduPublicOff.delayTimeOff   = m_AsduData->at(SAV91_DELAY_TIME)->GetElemOffset();
	asduPublicOff.statusWord1Off = m_AsduData->at(SAV91_STATUS_WORD_1)->GetElemOffset();
	asduPublicOff.statusWord2Off = m_AsduData->at(SAV91_STATUS_WORD_2)->GetElemOffset();
	asduPublicOff.smpCntOff      = m_AsduData->at(SAV91_SMPCNT)->GetElemOffset();
	asduPublicOff.smpRateOff     = m_AsduData->at(SAV91_SMPRATE)->GetElemOffset();
	asduPublicOff.confRevOff     = m_AsduData->at(SAV91_CONFREV)->GetElemOffset();
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
int Sav91AsduSeg::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	int i = 0;
	smpOffArr[i] = m_AsduData->at(SAV91_DATA_CHANNEL)->GetElemOffset();
	for ( i = 1; i < SAV91_ASDU_CHANNEL_NUM; i++ )
	{
		if ( i == arrSize )
			break;
		smpOffArr[i] = smpOffArr[i-1]+2;
//		smpOffArr[i] = m_AsduData->at(i+SAV91_DATA_CHANNEL_1)->GetElemOffset();
	}
	return i;
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
///     true - �ɹ��� false - ʧ��
///
bool Sav91AsduSeg::SetASDU(const SAV91_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	if ( bufSize > SAV91_ASDU_CHANNEL_NUM )
		return false;

	uint16_t uint16Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	// ���ù�����Ϣ
	uint16Tmp = BSWAP_16(asduPublic.asduLen);
	m_AsduData->at(SAV91_ASDU_LEN)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.lnName;
	m_AsduData->at(SAV91_LN_NAME)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.dataSetName;
	m_AsduData->at(SAV91_DATASET_NAME)->SetValue(&uint8Tmp,1);
	uint16Tmp = BSWAP_16(asduPublic.ldName);
	m_AsduData->at(SAV91_LD_NAME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsArtg);
	m_AsduData->at(SAV91_PHS_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.neutArtg);
	m_AsduData->at(SAV91_NEUT_ARTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.phsVrtg);
	m_AsduData->at(SAV91_PHS_VRTG)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.delayTime);
	m_AsduData->at(SAV91_DELAY_TIME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord1);
	m_AsduData->at(SAV91_STATUS_WORD_1)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord2);
	m_AsduData->at(SAV91_STATUS_WORD_2)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(SAV91_SMPCNT)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.smpRate;
	m_AsduData->at(SAV91_SMPRATE)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.confRev;
	m_AsduData->at(SAV91_CONFREV)->SetValue(&uint8Tmp,1);

	// ��������ͨ��
	static uint16_t buf[SAV91_ASDU_CHANNEL_NUM];
	for ( size_t i = 0; i < SAV91_ASDU_CHANNEL_NUM; i++ )
	{
		if ( i >= bufSize )
			break;
		buf[i] = BSWAP_16(smpValBuf[i]);
	}
	m_AsduData->at(SAV91_DATA_CHANNEL)->SetValue((uint8_t *)&buf,2*SAV91_ASDU_CHANNEL_NUM);
	return true;
}
