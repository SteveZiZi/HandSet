/// @file
///     sav92_asdu_seg.cpp
///
/// @brief
///     Sav92��IEC91850-9-2������ֵ���ĵ�ASDU��
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
///    |2013.09.27     |1.0.1.927  |�޸�ASDU��ͨ���������ֵΪ64                              |
///
#include "stdafx.h"
#include "../include/etypes.h"
#include "SAV92_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     Sav92����ֵ���ĵ�ASDU������ͨ����Size
///
#define SAV92_CHANNEL_SIZE       8

///
/// @brief
///     Sav92����ֵ���ĵ�ASDU������ͨ�������ͨ����
///
#define SAV92_MAX_CHANNEL_NUM   64

///
/// @brief
///     Sav92����ֵ���ĵ�ASDU�ĸ�ʽ
///
typedef enum {
	SAV92_SV_ID = 0,
	SAV92_DATA_SET,
	SAV92_SMP_CNT,
	SAV92_CONF_REV,
	SAV92_REFR_TM,
	SAV92_SMP_SYNCH,
	SAV92_SMP_RATE,
	SAV92_SEQ_OF_DATA
}SAV92_ASDU_FORMAT;

///
/// @brief
///     ��Ӧ�����ASDU��ʽ��Sav92����ֵ���ĵ�ASDU���ֶεķ���ASN.1��Tagֵ�б�
///
static uint8_t g_Sav92_Asdu_Tag_List[] = {
	SAV92_TAG_SV_ID,
	SAV92_TAG_DATA_SET,
	SAV92_TAG_SMP_CNT,
	SAV92_TAG_CONF_REV,
	SAV92_TAG_REFR_TM,
	SAV92_TAG_SMP_SYNCH,
	SAV92_TAG_SMP_RATE,
	SAV92_TAG_SEQ_OF_DATA
};

Sav92AsduSeg::Sav92AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_AsduLen(0)
, m_AsduOff(0)
{
	Init();
}

