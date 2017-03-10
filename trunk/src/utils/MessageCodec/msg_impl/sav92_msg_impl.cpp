 /// @file
///     sav92_msg_impl.cpp
///
/// @brief
///     Sav92采样值报文帧（IEC61850-9-2LE）的编解码接口的实现
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.3.18
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.5.14      |1.0.0.514  |将原来逐字段解码及保存字段数据方式改为一次保存整帧报文并解码字段偏移地址|
///    |2013.5.14      |1.0.0.514  |由于同一控制块发出的报文帧格式一般不变，修改每帧报文解码为同一控制块则只解码一次之后沿用该次的解码结果|
///    |2013.7.09      |1.0.0.709  |修改是否重新解码的判断条件，增加对协议数据单元长度的判断   |
///    |2013.09.06     |1.0.1.906  |缓存报文信息时添加对报文长度的判断，防止长度过长引起的异常 |
///    |2013.11.13     |1.0.1.1113 |是否重新解码的判断条件增加对传入的帧缓存大小的判断         |
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
///     对IEC61850-9-2采样值报文帧进行解码
///
/// @param[in]
///     msgDataBuf - 保存待解码的报文帧数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Sav92MsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize  = 0;

	/// 当报文中没有VLAN标志时，手动在报文里添加
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

	// 获取上一帧和当前帧的appID、协议数据单元长度、通道数、可选项
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

	// 缓存报文信息
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// 若两帧为同一控制块发出且通道数、可选项、帧缓存长度相同，沿用上一次的解析结果
	if ( m_bHaveDecode && appID1 == appID2 && pduLen1 == pduLen2 && channelNum1 == channelNum2 &&
		 opts1.bSecurityOpt == opts2.bSecurityOpt && opts1.bDataSetOpt == opts2.bDataSetOpt &&
		 opts1.bRefrTmOpt == opts2.bRefrTmOpt && opts1.bSmpRateOpt == opts2.bSmpRateOpt &&
		 m_MsgDataBufSize == msgLen )
	{
		return msgLen;
	}
	m_MsgDataBufSize = msgLen;

	// 清空SavPDU历史
	ClearSavPDU();
	m_bHaveDecode = false;

	// 以太网报文帧的头信息段解码
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(msgDataBuf,ethhdr);

