/// @file
///     sav91_msg_impl.cpp
///
/// @brief
///     Sav91����ֵ����֡��IEC61850-9-1���ı����ӿڵ�ʵ��
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
///    |2013.5.14      |1.0.0.514  |��ԭ�����ֶν��뼰�����ֶ����ݷ�ʽ��Ϊһ�α�����֡���Ĳ������ֶ�ƫ�Ƶ�ַ|
///    |2013.5.14      |1.0.0.514  |����ͬһ���ƿ鷢���ı���֡��ʽһ�㲻�䣬�޸�ÿ֡���Ľ���Ϊͬһ���ƿ���ֻ����һ��֮�����øôεĽ�����|
///    |2013.7.09      |1.0.0.709  |�޸��Ƿ����½�����ж����������Ӷ�Э�����ݵ�Ԫ���ȵ��ж�   |
///    |2013.09.06     |1.0.1.906  |���汨����Ϣʱ��ӶԱ��ĳ��ȵ��жϣ���ֹ���ȹ���������쳣 |
///
#include "stdafx.h"
#include "sav91_msg_impl.h"
#include "../msg_seg/ethernetII_seg.h"
#include "../msg_seg/ieee802_1q_seg.h"
#include "../msg_seg/iec8802_3_pdu_public_seg.h"
#include "../msg_seg/sav91_pdu_seg.h"
#include "../include/msg_rapid_decoder.h"

Sav91MsgImpl::Sav91MsgImpl()
: m_EtherSeg(new EthernetIISeg)
, m_Ieee8021QSeg(new Ieee8021QSeg)
, m_Iec8802Seg(new Iec88023PduPublicSeg)
, m_Sav91PduSeg(new Sav91PduSeg)
, m_MsgDataBuf(new uint8_t[ETHER_FRAME_MAX_LEN])
, m_bHaveDecode(false)
,m_bHasVlanFlag(true)
{
	memset(m_MsgDataBuf,0,ETHER_FRAME_MAX_LEN);
}

Sav91MsgImpl::~Sav91MsgImpl()
{
	if ( m_EtherSeg != NULL )
	{
		delete m_EtherSeg;
		m_EtherSeg = NULL;
	}
	if ( m_Ieee8021QSeg != NULL )
	{
		delete m_Ieee8021QSeg;
		m_Ieee8021QSeg = NULL;
	}
	if ( m_Iec8802Seg != NULL )
	{
		delete m_Iec8802Seg;
		m_Iec8802Seg = NULL;
	}
	if ( m_Sav91PduSeg != NULL )
	{
		delete m_Sav91PduSeg;
		m_Sav91PduSeg = NULL;
	}
	if ( m_MsgDataBuf != NULL )
	{
		delete[] m_MsgDataBuf;
		m_MsgDataBuf = NULL;
	}
}

///
/// @brief
///     ��IEC61850-9-1����֡���н���
///
/// @param[in]
///     msgDataBuf  - ���������ı���֡���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91MsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize = 0;

	// ��ȡ��һ֡�͵�ǰ֡��appID��ͨ������Э�����ݵ�Ԫ����
	MsgRapidDecoder rapidDecoder;
	uint16_t appID1,appID2;
	rapidDecoder.GetAppID(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,appID1);
	rapidDecoder.GetAppID(msgDataBuf,bufSize,appID2);
	size_t channelNum1,channelNum2;
	rapidDecoder.GetChannelNum(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,FRAME_TYPE_NET_9_1,channelNum1);
	rapidDecoder.GetChannelNum(msgDataBuf,bufSize,FRAME_TYPE_NET_9_1,channelNum2);
	uint16_t pduLen1,pduLen2;
	rapidDecoder.GetPDULen(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,pduLen1);
	rapidDecoder.GetPDULen(msgDataBuf,bufSize,pduLen2);

	// ���汨����Ϣ
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// ����֡Ϊͬһ���ƿ鷢����ͨ������ͬ��������һ�εĽ������
	if ( m_bHaveDecode && appID1 == appID2 && channelNum1 == channelNum2 && pduLen1 == pduLen2)
		return msgLen;

	// ���SavPDU��ʷ
	ClearSavPDU();
	m_bHaveDecode = false;

	// ��̫��֡ͷ��Ϣ�ν���
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(msgDataBuf,ethhdr);

