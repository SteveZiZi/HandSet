/// @file
///     sav92_pdu_seg.cpp
///
/// @brief
///     Sav92��IEC91850-9-2������ֵ���ĵ�SavPDU��
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
///    |2013.11.15     |1.0.1.1115 |����ASDU��ѡ���������ǰ��AddAnASDU()֮ǰ��������Ԥ��9-2���ĳ���ʱ��������|
///
#include "stdafx.h"
#include "../include/etypes.h"
#include "Sav92_pdu_seg.h"
#include "Sav92_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     ���ASDU��Ŀ
///
#define SAV92_PDU_MAX_NO_ASDU 8

///
/// @brief
///     Sav92 PDU������ȥ��ASDU���ʣ�ಿ�ֵ����Size������Security��
///
#define SAV92_PDU_NO_ASDU_MAX_SIZE1 11

///
/// @brief
///     Sav92 PDU������ȥ��ASDU���ʣ�ಿ�ֵ����Size����Security��
///
#define SAV92_PDU_NO_ASDU_MAX_SIZE2 48

Sav92PduSeg::Sav92PduSeg()
: m_pNoASDU(CreateMsgDataElemImpl(DT_TLV_DATA))
, m_pSecurity(CreateMsgDataElemImpl(DT_TLV_DATA))
, m_PduLen(0)
, m_PduOff(0)
, m_AsduSize(0)
, m_nNoASDU(0)
, m_MaxPduSize(ETHER_FRAME_MAX_LEN-26)
{
	m_pNoASDU->SetTag(SAV92_TAG_NO_ASDU);
	m_pSecurity->SetTag(SAV92_TAG_SECURITY);
	m_PduOpts.bSecurityOpt = false;
	m_PduOpts.bDataSetOpt  = false;
	m_PduOpts.bRefrTmOpt   = false;
	m_PduOpts.bSmpRateOpt  = false;
	m_AsduSubSeg.reserve(SAV92_PDU_MAX_NO_ASDU);
	for ( int i = 0; i < SAV92_PDU_MAX_NO_ASDU; i++ )
	{
		Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
}

Sav92PduSeg::~Sav92PduSeg()
{
	ReleaseMsgDataElemImpl(m_pNoASDU);
	ReleaseMsgDataElemImpl(m_pSecurity);
	std::vector<Sav92AsduSeg *>::iterator it;
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
///     �Ա���֡�е�Sav92PDU���ֽ��н���
///
/// @param[in]
///     offset      - �������Sav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     pduDataBuf  - ����������Sav92PDU���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav92PduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	if ( pduDataBuf == NULL )
		return 0;

	// ����savPDU��value��Ϣ
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV92_TAG_SAV_PDU);
	size_t dSize  = savPDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(savPDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;
	offTmp  += dSize - savPDU->GetValueLen();
	ReleaseMsgDataElemImpl(savPDU);

	// ����noASDU
	dSize = m_pNoASDU->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	// ��ȡnoASDU��valueֵ����ASDU�ĸ���
	m_nNoASDU = 0;
	size_t noAsduLen = m_pNoASDU->GetValueLen();
	memcpy((uint8_t *)&m_nNoASDU,&pduDataBuf[offTmp-noAsduLen],noAsduLen);
	if ( noAsduLen > 1 )
		m_nNoASDU = BSWAP_16(m_nNoASDU);
	if ( m_nNoASDU == 0 )
		return 0;

	// ����Security
	dSize = m_pSecurity->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
		m_PduOpts.bSecurityOpt = false;
	else
		m_PduOpts.bSecurityOpt = true;
	offTmp += dSize;
	
	// ����Sequence of ASDU��value��Ϣ
	MsgDataElem *seqOfASDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	seqOfASDU->SetTag(SAV92_TAG_SEQ_ASDU);
	dSize = seqOfASDU->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(seqOfASDU);
		return 0;
	}
	offTmp += dSize - seqOfASDU->GetValueLen();
	ReleaseMsgDataElemImpl(seqOfASDU);

	// ����Sequence of ASDU
	for (size_t i = 0; i < m_nNoASDU; i++ )
	{
		if ( offTmp >= bufSize )
		{
			m_nNoASDU = i;
			if ( m_nNoASDU == 0 )
				return 0;
			break;
		}
		// ��������ASDU��value
		if ( i >= m_AsduSubSeg.size() )
		{
			Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
			m_AsduSubSeg.push_back(asduSeg);
		}
		
		Sav92AsduSeg *asduSeg = m_AsduSubSeg.at(i);
		if ( asduSeg == NULL )
			return 0;

		dSize = asduSeg->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( dSize == 0 )
			return 0;
		
		offTmp += dSize;
	}

	// ���¿�ѡ��������Ϣ
	SAV92_ASDU_OPTS asduOpts;
	m_AsduSubSeg.at(0)->GetASDUOpts(asduOpts);
	m_PduOpts.bDataSetOpt = asduOpts.bDataSetOpt;
	m_PduOpts.bRefrTmOpt  = asduOpts.bRefrTmOpt;
	m_PduOpts.bSmpRateOpt = asduOpts.bSmpRateOpt;

	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���Sav92PDU������
///
/// @param[in]
///     pduDataArr - ����Sav92PDU������������
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ����Sav92PDU�������������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav92PduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	if ( pduDataArr == NULL )
		return 0;

	// ��ȡASDU����Ŀ
	if ( m_nNoASDU == 0 )
		return 0;

	size_t cSize = 0;
	size_t arrOff = 0;

	// ����noASDU
	if ( ( m_nNoASDU & 0xFF00 ) == 0 )
	{
		uint8_t uint8Tmp;
		uint8Tmp = (uint8_t)m_nNoASDU;
		m_pNoASDU->SetValue(&uint8Tmp,1);
	}
	else
	{
		uint16_t uint16Tmp;
		uint16Tmp = BSWAP_16(m_nNoASDU);
		m_pNoASDU->SetValue((uint8_t *)&uint16Tmp,2);
	}
	cSize = m_pNoASDU->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		return 0;
	arrOff += cSize;

	// ����Security
	if ( m_PduOpts.bSecurityOpt )
	{
		cSize = m_pSecurity->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// ����Sequence of ASDU��valueֵ
	size_t asduLen = 0;
	for (int i = 0; i < m_nNoASDU; i++ )
	{
		if ( arrOff + asduLen >= arrSize )
			break;
		// ���뵥��ASDU
		cSize = m_AsduSubSeg.at(i)->Code(&pduDataArr[arrOff+asduLen],arrSize-arrOff-asduLen);
		if ( cSize == 0 )
			return 0;
		asduLen += cSize;
	}

	// ����Sequence of ASDU
	MsgDataElem *seqOfASDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	seqOfASDU->SetTag(SAV92_TAG_SEQ_ASDU);
	seqOfASDU->SetValue(&pduDataArr[arrOff],asduLen);
	cSize = seqOfASDU->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(seqOfASDU);
		return 0;
	}
	arrOff += cSize;
	ReleaseMsgDataElemImpl(seqOfASDU);

	// ����savPDU
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV92_TAG_SAV_PDU);
	savPDU->SetValue(pduDataArr,arrOff);
	cSize = savPDU->Code(pduDataArr,arrSize);
	ReleaseMsgDataElemImpl(savPDU);
	m_PduLen = cSize;

	return cSize;
}

