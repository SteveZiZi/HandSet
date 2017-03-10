/// @file
///     sav91_ex_asdu_seg.cpp
///
/// @brief
///     Sav91Ex��IEC91850-9-1��չ������ֵ���ĵ�ASDU��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.06
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
#include "sav91_ex_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     Sav91����ֵ���ĵ�ASDU�ĸ�ʽ
///
typedef enum {
	SAV91_EX_ASDU_LEN = 0,
	SAV91_EX_LN_NAME,
	SAV91_EX_DATASET_NAME,
	SAV91_EX_LD_NAME,
	SAV91_EX_DATA_CHANNEL,
	SAV91_EX_STATUS_WORD_1,
	SAV91_EX_STATUS_WORD_2,
	SAV91_EX_SMPCNT,
	SAV91_EX_SMPRATE,
	SAV91_EX_CONFREV
}SAV91_EX_ASDU_FORMAT;

///
/// @brief
///     ��Ӧ�����ASDU��ʽ��IEC91850-9-1��չ���ĵ�ASDU���ֶ�ռ�õ��ֽڣ�����ͨ��ռ�õ��ֽ�δ֪
///
static uint8_t g_Sav91_Ex_Asdu_Size_List[] = {
	2,1,1,2,0xFF,2,2,2,1,1
};

Sav91ExAsduSeg::Sav91ExAsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_SmpsNum(0)
{
	m_AsduData->reserve(sizeof(g_Sav91_Ex_Asdu_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Sav91_Ex_Asdu_Size_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

Sav91ExAsduSeg::~Sav91ExAsduSeg()
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
size_t Sav91ExAsduSeg::Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize)
{
	int    decIdx = 0;
	size_t dSize  = 0;
	size_t offTmp = 0;
	// ���ݶ���õĸ�ʽ���ν������ֶ�����
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;

		if ( decIdx == SAV91_EX_DATA_CHANNEL )
			dSize = (*it)->Decode(offset+offTmp,&asduDataBuf[offTmp],m_SmpsNum*2);
		else
			dSize = (*it)->Decode(offset+offTmp,&asduDataBuf[offTmp],g_Sav91_Ex_Asdu_Size_List[decIdx]);
		if ( dSize == 0 )
			return 0;

		if ( decIdx == SAV91_EX_ASDU_LEN )
		{
			// ��ȡͨ����
			uint16_t uint16Tmp;
			memcpy((uint8_t *)&uint16Tmp,&asduDataBuf[offTmp],g_Sav91_Ex_Asdu_Size_List[decIdx]);
			m_SmpsNum = BSWAP_16(uint16Tmp);
			m_SmpsNum = ( m_SmpsNum - 12 ) / 2;
		}
		
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
size_t Sav91ExAsduSeg::Code(uint8_t *asduDataArr, const size_t arrSize)
{
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
bool Sav91ExAsduSeg::GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV91_EX_ASDU_PUBLIC &asduPublic)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_ASDU_LEN)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.asduLen = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_LN_NAME)->GetValueOffset()];
	asduPublic.lnName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_DATASET_NAME)->GetValueOffset()];
	asduPublic.dataSetName = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_LD_NAME)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.ldName = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_STATUS_WORD_1)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord1 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_STATUS_WORD_2)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.statusWord2 = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_SMPCNT)->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,2);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_SMPRATE)->GetValueOffset()];
	asduPublic.smpRate = *uint8Buf;

	uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_CONFREV)->GetValueOffset()];
	asduPublic.confRev = *uint8Buf;

	return true;
}

///
/// @brief
///     ��ȡASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ����ֵ���ݲ��ֵ�ͨ����
///
size_t Sav91ExAsduSeg::GetASDUSmpsNum()
{
	return m_SmpsNum;
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
size_t Sav91ExAsduSeg::GetASDUSmps(const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	ASSERT(msgDataBuf);
	uint16_t uint16Tmp = 0;
	size_t   i = 0;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;
	uint8_t *uint8Buf = &pMsgBuf[m_AsduData->at(SAV91_EX_DATA_CHANNEL)->GetValueOffset()];
	for ( i = 0; i < m_SmpsNum; i++ )
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
void Sav91ExAsduSeg::GetASDUPublicInfoOff(SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.asduLenOff     = m_AsduData->at(SAV91_EX_ASDU_LEN)->GetElemOffset();
	asduPublicOff.lnNameOff      = m_AsduData->at(SAV91_EX_LN_NAME)->GetElemOffset();
	asduPublicOff.dataSetNameOff = m_AsduData->at(SAV91_EX_DATASET_NAME)->GetElemOffset();
	asduPublicOff.ldNameOff      = m_AsduData->at(SAV91_EX_LD_NAME)->GetElemOffset();
	asduPublicOff.statusWord1Off = m_AsduData->at(SAV91_EX_STATUS_WORD_1)->GetElemOffset();
	asduPublicOff.statusWord2Off = m_AsduData->at(SAV91_EX_STATUS_WORD_2)->GetElemOffset();
	asduPublicOff.smpCntOff      = m_AsduData->at(SAV91_EX_SMPCNT)->GetElemOffset();
	asduPublicOff.smpRateOff     = m_AsduData->at(SAV91_EX_SMPRATE)->GetElemOffset();
	asduPublicOff.confRevOff     = m_AsduData->at(SAV91_EX_CONFREV)->GetElemOffset();
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
int Sav91ExAsduSeg::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	size_t i = 0;
	smpOffArr[i] = m_AsduData->at(SAV91_EX_DATA_CHANNEL)->GetElemOffset();
	for ( i = 1; i < m_SmpsNum; i++ )
	{
		if ( i == arrSize )
			break;
		smpOffArr[i] = smpOffArr[i-1]+2;
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
bool Sav91ExAsduSeg::SetASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	if ( bufSize < 1 || bufSize > 12/*((ETHER_FRAME_MAX_LEN-32)-12)/2*/ )
		return false;

	uint16_t uint16Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	// ���ù�����Ϣ
	uint16Tmp = bufSize*2+12;
	uint16Tmp = BSWAP_16(uint16Tmp);
	m_AsduData->at(SAV91_EX_ASDU_LEN)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.lnName;
	m_AsduData->at(SAV91_EX_LN_NAME)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.dataSetName;
	m_AsduData->at(SAV91_EX_DATASET_NAME)->SetValue(&uint8Tmp,1);
	uint16Tmp = BSWAP_16(asduPublic.ldName);
	m_AsduData->at(SAV91_EX_LD_NAME)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord1);
	m_AsduData->at(SAV91_EX_STATUS_WORD_1)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.statusWord2);
	m_AsduData->at(SAV91_EX_STATUS_WORD_2)->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(SAV91_EX_SMPCNT)->SetValue((uint8_t *)&uint16Tmp,2);
	uint8Tmp  = asduPublic.smpRate;
	m_AsduData->at(SAV91_EX_SMPRATE)->SetValue(&uint8Tmp,1);
	uint8Tmp  = asduPublic.confRev;
	m_AsduData->at(SAV91_EX_CONFREV)->SetValue(&uint8Tmp,1);

	// ��������ͨ��
	static uint16_t datBuf[ETHER_FRAME_MAX_LEN];
	for ( size_t i = 0; i < bufSize; i++ )
	{
		datBuf[i] = BSWAP_16(smpValBuf[i]);
	}
	m_AsduData->at(SAV91_EX_DATA_CHANNEL)->SetValue((uint8_t *)datBuf,2*bufSize);

	return true;
}
