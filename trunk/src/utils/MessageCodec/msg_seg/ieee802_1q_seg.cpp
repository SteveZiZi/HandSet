/// @file
///     ieee802_1q_seg.cpp
///
/// @brief
///     符合IEEE802.1Q的报文帧的VLAN标签段
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
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "ieee802_1q_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"
#include "..\service\datalayer\config\ParamConfigBasic.h"
///
/// @brief
///     符合IEEE802.1Q的报文帧的VLAN标签部分的长度
///
#define IEEE802_1Q_VLAN_TAG_SIZE 4

Ieee8021QSeg::Ieee8021QSeg()
: m_TCI(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_EtherType(CreateMsgDataElemImpl(DT_BASE_DATA))
{

}

Ieee8021QSeg::~Ieee8021QSeg()
{
	ReleaseMsgDataElemImpl(m_TCI);
	ReleaseMsgDataElemImpl(m_EtherType);
}

///
/// @brief
///     对符合IEEE802.1Q的报文帧的VLAN标签部分进行解码
///
/// @param[in]
///     offset      - 待解码的VLAN标签部分在报文帧中的偏移地址
/// @param[in]
///     vlanDataBuf - 保存待解码的VLAN标签部分数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Ieee8021QSeg::Decode(const size_t offset, const uint8_t *vlanDataBuf, const size_t bufSize)
{
	if ( bufSize < IEEE802_1Q_VLAN_TAG_SIZE )
		return 0;

	if ( m_TCI->Decode(offset,&vlanDataBuf[0],2) == 0 )
		return 0;
	if ( m_EtherType->Decode(offset+2,&vlanDataBuf[2],2) == 0 )
		return 0;

	return IEEE802_1Q_VLAN_TAG_SIZE;
}

///
/// @brief
///     根据设置的参数进行编码，获得VLAN标签及PDU部分的数据流
///
/// @param[in]
///     pduDataBuf  - 保存参与编码的PDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
/// @param[in]
///     vlanDataArr - 保存VLAN标签及PDU部分的数据流的数组
/// @param[in]
///     arrSize     - 数组大小
///
/// @return
///     保存VLAN标签及PDU部分的数据流的数组的实际大小 - 成功； 0 - 失败
///
size_t Ieee8021QSeg::Code(uint8_t *pduDataBuf, size_t bufSize, uint8_t *vlanDataArr, const size_t arrSize)
{
	if ( arrSize < IEEE802_1Q_VLAN_TAG_SIZE+bufSize )
		return 0;

	MsgDataElem *pduTmp = CreateMsgDataElemImpl(DT_BASE_DATA);
	pduTmp->SetValue(pduDataBuf,bufSize);

	size_t cSize  = 0;
	size_t arrOff = 0;

#if ZY_PRJ
	cSize = m_TCI->Code(&vlanDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
#else
	/*if (CParamConfigBasic::getInstance()->GetHasVlan())
	{
		cSize = m_TCI->Code(&vlanDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			goto errorHandle;
	}*/
	cSize = m_TCI->Code(&vlanDataArr[arrOff],arrSize-arrOff);
	 
#endif

	arrOff += cSize;
	cSize = m_EtherType->Code(&vlanDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = pduTmp->Code(&vlanDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;

	ReleaseMsgDataElemImpl(pduTmp);
	return arrOff;
errorHandle:
	ReleaseMsgDataElemImpl(pduTmp);
	return 0;
}

///
/// @brief
///     获取IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     info       - 保存IEEE802.1Q VLAN标签信息的结构体
///
/// @return
///
///
void Ieee8021QSeg::Get8021QVlanInfo(const uint8_t *msgDatabuf, IEEE802_1Q_VLAN_INFO &info)
{
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;

	uint8Buf = (uint8_t *)&msgDatabuf[m_TCI->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_TCI->GetValueLen());
	uint16Tmp = BSWAP_16(uint16Tmp);
	info.tci.userPriority = uint16Tmp >> 13;
	info.tci.cfi = ( uint16Tmp << 3 ) >> 15;
	info.tci.vlanID = ( uint16Tmp << 4 ) >> 4;

	uint8Buf = (uint8_t *)&msgDatabuf[m_EtherType->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_EtherType->GetValueLen());
	info.etherType = BSWAP_16(uint16Tmp);
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
void Ieee8021QSeg::Get8021QVlanInfoOff(IEEE802_1Q_VLAN_INFO_OFFSET &infoOff)
{
	infoOff.tciOff = m_TCI->GetElemOffset();
	infoOff.etherTypeOff = m_EtherType->GetElemOffset();
}

///
/// @brief
///     设置IEEE802.1Q VLAN标签信息
///
/// @param[in]
///     info - 保存IEEE802.1Q VLAN标签信息的结构体
///
/// @return
///
///
void Ieee8021QSeg::Set8021QVlanInfo(const IEEE802_1Q_VLAN_INFO &info, bool isHasVlan)
{
#if !CHUN_HUA
	uint16_t uint16Tmp = 0;
	uint16Tmp  = ( info.tci.userPriority << 13 );
	uint16Tmp |= ( info.tci.cfi << 15 ) >> 3;
	uint16Tmp |= ( info.tci.vlanID << 4 ) >> 4;
	uint16Tmp = BSWAP_16(uint16Tmp);

	if (isHasVlan)
	{
		m_TCI->SetValue((uint8_t *)&uint16Tmp,2);
		uint16Tmp = BSWAP_16(info.etherType);
		m_EtherType->SetValue((uint8_t *)&uint16Tmp,2);
	}
	else
	{
		m_TCI->SetValue((uint8_t *)&uint16Tmp,0);
		uint16Tmp = BSWAP_16(info.etherType);
		m_EtherType->SetValue((uint8_t *)&uint16Tmp,2);
	}
	
#else
	uint16_t uint16Tmp = 0;
	uint16Tmp  = ( info.tci.userPriority << 13 );
	uint16Tmp |= ( info.tci.cfi << 15 ) >> 3;
	uint16Tmp |= ( info.tci.vlanID << 4 ) >> 4;
	uint16Tmp = BSWAP_16(uint16Tmp);
	m_TCI->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(info.etherType);
	m_EtherType->SetValue((uint8_t *)&uint16Tmp,2);

#endif
}

