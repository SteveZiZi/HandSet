/// @file
///     msg_tlv_data_elem.cpp
///
/// @brief
///     符合ASN.1的BER编码规则的TLV数据元素的编解码接口实现
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
#include "msg_tlv_data_elem.h"

MsgTlvDataElem::MsgTlvDataElem()
: m_Tag(TAG_NONE)
, m_ElemLen(0)
, m_ElemOffset(0)
, m_ValueLen(0)
, m_ValueOffset(0)
, m_Value(NULL)
{

}

MsgTlvDataElem::~MsgTlvDataElem()
{
	if ( m_Value != NULL )
	{
		delete[] m_Value;
		m_Value = NULL;
	}
}

///
/// @brief
///     以ASN.1的BER编码规则解码一个TLV数据
///
/// @param[in]
///     offset  - 该数据元素在帧中的偏移地址
/// @param[in]
///     inData  - 待解码的数据
/// @param[in]
///     datSize - 待解码数据的长度
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t MsgTlvDataElem::Decode(const size_t offset, const uint8_t *inData, const size_t datSize)
{
	uint32_t lenTmp = 0;
	size_t   nxtIdx = 1;

	if ( m_Tag == TAG_NONE )
	{// Tag值未确定则直接赋值
		m_Tag = inData[0];
	}
	else
	{// Tag值确定则需保证值相同
		if ( m_Tag != inData[0] )
		{
			m_ValueOffset = 0;
			m_ValueLen = 0;
			m_ElemOffset = 0;
			m_ElemLen = 0;
			return 0;
		}
	}

	// 解析TLV数据中的Value部分的长度及其相对偏移地址
	if ( ( inData[1] & 0x80 ) == 0 )
	{
		lenTmp = inData[1];
		nxtIdx = 2;
	}
	else
	{
		int lByte = inData[1] & 0x7F;
		if ( lByte > 4)
			return 0;
		for ( int i = 0; i < lByte; i++ )
		{
			lenTmp |= ( ((uint32_t)inData[1+lByte-i]) << (i*8) );
		}
		nxtIdx = 2+lByte;
	}

	// 判断Value部分是否超过待解码数据的空间，是则返回0
	if ( nxtIdx + lenTmp > datSize )
	{
		m_ValueOffset = 0;
		m_ValueLen = 0;
		m_ElemOffset = 0;
		m_ElemLen = 0;
		return 0;
	}

// 	// 判断当前元素空间是否能容纳该Value值
// 	if ( m_Len < lenTmp && m_Value != NULL )
// 	{
// 		delete[] m_Value;
// 		m_Value = NULL;
// 		m_Len = 0;
// 	}
// 	if ( m_Value == NULL )
// 		m_Value = new uint8_t[lenTmp];

	// 保存解码结果
//	memcpy(m_Value,&inData[nxtIdx],lenTmp);
	m_ValueLen    = lenTmp;
	m_ValueOffset = offset+nxtIdx;
	m_ElemLen     = m_ValueLen+nxtIdx;
	m_ElemOffset  = offset;

	return m_ElemLen;
}

///
/// @brief
///     根据设置的信息以ASN.1的BER编码规则进行编码，获得一个TLV数据
///
/// @param[in]
///     outData - 保存数据元素的数组
/// @param[in]
///     datSize - 数组的元素数目
///
/// @return
///     数据元素的实际大小 - 成功； 0 - 失败
///
size_t MsgTlvDataElem::Code(uint8_t *outData, const size_t datSize)
{
	// 标签值未定义或无Value数据，则返回0
	if ( m_Tag == TAG_NONE || m_Value == NULL || m_ValueLen == 0 )
		return 0;

	// 填充Tag值
	outData[0] = m_Tag;

	// 判断长度值（L）所采用的格式（长/短格式），并填充长度值
	size_t nxtIdx = 1;
	if ( m_ValueLen < 0x80 )
	{
		outData[1] = (uint8_t)m_ValueLen;
		nxtIdx = 2;
	}
	else
	{
		int lByte = GetLenBytes();
		outData[1] = 0x80 + lByte;
		for ( int i = 0; i < lByte; i++ )
		{
			outData[1+lByte-i] = (uint8_t) ( ( m_ValueLen << ( (3-i)*8 ) ) >> 3*8 ); 
		}
		nxtIdx = 2+lByte;
	}

	// 填充Value值
	m_ElemLen = m_ValueLen + nxtIdx;
	if ( m_ElemLen > datSize )
		return 0;
	memcpy(&outData[nxtIdx],m_Value,m_ValueLen);
	return m_ElemLen;
}

