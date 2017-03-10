/// @file
///     msg_base_data_elem.cpp
///
/// @brief
///     һ����������Ԫ�صı����ӿ�ʵ�֣�������Ԫ�����ڴ�ֵ������ֵ
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
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
///     ����һ��һ�����ݣ���ֱ�ӿ�����ӦSize�Ĵ���������
///
/// @param[in]
///     offset  - ������Ԫ����֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     inData  - �����������
/// @param[in]
///     datSize - ���������ݵĳ���
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t MsgBaseDataElem::Decode(const size_t offset, const uint8_t *inData, const size_t datSize)
{
// 	// �жϵ�ǰԪ�ؿռ��ܷ����ɽ�������
// 	if ( m_Len < datSize && m_Value != NULL )
// 	{
// 		delete[] m_Value;
// 		m_Value = NULL;
// 	}

	// ���������
	m_ValueLen    = m_ElemLen    = datSize;
	m_ValueOffset = m_ElemOffset = offset;
// 	if ( m_Value == NULL )
// 		m_Value = new uint8_t[m_Len];
// 	memcpy(m_Value,inData,m_Len);

	return m_ElemLen;
}

///
/// @brief
///     �������õ���Ϣ���б��룬��ֱ�ӿ���������Ԫ��ֵ��Ϣ
///
/// @param[in]
///     outData - ��������Ԫ�ص�����
/// @param[in]
///     datSize - �����Ԫ����Ŀ
///
/// @return
///     ����Ԫ�ص�ʵ�ʴ�С - �ɹ��� 0 - ʧ��
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
///     ��ȡ������Ԫ�صı�ǩֵ
///
/// @return
///     ����Ԫ�صı�ǩֵ
///
uint8_t MsgBaseDataElem::GetTag() const
{
	return m_Tag;
}

///
/// @brief
///     ��ȡ������Ԫ�صĳ���
///
/// @return
///     ����Ԫ�صĳ���
///
size_t MsgBaseDataElem::GetElemLen() const
{
	return m_ElemLen;
}

///
/// @brief
///     ��ȡ������Ԫ����֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ����Ԫ����֡�е�ƫ�Ƶ�ַ
///
size_t MsgBaseDataElem::GetElemOffset() const
{
	return m_ElemOffset;
}

///
/// @brief
///     ��ȡ������Ԫ�ص�����ֵ���ֵĳ���
///
/// @return
///     ����Ԫ�ص�����ֵ���ֵĳ���
///
size_t MsgBaseDataElem::GetValueLen() const
{
	return m_ValueLen;
}

///
/// @brief
///     ��ȡ������Ԫ�ص�����ֵ������֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ����Ԫ�ص�����ֵ������֡�е�ƫ�Ƶ�ַ
///
size_t MsgBaseDataElem::GetValueOffset() const
{
	return m_ValueOffset;
}

///
/// @brief
///     ��ȡ������Ԫ�ص�ֵ��Ϣ
///
/// @param[in]
///     outData - ��������Ԫ��ֵ��Ϣ������
/// @param[in]
///     datSize - �����Ԫ����Ŀ
///
/// @return
///     ������Ϣ��ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
const uint8_t *MsgBaseDataElem::GetValue() const
{
	if ( m_Value == NULL || m_ValueLen == 0 )
		return NULL;
	return m_Value;
}

///
/// @brief
///     ���ø�����Ԫ�صı�ǩֵ
///
/// @param[in]
///     tag - ����Ԫ�صı�ǩֵ
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
///     ���ø�����Ԫ�ص�ֵ��Ϣ
///
/// @param[in]
///     inData  - ����Ԫ�ص�ֵ��Ϣ
/// @param[in]
///     datSize - ����Ԫ�ص�ֵ��Ϣ�ĳ���
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
