/// @file 
///     goose_msg_impl.cpp
///
/// @brief
///     GOOSE����֡�еı����ӿڵ�ʵ��
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
///    |2013.7.09      |1.0.0.709  |�޸��Ƿ����½�����ж����������Ӷ�Э�����ݵ�Ԫ���ȵ��ж��Լ���ȡͨ������ʱ�����Ƿ���ȷ���ж�|
///    |2013.09.06     |1.0.1.906  |���汨����Ϣʱ��ӶԱ��ĳ��ȵ��жϣ���ֹ���ȹ���������쳣 |
///    |2013.11.13     |1.0.1.1113 |�Ƿ����½�����ж��������ӶԴ����֡�����С���ж�         |
///
#include "stdafx.h"
#include "goose_msg_impl.h"
#include "../msg_seg/ethernetII_seg.h"
#include "../msg_seg/ieee802_1q_seg.h"
#include "../msg_seg/iec8802_3_pdu_public_seg.h"
#include "../msg_seg/goose_pdu_seg.h"
#include "../include/msg_rapid_decoder.h"
#include "FrameDecode.h"

GooseMsgImpl::GooseMsgImpl()
: m_EtherSeg(new EthernetIISeg)
, m_Ieee8021QSeg(new Ieee8021QSeg)
, m_Iec8802Seg(new Iec88023PduPublicSeg)
, m_GoosePduSeg(new GoosePduSeg)
, m_MsgDataBuf(new uint8_t[ETHER_FRAME_MAX_LEN])
, m_szMsgDataBufSize(0)
, m_bHaveDecode(false)
, m_bHasVlanFlag(true)
{
	memset(m_MsgDataBuf,0,ETHER_FRAME_MAX_LEN);
}

GooseMsgImpl::~GooseMsgImpl()
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
	if ( m_GoosePduSeg != NULL )
	{
		delete m_GoosePduSeg;
		m_GoosePduSeg = NULL;
	}
	if ( m_MsgDataBuf != NULL )
	{
		delete[] m_MsgDataBuf;
		m_MsgDataBuf = NULL;
	}
}

///
/// @brief
///     ��Goose����֡���н���
///
/// @param[in]
///     msgDataBuf - ����������Goose����֡���ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///	
///char vlanTag[4] ={0x81,0x00,0x80,0x00};
size_t GooseMsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize = 0;

