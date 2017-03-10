/// @file
///     ieee1588_header_seg.cpp
///
/// @brief
///     IEEE1588报文头段
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
#include "ieee1588_header_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     IEEE1588报文头的格式
///
typedef enum {
	IEEE1588_HEADER_TYPE = 0,
	IEEE1588_HEADER_VERSION,
	IEEE1588_HEADER_MESSAGE_LENGTH,
	IEEE1588_HEADER_DOMAIN_NUMBER,
	IEEE1588_HEADER_RESERVED1,
	IEEE1588_HEADER_FLAG_FIELD,
	IEEE1588_HEADER_CORRECTION_FIELD,
	IEEE1588_HEADER_RESERVED2,
	IEEE1588_HEADER_SOURCE_PORT_ID,
	IEEE1588_HEADER_SEQUENCE_ID,
	IEEE1588_HEADER_CONTROL_FIELD,
	IEEE1588_HEADER_LOG_MESSAGE_INTERVAL
}IEEE1588_HEADER_FORMAT;

///
/// @brief
///     对应上面的格式，IEEE1588报文头各字段占用的字节
///
static const uint8_t g_Ieee1588_Header_Size_List[] = {
	1,1,2,1,1,2,8,4,10,2,1,1
};

///
/// @brief
///     IEEE1588报文头的长度（字节）
///
#define IEEE1588_HEADER_LENGTH 34

Ieee1588HeaderSeg::Ieee1588HeaderSeg()
{
	m_vIeee1588Header.reserve(sizeof(g_Ieee1588_Header_Size_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Ieee1588_Header_Size_List)/sizeof(uint8_t); i++ )
	{
		m_vIeee1588Header.push_back(CreateMsgDataElemImpl(DT_BASE_DATA));
	}
}

Ieee1588HeaderSeg::~Ieee1588HeaderSeg()
{
	std::vector<MsgDataElem *>::iterator it;
	for ( it = m_vIeee1588Header.begin(); it != m_vIeee1588Header.end(); it++ )
	{
		ReleaseMsgDataElemImpl(*it);
	}
}

///
/// @brief
///     对报文帧中的ASDU部分进行解码
///
/// @param[in]
///     offset      - 待解码的ASDU在报文帧中的偏移地址
/// @param[in]
///     headerBuf   - 保存待解码的ASDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Ieee1588HeaderSeg::Decode(const size_t offset, const uint8_t *headerBuf, const size_t bufSize)
{
	if ( bufSize < IEEE1588_HEADER_LENGTH || headerBuf == NULL )
		return 0;

	size_t dSize  = 0;
	size_t offTmp = 0;

	// 根据定义好的格式依次解析各字段数据
	for ( int i=0; i < sizeof(g_Ieee1588_Header_Size_List)/sizeof(uint8_t); i++ )
	{
		if ( offTmp >= bufSize )
			break;

		dSize = m_vIeee1588Header.at(i)->Decode(offset+offTmp,&headerBuf[offTmp],g_Ieee1588_Header_Size_List[i]);
		if ( dSize == 0 )
			return 0;
		offTmp += dSize;
	}

	return offTmp;
}

///
/// @brief
///     对设置的参数进行编码，获得IEEE1588报文
///
/// @param[in]
///     bodyAndSuffixBuf - 保存参与编码的报文本体（body）和后缀（suffix）数据的缓存
/// @param[in]
///     bufSize          - 缓存大小
/// @param[in]
///     msgArr           - 保存报文帧数据的数组
/// @param[in]
///     arrSize          - 数组的大小，一般取以太网帧的最大Size
///
/// @return
///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
///
size_t Ieee1588HeaderSeg::Code(uint8_t *bodyAndSuffixBuf, size_t bufSize, uint8_t *msgArr, const size_t arrSize)
{
	if ( arrSize < IEEE1588_HEADER_LENGTH+bufSize || bodyAndSuffixBuf == NULL || msgArr == NULL )
		return 0;

	size_t  cSize  = 0;
	size_t  offTmp = 0;

	// 根据定义好的格式依次编码报文头各字段数据
	for ( int i=0; i < sizeof(g_Ieee1588_Header_Size_List)/sizeof(uint8_t); i++ )
	{
		cSize = m_vIeee1588Header.at(i)->Code(&msgArr[offTmp],arrSize-offTmp);
		offTmp += cSize;
	}

	// 连接报文本体（body）和后缀（suffix）
	MsgDataElem *bodyAndSuffixDataTmp = CreateMsgDataElemImpl(DT_BASE_DATA);
	bodyAndSuffixDataTmp->SetValue(bodyAndSuffixBuf,bufSize);
	cSize = bodyAndSuffixDataTmp->Code(&msgArr[offTmp],arrSize-offTmp);
	ReleaseMsgDataElemImpl(bodyAndSuffixDataTmp);

	if ( cSize == 0 )
		return 0;
	offTmp += cSize;
	return offTmp;
}

