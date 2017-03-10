/// @file
///     ethernetII_seg.cpp
///
/// @brief
///     GOOSE����֡�е�GoosePDU��
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
///    |2013.09.27     |1.0.1.927  |GOOSE������֡���̶�stNum��sqNum�ĳ���Ϊ4�ֽ�              |
///
#include "stdafx.h"
#include "goose_pdu_seg.h"
#include "goose_data_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     GOOSE PDU������ȥ��Data���к��ʣ�ಿ�ֵ����Size������Security��
///
#define GOOSE_PDU_NO_DATA_MAX_SIZE 247

///
/// @brief
///     GoosePdu��gocbRef������������Size
///
#define GOOSE_PDU_GOCB_REF_MAX_SIZE 65

///
/// @brief
///     GoosePdu��datSet������������Size
///
#define GOOSE_PDU_DATA_SET_MAC_SIZE 65

///
/// @brief
///     GoosePdu��goID������������Size
///
#define GOOSE_PDU_GO_ID_MAX_SIZE    65

///
/// @brief
///     Э����GoosePdu�й������֣���������AllData����Ĳ��֣��ĸ�ʽ
///
typedef enum{
	GOOSE_PDU_GOCB_REF = 0,
	GOOSE_PDU_TIME_ALLOWED,
	GOOSE_PDU_DATA_SET,
	GOOSE_PDU_GO_ID,
	GOOSE_PDU_EVENT_TIME,
	GOOSE_PDU_ST_NUM,
	GOOSE_PDU_SQ_NUM,
	GOOSE_PDU_TEST,
	GOOSE_PDU_CONF_REV,
	GOOSE_PDU_NDS_COM,
	GOOSE_PDU_NUM_DS_ENTRIES,
	GOOSE_PDU_SECURITY
}GOOSE_PDU_PUBLIC_TBL;

///
/// @brief
///     Э����GoosePdu�й������֣���������AllData����Ĳ��֣���Tagֵ�б�
///
static const uint8_t g_Goose_Pdu_Public_Tag_List[] = {
	GOOSE_TAG_GOCB_REF,
	GOOSE_TAG_TIME_ALLOWED,
	GOOSE_TAG_DATA_SET,
	GOOSE_TAG_GO_ID,
	GOOSE_TAG_EVENT_TIME,
	GOOSE_TAG_ST_NUM,
	GOOSE_TAG_SQ_NUM,
	GOOSE_TAG_TEST,
	GOOSE_TAG_CONF_REV,
	GOOSE_TAG_NDS_COM,
	GOOSE_TAG_NUM_DS_ENTRIES,
	GOOSE_TAG_SECURITY
};

GoosePduSeg::GoosePduSeg()
: m_PduPblc(new std::vector<MsgDataElem *>)
, m_GooseData(new GooseDataSeg)
, m_bSecurityOpt(false)
, m_GooseDataSize(0)
, m_MaxPduSize(1514-26)
{
	m_PduPblc->reserve(sizeof(g_Goose_Pdu_Public_Tag_List)/sizeof(uint8_t));
	for ( size_t i = 0; i < sizeof(g_Goose_Pdu_Public_Tag_List)/sizeof(uint8_t); i++ )
	{
		MsgDataElem *newElem = CreateMsgDataElemImpl(DT_TLV_DATA);
		newElem->SetTag(g_Goose_Pdu_Public_Tag_List[i]);
		m_PduPblc->push_back(newElem);
	}
}

GoosePduSeg::~GoosePduSeg()
{
	if ( m_PduPblc != NULL )
	{
		std::vector<MsgDataElem *>::iterator it;
		for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
		{
			ReleaseMsgDataElemImpl(*it);
		}
		delete m_PduPblc;
		m_PduPblc = NULL;
	}
	if ( m_GooseData != NULL )
	{
		delete m_GooseData;
		m_GooseData = NULL;
	}
}