Sav92AsduSeg::~Sav92AsduSeg()
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
size_t Sav92AsduSeg::Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize)
{
	if ( asduDataBuf == NULL )
		return 0;

	// ��ȡasdu��Value��Ϣ
	size_t dSize  = 0;
	size_t offTmp = 0;
	MsgDataElem *asduInst = CreateMsgDataElemImpl(DT_TLV_DATA);
	asduInst->SetTag(SAV92_TAG_ASDU);
	dSize = asduInst->Decode(offset+offTmp,&asduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(asduInst);
		return 0;
	}
	m_AsduLen = dSize;
	m_AsduOff = offset;
	offTmp   += dSize - asduInst->GetValueLen();
	ReleaseMsgDataElemImpl(asduInst);

	// ������õĸ�ʽ���ζ�asdu��Value���ֽ��н���
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;
		// ��������TLV�����ֶ�
		dSize = (*it)->Decode(offset+offTmp,&asduDataBuf[offTmp],bufSize-offTmp);
		// ��ѡ���ж�
		switch ( (*it)->GetTag() )
		{
		case SAV92_TAG_DATA_SET:
			{
				if ( dSize == 0 )
					m_AsduOpts.bDataSetOpt = false;
				else
					m_AsduOpts.bDataSetOpt = true;
			}
			break;
		case SAV92_TAG_REFR_TM:
			{
				if ( dSize == 0 )
					m_AsduOpts.bRefrTmOpt = false;
				else
					m_AsduOpts.bRefrTmOpt = true;
			}
			break;
		case SAV92_TAG_SMP_RATE:
			{
				if ( dSize == 0 )
					m_AsduOpts.bSmpRateOpt = false;
				else
					m_AsduOpts.bSmpRateOpt = true;
			}
			break;
		default:
			if ( dSize == 0 )
				return 0;
		}
		offTmp += dSize;
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
size_t Sav92AsduSeg::Code(uint8_t *asduDataArr, const size_t arrSize)
{
	if ( asduDataArr == NULL )
		return 0;

	size_t cSize = 0;
	size_t arrOff = 0;
	// ����asdu��Value����
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		// ���뵥���ֶΣ���ѡ���true��������
		if ( (*it)->GetTag() == SAV92_TAG_DATA_SET && !m_AsduOpts.bDataSetOpt )
			continue;
		if ( (*it)->GetTag() == SAV92_TAG_REFR_TM  && !m_AsduOpts.bRefrTmOpt  )
			continue;
		if ( (*it)->GetTag() == SAV92_TAG_SMP_RATE && !m_AsduOpts.bSmpRateOpt )
			continue;
		cSize = (*it)->Code(&asduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// ����ASDU
	MsgDataElem *asduInst = CreateMsgDataElemImpl(DT_TLV_DATA);
	asduInst->SetTag(SAV92_TAG_ASDU);
	asduInst->SetValue(asduDataArr,arrOff);
	cSize = asduInst->Code(asduDataArr,arrSize);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(asduInst);
		return 0;
	}
	ReleaseMsgDataElemImpl(asduInst);
	m_AsduLen = cSize;
	return cSize;
}

///
/// @brief
///     ��ȡASDU�ĳ���
///
/// @return
///     ASDU�ĳ���
///
size_t Sav92AsduSeg::GetASDULen()
{
	return m_AsduLen;
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
bool Sav92AsduSeg::GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic)
{
	if ( msgDataBuf == NULL )
		return false;

	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize;

	// ��ȡsvID��valueֵ
	strBuf = (char *)&msgDataBuf[m_AsduData->at(SAV92_SV_ID)->GetValueOffset()];
	datSize = m_AsduData->at(SAV92_SV_ID)->GetValueLen();
	if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
		datSize = SAV92_VISIBLE_STR_MAX_SIZE-1;
	memcpy_s(asduPublic.svIDStr,SAV92_VISIBLE_STR_MAX_SIZE,strBuf,datSize);
	asduPublic.svIDStr[datSize] = '\0';

	// ��ȡdatSet��valueֵ
	if ( m_AsduOpts.bDataSetOpt )
	{
		strBuf = (char *)&msgDataBuf[m_AsduData->at(SAV92_DATA_SET)->GetValueOffset()];
		datSize = m_AsduData->at(SAV92_DATA_SET)->GetValueLen();
		if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
			datSize = SAV92_VISIBLE_STR_MAX_SIZE-1;
		memcpy_s(asduPublic.datSetStr,SAV92_VISIBLE_STR_MAX_SIZE,strBuf,datSize);
		asduPublic.datSetStr[datSize] = '\0';
	}
	else
	{
		asduPublic.datSetStr[0] = '\0';
	}

	// ��ȡsmpCnt��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_CNT)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_SMP_CNT)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	// ��ȡconfRev��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_CONF_REV)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_CONF_REV)->GetValueLen();
	memcpy_s((uint8_t *)&uint32Tmp,4,uint8Buf,datSize);
	asduPublic.confRev = BSWAP_32(uint32Tmp);

	// ��ȡrefrTime��valueֵ
	if ( m_AsduOpts.bRefrTmOpt )
	{
		uint8_t refrTm[8];
		datSize = m_AsduData->at(SAV92_REFR_TM)->GetValueLen();
		memcpy_s(refrTm,8,&msgDataBuf[m_AsduData->at(SAV92_REFR_TM)->GetValueOffset()],datSize);
		asduPublic.refrTm.ts_s  = BSWAP_32(*(uint32_t *)refrTm);
		asduPublic.refrTm.ts_ns = BSWAP_32(*(uint32_t *)&refrTm[4])>>8;
		// IEC61850�е�UTCʱ���С�����ֵ�����4~6�ֽ�ת��Ϊ10���ƺ����2��24�η�����΢��ֵ�����ٳ���10��9�η�
		asduPublic.refrTm.ts_ns = (uint32_t)(((uint64_t)asduPublic.refrTm.ts_ns)*(1000000000)/(1<<24));
		asduPublic.refrTm.ts_q  = refrTm[7];
	}
	else
	{
		asduPublic.refrTm.ts_s  = 0;
		asduPublic.refrTm.ts_ns = 0;
		asduPublic.refrTm.ts_q  = 0;
	}

	// ��ȡsmpSynch��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_SYNCH)->GetValueOffset()];
	asduPublic.smpSynch = !( (*uint8Buf) == 0 );

	// ��ȡsmpRate��valueֵ
	if ( m_AsduOpts.bSmpRateOpt )
	{
		uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_RATE)->GetValueOffset()];
		datSize   = m_AsduData->at(SAV92_SMP_RATE)->GetValueLen();
		memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
		asduPublic.smpRate = BSWAP_16(uint16Tmp);
	}
	else
	{
		asduPublic.smpRate = 0;
	}

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
bool Sav92AsduSeg::GetSmpCntInASDU(const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( msgDataBuf == NULL )
		return false;

	uint16_t uint16Tmp = 0;
	size_t   datSize;

	// ��ȡsmpCnt��valueֵ
	uint8_t *uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_CNT)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_SMP_CNT)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	smpCnt = BSWAP_16(uint16Tmp);

	return true;
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�е�ͬ����ʶλ��Ϣ
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     smpSynch   - ����ͬ����ʶλ��Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav92AsduSeg::GetSmpSynchInASDU(const uint8_t *msgDataBuf, bool &smpSynch)
{
	if ( msgDataBuf == NULL )
		return false;

	// ��ȡsmpSynch��valueֵ
	uint8_t *uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_SYNCH)->GetValueOffset()];
	smpSynch = !( (*uint8Buf) == 0 );

	return true;
}