///
/// @brief
///     ��ȡ����֡��Sav92PDU(TLV��ʽ)�ĳ���
///
/// @return
///     Sav92PDU�ĳ���
///
size_t Sav92PduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     ��ȡ����֡��ASDU����Ŀ
///
/// @return
///     �ñ���֡������ASDU����Ŀ
///
uint16_t Sav92PduSeg::GetNoASDU()
{
	return m_nNoASDU;
//	return (uint16_t)m_AsduSubSeg.size();
}

///
/// @brief
///     ��ȡ����֡��ĳһASDU��TLV��ʽ���ĳ���
///
/// @return
///     ASDU��TLV��ʽ���ĳ���
///
size_t Sav92PduSeg::GetASDULen(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDULen();
}

///
/// @brief
///     ��ȡ����֡��ĳһASDU��Sequence of Data��TLV��ʽ���ĳ���
///
/// @return
///     Sequence of Data��TLV��ʽ���ĳ���
///
size_t Sav92PduSeg::GetSeqOfDatLen(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetSeqOfDatLen();
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
bool Sav92PduSeg::GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
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
bool Sav92PduSeg::GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpCntInASDU(msgDataBuf,smpCnt);
}


///
/// @brief
///     ��ȡ����֡�е�ASDU�е�ͬ����ʶλ��Ϣ
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     smpSynch   - ����ͬ����ʶλ��Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav92PduSeg::GetSmpSynchInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, bool &smpSynch)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpSynchInASDU(msgDataBuf,smpSynch);
}

///
/// @brief
///     ��ȡĳһASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
/// @param[in]
///     idxSeq  - ����֡��ASDU��˳��ţ���0��ʼ
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t Sav92PduSeg::GetASDUSmpsNum(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU)
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsNum();
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
///     smpQltArr  - �������˲ʱֵ��Ʒ��ֵ������
/// @param[in]
///     arrSize    - ������Ĵ�С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav92PduSeg::GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint32_t *smpValArr,
								uint32_t *smpQltArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmps(msgDataBuf,smpValArr,smpQltArr,arrSize);
}

