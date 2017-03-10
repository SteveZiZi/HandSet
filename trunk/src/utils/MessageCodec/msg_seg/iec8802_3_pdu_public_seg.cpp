/// @file
///     iec8802_3_pdu_public_seg.cpp
///
/// @brief
///     ����ISO/IEC8802-3�ı���֡��PDU�еĹ�����Ϣ��APDU����Ĳ��֣���
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
#include "iec8802_3_pdu_public_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     ����ISO/IEC8802-3�ı���֡��PDU�еĹ�����Ϣ���ֵĳ���
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
///     �Է���ISO/IEC8802-3�ı���֡��PDU�еĹ�����Ϣ���ֽ��н���
///
/// @param[in]
///     offset      - �������PDU�еĹ�����Ϣ�����ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     pduPblcBuf  - ����������PDU�еĹ�����Ϣ�������ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
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
///     �������õĲ������б��룬��÷���ISO/IEC8802-3�ı���֡��PDU
///
/// @param[in]
///     apduDataBuf - �����������APDU���ݵĻ���
/// @param[in]
///     bufSize     - �����С
/// @param[in]
///     pduDataArr  - �������ISO/IEC8802-3�ı���֡��PDU���ݵ�����
/// @param[in]
///     arrSize     - �����С
///
/// @return
///     �������ISO/IEC8802-3�ı���֡��PDU���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t Iec88023PduPublicSeg::Code(uint8_t *apduDataBuf, size_t bufSize, uint8_t *pduDataArr, const size_t arrSize)
{
	if ( arrSize < IEC8802_3_PDU_PUBLIC_SIZE+bufSize )
		return 0;

	// ����PDU�д�APPID�ʼ��APDU��β�����ݳ���
	uint16_t pduLen = (uint16_t)bufSize + IEC8802_3_PDU_PUBLIC_SIZE;
	pduLen = BSWAP_16(pduLen);
	m_Len->SetValue((uint8_t *)&pduLen,2);
	MsgDataElem *apduTmp = CreateMsgDataElemImpl(DT_BASE_DATA);
	apduTmp->SetValue(apduDataBuf,bufSize);

	// ����PDU
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
///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     info       - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�Ľṹ��
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
///     ��ȡ����ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     infoOff - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
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
///     ���÷���ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ����PDU��APDU����Ĳ���
///
/// @param[in]
///     info - �������ISO/IEC8802-3�ı���֡��PDU�Ĺ�����Ϣ�Ľṹ��
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