///
/// @brief
///     获取IEEE1588报文的报文类型
///
/// @param[in]
///     msgDataBuf     - 保存报文帧数据的缓存
///
/// @return
///     报文类型
///
IEEE1588_MESSAGE_TYPE Ieee1588HeaderSeg::GetIeee1588MsgType(const uint8_t *msgDataBuf)
{
	if ( msgDataBuf == NULL )
		return IEEE1588_TYPE_UNKNOWN;

	IEEE1588_TYPE_FIELD type;
	type.messageType = msgDataBuf[m_vIeee1588Header.at(IEEE1588_HEADER_TYPE)->GetValueOffset()];
	return type.GetMessageType();
}

///
/// @brief
///     获取IEEE1588报文头信息
///
/// @param[in]
///     msgDataBuf     - 保存报文帧数据的缓存
/// @param[in]
///     ieee1588Header - 保存报文头信息的结构体
///
/// @return
///
///
void Ieee1588HeaderSeg::GetIeee1588Header(const uint8_t *msgDataBuf, IEEE1588_HEADER &ieee1588Header)
{
	if ( msgDataBuf == NULL )
		return;

	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;
	uint8_t *uint8Buf = NULL;
	uint8_t *pMsgBuf = (uint8_t *)msgDataBuf;

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_TYPE)->GetValueOffset()];
	ieee1588Header.type.transportSpecific = (*uint8Buf) >> 4;
	ieee1588Header.type.messageType = (*uint8Buf);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_VERSION)->GetValueOffset()];
	ieee1588Header.version.reserved   = (*uint8Buf) >> 4;
	ieee1588Header.version.versionPTP = (*uint8Buf);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_MESSAGE_LENGTH)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,2);
	ieee1588Header.messageLength = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_DOMAIN_NUMBER)->GetValueOffset()];
	ieee1588Header.domainNumber = *uint8Buf;

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED1)->GetValueOffset()];
	ieee1588Header.reserved1 = *uint8Buf;

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_FLAG_FIELD)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,2);
	ieee1588Header.flags.iFlags = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_CORRECTION_FIELD)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint64Tmp,8,uint8Buf,8);
	ieee1588Header.correctionField = BSWAP_64(uint64Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED2)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint32Tmp,4,uint8Buf,4);
	ieee1588Header.reserved2 = BSWAP_32(uint32Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_SOURCE_PORT_ID)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint64Tmp,8,uint8Buf,8);
	ieee1588Header.sourcePortIdentity.clockIdentity = BSWAP_64(uint64Tmp);
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf+8,2);
	ieee1588Header.sourcePortIdentity.sourcePortID  = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_SEQUENCE_ID)->GetValueOffset()];
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,2);
	ieee1588Header.sequenceId = BSWAP_16(uint16Tmp);

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_CONTROL_FIELD)->GetValueOffset()];
	ieee1588Header.controlField = *uint8Buf;

	uint8Buf = &pMsgBuf[m_vIeee1588Header.at(IEEE1588_HEADER_LOG_MESSAGE_INTERVAL)->GetValueOffset()];
	ieee1588Header.logMessageInterval = *uint8Buf;
}

