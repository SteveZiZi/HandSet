/// @file
///     ethernetII_seg.cpp
///
/// @brief
///     以太网报文帧的头信息段
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
#include "ethernetII_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"
#include "..\service\datalayer\config\ParamConfigBasic.h"
#define ETHERNET_II_HEADER_SIZE ETHER_MAC_LEN*2+2

EthernetIISeg::EthernetIISeg()
: m_DesMac(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_SrcMac(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_EtherType(CreateMsgDataElemImpl(DT_BASE_DATA))
{

}

EthernetIISeg::~EthernetIISeg()
{
	ReleaseMsgDataElemImpl(m_DesMac);
	ReleaseMsgDataElemImpl(m_SrcMac);
	ReleaseMsgDataElemImpl(m_EtherType);
}

///
/// @brief
///     对以太网报文帧头进行解码
///
/// @param[in]
///     offset  - 待解码数据在报文帧中的偏移地址
/// @param[in]
///     msgBuf  - 保存待解码数据的缓存
/// @param[in]
///     bufSize - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t EthernetIISeg::Decode(const size_t offset, const uint8_t *msgBuf, const size_t bufSize)
{
	if ( bufSize < ETHERNET_II_HEADER_SIZE )
		return 0;
	size_t decNum = m_DesMac->Decode(offset,&msgBuf[0],ETHER_MAC_LEN);
	if ( decNum == 0 )
		return 0;
	decNum = m_SrcMac->Decode(offset+decNum,&msgBuf[ETHER_MAC_LEN],ETHER_MAC_LEN);
	if ( decNum == 0)
		return 0;
	decNum = m_EtherType->Decode(offset+2*ETHER_MAC_LEN,&msgBuf[2*ETHER_MAC_LEN],2);
	if ( decNum == 0 )
		return 0;

	return (ETHER_MAC_LEN*2+2);
}

///
/// @brief
///     对设置的参数进行编码，获得完整的报文帧
///
/// @param[in]
///     userDataBuf - 保存参与编码的用户数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
/// @param[in]
///     msgArr      - 保存报文帧数据的数组
/// @param[in]
///     arrSize     - 数组的大小，一般取以太网帧的最大Size
///
/// @return
///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
///
size_t EthernetIISeg::Code(uint8_t *userDataBuf, size_t bufSize, uint8_t *msgArr, const size_t arrSize)
{
	if ( arrSize < ETHERNET_II_HEADER_SIZE+bufSize )
		return 0;

	MsgDataElem *userDataTmp = CreateMsgDataElemImpl(DT_BASE_DATA);
	userDataTmp->SetValue(userDataBuf,bufSize);

	size_t cSize  = 0;
	size_t arrOff = 0;
	cSize = m_DesMac->Code(&msgArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = m_SrcMac->Code(&msgArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;

#if ZY_PRJ
	cSize = m_EtherType->Code(&msgArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
#else
	/*if (CParamConfigBasic::getInstance()->GetHasVlan())
	{
		cSize = m_EtherType->Code(&msgArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			goto errorHandle;
		arrOff += cSize;
	}*/
	cSize = m_EtherType->Code(&msgArr[arrOff],arrSize-arrOff);
	arrOff += cSize;
#endif
	
	cSize = userDataTmp->Code(&msgArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;

	ReleaseMsgDataElemImpl(userDataTmp);
	return arrOff;
errorHandle:
	ReleaseMsgDataElemImpl(userDataTmp);
	return 0;
}

///
/// @brief
///     获取以太网报文帧头信息
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     ethHdr     - 保存以太网报文帧头信息的结构体
///
/// @return
///
///
void EthernetIISeg::GetEtherHdr(const uint8_t *msgDataBuf, ETHER_II_HEADER &ethHdr)
{
	memcpy(ethHdr.desMac,&msgDataBuf[m_DesMac->GetValueOffset()],m_DesMac->GetValueLen());
	memcpy(ethHdr.srcMac,&msgDataBuf[m_SrcMac->GetValueOffset()],m_SrcMac->GetValueLen());
	memcpy((uint8_t *)&ethHdr.etherType,&msgDataBuf[m_EtherType->GetValueOffset()],m_EtherType->GetValueLen());
	ethHdr.etherType = BSWAP_16(ethHdr.etherType);
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
void EthernetIISeg::GetEtherHdrOff(ETHER_II_HEADER_OFFSET &ethHdrOff)
{
	ethHdrOff.desMacOff    = m_DesMac->GetElemOffset();
	ethHdrOff.srcMacOff    = m_SrcMac->GetElemOffset();
	ethHdrOff.etherTypeOff = m_EtherType->GetElemOffset();
}

///
/// @brief
///     设置以太网报文帧头信息
///
/// @param[in]
///     ethHdr - 保存以太网报文帧头信息的结构体
///
/// @return
///
///
void EthernetIISeg::SetEthernetHeader(const ETHER_II_HEADER &ethHdr, bool isHasVlan)
{
	uint16_t etherType;
	m_DesMac->SetValue(ethHdr.desMac,ETHER_MAC_LEN);
	m_SrcMac->SetValue(ethHdr.srcMac,ETHER_MAC_LEN);
	etherType = BSWAP_16(ethHdr.etherType);
#if !CHUN_HUA
	if (isHasVlan)
	{
		m_EtherType->SetValue((uint8_t *)&etherType,2);
	}
	else
	{
		m_EtherType->SetValue((uint8_t *)&etherType,0);
	}
#else
	m_EtherType->SetValue((uint8_t *)&etherType,2);
#endif
	
}
