/// @file
///     sav91_pdu_seg.cpp
///
/// @brief
///     Sav91��IEC91850-9-1������ֵ���ĵ�PDU��
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
#include "../include/etypes.h"
#include "sav91_pdu_seg.h"
#include "sav91_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     ���ASDU��Ŀ
///
#define SAV91_PDU_MAX_NO_ASDU 8

///
/// @brief
///     Sav91 PDU������ȥ��ASDU���ʣ�ಿ�ֵ����Size������Security��
///
#define SAV91_PDU_NO_ASDU_MAX_SIZE 6

Sav91PduSeg::Sav91PduSeg()
: m_NoASDU(0)
, m_NoASDUOff(0)
, m_PduLen(0)
, m_PduOff(0)
, m_AsduSize(0)
, m_MaxPduSize(ETHER_FRAME_MAX_LEN-26)
{
	m_AsduSubSeg.reserve(SAV91_PDU_MAX_NO_ASDU);
	for ( int i = 0; i < SAV91_PDU_MAX_NO_ASDU; i++ )
	{
		Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
}

Sav91PduSeg::~Sav91PduSeg()
{
	std::vector<Sav91AsduSeg *>::iterator it;
	for ( it = m_AsduSubSeg.begin(); it != m_AsduSubSeg.end(); it++ )
	{
		if ( *it != NULL )
		{
			delete *it;
			*it = NULL;
		}
	}
}

///
/// @brief
///     �Ա���֡�е�SavPDU���ֽ��н���
///
/// @param[in]
///     offset      - �������SavPDU�����ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     pduDataBuf  - ����������SavPDU���ֵ����ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91PduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	// ����SavPDU��value����
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV91_TAG_SAV_PDU);
	size_t dSize  = savPDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(savPDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;
	offTmp   = dSize - savPDU->GetValueLen();
	ReleaseMsgDataElemImpl(savPDU);

	// ����noASDU
	memcpy((uint8_t *)&m_NoASDU,&pduDataBuf[offTmp],2);
	m_NoASDU    = BSWAP_16(m_NoASDU);
	m_NoASDUOff = offset + offTmp;
	offTmp += 2;

	// ����ASDU
	for (size_t i = 0; i < m_NoASDU; i++ )
	{
		if ( offTmp >= bufSize )
		{
			m_NoASDU = i;
			break;
		}
		if ( i >= m_AsduSubSeg.size() )
		{
			Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
			m_AsduSubSeg.push_back(asduSeg);
		}

		Sav91AsduSeg *asduSeg = m_AsduSubSeg.at(i);
		if ( asduSeg == NULL )
			return 0;

		// ��������ASDU��value
		dSize = asduSeg->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( dSize == 0 )
			return 0;
		
		offTmp += dSize;
	}

	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���SavPDU������
///
/// @param[in]
///     pduDataArr - ����SavPDU������������
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ����SavPDU�������������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91PduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	size_t arrOff = 0;
	// ���õ�ǰ����ӵ�ASDU��
	if ( m_NoASDU == 0 )
		return 0;
	uint16_t noAsdu = BSWAP_16(m_NoASDU);
	memcpy(&pduDataArr[arrOff],(uint8_t *)&noAsdu,2);
	arrOff += 2;

	// ����ASDU
	size_t cSize  = 0;
	for (int i = 0; i < m_NoASDU; i++ )
	{
		if ( arrOff >= arrSize )
			break;
		cSize = m_AsduSubSeg.at(i)->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// ����SavPDU
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV91_TAG_SAV_PDU);
	savPDU->SetValue(pduDataArr,arrOff);
	cSize = savPDU->Code(pduDataArr,arrSize);
	ReleaseMsgDataElemImpl(savPDU);

	m_PduLen = cSize;
	return cSize;
}