///
/// @brief
///     �Ա���֡�е�GoosePdu���н���
///
/// @param[in]
///     offset     - �������GoosePdu���������ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     pduDataBuf - ����������GoosePdu�������ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t GoosePduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	// ��ȡgoosePdu��Value��Ϣ
	MsgDataElem *goosePDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	goosePDU->SetTag(GOOSE_TAG_GOOSE_PDU);
	size_t dSize  = goosePDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(goosePDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;	
	offTmp  += dSize - goosePDU->GetValueLen();
	ReleaseMsgDataElemImpl(goosePDU);

	// ������õĸ�ʽ���ζ�goosePdu��Value���ֽ��н���
	std::vector<MsgDataElem *>::iterator it;
	for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;
		// ���ݶ���õĸ�ʽ�����ν�������TLV������
		dSize = (*it)->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( (*it)->GetTag() == GOOSE_TAG_SECURITY )
		{
			if ( dSize == 0 )
				m_bSecurityOpt = false;
			else
				m_bSecurityOpt = true;
		}
		else
		{
			if ( dSize == 0 )
				return 0;
		}	
		offTmp += dSize;

		// ��ǰ��ΪnumDatSetEntries����������������������У�AllData��
		if ( (*it)->GetTag() == GOOSE_TAG_NUM_DS_ENTRIES )
		{
			dSize = m_GooseData->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
			if ( dSize == 0 )
				return 0;
			offTmp += dSize;
		}
	}
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬�õ�GoosePdu
///
/// @param[in]
///     pduDataArr - ����GoosePdu���ݵ�����
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ����GoosePdu���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t GoosePduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	size_t cSize  = 0;
	size_t arrOff = 0;
	std::vector<MsgDataElem *>::iterator it;

	// ����goosePdu��Value����
	for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
	{
		// ���ݶ���õĸ�ʽ�����α��뵥��TLV������
		if ( arrOff >= arrSize )
			break;
		if ( (*it)->GetTag() == GOOSE_TAG_SECURITY && !m_bSecurityOpt )
			continue;
		cSize = (*it)->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
		// ��ǰ��ΪnumDatSetEntries�������������������
		if ( (*it)->GetTag() == GOOSE_TAG_NUM_DS_ENTRIES )
		{
			cSize = m_GooseData->Code(&pduDataArr[arrOff],arrSize-arrOff);
			if ( cSize == 0 )
				return 0;
			arrOff += cSize;
		}
	}

	// ����������goosePdu��TLV��
	MsgDataElem *goosePDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	goosePDU->SetTag(GOOSE_TAG_GOOSE_PDU);
	goosePDU->SetValue(pduDataArr,arrOff);
	cSize = goosePDU->Code(pduDataArr,arrSize);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(goosePDU);
		return 0;
	}
	ReleaseMsgDataElemImpl(goosePDU);
	m_PduLen = cSize;
	return cSize;
}

///
/// @brief
///     ��ȡ����֡��goosePDU�ĳ��ȣ�������TLV���ݵĳ���
///
/// @return
///     goosePDU�ĳ���
///
size_t GoosePduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     ��ȡgoosePDU��Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
/// @return
///     Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
size_t GoosePduSeg::GetGooseDataLen()
{
	return m_GooseData->GetGooseDataLen();
}

///
/// @brief
///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     pduPublic  - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GoosePduSeg::GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC &pduPublic)
{
	ASSERT(msgDataBuf);
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize   = 0;

	// ��ȡgocbRef��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueLen();
	pduPublic.gocbRef.assign(strBuf,datSize);
	pduPublic.gocbRef += '\0';

	// ��ȡtimeAllowedtoLive��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.timeAllowedtoLive = 0;
	}
	
	// ��ȡdatSet��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueLen();
	pduPublic.datSet.assign(strBuf,datSize);
	pduPublic.datSet += '\0';

	// ��ȡgoID��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueLen();
	pduPublic.goID.assign(strBuf,datSize);
	pduPublic.goID += '\0';

	// ��ȡeventTime��valueֵ
	uint8_t eventTime[8];
	datSize = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueLen();
	memcpy_s(eventTime,8,&msgDataBuf[m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueOffset()],datSize);
	pduPublic.eventTm.ts_s  = BSWAP_32(*(uint32_t *)&eventTime[0]);
	pduPublic.eventTm.ts_ns = BSWAP_32(*(uint32_t *)&eventTime[4])>>8;
	// IEC61850�е�UTCʱ���С�����ֵ�����4~6�ֽ�ת��Ϊ10���ƺ����2��24�η�����΢��ֵ�����ٳ���10��9�η���
	pduPublic.eventTm.ts_ns = (uint32_t)(((uint64_t)pduPublic.eventTm.ts_ns)*(1000000000)/(1<<24));
	pduPublic.eventTm.ts_q  = eventTime[7];

	// ��ȡstNum��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.stNum = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.stNum = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.stNum = 0;
	}

	// ��ȡsqNum��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.sqNum = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.sqNum = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.sqNum = 0;
	}

	// ��ȡtest��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TEST)->GetValueOffset()];
	pduPublic.test = *uint8Buf;

	// ��ȡconfRev��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.confRev = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.confRev = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.confRev = 0;
	}

	// ��ȡndsCom��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetValueOffset()];
	pduPublic.ndsCom = *uint8Buf;

	// ��ȡnumDatSetEntries��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.numDatSetEntries = 0;
	}

	// ��ȡSecurity��valueֵ
	if ( m_bSecurityOpt )
	{
		strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueOffset()];
		datSize = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueLen();
		pduPublic.security.assign(strBuf,datSize);
		pduPublic.security += '\0';
	}
	else
		pduPublic.security = "";
	
	return true;
}

