/// @file
///     sav91_ex_msg_impl.cpp
///
/// @brief
///     IEC61850-9-1扩展报文帧（由南京新宁光电制定，不常见）的编解码接口的实现
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.06
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
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
///     对IEC61850-9-1扩展报文帧进行解码
///
/// @param[in]
///     msgDataBuf  - 保存待解码的报文帧数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Sav91ExMsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize = 0;

	// 获取上一帧和当前帧的appID、通道数、协议数据单元长度
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

	// 缓存报文信息
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// 若两帧为同一控制块发出且通道数相同，沿用上一次的解析结果
	if ( m_bHaveDecode && appID1 == appID2 && channelNum1 == channelNum2 && pduLen1 == pduLen2)
		return msgLen;

	// 清空SavPDU历史
	ClearSavPDU();
	m_bHaveDecode = false;

	// 以太网帧头信息段解码
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(msgDataBuf,ethhdr);
	if ( ethhdr.etherType != ETHERTYPE_VLAN )
		return 0;

	// IEEE802.1Q VLAN 标签段解码
	dSize = m_Ieee8021QSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	IEEE802_1Q_VLAN_INFO info;
	m_Ieee8021QSeg->Get8021QVlanInfo(msgDataBuf,info);
	if ( info.etherType != ETHERTYPE_IEC61850_SV )
		return 0;

	// ISO/IEC 8802-3数据帧格式下的PDU公共信息解码
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	// SAV91_EX PDU段解码
	dSize = m_Sav91ExPduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;
	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，获得IEC61850-9-1扩展报文帧
///
/// @param[in]
///     msgDataArr - 保存SavPDU数据流的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存IEC61850-9-1扩展报文帧的数组的实际大小 - 成功； 0 - 失败
///
size_t Sav91ExMsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	// 编码Sav91Ex PDU段
	cSize = m_Sav91ExPduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 编码ISO/IEC 8802-3数据帧格式下的PDU公共信息，并合并上一段的信息
	cSize = m_Iec8802Seg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 编码IEEE802.1Q的Vlan 标签段，并合并上一段的信息
	cSize = m_Ieee8021QSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 编码以太网帧头信息端，并合并上一段的信息
	cSize = m_EtherSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;

	// 目标缓存空间不足，则返回0
	if ( arrSize < cSize )
		goto errorHandle;
	// 将编码结果拷贝到目标缓存
	memcpy(msgDataArr,m_MsgDataBuf,cSize);
	return cSize;

errorHandle:
	return 0;
}

///
/// @brief
///     获取以太网报文帧头信息
///
/// @param[in]
///     ethHdr - 保存以太网报文帧头信息的结构体
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
///     获取IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     info - 保存IEEE802.1Q VLAN标签信息的结构体
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
///     获取ISO/IEC8802-3版本的以太网帧中的PDU的公共信息，即PDU中APDU以外的部分
///
/// @param[in]
///     info - 保存ISO/IEC8802-3版本的以太网帧中的PDU的公共信息的结构体
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
///     获取报文帧中Sav91ExPDU的长度
///
/// @return
///     Sav91ExPDU的长度
///
size_t Sav91ExMsgImpl::GetSavPduLen()
{
	return m_Sav91ExPduSeg->GetPduLen();
}

///
/// @brief
///     获取报文帧中ASDU的数目
///
/// @return
///     报文帧包含的ASDU的数目
///
uint16_t Sav91ExMsgImpl::GetNoASDU()
{
	return m_Sav91ExPduSeg->GetNoASDU();
}

///
/// @brief
///     获取报文帧中的ASDU的公共信息，即ASDU中的采样值数据以外的部分
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91ExMsgImpl::GetASDUPublicInfo(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC &asduPublic)
{
	return m_Sav91ExPduSeg->GetASDUPublicInfo(idxSeq,m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     获取协议默认的ASDU的采样值数据部分的通道数
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t Sav91ExMsgImpl::GetASDUSmpsNum()
{
	return m_Sav91ExPduSeg->GetASDUSmpsNum();
}

///
/// @brief
///     获取报文帧中ASDU中的采样值数据
///
/// @param[in]
///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     smpValArr - 保存采样瞬时值的数组
/// @param[in]
///     arrSize   - 数组大小
///
/// @return
///     数组的实际大小
///
size_t Sav91ExMsgImpl::GetASDUSmps(const size_t idxSeq, uint16_t *smpValArr, const size_t arrSize)
{
	return m_Sav91ExPduSeg->GetASDUSmps(idxSeq,m_MsgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     获取以太网报文帧头信息在报文帧中的偏移地址信息
///
/// @param[in]
///     ethHdrOff - 保存以太网报文帧头信息的偏移地址信息的结构体
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
///     获取IEEE802.1Q VLAN标签信息在报文帧中的偏移地址信息
///
/// @param[in]
///     infoOff - 保存IEEE802.1Q VLAN标签信息的偏移地址信息的结构体
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
///     获取ISO/IEC8802-3版本的以太网帧中的PDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     infoOff - 保存ISO/IEC8802-3版本的以太网帧中的PDU的公共信息的偏移地址信息的结构体
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
///     获取Sav91ExPDU在报文帧中的偏移地址
///
/// @return
///     Sav91ExPDU在报文帧中的偏移地址
///
size_t Sav91ExMsgImpl::GetSavPduOff()
{
	return m_Sav91ExPduSeg->GetPduOff();
}

///
/// @brief
///     获取ASDU的数目信息在报文帧中的偏移地址
///
/// @return
///     ASDU的数目信息在报文帧中的偏移地址
///
size_t Sav91ExMsgImpl::GetNoASDUOff()
{
	return m_Sav91ExPduSeg->GetNoASDUOff();
}

///
/// @brief
///     获取报文帧中的ASDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91ExMsgImpl::GetASDUPublicInfoOff(const size_t idxSeq, SAV91_EX_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	return m_Sav91ExPduSeg->GetASDUPublicInfoOff(idxSeq,asduPublicOff);
}

///
/// @brief
///     获取某一ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     arrSize   - 数组大小
///
/// @return
///     数组的实际大小
///
size_t Sav91ExMsgImpl::GetASDUSmpsOff(const size_t idxSeq, uint32_t *smpOffArr, const size_t arrSize)
{
	return m_Sav91ExPduSeg->GetASDUSmpsOff(idxSeq,smpOffArr,arrSize);
}

///
/// @brief
///     设置Mac地址
///
/// @param[in]
///     desMac - 指向目的Mac地址的指针
/// @param[in]
///     srcMac - 指向源Mac地址的指针
/// @param[in]
///     macLen - MAC地址的长度
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
///     设置IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     userPriority - 用户优先级（0~7）
/// @param[in]
///     vlanID       - 虚拟局域网标识，用于标记该报文应该发送到哪个VLAN（12位）
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
///     设置AppID
///
/// @param[in]
///     appID - 应用信息标识码，用于区别应用关联，即区分GOOSE和模拟量采样值（SAV）协议
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
///     清空SavPDU的编解码结果
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
///     添加一个ASDU到报文帧中
///
/// @param[in]
///     asduPublic - ASDU的公共信息
/// @param[in]
///     smpValBuf  - 保存采样瞬时值的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91ExMsgImpl::AddAnASDU(const SAV91_EX_ASDU_PUBLIC &asduPublic,
							 const uint16_t *smpValBuf, const size_t bufSize)
{
	return m_Sav91ExPduSeg->AddAnASDU(asduPublic,smpValBuf,bufSize);
}