#if ZY_PRJ
	if ( ethhdr.etherType != ETHERTYPE_VLAN )
		return 0;

	// IEEE802.1Q VLAN ��ǩ�ν���
	dSize = m_Ieee8021QSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	IEEE802_1Q_VLAN_INFO info;
	m_Ieee8021QSeg->Get8021QVlanInfo(m_MsgDataBuf,info);
	if ( info.etherType != ETHERTYPE_IEC61850_SV )
		return 0;

	// ISO/IEC 8802-3����֡��ʽ�µ�PDU������Ϣ�Ľ���
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
#else
	if ( ethhdr.etherType == ETHERTYPE_VLAN )
	{
		// IEEE802.1Q VLAN ��ǩ�ν���
		dSize = m_Ieee8021QSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
		if ( dSize == 0 )
			return 0;
		offTmp += dSize;
		IEEE802_1Q_VLAN_INFO info;
		m_Ieee8021QSeg->Get8021QVlanInfo(m_MsgDataBuf,info);
		if ( info.etherType != ETHERTYPE_IEC61850_SV )
			return 0;

		m_bHasVlanFlag = true;
	}
	else if (ethhdr.etherType != ETHERTYPE_IEC61850_SV)
	{
		return 0;
	}
	else
	{
		m_bHasVlanFlag = false;
	}
	// ISO/IEC 8802-3����֡��ʽ�µ�PDU������Ϣ�Ľ���
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
#endif

	// SAV91 PDU�ν���
	dSize = m_Sav91PduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���IEC61850-9-1����֡
///
/// @param[in]
///     msgDataArr - ����SavPDU������������
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ����IEC61850-9-1����֡�������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91MsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	// ����Sav91 PDU��
	cSize = m_Sav91PduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// ����ISO/IEC 8802-3����֡��ʽ�µ�PDU������Ϣ�����ϲ���һ�ε���Ϣ
	cSize = m_Iec8802Seg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// ����IEEE802.1Q��Vlan ��ǩ�Σ����ϲ���һ�ε���Ϣ
	cSize = m_Ieee8021QSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// ������̫��֡ͷ��Ϣ�ˣ����ϲ���һ�ε���Ϣ
	cSize = m_EtherSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;

	// Ŀ�껺��ռ䲻�㣬�򷵻�0
	if ( arrSize < cSize )
		goto errorHandle;
	// ��������������Ŀ�껺��
	memcpy(msgDataArr,m_MsgDataBuf,cSize);
	return cSize;

errorHandle:
	return 0;
}

///
/// @brief
///     ��ȡ��̫������֡ͷ��Ϣ
///
/// @param[in]
///     ethHdr - ������̫������֡ͷ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::GetEthernetHeader(ETHER_II_HEADER &ethHdr)
{
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethHdr);
}

