/// @file
///     ieee1588_msg_impl.cpp
///
/// @brief
///     IEEE1588报文帧中的编解码接口的实现
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.9.29
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
#include "ieee1588_msg_impl.h"
#include "../msg_seg/ethernetII_seg.h"
#include "../msg_seg/ieee1588_header_seg.h"
#include "../msg_seg/bswap.h"

Ieee1588MsgImpl::Ieee1588MsgImpl()
: m_EtherSeg(new EthernetIISeg)
, m_Ieee1588HeaderSeg(new Ieee1588HeaderSeg)
, m_szMsgDataBufSize(0)
, m_bHaveDecode(false)
, m_eMsgType(IEEE1588_TYPE_UNKNOWN)
, m_szIeee1588BodyOff(0)
{
	memset(m_MsgDataBuf,0,ETHER_FRAME_MAX_LEN);
}

Ieee1588MsgImpl::~Ieee1588MsgImpl()
{
	if ( m_EtherSeg != NULL )
	{
		delete m_EtherSeg;
		m_EtherSeg = NULL;
	}
	if ( m_Ieee1588HeaderSeg != NULL )
	{
		delete m_Ieee1588HeaderSeg;
		m_Ieee1588HeaderSeg = NULL;
	}
}

///
/// @brief
///     对IEEE1588报文帧进行解码
///
/// @param[in]
///     msgDataBuf - 保存待解码的IEEE1588报文帧数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Ieee1588MsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	if ( msgDataBuf == NULL )
		return 0;

	size_t offTmp = 0;
	size_t dSize = 0;

	// 缓存报文信息
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);
	m_szMsgDataBufSize = msgLen;

	// 当前仅解码到IEEE1588报文头，不同类型的1588报文，报文头之前的部分格式一致，不重复解码
	if ( m_bHaveDecode )
	{
		return msgLen;
	}

	m_bHaveDecode = false;

	// 以太网报文段解码
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp); 
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethhdr);
	if ( ethhdr.etherType != ETHERTYPE_IEEE1588 )
		return 0;

	// IEEE802.1Q VLAN 标签段解码
	dSize = m_Ieee1588HeaderSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	m_eMsgType = m_Ieee1588HeaderSeg->GetIeee1588MsgType(msgDataBuf);
	m_szIeee1588BodyOff = offTmp;

	m_bHaveDecode = true;

	return offTmp;
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
void Ieee1588MsgImpl::GetEthernetHeader(ETHER_II_HEADER &ethHdr)
{
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethHdr);
}

///
/// @brief
///     获取IEEE1588报文头信息
///
/// @param[in]
///     ieee1588Hdr - 保存IEEE1588报文头信息的结构体
///
/// @return
///
///
void Ieee1588MsgImpl::GetIeee1588Header(IEEE1588_HEADER &ieee1588Hdr)
{
	m_Ieee1588HeaderSeg->GetIeee1588Header(m_MsgDataBuf,ieee1588Hdr);
}

///
/// @brief
///     获取Sync报文离开主时钟时的时刻，即Sync报文或其Follow_Up报文的OriginTimestamp字段
///
/// @param[in]
///     originTs - OriginTimestamp
///
/// @return
///     true - 操作成功； false - 操作失败
///
bool Ieee1588MsgImpl::GetOriginTimestamp(IEEE1588_TIMESTAMP &originTimestamp)
{
	if ( m_eMsgType == IEEE1588_TYPE_SYNC || m_eMsgType == IEEE1588_TYPE_FOLLOW_UP )
	{
		// 获取时间戳的整数部分
		memcpy_s((uint8_t *)&originTimestamp.t_s,sizeof(uint64_t),&m_MsgDataBuf[m_szIeee1588BodyOff],6);
		originTimestamp.t_s = BSWAP_64(originTimestamp.t_s);
		originTimestamp.t_s = originTimestamp.t_s>>16;

		// 获取时间戳的小数部分
		memcpy_s((uint8_t *)&originTimestamp.t_ns,sizeof(uint32_t),&m_MsgDataBuf[m_szIeee1588BodyOff+6],4);
		originTimestamp.t_ns = BSWAP_32(originTimestamp.t_ns);
		return true;
	}
	return false;
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
void Ieee1588MsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
{
	m_EtherSeg->GetEtherHdrOff(ethHdrOff);
}

///
/// @brief
///     获取IEEE1588报文头在报文帧中的偏移地址信息
///
/// @param[in]
///     ieee1588HdrOff - 保存IEEE1588报文头在报文帧中的偏移地址信息的结构体
///
/// @return
///
///
void Ieee1588MsgImpl::GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HdrOff)
{
	m_Ieee1588HeaderSeg->GetIeee1588HeaderOff(ieee1588HdrOff);
}

///
/// @brief
///     获取Sync报文离开主时钟时的时刻，即Sync报文或其Follow_Up报文的OriginTimestamp字段的偏移地址信息
///
/// @param[in]
///     originTimestampOff - OriginTimestamp的偏移地址
///
/// @return
///     true - 操作成功； false - 操作失败
///
bool Ieee1588MsgImpl::GetOriginTimestampOff(IEEE1588_TIMESTAMP_OFFSET &originTimestampOff)
{
	if ( m_eMsgType == IEEE1588_TYPE_SYNC || m_eMsgType == IEEE1588_TYPE_FOLLOW_UP )
	{
		originTimestampOff.t_s  = m_szIeee1588BodyOff;
		originTimestampOff.t_ns = m_szIeee1588BodyOff+6;
		return true;
	}
	return false;
}