///
/// @brief
///     ��ȡSavPDU�еĿ�ѡ���������Ϣ
///
/// @param[in]
///     asduOpts - �����ѡ��������Ϣ�Ľṹ��
///
/// @return
///     
///
void Sav92PduSeg::GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts)
{
	pduOpts = m_PduOpts;
}

///
/// @brief
///     ��ȡSav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Sav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav92PduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ����֡�е�ASDU����Ŀ��Ϣ��ƫ�Ƶ�ַ
///
size_t Sav92PduSeg::GetNoASDUOff()
{
	return m_pNoASDU->GetElemOffset();
}

///
/// @brief
///     ��ȡĳ��ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @param[in]
///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
///
/// @return
///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav92PduSeg::GetASDUOff(const size_t idxSeq)
{
	return m_AsduSubSeg.at(idxSeq)->GetASDUOff();
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
bool Sav92PduSeg::GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;
	m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfoOff(asduPublicOff);
	return true;
}

///
/// @brief
///     ��ȡASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     idxSeq       - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     seqOfDataOff - ����˲ʱֵ���е�ƫ�Ƶ�ַ
/// @param[in]
///     smpValOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     smpQltOffArr - �������˲ʱֵ��Ʒ��ֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     arrSize      - ������Ĵ�С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav92PduSeg::GetASDUSmpsOff(const size_t idxSeq, size_t &seqofDataOff, size_t *smpValOffArr, size_t *smpQltOffArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsOff(seqofDataOff,smpValOffArr,smpQltOffArr,arrSize);
}

///
/// @brief
///     ��ձ������
///
/// @return
///     
///
void Sav92PduSeg::Clear()
{
	m_nNoASDU = 0;
	m_pNoASDU->SetValue(NULL,0);
	m_pSecurity->SetValue(NULL,0);
	m_PduLen    = 0;
	m_PduOff    = 0;
}

///
/// @brief
///     ����Sav92PDU�������ϲ�Э�����ͣ�������Sav92Pdu�����Size
///
/// @param[in]
///     type - Sav92PDU�������ϲ�Э������
///
/// @return
///     
///
void Sav92PduSeg::SetULPType(const SAV92_ULP_TYPE type)
{
	switch (type)
	{
	case SAV92_IEEE802_1Q:
		// ��̫��֡��󳤶ȼ�ȥSav92PDU֮ǰ�Ĳ��ֵĳ���
		m_MaxPduSize = ETHER_FRAME_MAX_LEN - 26;
		break;
	default:
		m_MaxPduSize = ETHER_FRAME_MAX_LEN - 26;
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
///     smpQltBuf  - �������˲ʱֵ��Ʒ��ֵ�Ļ���
/// @param[in]
///     bufSize    - ����Ĵ�С,��ͨ����
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav92PduSeg::AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf, const size_t &bufSize)
{
	// ��¼��1��ASDU��Size,ͬһ֡��ASDU�ĸ�ʽһ��
	if ( m_nNoASDU == 1 )
	{
		uint8_t asduTmp[ETHER_FRAME_MAX_LEN];
		m_AsduSize = m_AsduSubSeg.at(0)->Code(asduTmp,ETHER_FRAME_MAX_LEN);
	}

	// �ж������һ��ASDU�Ƿ����ɱ���֡���
	if ( m_PduOpts.bSecurityOpt )
	{
		if ( m_AsduSize * ( m_nNoASDU + 1 ) > ( m_MaxPduSize - SAV92_PDU_NO_ASDU_MAX_SIZE2 ) )
			return false;
	}
	else
	{
		if ( m_AsduSize * ( m_nNoASDU + 1 ) > ( m_MaxPduSize - SAV92_PDU_NO_ASDU_MAX_SIZE1 ) )
			return false;
	}

	// ���ASDU
	if ( m_nNoASDU >= m_AsduSubSeg.size() )
	{
		Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
	Sav92AsduSeg *asduSeg = m_AsduSubSeg.at(m_nNoASDU);
	if ( asduSeg == NULL )
		return false;

	// ���ÿ�ѡ��
	SAV92_ASDU_OPTS asduOpts = {
		m_PduOpts.bDataSetOpt,
		m_PduOpts.bRefrTmOpt,
		m_PduOpts.bSmpRateOpt
	};
	asduSeg->SetASDUOpts(asduOpts);
	// ����ASDU��Ϣ
	if ( asduSeg->SetASDU(asduPublic,smpValBuf,smpQltBuf,bufSize) == false )
		return false;

	m_nNoASDU++;
	return true;
}

///
/// @brief
///     ����SavPDU�еĿ�ѡ��������Ϣ��������AddAnASDU()֮ǰ����
///
/// @param[in]
///     asduOpts - ��ѡ��������Ϣ
///
/// @return
///     
///
void Sav92PduSeg::SetSavPduOpts(const SAV92_SAVPDU_OPTS &pduOpts)
{
	m_PduOpts = pduOpts;
}
