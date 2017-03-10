/// @file
///     ethernetII_seg.cpp
///
/// @brief
///     ��̫������֡��ͷ��Ϣ��
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
///     ����̫������֡ͷ���н���
///
/// @param[in]
///     offset  - �����������ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     msgBuf  - ������������ݵĻ���
/// @param[in]
///     bufSize - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
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
///     �����õĲ������б��룬��������ı���֡
///
/// @param[in]
///     userDataBuf - ������������û����ݵĻ���
/// @param[in]
///     bufSize     - �����С
/// @param[in]
///     msgArr      - ���汨��֡���ݵ�����
/// @param[in]
///     arrSize     - ����Ĵ�С��һ��ȡ��̫��֡�����Size
///
/// @return
///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
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
///     ��ȡ��̫������֡ͷ��Ϣ
///
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     ethHdr     - ������̫������֡ͷ��Ϣ�Ľṹ��
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
///     ��ȡ��̫������֡ͷ��Ϣ�ڱ���֡�е�ƫ�Ƶ�ַ��Ϣ
///
/// @param[in]
///     ethHdrOff - ������̫������֡ͷ��Ϣ��ƫ�Ƶ�ַ��Ϣ�Ľṹ��
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
///     ������̫������֡ͷ��Ϣ
///
/// @param[in]
///     ethHdr - ������̫������֡ͷ��Ϣ�Ľṹ��
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