///
/// @brief
///     获取该TLV数据元素的标签值
///
/// @return
///     数据元素的标签值
///
uint8_t MsgTlvDataElem::GetTag() const
{
	return m_Tag;
}

///
/// @brief
///     获取该TLV数据元素在帧中的偏移地址
///
/// @return
///     TLV数据在帧中的偏移地址
///
size_t MsgTlvDataElem::GetElemOffset() const
{
	return m_ElemOffset;
}

///
/// @brief
///     获取该TLV数据元素的Value部分的长度
///
/// @return
///     TLV数据元素的Value部分的长度
///
size_t MsgTlvDataElem::GetElemLen() const
{
	return m_ElemLen;
}

///
/// @brief
///     获取该数据元素的数据值部分的长度
///
/// @return
///     数据元素的数据值部分的长度
///
size_t MsgTlvDataElem::GetValueLen() const
{
	return m_ValueLen;
}

///
/// @brief
///     获取该数据元素的数据值部分在帧中的偏移地址
///
/// @return
///     数据元素的数据值部分在帧中的偏移地址
///
size_t MsgTlvDataElem::GetValueOffset() const
{
	return m_ValueOffset;
}

///
/// @brief
///     获取该数据元素的值信息，若为TLV数据，则为指向其Value数据
///
/// @return
///     指向元素的值数据的指针;失败则返回NULL
///
const uint8_t *MsgTlvDataElem::GetValue() const
{
	if ( m_Value == NULL || m_ValueLen == 0 )
		return NULL;
	return m_Value;
}

///
/// @brief
///     设置该TLV数据元素的标签值
///
/// @param[in]
///     tag - 数据元素的标签值
///
/// @return
///     
///
void MsgTlvDataElem::SetTag(const uint8_t tag)
{
	m_Tag = tag;
}

///
/// @brief
///     设置该TLV数据元素的Value值
///
/// @param[in]
///     inData  - TLV数据元素的Value值
/// @param[in]
///     datSize - Value值的长度
///
/// @return
///     
///
void MsgTlvDataElem::SetValue(const uint8_t *inData, const size_t datSize)
{
	if ( inData == NULL || datSize == 0 )
	{
		m_ValueLen = 0;
		return;
	}

	if ( m_ValueLen < datSize && m_Value != NULL )
	{
		delete[] m_Value;
		m_Value = NULL;
	}

	if ( m_Value == NULL )
		m_Value = new uint8_t[datSize];
	memcpy(m_Value,inData,datSize);
	m_ValueLen = datSize;
}

///
/// @brief
///     获取长度值需占用的字节数
///
/// @note
///     该函数用于长度值不超过4Byte的情况
///
/// @return
///     长度值需占用的字节数
///
int MsgTlvDataElem::GetLenBytes() const
{
	if ( ( m_ValueLen & 0xFF000000 ) != 0 )
		return 4;
	if ( ( m_ValueLen & 0x00FF0000 ) != 0 )
		return 3;
	if ( ( m_ValueLen & 0x0000FF00 ) != 0 )
		return 2;
	if ( ( m_ValueLen & 0x000000FF ) != 0 )
		return 1;
	return 0;
}
