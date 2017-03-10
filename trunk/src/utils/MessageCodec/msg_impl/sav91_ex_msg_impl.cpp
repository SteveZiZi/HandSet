/// @file
///     sav91_ex_msg_impl.cpp
///
/// @brief
///     IEC61850-9-1��չ����֡�����Ͼ���������ƶ������������ı����ӿڵ�ʵ��
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
#include "sav91_ex_msg_impl.h"
#include "../msg_seg/ethernetII_seg.h"
#include "../msg_seg/ieee802_1q_seg.h"
#include "../msg_seg/iec8802_3_pdu_public_seg.h"
#include "../msg_seg/sav91_ex_pdu_seg.h"
#include "../include/msg_rapid_decoder.h"

Sav91ExMsgImpl::Sav91ExMsgImpl()
: m_EtherSeg(new EthernetIISeg)
, m_Ieee8021QSeg(new Ieee8021QSeg)
, m_Iec8802Seg(new Iec88023PduPublicSeg)
, m_Sav91ExPduSeg(new Sav91ExPduSeg)
, m_MsgDataBuf(new uint8_t[ETHER_FRAME_MAX_LEN])
, m_bHaveDecode(false)
{
	memset(m_MsgDataBuf,0,ETHER_FRAME_MAX_LEN);
}

Sav91ExMsgImpl::~Sav91ExMsgImpl()
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
	if ( m_Sav91ExPduSeg != NULL )
	{
		delete m_Sav91ExPduSeg;
		m_Sav91ExPduSeg = NULL;
	}
	if ( m_MsgDataBuf != NULL )
	{
		delete[] m_MsgDataBuf;
		m_MsgDataBuf = NULL;
	}
}

///
/// @brief
///     ��IEC61850-9-1��չ����֡���н���
///
/// @param[in]
///     msgDataBuf  - ���������ı���֡���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91ExMsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
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
	if ( ethhdr.etherType != ETHERTYPE_VLAN )
		return 0;

	// IEEE802.1Q VLAN ��ǩ�ν���
	dSize = m_Ieee8021QSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	IEEE802_1Q_VLAN_INFO info;
	m_Ieee8021QSeg->Get8021QVlanInfo(msgDataBuf,info);
	if ( info.etherType != ETHERTYPE_IEC61850_SV )
		return 0;

	// ISO/IEC 8802-3����֡��ʽ�µ�PDU������Ϣ����
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	// SAV91_EX PDU�ν���
	dSize = m_Sav91ExPduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���IEC61850-9-1��չ����֡
///
/// @param[in]
///     msgDataArr - ����SavPDU������������
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ����IEC61850-9-1��չ����֡�������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav91ExMsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	// ����Sav91Ex PDU��
	cSize = m_Sav91ExPduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
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
void Sav91ExMsgImpl::GetEthernetHeader(ETHER_II_HEADER &ethHdr)
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
void Sav91ExMsgImpl::Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info)
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
void Sav91ExMsgImpl::GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info)
{
	m_Iec8802Seg->GetIEC8802Info(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡ����֡��Sav91ExPDU�ĳ���
///
/// @return
///     Sav91ExPDU�ĳ���
///
size_t Sav91ExMsgImpl::GetSavPduLen()
{
	return m_Sav91ExPduSeg->GetPduLen();
}

///
/// @brief
///     ��ȡ����֡��ASDU����Ŀ
///
/// @return
///     ����֡������ASDU����Ŀ
///
uint16_t Sav91ExMsgImpl::GetNoASDU()
{
	return m_Sav91ExPduSeg->GetNoASDU();
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
bool Sav91ExMsgImpl::GetASDUPublicInfo(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC &asduPublic)
{
	return m_Sav91ExPduSeg->GetASDUPublicInfo(idxSeq,m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     ��ȡЭ��Ĭ�ϵ�ASDU�Ĳ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t Sav91ExMsgImpl::GetASDUSmpsNum()
{
	return m_Sav91ExPduSeg->GetASDUSmpsNum();
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
size_t Sav91ExMsgImpl::GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize)
{
	return m_Sav91ExPduSeg->GetASDUSmps(idxSeq,m_MsgDataBuf,smpValArr,arrSize);
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
void Sav91ExMsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
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
void Sav91ExMsgImpl::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
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
void Sav91ExMsgImpl::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	m_Iec8802Seg->GetIEC8802InfoOff(infoOff);
}

///
/// @brief
///     ��ȡSav91ExPDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Sav91ExPDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91ExMsgImpl::GetSavPduOff()
{
	return m_Sav91ExPduSeg->GetPduOff();
}

///
/// @brief
///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav91ExMsgImpl::GetNoASDUOff()
{
	return m_Sav91ExPduSeg->GetNoASDUOff();
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
bool Sav91ExMsgImpl::GetASDUPublicInfoOff(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	return m_Sav91ExPduSeg->GetASDUPublicInfoOff(idxSeq,asduPublicOff);
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
size_t Sav91ExMsgImpl::GetASDUSmpsOff(const size_t idxSeq, uint32_t *smpOffArr, const size_t arrSize)
{
	return m_Sav91ExPduSeg->GetASDUSmpsOff(idxSeq,smpOffArr,arrSize);
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
void Sav91ExMsgImpl::SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen)
{
	ETHER_II_HEADER ethhdr;
	memcpy(ethhdr.desMac,desMac,macLen);
	memcpy(ethhdr.srcMac,srcMac,macLen);
	ethhdr.etherType = ETHERTYPE_VLAN;
	m_EtherSeg->SetEthernetHeader(ethhdr);
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
void Sav91ExMsgImpl::Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID)
{
	IEEE802_1Q_VLAN_INFO info;
	info.tci.userPriority = userPriority;
	info.tci.cfi = 0;
	info.tci.vlanID = vlanID;
	info.etherType = ETHERTYPE_IEC61850_SV;
	m_Ieee8021QSeg->Set8021QVlanInfo(info);
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
void Sav91ExMsgImpl::SetAppID(const uint16_t appID)
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
void Sav91ExMsgImpl::ClearSavPDU()
{
	m_Sav91ExPduSeg->Clear();
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
bool Sav91ExMsgImpl::AddAnASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic,
							 const uint16_t *smpValBuf, const size_t bufSize)
{
	return m_Sav91ExPduSeg->AddAnASDU(asduPublic,smpValBuf,bufSize);
}