///
/// @brief
///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     pduPublic  - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GoosePduSeg::GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC_EX &pduPublic)
{
	ASSERT(msgDataBuf);
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize   = 0;

	// ��ȡgocbRef��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueLen();
	pduPublic.gocbRef.assign(strBuf,datSize);
	pduPublic.gocbRef += '\0';

	// ��ȡtimeAllowedtoLive��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.timeAllowedtoLive = 0;
	}

	// ��ȡdatSet��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueLen();
	pduPublic.datSet.assign(strBuf,datSize);
	pduPublic.datSet += '\0';

	// ��ȡgoID��valueֵ
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueLen();
	pduPublic.goID.assign(strBuf,datSize);
	pduPublic.goID += '\0';

	// ��ȡeventTime��valueֵ
	uint8_t eventTime[8];
	datSize = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueLen();
	memcpy_s(eventTime,8,&msgDataBuf[m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueOffset()],datSize);
	pduPublic.eventTm.ts_s  = BSWAP_32(*(uint32_t *)&eventTime[0]);
	pduPublic.eventTm.ts_ns = BSWAP_32(*(uint32_t *)&eventTime[4])>>8;
	// IEC61850�е�UTCʱ���С�����ֵ�����4~6�ֽ�ת��Ϊ10���ƺ����2��24�η�����΢��ֵ�����ٳ���10��9�η���
	pduPublic.eventTm.ts_ns = (uint32_t)(((uint64_t)pduPublic.eventTm.ts_ns)*(1000000000)/(1<<24));
	pduPublic.eventTm.ts_q  = eventTime[7];

	// ��ȡstNum��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.stNum = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.stNum = 0;
	}

	// ��ȡsqNum��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.sqNum = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.sqNum = 0;
	}

	// ��ȡtest��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TEST)->GetValueOffset()];
	pduPublic.test = *uint8Buf;

	// ��ȡconfRev��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.confRev = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.confRev = 0;
	}

	// ��ȡndsCom��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetValueOffset()];
	pduPublic.ndsCom = *uint8Buf;

	// ��ȡnumDatSetEntries��valueֵ
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.numDatSetEntries = 0;
	}

	// ��ȡSecurity��valueֵ
	if ( m_bSecurityOpt )
	{
		strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueOffset()];
		datSize = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueLen();
		pduPublic.security.assign(strBuf,datSize);
		pduPublic.security += '\0';
	}
	else
		pduPublic.security = "";

	return true;
}

///
/// @brief
///     ����ͨ�������ѽ��루����ֵ->��ʵֵ���ı�־
///
void GoosePduSeg::ResetChannelDecodedFlag()
{
	return m_GooseData->ResetChannelDecodedFlag();
}

///
/// @brief
///     ��ȡGoosePdu�е�����ͨ������Ŀ
///
/// @return
///     ����ͨ������Ŀ
///
size_t GoosePduSeg::GetChannelNum()
{
	return m_GooseData->GetChannelNum();
}

///
/// @brief
///     ��ȡGoosePdu�еĵ���ͨ��������
///
/// @param[in]
///     idxSeq     - GOOSE����ͨ����˳��ţ���0��ʼ
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ָ��Gooseͨ�����ݵ�ָ��
///
GOOSE_DATA *GoosePduSeg::GetAChannel(size_t idxSeq, const uint8_t *msgDataBuf, size_t bufSize)
{
	return m_GooseData->GetAChannel(idxSeq,msgDataBuf,bufSize);
}

///
/// @brief
///     ��ȡSecurity��ѡ���������Ϣ
///
/// @return
///     true - �����к���Security�� false - ������
///
bool GoosePduSeg::GetSecurityOpt()
{
	return m_bSecurityOpt;
}

