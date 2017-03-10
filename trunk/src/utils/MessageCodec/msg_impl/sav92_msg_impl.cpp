 /// @file
///     sav92_msg_impl.cpp
///
/// @brief
///     Sav92����ֵ����֡��IEC61850-9-2LE���ı����ӿڵ�ʵ��
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
///    |2013.11.13     |1.0.1.1113 |�Ƿ����½�����ж��������ӶԴ����֡�����С���ж�         |
///
#include "stdafx.h"
#include "sav92_msg_impl.h"
#include "../msg_seg/ethernetII_seg.h"
#include "../msg_seg/ieee802_1q_seg.h"
#include "../msg_seg/iec8802_3_pdu_public_seg.h"
#include "../msg_seg/sav92_pdu_seg.h"
#include "../include/msg_rapid_decoder.h"
#include "FrameDecode.h"
Sav92MsgImpl::Sav92MsgImpl()
: m_EtherSeg(new EthernetIISeg)
, m_Ieee8021QSeg(new Ieee8021QSeg)
, m_Iec8802Seg(new Iec88023PduPublicSeg)
, m_Sav92PduSeg(new Sav92PduSeg)
, m_MsgDataBuf(new uint8_t[ETHER_FRAME_MAX_LEN])
, m_MsgDataBufSize(0)
, m_bHaveDecode(false)
, m_bHasVlanFlag(true)
{
	memset(m_MsgDataBuf,0,ETHER_FRAME_MAX_LEN);
}

Sav92MsgImpl::~Sav92MsgImpl()
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
	if ( m_Sav92PduSeg != NULL )
	{
		delete m_Sav92PduSeg;
		m_Sav92PduSeg = NULL;
	}
	if ( m_MsgDataBuf != NULL )
	{
		delete[] m_MsgDataBuf;
		m_MsgDataBuf = NULL;
	}
}

///
/// @brief
///     ��IEC61850-9-2����ֵ����֡���н���
///
/// @param[in]
///     msgDataBuf - ���������ı���֡���ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav92MsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize  = 0;

	/// ��������û��VLAN��־ʱ���ֶ��ڱ��������
	/// 
	/*char vlanTag[4] ={0x81,0x00,0x80,0x00};
	uint8_t *msgDataBuf;
	if (!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		
		uint8_t *DataBuf = new uint8_t[ETHER_FRAME_MAX_LEN];
		memset(DataBuf,0,ETHER_FRAME_MAX_LEN);
		memcpy(DataBuf,msgBuf,12);
		memcpy(DataBuf+12,vlanTag,4);
		memcpy(DataBuf+16,msgBuf+12,bufSize-12);
		msgDataBuf = DataBuf;
	}
	else
		msgDataBuf =(uint8_t *) msgBuf;*/

	// ��ȡ��һ֡�͵�ǰ֡��appID��Э�����ݵ�Ԫ���ȡ�ͨ��������ѡ��
	MsgRapidDecoder rapidDecoder;
	uint16_t appID1,appID2;
	rapidDecoder.GetAppID(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,appID1);
	rapidDecoder.GetAppID(msgDataBuf,bufSize,appID2);
	uint16_t pduLen1,pduLen2;
	rapidDecoder.GetPDULen(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,pduLen1);
	rapidDecoder.GetPDULen(msgDataBuf,bufSize,pduLen2);
	size_t channelNum1,channelNum2;
	rapidDecoder.GetChannelNum(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,FRAME_TYPE_NET_9_2,channelNum1);
	rapidDecoder.GetChannelNum(msgDataBuf,bufSize,FRAME_TYPE_NET_9_2,channelNum2);
	SAV92_SAVPDU_OPTS opts1,opts2;
	rapidDecoder.GetSav92Opts(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,opts1);
	rapidDecoder.GetSav92Opts(msgDataBuf,bufSize,opts2);

	// ���汨����Ϣ
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// ����֡Ϊͬһ���ƿ鷢����ͨ��������ѡ�֡���泤����ͬ��������һ�εĽ������
	if ( m_bHaveDecode && appID1 == appID2 && pduLen1 == pduLen2 && channelNum1 == channelNum2 &&
		 opts1.bSecurityOpt == opts2.bSecurityOpt && opts1.bDataSetOpt == opts2.bDataSetOpt &&
		 opts1.bRefrTmOpt == opts2.bRefrTmOpt && opts1.bSmpRateOpt == opts2.bSmpRateOpt &&
		 m_MsgDataBufSize == msgLen )
	{
		return msgLen;
	}
	m_MsgDataBufSize = msgLen;

	// ���SavPDU��ʷ
	ClearSavPDU();
	m_bHaveDecode = false;

	// ��̫������֡��ͷ��Ϣ�ν���
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

	// Sav92 PDU�ν���
	dSize = m_Sav92PduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬���IEC61850-9-2����֡
