/// @file
///     msg_tlv_data_elem.cpp
///
/// @brief
///     ����ASN.1��BER��������TLV����Ԫ�صı����ӿ�ʵ��
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
///     ��ASN.1��BER����������һ��TLV����
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
size_t MsgTlvDataElem::Decode(const size_t offset, const uint8_t *inData, const size_t datSize)
{
	uint32_t lenTmp = 0;
	size_t   nxtIdx = 1;

	if ( m_Tag == TAG_NONE )
	{// Tagֵδȷ����ֱ�Ӹ�ֵ
		m_Tag = inData[0];
	}
	else
	{// Tagֵȷ�����豣ֵ֤��ͬ
		if ( m_Tag != inData[0] )
		{
			m_ValueOffset = 0;
			m_ValueLen = 0;
			m_ElemOffset = 0;
			m_ElemLen = 0;
			return 0;
		}
	}

	// ����TLV�����е�Value���ֵĳ��ȼ������ƫ�Ƶ�ַ
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

	// �ж�Value�����Ƿ񳬹����������ݵĿռ䣬���򷵻�0
	if ( nxtIdx + lenTmp > datSize )
	{
		m_ValueOffset = 0;
		m_ValueLen = 0;
		m_ElemOffset = 0;
		m_ElemLen = 0;
		return 0;
	}

// 	// �жϵ�ǰԪ�ؿռ��Ƿ������ɸ�Valueֵ
// 	if ( m_Len < lenTmp && m_Value != NULL )
// 	{
// 		delete[] m_Value;
// 		m_Value = NULL;
// 		m_Len = 0;
// 	}
// 	if ( m_Value == NULL )
// 		m_Value = new uint8_t[lenTmp];

	// ���������
//	memcpy(m_Value,&inData[nxtIdx],lenTmp);
	m_ValueLen    = lenTmp;
	m_ValueOffset = offset+nxtIdx;
	m_ElemLen     = m_ValueLen+nxtIdx;
	m_ElemOffset  = offset;

	return m_ElemLen;
}

///
/// @brief
///     �������õ���Ϣ��ASN.1��BER���������б��룬���һ��TLV����
///
/// @param[in]
///     outData - ��������Ԫ�ص�����
/// @param[in]
///     datSize - �����Ԫ����Ŀ
///
/// @return
///     ����Ԫ�ص�ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t MsgTlvDataElem::Code(uint8_t *outData, const size_t datSize)
{
	// ��ǩֵδ�������Value���ݣ��򷵻�0
	if ( m_Tag == TAG_NONE || m_Value == NULL || m_ValueLen == 0 )
		return 0;

	// ���Tagֵ
	outData[0] = m_Tag;

	// �жϳ���ֵ��L�������õĸ�ʽ����/�̸�ʽ��������䳤��ֵ
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

	// ���Valueֵ
	m_ElemLen = m_ValueLen + nxtIdx;
	if ( m_ElemLen > datSize )
		return 0;
	memcpy(&outData[nxtIdx],m_Value,m_ValueLen);
	return m_ElemLen;
}

///
/// @brief
///     ��ȡ��TLV����Ԫ�صı�ǩֵ
///
/// @return
///     ����Ԫ�صı�ǩֵ
///
uint8_t MsgTlvDataElem::GetTag() const
{
	return m_Tag;
}

///
/// @brief
///     ��ȡ��TLV����Ԫ����֡�е�ƫ�Ƶ�ַ
///
/// @return
///     TLV������֡�е�ƫ�Ƶ�ַ
///
size_t MsgTlvDataElem::GetElemOffset() const
{
	return m_ElemOffset;
}

///
/// @brief
///     ��ȡ��TLV����Ԫ�ص�Value���ֵĳ���
///
/// @return
///     TLV����Ԫ�ص�Value���ֵĳ���
///
size_t MsgTlvDataElem::GetElemLen() const
{
	return m_ElemLen;
}

///
/// @brief
///     ��ȡ������Ԫ�ص�����ֵ���ֵĳ���
///
/// @return
///     ����Ԫ�ص�����ֵ���ֵĳ���
///
size_t MsgTlvDataElem::GetValueLen() const
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
size_t MsgTlvDataElem::GetValueOffset() const
{
	return m_ValueOffset;
}

///
/// @brief
///     ��ȡ������Ԫ�ص�ֵ��Ϣ����ΪTLV���ݣ���Ϊָ����Value����
///
/// @return
///     ָ��Ԫ�ص�ֵ���ݵ�ָ��;ʧ���򷵻�NULL
///
const uint8_t *MsgTlvDataElem::GetValue() const
{
	if ( m_Value == NULL || m_ValueLen == 0 )
		return NULL;
	return m_Value;
}

///
/// @brief
///     ���ø�TLV����Ԫ�صı�ǩֵ
///
/// @param[in]
///     tag - ����Ԫ�صı�ǩֵ
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
///     ���ø�TLV����Ԫ�ص�Valueֵ
///
/// @param[in]
///     inData  - TLV����Ԫ�ص�Valueֵ
/// @param[in]
///     datSize - Valueֵ�ĳ���
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
///     ��ȡ����ֵ��ռ�õ��ֽ���
///
/// @note
///     �ú������ڳ���ֵ������4Byte�����
///
/// @return
///     ����ֵ��ռ�õ��ֽ���
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