///
/// @brief
///     ��ȡgoosePDU�ڱ���֡��ƫ�Ƶ�ַ
///
/// @return
///     goosePDU�ڱ���֡��ƫ�Ƶ�ַ
///
size_t GoosePduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     ��ȡgoosePDU��Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t GoosePduSeg::GetGooseDataOff()
{
	return m_GooseData->GetGooseDataOff();
}

///
/// @brief
///     ��ȡGoosePdu�Ĺ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     pduPblcOff - ����GoosePdu�Ĺ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     
///
void GoosePduSeg::GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff)
{
	pduPblcOff.gocbRefOff           = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetElemOffset();
	pduPblcOff.timeAllowedtoLiveOff = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetElemOffset();
	pduPblcOff.datSetOff            = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetElemOffset();
	pduPblcOff.goIDOff              = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetElemOffset();
	pduPblcOff.eventTmOff           = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetElemOffset();
	pduPblcOff.stNumOff             = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetElemOffset();
	pduPblcOff.sqNumOff             = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetElemOffset();
	pduPblcOff.testOff              = m_PduPblc->at(GOOSE_PDU_TEST)->GetElemOffset();
	pduPblcOff.confRevOff           = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetElemOffset();
	pduPblcOff.ndsComOff            = m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetElemOffset();
	pduPblcOff.numDatSetEntriesOff  = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetElemOffset();
	pduPblcOff.securityOff          = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetElemOffset();
}