///
/// @brief
///     ��ȡIEEE802.1Q VLAN��ǩ��Ϣ
///
/// @param[in]
///     info - ����IEEE802.1Q VLAN��ǩ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info)
{
	m_Ieee8021QSeg->Get8021QVlanInfo(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
///
/// @param[in]
///     info - ����ISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info)
{
	m_Iec8802Seg->GetIEC8802Info(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡ����֡��Sav91PDU�ĳ���
///
/// @return
///     Sav91PDU�ĳ���
///
size_t Sav91MsgImpl::GetSavPduLen()
{
	return m_Sav91PduSeg->GetPduLen();
}

///
/// @brief
///     ��ȡ����֡��ASDU����Ŀ
///
/// @return
///     ����֡������ASDU����Ŀ
///
uint16_t Sav91MsgImpl::GetNoASDU()
{
	return m_Sav91PduSeg->GetNoASDU();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91MsgImpl::GetASDUPublicInfo(const size_t idxSeq, SAV91_ASDU_PUBLIC &asduPublic)
{
	return m_Sav91PduSeg->GetASDUPublicInfo(idxSeq,m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ�����������Ϣ
///
/// @param[in]
///     idxSeq - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     smpCnt - ���������������Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91MsgImpl::GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt)
{
	return m_Sav91PduSeg->GetSmpCntInASDU(idxSeq,m_MsgDataBuf,smpCnt);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�е�״̬����Ϣ
///
/// @param[in]
///     idxSeq      - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     statusWord1 - ����״̬��1��Ϣ
/// @param[in]
///     statusWord2 - ����״̬��2��Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91MsgImpl::GetStatusWordsInASDU(const size_t idxSeq, uint16_t &statusWord1, uint16_t &statusWord2)
{
	return m_Sav91PduSeg->GetStatusWordsInASDU(idxSeq,m_MsgDataBuf,statusWord1,statusWord2);
}

///
/// @brief
///     ��ȡЭ��Ĭ�ϵ�ASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t Sav91MsgImpl::GetASDUDefaultSmpsNum()
{
	return m_Sav91PduSeg->GetASDUSmpsNum();
}

///
/// @brief
///     ��ȡ����֡��ASDU�еĲ���ֵ����
///
/// @param[in]
///     idxSeq    - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     smpValArr - �������˲ʱֵ������
/// @param[in]
///     arrSize   - �����С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Sav91MsgImpl::GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize)
{
	return m_Sav91PduSeg->GetASDUSmps(idxSeq,m_MsgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     ��ȡ��̫������֡ͷ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     ethHdrOff - ������̫������֡ͷ��Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
{
	m_EtherSeg->GetEtherHdrOff(ethHdrOff);
}

///
/// @brief
///     ��ȡIEEE802.1Q VLAN��ǩ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     infoOff - ����IEEE802.1Q VLAN��ǩ��Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
{
	m_Ieee8021QSeg->Get8021QVlanInfoOff(infoOff);
}

///
/// @brief
///     ��ȡISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     infoOff - ����ISO/IEC8802-3�汾����̫��֡�е�PDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav91MsgImpl::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	m_Iec8802Seg->GetIEC8802InfoOff(infoOff);
}

///
/// @brief
///     ��ȡSav91PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Sav91PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91MsgImpl::GetSavPduOff()
{
	return m_Sav91PduSeg->GetPduOff();
}

///
/// @brief
///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91MsgImpl::GetNoASDUOff()
{
	return m_Sav91PduSeg->GetNoASDUOff();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     idxSeq     - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav91MsgImpl::GetASDUPublicInfoOff(const size_t idxSeq, SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	return m_Sav91PduSeg->GetASDUPublicInfoOff(idxSeq,asduPublicOff);
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
size_t Sav91MsgImpl::GetASDUSmpsOff(const size_t idxSeq, uint32_t *smpOffArr, const size_t arrSize)
{
	return m_Sav91PduSeg->GetASDUSmpsOff(idxSeq,smpOffArr,arrSize);
}

///
/// @brief
///     ����Mac��ַ
///
/// @param[in]
///     desMac - ָ��Ŀ��Mac��ַ��ָ��
/// @param[in]
///     srcMac - ָ��ԴMac��ַ��ָ��
/// @param[in]
///     macLen - MAC��ַ�ĳ���
///
/// @return
///
///
void Sav91MsgImpl::SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan)
{
	ETHER_II_HEADER ethhdr;
	memcpy(ethhdr.desMac,desMac,macLen);
	memcpy(ethhdr.srcMac,srcMac,macLen);
	ethhdr.etherType = ETHERTYPE_VLAN;
	m_EtherSeg->SetEthernetHeader(ethhdr, isHasVlan);
}

///
/// @brief
///     ����IEEE802.1Q VLAN��ǩ��Ϣ
///
/// @param[in]
///     userPriority - �û����ȼ���0~7��
/// @param[in]
///     vlanID       - �����������ʶ�����ڱ�Ǹñ���Ӧ�÷��͵��ĸ�VLAN��12λ��
///
/// @return
///
///
void Sav91MsgImpl::Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan)
{
	IEEE802_1Q_VLAN_INFO info;
	info.tci.userPriority = userPriority;
	info.tci.cfi = 0;
	info.tci.vlanID = vlanID;
	info.etherType = ETHERTYPE_IEC61850_SV;
	m_Ieee8021QSeg->Set8021QVlanInfo(info,isHasVlan);
}

///
/// @brief
///     ����AppID
///
/// @param[in]
///     appID - Ӧ����Ϣ��ʶ�룬��������Ӧ�ù�����������GOOSE��ģ��������ֵ��SAV��Э��
///
/// @return
///
///
void Sav91MsgImpl::SetAppID(const uint16_t appID)
{
	IEC8802_3_PDU_PUBLIC info;
	info.appID = appID;
	info.len = 0;
	info.reserved1 = 0;
	info.reserved2 = 0;
	m_Iec8802Seg->SetIEC8802Info(info);
}

///
/// @brief
///     ���SavPDU�ı������
///
/// @return
///     
///
void Sav91MsgImpl::ClearSavPDU()
{
	m_Sav91PduSeg->Clear();
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
bool Sav91MsgImpl::AddAnASDU(const SAV91_ASDU_PUBLIC &asduPublic,
							 const uint16_t *smpValBuf, const size_t bufSize)
{
	return m_Sav91PduSeg->AddAnASDU(asduPublic,smpValBuf,bufSize);
}
