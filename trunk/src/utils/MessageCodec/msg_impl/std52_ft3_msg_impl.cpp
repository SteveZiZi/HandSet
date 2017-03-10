/// @file
///     std52_ft3_msg_impl.cpp
///
/// @brief
///     ��׼FT3����ֵ����֡��52�ֽڰ汾,��CRC���ı����ӿڵ�ʵ��
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.05
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.09.06     |1.0.1.906  |���汨����Ϣʱ��ӶԱ��ĳ��ȵ��жϣ���ֹ���ȹ���������쳣 |
///
#include "stdafx.h"
#include "std52_ft3_msg_impl.h"
#include "../msg_seg/std52_ft3_asdu_seg.h"
#include "../msg_seg/bswap.h"

Std52FT3MsgImpl::Std52FT3MsgImpl()
: m_AsduSeg(new Std52FT3AsduSeg)
, m_bHaveDecode(false)
{
	memset(m_MsgDataBuf,0,STD52_FT3_MSG_WORD);
}

Std52FT3MsgImpl::~Std52FT3MsgImpl()
{
	if ( m_AsduSeg != NULL )
	{
		delete m_AsduSeg;
		m_AsduSeg = NULL;
	}
}

///
/// @brief
///     �Բ���ı�׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC��֡���н���
///
/// @param[in]
///     msgDataBuf - ���沶��ı���֡���ݵĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t Std52FT3MsgImpl::Decode(const uint16_t *msgDataBuf, const size_t bufSize)
{
	if ( msgDataBuf[0] != 0x0564 )
		return 0;

	// �Ա���֡���ݽ��д�С��ת��������
	size_t msgLen = bufSize;
	if ( msgLen > STD52_FT3_MSG_WORD )
		msgLen = STD52_FT3_MSG_WORD;
	for ( size_t i = 0; i < msgLen; i++ )
		m_MsgDataBuf[i] = BSWAP_16(msgDataBuf[i]);

	// ��������FT3���ĸ�ʽ�̶�����������һ�εĽ������
	if ( m_bHaveDecode )
		return msgLen;

	m_bHaveDecode = false;

	// ����ASDU
	size_t dSize = m_AsduSeg->Decode(2,&m_MsgDataBuf[1],msgLen-1);
	if ( dSize == 0 )
		return 0;
	m_bHaveDecode = true;
	return (dSize+1);
}

///
/// @brief
///     �������õĲ������б��룬��ñ�׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC��֡��ASDU��������CRC����������
///
/// @param[in]
///     msgDataArr - ���汨��֡���ݵ�����
/// @param[in]
///     arrSize    - �����С
///
/// @return
///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Std52FT3MsgImpl::Code(uint16_t *msgDataArr, const size_t arrSize)
{
	m_bHaveDecode = false;
	return m_AsduSeg->Code(msgDataArr,arrSize);
}

///
/// @brief
///     ��ȡASDU�ĳ���
///
/// @return
///     ASDU�ĳ���
///
size_t Std52FT3MsgImpl::GetASDULen()
{
	return m_AsduSeg->GetASDULen();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ����ASDU�еĲ���ֵ��������Ĳ���
///
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ�Ľṹ��
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool Std52FT3MsgImpl::GetASDUPublicInfo(STD52_FT3_ASDU_PUBLIC &asduPublic)
{
	return m_AsduSeg->GetASDUPublicInfo(m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
/// @return
///     ASDU�еĲ���ֵ���ݲ��ֵ�ͨ����
///
size_t Std52FT3MsgImpl::GetASDUSmpsNum()
{
	return m_AsduSeg->GetASDUSmpsNum();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ���ֵ����
///
/// @param[in]
///     smpValArr  - �������˲ʱֵ������
/// @param[in]
///     arrSize    - �����С,��ֵ��>=ͨ����
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Std52FT3MsgImpl::GetASDUSmps(uint16_t *smpValArr, const size_t arrSize)
{
	return m_AsduSeg->GetASDUSmps(m_MsgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     ��ȡASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     ASDU�ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t Std52FT3MsgImpl::GetASDUOff()
{
	return m_AsduSeg->GetASDUOff();
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     asduPublic - ����ASDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
///
/// @return
///     
///
void Std52FT3MsgImpl::GetASDUPublicInfoOff(STD52_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	m_AsduSeg->GetASDUPublicInfoOff(asduPublicOff);
}

///
/// @brief
///     ��ȡ����֡�е�ASDU�еĲ���ֵ�����ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     smpOffArr - �������˲ʱֵ��ƫ�Ƶ�ַ��Ϣ������
/// @param[in]
///     arrSize   - ����Ĵ�С,��ֵ��>=ͨ����
///
/// @return
///     �����ʵ�ʴ�С
///
size_t Std52FT3MsgImpl::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	return m_AsduSeg->GetASDUSmpsOff(smpOffArr,arrSize);
}

///
/// @brief
///     ���ñ���֡�е�ASDU��Ϣ
///
/// @param[in]
///     asduPublic - ASDU�Ĺ�����Ϣ
/// @param[in]
///     smpValBuf  - �������˲ʱֵ�Ļ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     
///
void Std52FT3MsgImpl::SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize)
{
	m_AsduSeg->SetASDU(asduPublic,smpValBuf,bufSize);
}