///
/// @brief
///     ����GoosePdu�й������ֵ���Ϣ������ͨ����������Ĳ��ֵ���Ϣ��
///
/// @param[in]
///     pduPblc - GoosePdu�й������ֵ���Ϣ
///
/// @return
///
///
void GoosePduSeg::SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc)
{
	uint32_t uint32Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	size_t   datSize   = 0;
	char    *strBuf    = NULL;
	
	// ����gocbRef��valueֵ
	datSize = pduPblc.gocbRef.size();
	strBuf  = const_cast<char *>(pduPblc.gocbRef.c_str());
	if ( datSize > GOOSE_PDU_GOCB_REF_MAX_SIZE )
		m_PduPblc->at(GOOSE_PDU_GOCB_REF)->SetValue((uint8_t *)strBuf,GOOSE_PDU_GOCB_REF_MAX_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_GOCB_REF)->SetValue((uint8_t *)strBuf,datSize);

	// ����timeAllowedtoLive��valueֵ
	datSize   = GetUint32MinLen(pduPblc.timeAllowedtoLive);
	uint32Tmp = BSWAP_32( (pduPblc.timeAllowedtoLive<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// ����datSet��valueֵ
	datSize = pduPblc.datSet.size();
	strBuf  = const_cast<char *>(pduPblc.datSet.c_str());
	if ( datSize > GOOSE_PDU_DATA_SET_MAC_SIZE )
		m_PduPblc->at(GOOSE_PDU_DATA_SET)->SetValue((uint8_t *)strBuf,GOOSE_PDU_DATA_SET_MAC_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_DATA_SET)->SetValue((uint8_t *)strBuf,datSize);

	// ����goID��valueֵ
	datSize = pduPblc.goID.size();
	strBuf  = const_cast<char *>(pduPblc.goID.c_str());
	if ( datSize > GOOSE_PDU_GO_ID_MAX_SIZE )
		m_PduPblc->at(GOOSE_PDU_GO_ID)->SetValue((uint8_t *)strBuf,GOOSE_PDU_GO_ID_MAX_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_GO_ID)->SetValue((uint8_t *)strBuf,datSize);

	// ����eventTime��valueֵ
	uint8_t eventTm[8];
	uint32Tmp = BSWAP_32(pduPblc.eventTm.ts_s);
	memcpy(eventTm,(uint8_t *)&uint32Tmp,4);
	uint32Tmp = ((uint32_t)((uint64_t)pduPblc.eventTm.ts_ns*(1<<28)/(1000000000)))<<4;
	// �ж�ֵ�Ƿ��1
	if ( (uint32Tmp & 0xFF) >= 0x80 )
		uint32Tmp += 0x100;
	uint32Tmp = uint32Tmp & 0xFFFFFF00;
	uint32Tmp = BSWAP_32(uint32Tmp);
	memcpy(&eventTm[4],(uint8_t *)&uint32Tmp,3);
	eventTm[7] = pduPblc.eventTm.ts_q;
	m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->SetValue(eventTm,8);

	// ����stNum��valueֵ
	datSize   = /*GetUint32MinLen(pduPblc.stNum)*/sizeof(uint32_t);
	uint32Tmp = BSWAP_32( (pduPblc.stNum<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_ST_NUM)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// ����sqNum��valueֵ
	datSize   = /*GetUint32MinLen(pduPblc.sqNum)*/sizeof(uint32_t);
	uint32Tmp = BSWAP_32( (pduPblc.sqNum<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_SQ_NUM)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// ����test��valueֵ
	uint8Tmp = pduPblc.test;
	m_PduPblc->at(GOOSE_PDU_TEST)->SetValue(&uint8Tmp,1);

	// ����confRef��valueֵ
	datSize   = GetUint32MinLen(pduPblc.confRev);
	uint32Tmp = BSWAP_32( (pduPblc.confRev<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_CONF_REV)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// ����ndsCom��valueֵ
	uint8Tmp = pduPblc.ndsCom;
	m_PduPblc->at(GOOSE_PDU_NDS_COM)->SetValue(&uint8Tmp,1);

	// ����numDatSetEntries��valueֵ
	datSize   = GetUint32MinLen(pduPblc.numDatSetEntries);
	uint32Tmp = BSWAP_32( (pduPblc.numDatSetEntries<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// ����security��valueֵ
	if ( m_bSecurityOpt )
	{
		datSize = pduPblc.security.size();
		strBuf  = const_cast<char *>(pduPblc.security.c_str());
		m_PduPblc->at(GOOSE_PDU_SECURITY)->SetValue((uint8_t *)strBuf,datSize);
	}
	else
		m_PduPblc->at(GOOSE_PDU_SECURITY)->SetValue(NULL,0);
}

///
/// @brief
///     ���һ������ͨ��
///
/// @param[in]
///     datBuf  - ���浥��GOOSE����ͨ�����е����ݵĻ���
/// @param[in]
///     bufSize - �����С��������GOOSE����ͨ����������Ŀ��Ĭ��Ϊ1����ʾ��ͨ�������ݷǽṹ�����ͣ�
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GoosePduSeg::AddAChannel(const GOOSE_DATA *channel)
{
	size_t len = m_GooseDataSize + GooseDataTLVBytes(channel);
	if ( len > ( m_MaxPduSize - GOOSE_PDU_NO_DATA_MAX_SIZE ) )
		return false;

	if ( m_GooseData->AddAChannel(channel) == false )
		return false;
	m_GooseDataSize = len;
	return true;
}

///
/// @brief
///     ����Security��ѡ���������Ϣ
///
/// @param[in]
///     opt - �������Ƿ��������
///
/// @return
///     
///
void GoosePduSeg::SetSecurityOpt(bool opt)
{
	m_bSecurityOpt = opt;
}

///
/// @brief
///     ��ձ������
///
/// @return
///     
///
void GoosePduSeg::Clear()
{
	m_GooseData->Clear();
	m_GooseDataSize = 0;
	m_PduLen        = 0;
	m_PduOff        = 0;
}

///
/// @brief
///     ����Sav92PDU�������ϲ�Э�����ͣ�������GoosePdu�����Size
///
/// @param[in]
///     type - GoosePDU�������ϲ�Э������
///
/// @return
///     
///
void GoosePduSeg::SetULPType(const GOOSE_ULP_TYPE type)
{
	switch (type)
	{
	case GOOSE_IEEE802_1Q:
		// ��̫��֡��󳤶ȼ�ȥGoosePDU֮ǰ�Ĳ��ֵĳ���
		m_MaxPduSize = 1514 - 26;
		break;
	default:
		m_MaxPduSize = 1514 - 26;
	}
}

///
/// @brief
///     ��ȡ32λ�޷���������Ч���ݵ��ֽ���
///
/// @param[in]
///     inData - 32λ�޷�����
///
/// @return
///     ��Ч���ݵ��ֽ���
///
size_t GoosePduSeg::GetUint32MinLen(uint32_t &inData)
{
	if ( ( inData & 0xFF000000 ) != 0 )
		return 4;
	if ( ( inData & 0x00FF0000 ) != 0 )
		return 3;
	if ( ( inData & 0x0000FF00 ) != 0 )
		return 2;
	if ( ( inData & 0x000000FF ) != 0 )
		return 1;
	return 1;
}

