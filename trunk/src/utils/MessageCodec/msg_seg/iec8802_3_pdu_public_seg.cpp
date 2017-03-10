/// @file
///     iec8802_3_pdu_public_seg.cpp
///
/// @brief
///     符合ISO/IEC8802-3的报文帧的PDU中的公共信息（APDU以外的部分）段
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
#include "iec8802_3_pdu_public_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     符合ISO/IEC8802-3的报文帧的PDU中的公共信息部分的长度
///
#define IEC8802_3_PDU_PUBLIC_SIZE 8

Iec88023PduPublicSeg::Iec88023PduPublicSeg()
: m_AppID(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_Len(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_Reserved1(CreateMsgDataElemImpl(DT_BASE_DATA))
, m_Reserved2(CreateMsgDataElemImpl(DT_BASE_DATA))
{

}

Iec88023PduPublicSeg::~Iec88023PduPublicSeg()
{
	ReleaseMsgDataElemImpl(m_AppID);
	ReleaseMsgDataElemImpl(m_Len);
	ReleaseMsgDataElemImpl(m_Reserved1);
	ReleaseMsgDataElemImpl(m_Reserved2);
}

///
/// @brief
///     对符合ISO/IEC8802-3的报文帧的PDU中的公共信息部分进行解码
///
/// @param[in]
///     offset      - 待解码的PDU中的公共信息部分在报文帧中的偏移地址
/// @param[in]
///     pduPblcBuf  - 保存待解码的PDU中的公共信息部分数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Iec88023PduPublicSeg::Decode(const size_t offset, const uint8_t *pduPblcBuf, const size_t bufSize)
{
	if ( bufSize < IEC8802_3_PDU_PUBLIC_SIZE )
		return 0;
	if ( m_AppID->Decode(offset,&pduPblcBuf[0],2) == 0 )
		return 0;
	if ( m_Len->Decode(offset+2,&pduPblcBuf[2],2) == 0 )
		return 0;
	if ( m_Reserved1->Decode(offset+4,&pduPblcBuf[4],2) == 0 )
		return 0;
	if ( m_Reserved2->Decode(offset+6,&pduPblcBuf[6],2) == 0 )
		return 0;

	return IEC8802_3_PDU_PUBLIC_SIZE;
}

///
/// @brief
///     根据设置的参数进行编码，获得符合ISO/IEC8802-3的报文帧的PDU
///
/// @param[in]
///     apduDataBuf - 保存参与编码的APDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
/// @param[in]
///     pduDataArr  - 保存符合ISO/IEC8802-3的报文帧的PDU数据的数组
/// @param[in]
///     arrSize     - 数组大小
///
/// @return
///     保存符合ISO/IEC8802-3的报文帧的PDU数据的数组的实际大小 - 成功； 0 - 失败
///
size_t Iec88023PduPublicSeg::Code(uint8_t *apduDataBuf, size_t bufSize, uint8_t *pduDataArr, const size_t arrSize)
{
	if ( arrSize < IEC8802_3_PDU_PUBLIC_SIZE+bufSize )
		return 0;

	// 设置PDU中从APPID项开始至APDU结尾的数据长度
	uint16_t pduLen = (uint16_t)bufSize + IEC8802_3_PDU_PUBLIC_SIZE;
	pduLen = BSWAP_16(pduLen);
	m_Len->SetValue((uint8_t *)&pduLen,2);
	MsgDataElem *apduTmp = CreateMsgDataElemImpl(DT_BASE_DATA);
	apduTmp->SetValue(apduDataBuf,bufSize);

	// 编码PDU
	size_t cSize  = 0;
	size_t arrOff = 0;
	cSize  = m_AppID->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = m_Len->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = m_Reserved1->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = m_Reserved2->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;
	cSize = apduTmp->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		goto errorHandle;
	arrOff += cSize;

	ReleaseMsgDataElemImpl(apduTmp);
	return arrOff;
errorHandle:
	ReleaseMsgDataElemImpl(apduTmp);
	return 0;
}

///
/// @brief
///     获取符合ISO/IEC8802-3的报文帧的PDU的公共信息，即PDU中APDU以外的部分
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     info       - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的结构体
///
/// @return
///
///
void Iec88023PduPublicSeg::GetIEC8802Info(const uint8_t *msgDataBuf, IEC8802_3_PDU_PUBLIC &info)
{
	uint16_t uint16Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8Buf = (uint8_t *)&msgDataBuf[m_AppID->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_AppID->GetValueLen());
	info.appID = BSWAP_16(uint16Tmp);

	uint8Buf = (uint8_t *)&msgDataBuf[m_Len->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_Len->GetValueLen());
	info.len = BSWAP_16(uint16Tmp);

	uint8Buf = (uint8_t *)&msgDataBuf[m_Reserved1->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_Reserved1->GetValueLen());
	info.reserved1 = BSWAP_16(uint16Tmp);

	uint8Buf = (uint8_t *)&msgDataBuf[m_Reserved2->GetValueOffset()];
	memcpy((uint8_t *)&uint16Tmp,uint8Buf,m_Reserved2->GetValueLen());
	info.reserved2 = BSWAP_16(uint16Tmp);
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
void Iec88023PduPublicSeg::GetIEC8802InfoOff(IEC8802_3_PDU_PUBLIC_OFFSET &infoOff)
{
	infoOff.appIDOff     = m_AppID->GetElemOffset();
	infoOff.lenOff       = m_Len->GetElemOffset();
	infoOff.reserved1Off = m_Reserved1->GetElemOffset();
	infoOff.reserved2Off = m_Reserved2->GetElemOffset();
}

///
/// @brief
///     设置符合ISO/IEC8802-3的报文帧的PDU的公共信息，即PDU中APDU以外的部分
///
/// @param[in]
///     info - 保存符合ISO/IEC8802-3的报文帧的PDU的公共信息的结构体
///
/// @return
///
///
void Iec88023PduPublicSeg::SetIEC8802Info(const IEC8802_3_PDU_PUBLIC &info)
{
	uint16_t uint16Tmp = 0;
	uint16Tmp = BSWAP_16(info.appID);
	m_AppID->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(info.len);
	m_Len->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(info.reserved1);
	m_Reserved1->SetValue((uint8_t *)&uint16Tmp,2);
	uint16Tmp = BSWAP_16(info.reserved2);
	m_Reserved2->SetValue((uint8_t *)&uint16Tmp,2);
}
