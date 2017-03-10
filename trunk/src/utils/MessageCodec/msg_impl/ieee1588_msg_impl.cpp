/// @file
///     ieee1588_msg_impl.cpp
///
/// @brief
///     IEEE1588����֡�еı����ӿڵ�ʵ��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.9.29
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
///     ��IEEE1588����֡���н���
///
/// @param[in]
///     msgDataBuf - ����������IEEE1588����֡���ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Ieee1588MsgImpl::Decode(const uint8_t *msgDataBuf, const size_t bufSize)
{
	if ( msgDataBuf == NULL )
		return 0;

	size_t offTmp = 0;
	size_t dSize = 0;

	// ���汨����Ϣ
	size_t msgLen = bufSize;
	if ( msgLen > ETHER_FRAME_MAX_LEN )
		msgLen = ETHER_FRAME_MAX_LEN;
	memcpy(m_MsgDataBuf,msgDataBuf,msgLen);
	m_szMsgDataBufSize = msgLen;

	// ��ǰ�����뵽IEEE1588����ͷ����ͬ���͵�1588���ģ�����ͷ֮ǰ�Ĳ��ָ�ʽһ�£����ظ�����
	if ( m_bHaveDecode )
	{
		return msgLen;
	}

	m_bHaveDecode = false;

	// ��̫�����Ķν���
	dSize = m_EtherSeg->Decode(offTmp,&msgDataBuf[offTmp],msgLen-offTmp); 
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;
	ETHER_II_HEADER ethhdr;
	m_EtherSeg->GetEtherHdr(m_MsgDataBuf,ethhdr);
	if ( ethhdr.etherType != ETHERTYPE_IEEE1588 )
		return 0;

	// IEEE802.1Q VLAN ��ǩ�ν���
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
///     ��ȡ��̫������֡ͷ��Ϣ
///
/// @param[in]
///     ethHdr - ������̫������֡ͷ��Ϣ�Ľṹ��
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
///     ��ȡIEEE1588����ͷ��Ϣ
///
/// @param[in]
///     ieee1588Hdr - ����IEEE1588����ͷ��Ϣ�Ľṹ��
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
///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶ�
///
/// @param[in]
///     originTs - OriginTimestamp
///
/// @return
///     true - �����ɹ��� false - ����ʧ��
///
bool Ieee1588MsgImpl::GetOriginTimestamp(IEEE1588_TIMESTAMP &originTimestamp)
{
	if ( m_eMsgType == IEEE1588_TYPE_SYNC || m_eMsgType == IEEE1588_TYPE_FOLLOW_UP )
	{
		// ��ȡʱ�������������
		memcpy_s((uint8_t *)&originTimestamp.t_s,sizeof(uint64_t),&m_MsgDataBuf[m_szIeee1588BodyOff],6);
		originTimestamp.t_s = BSWAP_64(originTimestamp.t_s);
		originTimestamp.t_s = originTimestamp.t_s>>16;

		// ��ȡʱ�����С������
		memcpy_s((uint8_t *)&originTimestamp.t_ns,sizeof(uint32_t),&m_MsgDataBuf[m_szIeee1588BodyOff+6],4);
		originTimestamp.t_ns = BSWAP_32(originTimestamp.t_ns);
		return true;
	}
	return false;
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
void Ieee1588MsgImpl::GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
{
	m_EtherSeg->GetEtherHdrOff(ethHdrOff);
}

///
/// @brief
///     ��ȡIEEE1588����ͷ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     ieee1588HdrOff - ����IEEE1588����ͷ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ�Ľṹ��
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
///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶε�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     originTimestampOff - OriginTimestamp��ƫ�Ƶ�ַ
///
/// @return
///     true - �����ɹ��� false - ����ʧ��
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