///
/// @brief
///     ��ȡASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t Sav92AsduSeg::GetASDUSmpsNum()
{
	return m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemLen()/SAV92_CHANNEL_SIZE;
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
///     smpQltArr  - �������˲ʱֵ��Ʒ��ֵ������
/// @param[in]
///     arrSize    - �������С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav92AsduSeg::GetASDUSmps(const uint8_t *msgDataBuf, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize)
{
	if ( msgDataBuf == NULL )
		return 0;
	if ( smpValArr == NULL && smpQltArr == NULL )
		return 0;

	// ��ȡSequence of Data ��valueֵ
	size_t   datLen = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetValueLen();
	uint8_t *datBuf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SEQ_OF_DATA)->GetValueOffset()];

	// ��ȡ��ͨ������
	size_t i = 0;
	size_t bufOff = 0;
	uint32_t uint32Tmp = 0;
	for ( i = 0; i < datLen/SAV92_CHANNEL_SIZE ; i++ )
	{
		if ( i >= arrSize )
			break;
		if ( smpValArr != NULL )
		{
			memcpy((uint8_t *)&uint32Tmp,&datBuf[bufOff],4);
			smpValArr[i] = BSWAP_32(uint32Tmp);
		}
		if ( smpQltArr != NULL )
		{
			memcpy((uint8_t *)&uint32Tmp,&datBuf[bufOff+SAV92_CHANNEL_SIZE/2],4);
			smpQltArr[i] = BSWAP_32(uint32Tmp);
		}
		bufOff      += SAV92_CHANNEL_SIZE;
	}
	return i;
}

///
/// @brief
///     ��ȡASDU����Sequence of Data�TLV��ʽ���ĳ���
///
/// @return
///     Sequence of Data�TLV��ʽ���ĳ���
///
size_t Sav92AsduSeg::GetSeqOfDatLen()
{
	return m_AsduLen - ( m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemOffset() - m_AsduOff );
}

///
/// @brief
///     ��ȡASDU�еĿ�ѡ���������Ϣ
///
/// @param[in]
///     asduOpts - �����ѡ��������Ϣ�Ľṹ��
///
/// @return
///     
///
void Sav92AsduSeg::GetASDUOpts(SAV92_ASDU_OPTS &asduOpts)
{
	asduOpts = m_AsduOpts;
}