#if ZY_PRJ
	if ( ethhdr.etherType != ETHERTYPE_VLAN )
		return 0;

	// IEEE802.1Q VLAN 标签段解码
	dSize = m_Ieee8021QSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	IEEE802_1Q_VLAN_INFO info;
	m_Ieee8021QSeg->Get8021QVlanInfo(m_MsgDataBuf,info);
	if ( info.etherType != ETHERTYPE_IEC61850_SV )
		return 0;

	// ISO/IEC 8802-3数据帧格式下的PDU公共信息的解码
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
#else
	if ( ethhdr.etherType == ETHERTYPE_VLAN )
	{
		// IEEE802.1Q VLAN 标签段解码
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
	// ISO/IEC 8802-3数据帧格式下的PDU公共信息的解码
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
#endif

	// Sav92 PDU段解码
	dSize = m_Sav92PduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;
	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，获得IEC61850-9-2报文帧
///
/// @param[in]
///     msgDataArr - 保存报文帧数据的数组
/// @param[in]
///     arrSize    - 数组的元素数目
///
/// @return
///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
///
size_t Sav92MsgImpl::Code(uint8_t *msgDataArr, const size_t arrSize)
{
	size_t cSize = 0;

	m_bHaveDecode = false;

	// 编码Sav92 PDU段
	cSize = m_Sav92PduSeg->Code(m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 编码符合ISO/IEC 8802-3数据帧格式的PDU
	cSize = m_Iec8802Seg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 编码IEEE802.1Q VLAN标签及PDU部分的数据流
	cSize = m_Ieee8021QSeg->Code(m_MsgDataBuf,cSize,m_MsgDataBuf,ETHER_FRAME_MAX_LEN);
	if ( cSize == 0 )
		goto errorHandle;
	// 结合以太网帧的头信息，编码出完整报文帧
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
///     获取以太网报文帧头信息
///
/// @param[in]
///     ethHdr - 保存以太网报文帧头信息的结构体
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
///     获取IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     info - 保存IEEE802.1Q VLAN标签信息的结构体
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
///     获取符合ISO/IEC8802-3的报文帧的PDU的公共信息，即PDU中APDU以外的部分
///
/// @param[in]
///     info - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的结构体
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
///     获取报文帧中Sav92PDU(TLV形式)的长度
///
/// @return
///     Sav92PDU的长度
///
size_t Sav92MsgImpl::GetSavPduLen()
{
	return m_Sav92PduSeg->GetPduLen();
}

///
/// @brief
///     获取报文帧中的ASDU的数目
///
/// @param[in]
///     noASDU - 表示该报文帧包含的ASDU的数目，与采样率有关，length≤2 byte
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
///     获取报文帧中某一ASDU（TLV形式）的长度
///
/// @return
///     ASDU（TLV形式）的长度
///
size_t Sav92MsgImpl::GetASDULen(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDULen(idxSeq);
}

///
/// @brief
///     获取报文帧中某一ASDU的Sequence of Data（TLV形式）的长度
///
/// @return
///     Sequence of Data（TLV形式）的长度
///
size_t Sav92MsgImpl::GetSeqOfDatLen(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetSeqOfDatLen(idxSeq);
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
bool Sav92MsgImpl::GetASDUPublicInfo(const size_t idxSeq, SAV92_ASDU_PUBLIC &asduPublic)
{
	return m_Sav92PduSeg->GetASDUPublicInfo(idxSeq,m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     获取报文帧中的ASDU中的采样计数器信息
///
/// @param[in]
///     idxSeq - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     smpCnt - 保存采样计数器信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92MsgImpl::GetSmpCntInASDU(const size_t idxSeq, uint16_t &smpCnt)
{
	return m_Sav92PduSeg->GetSmpCntInASDU(idxSeq,m_MsgDataBuf,smpCnt);
}

///
/// @brief
///     获取报文帧中的ASDU中的同步标识位信息
///
/// @param[in]
///     idxSeq   - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     smpSynch - 保存同步标识位信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92MsgImpl::GetSmpSynchInASDU(const size_t idxSeq, bool &smpSynch)
{
	return m_Sav92PduSeg->GetSmpSynchInASDU(idxSeq,m_MsgDataBuf,smpSynch);
}

///
/// @brief
///     获取某一ASDU中的采样值数据部分的通道数
///
/// @param[in]
///     idxSeq  - 报文帧中ASDU的顺序号，从0开始
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t Sav92MsgImpl::GetASDUSmpsNum(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDUSmpsNum(idxSeq);
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
///     smpQltArr - 保存采样瞬时值的品质值的数组
/// @param[in]
///     arrSize   - 两数组的大小
///
/// @return
///     数组的实际大小
///
size_t  Sav92MsgImpl::GetASDUSmps(const size_t idxSeq, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize)
{
	return m_Sav92PduSeg->GetASDUSmps(idxSeq,m_MsgDataBuf,smpValArr,smpQltArr,arrSize);
}

///
/// @brief
///     获取SavPDU中的可选项信息
///
/// @param[in]
///     asduOpts - 保存可选项信息的结构体
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
///     获取以太网报文帧头信息在报文帧中的偏移地址信息
///
/// @param[in]
///     ethHdrOff - 保存以太网报文帧头信息的偏移地址信息的结构体
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
///     获取IEEE802.1Q VLAN标签信息在报文帧中的偏移地址信息
///
/// @param[in]
///     infoOff - 保存IEEE802.1Q VLAN标签信息的偏移地址信息的结构体
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
///     获取符合ISO/IEC8802-3的报文帧的PDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     infoOff - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的偏移地址信息的结构体
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
///     获取Sav92PDU在报文帧中的偏移地址
///
/// @return
///     Sav92PDU在报文帧中的偏移地址
///
size_t Sav92MsgImpl::GetSavPduOff()
{
	return m_Sav92PduSeg->GetPduOff();
}

///
/// @brief
///     获取ASDU的数目信息在报文帧中的偏移地址
///
/// @return
///     报文帧中的ASDU的数目信息的偏移地址
///
size_t Sav92MsgImpl::GetNoASDUOff()
{
	return m_Sav92PduSeg->GetNoASDUOff();
}

///
/// @brief
///     获取某个ASDU在报文帧中的偏移地址
///
/// @param[in]
///     idxSeq - 报文帧中ASDU的顺序号，从0开始
///
/// @return
///     ASDU在报文帧中的偏移地址
///
size_t Sav92MsgImpl::GetASDUOff(const size_t idxSeq)
{
	return m_Sav92PduSeg->GetASDUOff(idxSeq);
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
bool Sav92MsgImpl::GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	return m_Sav92PduSeg->GetASDUPublicInfoOff(idxSeq,asduPublicOff);
}

///
/// @brief
///     获取ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq       - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     seqOfDataOff - 采样瞬时值序列的偏移地址
/// @param[in]
///     smpValOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     smpQltOffArr - 保存采样瞬时值的品质值的偏移地址信息的数组
/// @param[in]
///     arrSize      - 两数组的大小
///
/// @return
///     数组的实际大小
///
size_t Sav92MsgImpl::GetASDUSmpsOff(const size_t idxSeq, size_t &seqOfDataOff, size_t *smpValOffArr,
									size_t *smpQltOffArr, const size_t arrSize)
{
	return m_Sav92PduSeg->GetASDUSmpsOff(idxSeq,seqOfDataOff,smpValOffArr,smpQltOffArr,arrSize);
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
///     设置AppID
///
/// @param[in]
///     appID - 应用信息标识码，用于区别应用关联，即区分GOOSE和模拟量采样值（SAV）协议
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
///     清空Sav92PDU的编解码结果
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
///     添加一个ASDU到报文帧中
///
/// @param[in]
///     asduPublic - ASDU的公共信息
/// @param[in]
///     smpValBuf  - 保存采样瞬时值的缓存
/// @param[in]
///     smpQltBuf  - 保存采样瞬时值的品质值的缓存
/// @param[in]
///     bufSize    - 缓存的大小,即通道数
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92MsgImpl::AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf,
							 const uint32_t *smpQltBuf, const size_t bufSize)
{
	return m_Sav92PduSeg->AddAnASDU(asduPublic,smpValBuf,smpQltBuf,bufSize);
}

///
/// @brief
///     设置SavPDU中的可选项信息
///
/// @param[in]
///     asduOpts - 可选项信息
///
/// @return
///     
///
void Sav92MsgImpl::SetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts)
{
	m_Sav92PduSeg->SetSavPduOpts(pduOpts);
}
