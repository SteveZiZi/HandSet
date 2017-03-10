/// @file 
///     goose_msg_impl.cpp
///
/// @brief
///     GOOSE报文帧中的编解码接口的实现
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
///    |2013.7.09      |1.0.0.709  |修改是否重新解码的判断条件，增加对协议数据单元长度的判断以及获取通道数据时数据是否正确的判断|
///    |2013.09.06     |1.0.1.906  |缓存报文信息时添加对报文长度的判断，防止长度过长引起的异常 |
///    |2013.11.13     |1.0.1.1113 |是否重新解码的判断条件增加对传入的帧缓存大小的判断         |
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
///     对Goose报文帧进行解码
///
/// @param[in]
///     msgDataBuf - 保存待解码的Goose报文帧数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///	
///char vlanTag[4] ={0x81,0x00,0x80,0x00};
size_t GooseMsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	size_t offTmp = 0;
	size_t dSize = 0;

#ifndef ZY_PRJ

	/// 当报文中没有VLAN标志时，手动在报文里添加
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


	// 获取上一帧和当前帧的appID、协议数据单元长度、通道数、可选项
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

	// 缓存报文信息
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);

	// 若两帧为同一控制块发出且通道数、可选项相同，沿用上一次的解析结果
	if ( m_bHaveDecode && appID1 == appID2 && pduLen1 == pduLen2 && channelNum1 == channelNum2 &&
		 bHaveSecurity1 == bHaveSecurity2 && m_szMsgDataBufSize == msgLen )
	{
		m_GoosePduSeg->ResetChannelDecodedFlag();
		return msgLen;
	}
	m_szMsgDataBufSize = msgLen;

	// 清空GoosePDU历史
	ClearGoosePDU();
	m_bHaveDecode = false;

	// 以太网报文段解码
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp); 
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethhdr);

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
	if ( info.etherType != ETHERTYPE_IEC61850_GOOSE )
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
	// ISO/IEC 8802-3数据帧格式下的PDU公共信息的解码
	dSize = m_Iec8802Seg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
#endif
	// Goose PDU段解码
	dSize = m_GoosePduSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_bHaveDecode = true;

	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，得到GoosePdu
///
/// @param[in]
///     msgDataArr - 保存Goose报文帧数据的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
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
///     获取以太网报文帧头信息
///
/// @param[in]
///     ethHdr - 保存以太网报文帧头信息的结构体
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
///     获取IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     info - 保存IEEE802.1Q VLAN标签信息的结构体
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
///     获取ISO/IEC8802-3版本的以太网帧中的PDU的公共信息，即PDU中APDU以外的部分
///
/// @param[in]
///     info - 保存ISO/IEC8802-3版本的以太网帧中的PDU的公共信息的结构体
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
///     获取报文帧中goosePDU的长度，即整个TLV数据的长度
///
/// @return
///     goosePDU的长度
///
size_t GooseMsgImpl::GetGoosePduLen()
{
	return m_GoosePduSeg->GetPduLen();
}

///
/// @brief
///     获取报文帧中Goose数据序列（All Data）的长度（整个TLV数据）
///
/// @return
///     Goose数据序列（All Data）的长度（整个TLV数据）
///
size_t GooseMsgImpl::GetGooseDataLen()
{
	return m_GoosePduSeg->GetGooseDataLen();
}

///
/// @brief
///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
///
/// @param[in]
///     pduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool GooseMsgImpl::GetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPublic)
{
	return m_GoosePduSeg->GetPduPublicInfo(m_MsgDataBuf,pduPublic);
}

///
/// @brief
///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
///
/// @param[in]
///     pduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool GooseMsgImpl::GetPduPublicInfo(GOOSE_PDU_PUBLIC_EX &pduPublic)
{
	return m_GoosePduSeg->GetPduPublicInfo(m_MsgDataBuf,pduPublic);
}

///
/// @brief
///     获取报文帧中的数据通道的通道数
///
/// @return
///     数据通道的通道数
///
size_t GooseMsgImpl::GetChannelNum()
{
	return m_GoosePduSeg->GetChannelNum();
}

///
/// @brief
///     获取GoosePdu中的单个通道的数据
///
/// @param[in]
///     idxSeq     - GOOSE数据通道的顺序号，从0开始
///
/// @return
///     指向Goose通道数据的指针
///
GOOSE_DATA *GooseMsgImpl::GetAChannel(size_t idxSeq)
{
	GOOSE_DATA *pGooseData = m_GoosePduSeg->GetAChannel(idxSeq,m_MsgDataBuf,m_szMsgDataBufSize);

	// 通道数据发生错误，需重新解码
	if ( pGooseData == NULL )
		m_bHaveDecode = false;
	return pGooseData;
}

///
/// @brief
///     获取Security可选项的配置信息
///
/// @return
///     true - 报文中含有Security； false - 不含有
///
bool GooseMsgImpl::GetSecurityOpt()
{
	return m_GoosePduSeg->GetSecurityOpt();
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
void GooseMsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
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
void GooseMsgImpl::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
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
void GooseMsgImpl::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	m_Iec8802Seg->GetIEC8802InfoOff(infoOff);
}

///
/// @brief
///     获取goosePDU在报文帧中偏移地址
///
/// @return
///     goosePDU在报文帧中偏移地址
///
size_t GooseMsgImpl::GetGoosePduOff()
{
	return m_GoosePduSeg->GetPduOff();
}

///
/// @brief
///     获取报文帧中Goose数据序列（All Data）在报文帧中的偏移地址
///
/// @return
///     Goose数据序列（All Data）的在报文帧中的偏移地址
///
size_t GooseMsgImpl::GetGooseDataOff()
{
	return m_GoosePduSeg->GetGooseDataOff();
}

///
/// @brief
///     获取GoosePdu的公共信息部分在报文帧中的偏移地址信息
///
/// @param[in]
///     pduPblcOff - 保存GoosePdu的公共信息部分在报文帧中的偏移地址信息的结构体
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
///     设置AppID
///
/// @param[in]
///     appID - 应用信息标识码，用于区别应用关联，即区分GOOSE和模拟量采样值（SAV）协议
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
///     设置GoosePdu中公共部分的信息，即除通道数据以外的部分的信息
///
/// @param[in]
///     pduPblc - GoosePdu中公共部分的信息
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
///     添加一个数据通道
///
/// @param[in]
///     datBuf  - 保存单个GOOSE数据通道含有的数据的缓存
/// @param[in]
///     bufSize - 缓存大小，即单个GOOSE数据通道的数据数目（为1，表示该通道的数据非结构体类型）
///
/// @return
///     true - 成功； false - 失败
///
bool GooseMsgImpl::AddAChannel(const GOOSE_DATA *channel)
{
	return m_GoosePduSeg->AddAChannel(channel);
}

///
/// @brief
///     设置Security可选项的信息
///
/// @param[in]
///     opt - 报文中是否包含该项
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
///     清空报文帧中GoosePDU的编解码结果
///
/// @return
///     
///
void GooseMsgImpl::ClearGoosePDU()
{
	m_GoosePduSeg->Clear();
}