///
/// @brief
///     ��ȡASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav92AsduSeg::GetASDUOff()
{
	return m_AsduOff;
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
void Sav92AsduSeg::GetASDUPublicInfoOff(SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.svIDOff     = m_AsduData->at(SAV92_SV_ID)->GetElemOffset();
	asduPublicOff.datSetOff   = m_AsduData->at(SAV92_DATA_SET)->GetElemOffset();
	asduPublicOff.smpCntOff   = m_AsduData->at(SAV92_SMP_CNT)->GetElemOffset();
	asduPublicOff.confRevOff  = m_AsduData->at(SAV92_CONF_REV)->GetElemOffset();
	asduPublicOff.refrTmOff   = m_AsduData->at(SAV92_REFR_TM)->GetElemOffset();
	asduPublicOff.smpSynchOff = m_AsduData->at(SAV92_SMP_SYNCH)->GetElemOffset();
	asduPublicOff.smpRateOff  = m_AsduData->at(SAV92_SMP_RATE)->GetElemOffset();
}

///
/// @brief
///     ��ȡASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     seqOfDataOff - ����˲ʱֵ����(TLV)��ƫ�Ƶ�ַ
/// @param[in]
///     smpValOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     smpQltOffArr - �������˲ʱֵ��Ʒ��ֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     arrSize      - ����Ĵ�С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav92AsduSeg::GetASDUSmpsOff(size_t &seqOfDataOff, size_t *smpValOffArr, size_t *smpQltOffArr, const size_t arrSize)
{
	if ( smpQltOffArr == NULL && smpValOffArr == NULL )
		return 0;

	seqOfDataOff  = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemOffset();
	size_t datLen = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemLen();

	// ��ȡ��һ��ͨ����ƫ�Ƶ�ַ
	if ( smpValOffArr != NULL )
		smpValOffArr[0] = m_AsduOff + m_AsduLen - datLen;
	if ( smpQltOffArr != NULL )
		smpQltOffArr[0] = m_AsduOff + m_AsduLen - datLen + SAV92_CHANNEL_SIZE/2;

	// ��ȡʣ��ͨ����ƫ�Ƶ�ַ
	size_t i;
	for ( i = 1; i < datLen/SAV92_CHANNEL_SIZE ; i++ )
	{
		if ( i == arrSize )
			break;
		if ( smpValOffArr != NULL )
			smpValOffArr[i] = smpValOffArr[i-1] + SAV92_CHANNEL_SIZE;
		if ( smpQltOffArr != NULL )
			smpQltOffArr[i] = smpQltOffArr[i-1] + SAV92_CHANNEL_SIZE;
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
///     smpQltBuf  - �������˲ʱֵ��Ʒ��ֵ�Ļ���
/// @param[in]
///     bufSize    - ����Ĵ�С,��ͨ����
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav92AsduSeg::SetASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf, const size_t bufSize)
{
	if ( bufSize > SAV92_MAX_CHANNEL_NUM || smpValBuf == NULL || smpQltBuf == NULL )
		return false;
	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	char    *strBuf    = NULL;

	// ����svID��valueֵ
	size_t datSize = strlen(asduPublic.svIDStr);
	if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
		m_AsduData->at(SAV92_SV_ID)->SetValue((uint8_t *)asduPublic.svIDStr,SAV92_VISIBLE_STR_MAX_SIZE-1);
	else
		m_AsduData->at(SAV92_SV_ID)->SetValue((uint8_t *)asduPublic.svIDStr,datSize);

	// ����datSet��valueֵ
	if ( m_AsduOpts.bDataSetOpt )
	{
		datSize  = strlen(asduPublic.datSetStr);
		if ( datSize > SAV92_VISIBLE_STR_MAX_SIZE )
			m_AsduData->at(SAV92_DATA_SET)->SetValue((uint8_t *)asduPublic.datSetStr,SAV92_VISIBLE_STR_MAX_SIZE);
		else
			m_AsduData->at(SAV92_DATA_SET)->SetValue((uint8_t *)asduPublic.datSetStr,datSize);
	}
	else
	{
		m_AsduData->at(SAV92_DATA_SET)->SetValue(NULL,0);
	}

	// ����smpCnt��valueֵ
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(SAV92_SMP_CNT)->SetValue((uint8_t *)&uint16Tmp,2);

	// ����confRev��valueֵ
	uint32Tmp = BSWAP_32(asduPublic.confRev);
	m_AsduData->at(SAV92_CONF_REV)->SetValue((uint8_t *)&uint32Tmp,4);

	// ����refrTm��valueֵ
	if ( m_AsduOpts.bRefrTmOpt )
	{
		uint8_t refrTm[8];
		uint32Tmp = BSWAP_32(asduPublic.refrTm.ts_s);
		memcpy(refrTm,(uint8_t *)&uint32Tmp,4);
		uint32Tmp = ((uint32_t)((uint64_t)asduPublic.refrTm.ts_ns*(1<<28)/(1000000000)))<<4;
		// �ж�ֵ�Ƿ��1
		if ( (uint32Tmp & 0xFF) >= 0x80 )
			uint32Tmp += 0x100;
		uint32Tmp = uint32Tmp & 0xFFFFFF00;
		uint32Tmp = BSWAP_32(uint32Tmp);
		memcpy(&refrTm[4],(uint8_t *)&uint32Tmp,3);
		refrTm[7] = asduPublic.refrTm.ts_q;
		m_AsduData->at(SAV92_REFR_TM)->SetValue(refrTm,8);
	}
	else
	{
		m_AsduData->at(SAV92_REFR_TM)->SetValue(NULL,0);
	}

	// ����smpSynch��valueֵ
	uint8Tmp = asduPublic.smpSynch;
	m_AsduData->at(SAV92_SMP_SYNCH)->SetValue(&uint8Tmp,1);

	// ����smpRate��valueֵ
	if ( m_AsduOpts.bSmpRateOpt )
	{
		uint16Tmp = BSWAP_16(asduPublic.smpRate);
		m_AsduData->at(SAV92_SMP_RATE)->SetValue((uint8_t *)&uint16Tmp,2);
	}
	else
	{
		m_AsduData->at(SAV92_SMP_RATE)->SetValue(NULL,0);
	}

	// ����Sequence of Data��valueֵ
	static uint8_t datBuf[ETHER_FRAME_MAX_LEN];
	int bufLen = 0;
	for ( size_t i = 0; i < bufSize ; i++ )
	{
		uint32Tmp = BSWAP_32(smpValBuf[i]);
		memcpy(&datBuf[bufLen],(uint8_t *)&uint32Tmp,4);
		uint32Tmp = BSWAP_32(smpQltBuf[i]);
		memcpy(&datBuf[bufLen+4],(uint8_t *)&uint32Tmp,4);
		bufLen += SAV92_CHANNEL_SIZE;
	}
	m_AsduData->at(SAV92_SEQ_OF_DATA)->SetValue(datBuf,bufLen);

	return true;
}

///
/// @brief
///     ����ASDU�еĿ�ѡ����Ϣ
///
/// @param[in]
///     asduOpts - ��ѡ����Ϣ
///
/// @return
///     
///
void Sav92AsduSeg::SetASDUOpts(SAV92_ASDU_OPTS &asduOpts)
{
	m_AsduOpts = asduOpts;
}

void Sav92AsduSeg::Init()
{
	// ��ʼ����ѡ�������
	m_AsduOpts.bDataSetOpt = false;
	m_AsduOpts.bRefrTmOpt  = false;
	m_AsduOpts.bSmpRateOpt = false;

	// ��ʼ��ASDU Vector����������Ӧ�ֶε�Tagֵ
	m_AsduData->reserve(sizeof(g_Sav92_Asdu_Tag_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Sav92_Asdu_Tag_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_TLV_DATA));
		m_AsduData->at(i)->SetTag(g_Sav92_Asdu_Tag_List[i]);
	}
}
