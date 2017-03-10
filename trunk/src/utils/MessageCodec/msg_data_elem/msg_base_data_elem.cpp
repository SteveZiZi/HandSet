/// @file
///     msg_base_data_elem.cpp
///
/// @brief
///     一般类型数据元素的编解码接口实现，该类型元素其内存值即数据值
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
#include "msg_base_data_elem.h"

MsgBaseDataElem::MsgBaseDataElem()
: m_Tag(TAG_NONE)
, m_ElemLen(0)
, m_ElemOffset(0)
, m_ValueLen(0)
, m_ValueOffset(0)
, m_Value(NULL)
{

}

MsgBaseDataElem::~MsgBaseDataElem()
{
	if ( m_Value != NULL )
	{
		delete[] m_Value;
		m_Value = NULL;
	}
}

///
/// @brief
///     解码一个一般数据，即直接拷贝相应Size的待解码数据
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
size_t MsgBaseDataElem::Decode(const size_t offset, const uint8_t *inData, const size_t datSize)
{
// 	// 判断当前元素空间能否容纳解码数据
// 	if ( m_Len < datSize && m_Value != NULL )
// 	{
// 		delete[] m_Value;
// 		m_Value = NULL;
// 	}

	// 保存解码结果
	m_ValueLen    = m_ElemLen    = datSize;
	m_ValueOffset = m_ElemOffset = offset;
// 	if ( m_Value == NULL )
// 		m_Value = new uint8_t[m_Len];
// 	memcpy(m_Value,inData,m_Len);

	return m_ElemLen;
}

///
/// @brief
///     根据设置的信息进行编码，即直接拷贝该数据元素值信息
///
/// @param[in]
///     outData - 保存数据元素的数组
/// @param[in]
///     datSize - 数组的元素数目
///
/// @return
///     数据元素的实际大小 - 成功； 0 - 失败
///
size_t MsgBaseDataElem::Code(uint8_t *outData, const size_t datSize)
{
	if ( m_Value == NULL || m_ValueLen == 0 )
		return 0;
	if ( datSize < m_ValueLen)
		return 0;

	memcpy(outData,m_Value,m_ValueLen);
	m_ElemLen = m_ValueLen;
	return m_ElemLen;
}

///
/// @brief
///     获取该数据元素的标签值
///
/// @return
///     数据元素的标签值
///
uint8_t MsgBaseDataElem::GetTag() const
{
	return m_Tag;
}

///
/// @brief
///     获取该数据元素的长度
///
/// @return
///     数据元素的长度
///
size_t MsgBaseDataElem::GetElemLen() const
{
	return m_ElemLen;
}

///
/// @brief
///     获取该数据元素在帧中的偏移地址
///
/// @return
///     数据元素在帧中的偏移地址
///
size_t MsgBaseDataElem::GetElemOffset() const
{
	return m_ElemOffset;
}

///
/// @brief
///     获取该数据元素的数据值部分的长度
///
/// @return
///     数据元素的数据值部分的长度
///
size_t MsgBaseDataElem::GetValueLen() const
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
size_t MsgBaseDataElem::GetValueOffset() const
{
	return m_ValueOffset;
}

///
/// @brief
///     获取该数据元素的值信息
///
/// @param[in]
///     outData - 保存数据元素值信息的数组
/// @param[in]
///     datSize - 数组的元素数目
///
/// @return
///     数据信息的实际大小 - 成功； 0 - 失败
///
const uint8_t *MsgBaseDataElem::GetValue() const
{
	if ( m_Value == NULL || m_ValueLen == 0 )
		return NULL;
	return m_Value;
}

///
/// @brief
///     设置该数据元素的标签值
///
/// @param[in]
///     tag - 数据元素的标签值
///
/// @return
///     
///
void MsgBaseDataElem::SetTag(const uint8_t tag)
{
	return;
}

///
/// @brief
///     设置该数据元素的值信息
///
/// @param[in]
///     inData  - 数据元素的值信息
/// @param[in]
///     datSize - 数据元素的值信息的长度
///
/// @return
///     
///
void MsgBaseDataElem::SetValue(const uint8_t *inData, const size_t datSize)
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