#ifndef ZY_PRJ

	/// ��������û��VLAN��־ʱ���ֶ��ڱ��������
	/// 
	/*uint8_t *msgDataBuf;
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

	//m_bHasVlanFlag =GetDecodeMgr()->HadVlanType((PBYTE)msgDataBuf,bufSize);
#endif


	// ��ȡ��һ֡�͵�ǰ֡��appID��Э�����ݵ�Ԫ���ȡ�ͨ��������ѡ��
	MsgRapidDecoder rapidDecoder;
	uint16_t appID1,appID2;
	rapidDecoder.GetAppID(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,appID1);
	rapidDecoder.GetAppID(msgDataBuf,bufSize,appID2);
	uint16_t pduLen1,pduLen2;
	rapidDecoder.GetPDULen(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,pduLen1);
	rapidDecoder.GetPDULen(msgDataBuf,bufSize,pduLen2);
	size_t channelNum1,channelNum2;
	rapidDecoder.GetChannelNum(m_MsgDataBuf,ETHER_FRAME_MAX_LEN,FRAME_TYPE_NET_GOOSE,channelNum1);
	rapidDecoder.GetChannelNum(msgDataBuf,bufSize,FRAME_TYPE_NET_GOOSE,channelNum2);
	bool bHaveSecurity1,bHaveSecurity2;
	bHaveSecurity1 = rapidDecoder.IsHaveSecurityInGoose(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	bHaveSecurity2 = rapidDecoder.IsHaveSecurityInGoose(msgDataBuf,bufSize);

	// ���汨����Ϣ
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// ����֡Ϊͬһ���ƿ鷢����ͨ��������ѡ����ͬ��������һ�εĽ������
	if ( m_bHaveDecode && appID1 == appID2 && pduLen1 == pduLen2 && channelNum1 == channelNum2 &&
		 bHaveSecurity1 == bHaveSecurity2 && m_szMsgDataBufSize == msgLen )
	{
		m_GoosePduSeg->ResetChannelDecodedFlag();
		return msgLen;
	}
	m_szMsgDataBufSize = msgLen;

	// ���GoosePDU��ʷ
	ClearGoosePDU();
	m_bHaveDecode = false;

	// ��̫�����Ķν���
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp); 
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethhdr);

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
	if ( info.etherType != ETHERTYPE_IEC61850_GOOSE )
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
		if ( info.etherType != ETHERTYPE_IEC61850_GOOSE )
			return 0;

		m_bHasVlanFlag = true;
	}
	else if (ethhdr.etherType != ETHERTYPE_IEC61850_GOOSE)
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
	// Goose PDU�ν���
	dSize = m_GoosePduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;

	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬�õ�GoosePdu
///
/// @param[in]
///     msgDataArr - ����Goose����֡���ݵ�����
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t GooseMsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	cSize = m_GoosePduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	cSize = m_Iec8802Seg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	cSize = m_Ieee8021QSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	cSize = m_EtherSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;

	if ( arrSize < cSize )
		goto errorHandle;
	memcpy(msgDataArr,m_MsgDataBuf,cSize);
	m_szMsgDataBufSize = cSize;

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
void GooseMsgImpl::GetEthernetHeader(ETHER_II_HEADER &ethHdr)
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
void GooseMsgImpl::Get8021QVlanInfo(IEEE802_1Q_VLAN_INFO &info)
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
void GooseMsgImpl::GetIEC8802Info(IEC8802_3_PDU_PUBLIC &info)
{
	m_Iec8802Seg->GetIEC8802Info(m_MsgDataBuf,info);
}

///
/// @brief
///     ��ȡ����֡��goosePDU�ĳ��ȣ�������TLV���ݵĳ���
///
/// @return
///     goosePDU�ĳ���
///
size_t GooseMsgImpl::GetGoosePduLen()
{
	return m_GoosePduSeg->GetPduLen();
}

///
/// @brief
///     ��ȡ����֡��Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
/// @return
///     Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
size_t GooseMsgImpl::GetGooseDataLen()
{
	return m_GoosePduSeg->GetGooseDataLen();
}

///
/// @brief
///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
///
/// @param[in]
///     pduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GooseMsgImpl::GetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPublic)
{
	return m_GoosePduSeg->GetPduPublicInfo(m_MsgDataBuf,pduPublic);
}

///
/// @brief
///     ��ȡ����֡�е�GoosePdu�Ĺ�����Ϣ����GoosePdu�е�ͨ����������Ĳ���
///
/// @param[in]
///     pduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GooseMsgImpl::GetPduPublicInfo(GOOSE_PDU_PUBLIC_EX &pduPublic)
{
	return m_GoosePduSeg->GetPduPublicInfo(m_MsgDataBuf,pduPublic);
}

///
/// @brief
///     ��ȡ����֡�е�����ͨ����ͨ����
///
/// @return
///     ����ͨ����ͨ����
///
size_t GooseMsgImpl::GetChannelNum()
{
	return m_GoosePduSeg->GetChannelNum();
}

///
/// @brief
///     ��ȡGoosePdu�еĵ���ͨ��������
///
/// @param[in]
///     idxSeq     - GOOSE����ͨ����˳��ţ���0��ʼ
///
/// @return
///     ָ��Gooseͨ�����ݵ�ָ��
///
GOOSE_DATA *GooseMsgImpl::GetAChannel(size_t idxSeq)
{
	GOOSE_DATA *pGooseData = m_GoosePduSeg->GetAChannel(idxSeq,m_MsgDataBuf,m_szMsgDataBufSize);

	// ͨ�����ݷ������������½���
	if ( pGooseData == NULL )
		m_bHaveDecode = false;
	return pGooseData;
}

///
/// @brief
///     ��ȡSecurity��ѡ���������Ϣ
///
/// @return
///     true - �����к���Security�� false - ������
///
bool GooseMsgImpl::GetSecurityOpt()
{
	return m_GoosePduSeg->GetSecurityOpt();
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
void GooseMsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
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
void GooseMsgImpl::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
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
void GooseMsgImpl::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	m_Iec8802Seg->GetIEC8802InfoOff(infoOff);
}

///
/// @brief
///     ��ȡgoosePDU�ڱ���֡��ƫ�Ƶ�ַ
///
/// @return
///     goosePDU�ڱ���֡��ƫ�Ƶ�ַ
///
size_t GooseMsgImpl::GetGoosePduOff()
{
	return m_GoosePduSeg->GetPduOff();
}

///
/// @brief
///     ��ȡ����֡��Goose�������У�All Data���ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t GooseMsgImpl::GetGooseDataOff()
{
	return m_GoosePduSeg->GetGooseDataOff();
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
void GooseMsgImpl::GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff)
{
	m_GoosePduSeg->GetPduPublicInfoOff(pduPblcOff);
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
void GooseMsgImpl::SetMacAddr(const uint8_t *desMac, const uint8_t *srcMac, size_t macLen, bool isHasVlan)
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
void GooseMsgImpl::Set8021QVlanInfo(uint8_t userPriority, uint16_t vlanID, bool isHasVlan)
{
	IEEE802_1Q_VLAN_INFO info;
	info.tci.userPriority = userPriority;
	info.tci.cfi = 0;
	info.tci.vlanID = vlanID;
	info.etherType = ETHERTYPE_IEC61850_GOOSE;
	m_Ieee8021QSeg->Set8021QVlanInfo(info, isHasVlan);
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
void GooseMsgImpl::SetAppID(const uint16_t appID)
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
///     ����GoosePdu�й������ֵ���Ϣ������ͨ����������Ĳ��ֵ���Ϣ
///
/// @param[in]
///     pduPblc - GoosePdu�й������ֵ���Ϣ
///
/// @return
///
///
void GooseMsgImpl::SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc)
{
	m_GoosePduSeg->SetPduPublicInfo(pduPblc);
}

///
/// @brief
///     ���һ������ͨ��
///
/// @param[in]
///     datBuf  - ���浥��GOOSE����ͨ�����е����ݵĻ���
/// @param[in]
///     bufSize - �����С��������GOOSE����ͨ����������Ŀ��Ϊ1����ʾ��ͨ�������ݷǽṹ�����ͣ�
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GooseMsgImpl::AddAChannel(const GOOSE_DATA *channel)
{
	return m_GoosePduSeg->AddAChannel(channel);
}

///
/// @brief
///     ����Security��ѡ�����Ϣ
///
/// @param[in]
///     opt - �������Ƿ��������
///
/// @return
///     
///
void GooseMsgImpl::SetSecurityOpt(bool opt)
{
	m_GoosePduSeg->SetSecurityOpt(opt);
}

///
/// @brief
///     ��ձ���֡��GoosePDU�ı������
///
/// @return
///     
///
void GooseMsgImpl::ClearGoosePDU()
{
	m_GoosePduSeg->Clear();
}