///
/// @param[in]
///     msgDataArr - ���汨��֡���ݵ�����
/// @param[in]
///     arrSize    - �����Ԫ����Ŀ
///
/// @return
///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Sav92MsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	// ����Sav92 PDU��
	cSize = m_Sav92PduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// �������ISO/IEC 8802-3����֡��ʽ��PDU
	cSize = m_Iec8802Seg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// ����IEEE802.1Q VLAN��ǩ��PDU���ֵ�������
	cSize = m_Ieee8021QSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// �����̫��֡��ͷ��Ϣ���������������֡
	cSize = m_EtherSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;

	if ( arrSize < cSize )
		goto errorHandle;
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
void Sav92MsgImpl::GetEthernetHeader(ETHER_II_HEADER &ethHdr)
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
void Sav92MsgImpl::Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info)
{
	m_Ieee8021QSeg->Get8021QVlanInfo(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
///
/// @param[in]
///     info - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///
///
void Sav92MsgImpl::GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info)
{
	m_Iec8802Seg->GetIEC8802Info(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡ����֡��Sav92PDU(TLV��ʽ)�ĳ���
///
/// @return
///     Sav92PDU�ĳ���
///
size_t Sav92MsgImpl::GetSavPduLen()
{
	return m_Sav92PduSeg->GetPduLen();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU����Ŀ
///
/// @param[in]
///     noASDU - ��ʾ�ñ���֡������ASDU����Ŀ����������йأ�length��2 byte
///
/// @return
///
///
uint16_t Sav92MsgImpl::GetNoASDU()
{
	return m_Sav92PduSeg->GetNoASDU();
}

///
/// @brief
///     ��ȡ����֡��ĳһASDU��TLV��ʽ���ĳ���
///
/// @return
///     ASDU��TLV��ʽ���ĳ���
///
size_t Sav92MsgImpl::GetASDULen(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDULen(idxSeq);
}

///
/// @brief
///     ��ȡ����֡��ĳһASDU��Sequence of Data��TLV��ʽ���ĳ���
///
/// @return
///     Sequence of Data��TLV��ʽ���ĳ���
///
size_t Sav92MsgImpl::GetSeqOfDatLen(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetSeqOfDatLen(idxSeq);
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
bool Sav92MsgImpl::GetASDUPublicInfo(const size_t idxSeq, SAV92_ASDU_PUBLIC &asduPublic)
{
	return m_Sav92PduSeg->GetASDUPublicInfo(idxSeq,m_MsgDataBuf,asduPublic);
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
bool Sav92MsgImpl::GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt)
{
	return m_Sav92PduSeg->GetSmpCntInASDU(idxSeq,m_MsgDataBuf,smpCnt);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�е�ͬ����ʶλ��Ϣ
///
/// @param[in]
///     idxSeq   - ����֡��ASDU��˳��ţ���0��ʼ
/// @param[in]
///     smpSynch - ����ͬ����ʶλ��Ϣ
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Sav92MsgImpl::GetSmpSynchInASDU(const size_t idxSeq, bool &smpSynch)
{
	return m_Sav92PduSeg->GetSmpSynchInASDU(idxSeq,m_MsgDataBuf,smpSynch);
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
size_t Sav92MsgImpl::GetASDUSmpsNum(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDUSmpsNum(idxSeq);
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
///     smpQltArr - �������˲ʱֵ��Ʒ��ֵ������
/// @param[in]
///     arrSize   - ������Ĵ�С
///
/// @return
///     �����ʵ�ʴ�С
///
size_t  Sav92MsgImpl::GetASDUSmps(const size_t idxSeq, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize)
{
	return m_Sav92PduSeg->GetASDUSmps(idxSeq,m_MsgDataBuf,smpValArr,smpQltArr,arrSize);
}

///
/// @brief
///     ��ȡSavPDU�еĿ�ѡ����Ϣ
///
/// @param[in]
///     asduOpts - �����ѡ����Ϣ�Ľṹ��
///
/// @return
///     
///
void Sav92MsgImpl::GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts)
{
	m_Sav92PduSeg->GetSavPduOpts(pduOpts);
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
void Sav92MsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
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
void Sav92MsgImpl::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
{
	m_Ieee8021QSeg->Get8021QVlanInfoOff(infoOff);
}

///
/// @brief
///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     infoOff - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///
///
void Sav92MsgImpl::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	m_Iec8802Seg->GetIEC8802InfoOff(infoOff);
}

///
/// @brief
///     ��ȡSav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Sav92PDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav92MsgImpl::GetSavPduOff()
{
	return m_Sav92PduSeg->GetPduOff();
}

///
/// @brief
///     ��ȡASDU����Ŀ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ����֡�е�ASDU����Ŀ��Ϣ��ƫ�Ƶ�ַ
///
size_t Sav92MsgImpl::GetNoASDUOff()
{
	return m_Sav92PduSeg->GetNoASDUOff();
}

///
/// @brief
///     ��ȡĳ��ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @param[in]
///     idxSeq - ����֡��ASDU��˳��ţ���0��ʼ
///
/// @return
///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Sav92MsgImpl::GetASDUOff(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDUOff(idxSeq);
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
bool Sav92MsgImpl::GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	return m_Sav92PduSeg->GetASDUPublicInfoOff(idxSeq,asduPublicOff);
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
size_t Sav92MsgImpl::GetASDUSmpsOff(const size_t idxSeq, size_t &seqOfDataOff, size_t *smpValOffArr,
									size_t *smpQltOffArr, const size_t arrSize)
{
	return m_Sav92PduSeg->GetASDUSmpsOff(idxSeq,seqOfDataOff,smpValOffArr,smpQltOffArr,arrSize);
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
void Sav92MsgImpl::SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan)
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
void Sav92MsgImpl::Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan)
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
void Sav92MsgImpl::SetAppID(const uint16_t appID)
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
///     ���Sav92PDU�ı������
///
/// @return
///     
///
void Sav92MsgImpl::ClearSavPDU()
{
	m_Sav92PduSeg->Clear();
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
bool Sav92MsgImpl::AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf,
							 const uint32_t *smpQltBuf, const size_t bufSize)
{
	return m_Sav92PduSeg->AddAnASDU(asduPublic,smpValBuf,smpQltBuf,bufSize);
}

///
/// @brief
///     ����SavPDU�еĿ�ѡ����Ϣ
///
/// @param[in]
///     asduOpts - ��ѡ����Ϣ
///
/// @return
///     
///
void Sav92MsgImpl::SetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts)
{
	m_Sav92PduSeg->SetSavPduOpts(pduOpts);
}