///
/// @brief
///     获取IEEE1588报文头信息在报文帧中的偏移地址信息
///
/// @param[in]
///     ieee1588HeaderOff - 保存报文头信息的偏移地址信息的结构体
///
/// @return
///
///
void Ieee1588HeaderSeg::GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HeaderOff)
{
	ieee1588HeaderOff.typeOff               = m_vIeee1588Header.at(IEEE1588_HEADER_TYPE)->GetElemOffset();
	ieee1588HeaderOff.versionOff            = m_vIeee1588Header.at(IEEE1588_HEADER_VERSION)->GetElemOffset();
	ieee1588HeaderOff.messageLengthOff      = m_vIeee1588Header.at(IEEE1588_HEADER_MESSAGE_LENGTH)->GetElemOffset();
	ieee1588HeaderOff.domainNumberOff       = m_vIeee1588Header.at(IEEE1588_HEADER_DOMAIN_NUMBER)->GetElemOffset();
	ieee1588HeaderOff.reserved1Off          = m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED1)->GetElemOffset();
	ieee1588HeaderOff.flagsOff              = m_vIeee1588Header.at(IEEE1588_HEADER_FLAG_FIELD)->GetElemOffset();
	ieee1588HeaderOff.correctionFieldOff    = m_vIeee1588Header.at(IEEE1588_HEADER_CORRECTION_FIELD)->GetElemOffset();
	ieee1588HeaderOff.reserved2Off          = m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED2)->GetElemOffset();
	ieee1588HeaderOff.sourcePortIdentityOff.clockIdentityOff = m_vIeee1588Header.at(IEEE1588_HEADER_SOURCE_PORT_ID)->GetElemOffset();
	ieee1588HeaderOff.sourcePortIdentityOff.sourcePortIDOff  = ieee1588HeaderOff.sourcePortIdentityOff.clockIdentityOff + 8;
	ieee1588HeaderOff.sequenceIdOff         = m_vIeee1588Header.at(IEEE1588_HEADER_SEQUENCE_ID)->GetElemOffset();
	ieee1588HeaderOff.controlFieldOff       = m_vIeee1588Header.at(IEEE1588_HEADER_CONTROL_FIELD)->GetElemOffset();
	ieee1588HeaderOff.logMessageIntervalOff = m_vIeee1588Header.at(IEEE1588_HEADER_LOG_MESSAGE_INTERVAL)->GetElemOffset();
}

///
/// @brief
///     设置IEEE1588报文头信息
///
/// @param[in]
///     ieee1588Header - 保存报文头信息的结构体
///
/// @return
///
///
void Ieee1588HeaderSeg::SetIeee1588Header(const IEEE1588_HEADER &ieee1588Header)
{
	uint8_t  uint8Tmp  = 0;
	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;

	uint8Tmp = (ieee1588Header.type.transportSpecific << 4) + ieee1588Header.type.messageType;
	m_vIeee1588Header.at(IEEE1588_HEADER_TYPE)->SetValue(&uint8Tmp,1);

	uint8Tmp = (ieee1588Header.version.reserved << 4) + ieee1588Header.version.versionPTP;
	m_vIeee1588Header.at(IEEE1588_HEADER_VERSION)->SetValue(&uint8Tmp,1);

	uint16Tmp = BSWAP_16(ieee1588Header.messageLength);
	m_vIeee1588Header.at(IEEE1588_HEADER_MESSAGE_LENGTH)->SetValue((uint8_t *)&uint16Tmp,2);

	uint8Tmp = ieee1588Header.domainNumber;
	m_vIeee1588Header.at(IEEE1588_HEADER_DOMAIN_NUMBER)->SetValue(&uint8Tmp,1);

	uint8Tmp = ieee1588Header.reserved1;
	m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED1)->SetValue(&uint8Tmp,1);

	uint16Tmp = BSWAP_16(ieee1588Header.flags);
	m_vIeee1588Header.at(IEEE1588_HEADER_FLAG_FIELD)->SetValue((uint8_t *)&uint16Tmp,2);

	uint64Tmp = BSWAP_64(ieee1588Header.correctionField);
	m_vIeee1588Header.at(IEEE1588_HEADER_CORRECTION_FIELD)->SetValue((uint8_t *)&uint64Tmp,8);

	uint32Tmp = BSWAP_32(ieee1588Header.reserved2);
	m_vIeee1588Header.at(IEEE1588_HEADER_RESERVED2)->SetValue((uint8_t *)&uint32Tmp,4);

	uint64Tmp = BSWAP_64(ieee1588Header.sourcePortIdentity.clockIdentity);
	uint16Tmp = BSWAP_16(ieee1588Header.sourcePortIdentity.sourcePortID);
	uint8_t srcPortID[10];
	memcpy_s(srcPortID,8,&uint64Tmp,8);
	memcpy_s(&srcPortID[8],2,&uint16Tmp,2);
	m_vIeee1588Header.at(IEEE1588_HEADER_SOURCE_PORT_ID)->SetValue(srcPortID,10);

	uint16Tmp = BSWAP_16(ieee1588Header.sequenceId);
	m_vIeee1588Header.at(IEEE1588_HEADER_SEQUENCE_ID)->SetValue((uint8_t *)&uint16Tmp,2);

	uint8Tmp = ieee1588Header.controlField;
	m_vIeee1588Header.at(IEEE1588_HEADER_CONTROL_FIELD)->SetValue(&uint8Tmp,1);

	uint8Tmp = ieee1588Header.logMessageInterval;
	m_vIeee1588Header.at(IEEE1588_HEADER_LOG_MESSAGE_INTERVAL)->SetValue(&uint8Tmp,1);
}