///
/// @brief
///     ��ȡ����֡��Sav91PDU�ĳ���
///
/// @return
///     Sav91PDU�ĳ���
///
size_t Sav91PduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     ��ȡ����֡��ASDU����Ŀ
///
/// @return
///     ����֡������ASDU����Ŀ
///
uint16_t Sav91PduSeg::GetNoASDU()
{
	return m_NoASDU;
}

///
/// @brief
///     ��ȡ����֡��ASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91PduSeg::GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV91_ASDU_PUBLIC &asduPublic)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfo(msgDataBuf,asduPublic);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ�����������Ϣ
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     smpCnt     - ���������������Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91PduSeg::GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpCntInASDU(msgDataBuf,smpCnt);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�е�״̬����Ϣ
///
/// @param[in]
///     idxSeq      - ����֡��ASDU��˳��ţ���0��ʼ
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
bool Sav91PduSeg::GetStatusWordsInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &statusWord1, uint16_t &statusWord2)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetStatusWordsInASDU(msgDataBuf,statusWord1,statusWord2);
}

///
/// @brief
///     ��ȡЭ�鶨���ASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     Э�鶨���ASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
///
size_t Sav91PduSeg::GetASDUSmpsNum()
{
	return SAV91_ASDU_CHANNEL_NUM;
}

///
/// @brief
///     ��ȡ����֡��ASDU�еĲ���ֵ����
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
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
size_t Sav91PduSeg::GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmps(msgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     ��ȡSav91PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Sav91PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91PduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91PduSeg::GetNoASDUOff()
{
	return m_NoASDUOff;
}

///
/// @brief
///     ��ȡ����֡��ASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91PduSeg::GetASDUPublicInfoOff(const size_t idxSeq, SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;
	m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfoOff(asduPublicOff);
	return true;
}

///
/// @brief
///     ��ȡĳһASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     smpOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     arrSize   - �����С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav91PduSeg::GetASDUSmpsOff(const size_t idxSeq, size_t *smpOffArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsOff(smpOffArr,arrSize);
}

///
/// @brief
///     ��ձ������
///
/// @return
///     
///
void Sav91PduSeg::Clear()
{
	m_NoASDU    = 0;
	m_NoASDUOff = 0;
	m_PduLen    = 0;
	m_PduOff    = 0;
	m_AsduSize  = 0;
}

///
/// @brief
///     ����Sav91PDU�������ϲ�Э�����ͣ�������Sav91Pdu�����Size
///
/// @param[in]
///     type - Sav91PDU�������ϲ�Э������
///
/// @return
///     
///
void Sav91PduSeg::SetULPType(const SAV91_ULP_TYPE type)
{
	switch (type)
	{
	case SAV91_IEEE802_1Q:
		// ��̫��֡��󳤶ȼ�ȥSav91PDU֮ǰ�Ĳ��ֵĳ���
		m_MaxPduSize = 1514 - 26;
		break;
	default:
		m_MaxPduSize = 1514 - 26;
	}
}

///
/// @brief
///     ���һ��ASDU������֡��
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
bool Sav91PduSeg::AddAnASDU(const SAV91_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	// ��¼��1��ASDU��Size
	if ( m_NoASDU == 1 )
	{
		uint8_t asduTmp[ETHER_FRAME_MAX_LEN];
		m_AsduSize = m_AsduSubSeg.at(0)->Code(asduTmp,ETHER_FRAME_MAX_LEN);
	}

	// �ж������һ��ASDU�Ƿ����ɱ���֡���
	if ( m_AsduSize * ( m_NoASDU + 1 ) > ( m_MaxPduSize - SAV91_PDU_NO_ASDU_MAX_SIZE ) )
		return false;

	// ���ASDU
	if ( m_NoASDU >= m_AsduSubSeg.size() )
	{
		Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
	Sav91AsduSeg *asduSeg = m_AsduSubSeg.at(m_NoASDU);
	if ( asduSeg == NULL )
		return false;

	if ( asduSeg->SetASDU(asduPublic,smpValBuf,bufSize) == false )
		return false;
	
	m_NoASDU ++;
	return true;
}

