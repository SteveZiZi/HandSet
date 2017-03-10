#include "stdafx.h"
#include "../include/msg_rapid_decoder.h"
#include "../include/etypes.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "../msg_seg/bswap.h"
#include "./FrameDecode.h"

///
/// @brief
///     ��̫������֡��Ŀ��Mac��ַ��ƫ�Ƶ�ַ
///
#define ETHERNET_DST_MAC_OFFSET   0 

///
/// @brief
///     ��̫������֡��ԴMac��ַ��ƫ�Ƶ�ַ
///
#define ETHERNET_SRC_MAC_OFFSET   6

///
/// @brief
///     ��ͨ��̫������֡����̫�����͵�ƫ�Ƶ�ַ
///
#define ETHERNET_TYPE1_OFFSET      12

///
/// @brief
///     ISO/IEC8802-3��ʽ����Vlan Tag������̫������֡����̫�����͵�ƫ�Ƶ�ַ
///
#define ETHERNET_TYPE2_OFFSET      16

///
/// @brief
///     ISO/IEC8802-3��ʽ����Vlan Tag������̫������֡��AppID��ƫ�Ƶ�ַ
///
#define ISOIEC8802_3_APPID_OFFSET     18

///
/// @brief
///     FT3����֡������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩�����ݼ������ֶε�ƫ�Ƶ�ַ
///
#define DATASET_LEN_OFFSET_FT3  2

///
/// @brief
///     ����FT3����֡������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩��LNName��ƫ�Ƶ�ַ
///
#define LNNAME_OFFSET_NANRUI_FT3  5

///
/// @brief
///     ����FT3����֡������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩��LNName��ƫ�Ƶ�ַ
///
#define LNNAME_OFFSET_GUOWANG_FT3 5

///
/// @brief
///     ��׼FT3����֡��52�ֽڰ汾������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩��LNName��ƫ�Ƶ�ַ
///
#define LNNAME_OFFSET_STD52_FT3 5

///
/// @brief
///     ����FT3����֡������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩��DataSetName��ƫ�Ƶ�ַ
///
#define DATASET_NAME_OFFSET_NANRUI_FT3  4

///
/// @brief
///     ����FT3����֡������ʼ�ֺ�CRCУ�飬�ұ���Ϊ�ֶ��룩��DataSetName��ƫ�Ƶ�ַ
///
#define DATASET_NAME_OFFSET_GUOWANG_FT3 4

///
/// @brief
///     ISO/IEC8802-3��ʽ����Vlan Tag������̫������֡��APDU��ƫ�Ƶ�ַ
///
#define ISOIEC8802_3_APDU_OFFSET        26

///
/// @brief
///     Sav92����֡������ͨ���Ĵ�С
///
#define SAV92_CHANNEL_DATA_SIZE         8

///
/// @brief
///     FT3����֡����ʼ��
///
#define FT3_START_WORD 0x0564

///
/// @brief
///     FT3֡�����ɶ���ʽ�ļ�ʽ��д��ʵ��Ϊ0x13D65�����λ��1����Ӱ����
///
#define POLY16     0x3D65

 

///
/// @brief
///     8λ��������CRCУ��
///
/// @param[in]
///     toConvert - ָ��Ҫ����CRCУ���������
/// @param[in]
///     length    - ���������ֽ���
///
/// @return
///     CRCУ��ֵ
///
uint16_t CRC_Convert(uint8_t *toConvert, uint16_t length)
{  
	uint16_t crc = 0;
	uint8_t  i;
	uint8_t *ptr = toConvert;

	while(length--)
	{
		for(i = 0x80; i != 0; i = i >> 1)
		{
			if((crc & 0x8000) != 0)
			{
				crc = crc << 1;
				crc = crc ^ POLY16;
			}  
			else
			{
				crc = crc << 1;
			}
			if((*ptr & i) != 0)
			{
				crc = crc ^ POLY16;
			}
		}
		ptr++;  
	}
	return crc;
}

///
/// @brief
///     ��ȡ����֡������
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
///
/// @return
///     ��������
///
static size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
enumFrameType MsgRapidDecoder::GetMessageType(const uint8_t *msgBuf, size_t bufSize)
{
	if ( msgBuf == NULL )
		return FRAME_TYPE_UNKOWN;

	uint16_t etherType = 0;
	uint16_t startWord = 0;
	if ( bufSize >= 2 )
		memcpy((uint8_t *)&startWord,msgBuf,2);
	GetEtherType(msgBuf,bufSize,etherType);

#if ZY_PRJ
	if ( etherType == ETHERTYPE_IEC61850_SV )
	{
		if ( bufSize > ISOIEC8802_3_APDU_OFFSET )
		{
			if ( msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV92_TAG_SAV_PDU )
				return FRAME_TYPE_NET_9_2;
			else if ( msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU)
			{
				// ��ȡAsdu Len
				size_t valIdx = 0;
				if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx) != 0 )
				{
					if ( valIdx != 0 && valIdx+2+2 <= bufSize )
					{
						uint16_t asduLen;
						memcpy((uint8_t *)&asduLen,&msgBuf[valIdx+2],2);
						asduLen = BSWAP_16(asduLen);

						// 9-1��չ֡asduLenΪ12+channelNum*2������׼9-1 asduLen�̶�Ϊ0x2C
						if ( asduLen == 0x2C )
							return FRAME_TYPE_NET_9_1;
						else if ( asduLen <= 12+12*2 )
							return FRAME_TYPE_NET_9_1_EX;
					}
				}
			}
		}
	}
#else
	if ( etherType == ETHERTYPE_IEC61850_SV )
	{
		ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
		if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
		{
			ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;
		}
		if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index )
		{
			if ( msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV92_TAG_SAV_PDU )
				return FRAME_TYPE_NET_9_2;
			else if ( msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU)
			{
				// ��ȡAsdu Len
				size_t valIdx = 0;
				if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx) != 0 )
				{
					if ( valIdx != 0 && valIdx+2+2 <= bufSize )
					{
						uint16_t asduLen;
						memcpy((uint8_t *)&asduLen,&msgBuf[valIdx+2],2);
						asduLen = BSWAP_16(asduLen);

						// 9-1��չ֡asduLenΪ12+channelNum*2������׼9-1 asduLen�̶�Ϊ0x2C
						if ( asduLen == 0x2C )
							return FRAME_TYPE_NET_9_1;
						else if ( asduLen <= 12+12*2 )
							return FRAME_TYPE_NET_9_1_EX;
					}
				}
			}
		}
	}
#endif

	
	else if ( etherType == ETHERTYPE_IEC61850_GOOSE )
	{
		return FRAME_TYPE_NET_GOOSE;
	}
	else if ( etherType == ETHERTYPE_IEEE1588 )
	{
		return FRAME_TYPE_NET_1588;
	}
	else if ( startWord == FT3_START_WORD )
	{
		// �Ƚ�����֡���ݶεĵ�һ���֣��ж�����
		if ( bufSize < 4 )
			return FRAME_TYPE_UNKOWN;
		uint16_t data1stWord;
		memcpy((uint8_t *)&data1stWord,&msgBuf[2],2);

		if ( data1stWord == 0x003E )
			return FRAME_TYPE_FT3_GUOWANG;
		else if ( data1stWord == 0x002C )
			return FRAME_TYPE_FT3_NANRUI;
		else if ( data1stWord == 0x002A )
			return FRAME_TYPE_FT3_STD52;
		else
		{
			if ( (data1stWord >> 8) == 0x01 )
				return FRAME_TYPE_FT3_ONEPHASE_CTORVT;
			else if ( (data1stWord >> 8) == 0x02 )
				return FRAME_TYPE_FT3_TRIPHASE_CT;
			else if ( (data1stWord >> 8) == 0x04 )
				return FRAME_TYPE_FT3_TRIPHASE_CTORVT;
			else if ( (data1stWord >> 8) == 0x03 )
			{
				// ��ȡ�����ѹ������FT3�����3���ֵ��û����ݼ�crcֵ
				if ( bufSize < 28 )
					return FRAME_TYPE_UNKOWN;

				uint16_t buf[3];
				for ( int i = 0; i < 3; i++ )
				{
					memcpy((uint8_t *)&buf[i],&msgBuf[20+2*i],2);
					buf[i] = BSWAP_16(buf[i]);
				}
				uint16_t crc;
				memcpy((uint8_t *)&crc,&msgBuf[26],2);
				crc = ~crc;

				// �����ѹ������FT3֡(2011)��3���ֵ��û�����+crc��β
				if ( CRC_Convert((uint8_t *)&buf[0],sizeof(buf)) == crc )
					return FRAME_TYPE_FT3_TRIPHASE_VT_2011;
				else
					return FRAME_TYPE_FT3_TRIPHASE_VT_2010;
			}
		}
	}
	return FRAME_TYPE_UNKOWN;
}

///
/// @brief
///     ��ȡ����֡�е�Ŀ��Mac��ַ
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     dstMac  - ����Ŀ��Mac��ַ�����飬��С��>=6
///
void MsgRapidDecoder::GetDstMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *dstMac)
{
	if ( msgBuf == NULL || bufSize < ETHERNET_DST_MAC_OFFSET+ETHER_MAC_LEN )
	{
		memset(dstMac,0,ETHER_MAC_LEN);
		return;
	}
	memcpy(dstMac,&msgBuf[ETHERNET_DST_MAC_OFFSET],ETHER_MAC_LEN);
}

///
/// @brief
///     ��ȡ����֡�е�ԴMac��ַ
///
/// @param[in]
///     msgBuf    - ָ����֡�����ָ��
/// @param[in]
///     bufSize   - ����֡����Ĵ�С
/// @param[in]
///     srcMacArr - ����ԴMac��ַ�����飬��С��>=6
///
void MsgRapidDecoder::GetSrcMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *srcMacArr)
{
	if ( msgBuf == NULL || bufSize < ETHERNET_SRC_MAC_OFFSET+ETHER_MAC_LEN )
	{
		memset(srcMacArr,0,ETHER_MAC_LEN);
		return;
	}
	memcpy(srcMacArr,&msgBuf[ETHERNET_SRC_MAC_OFFSET],ETHER_MAC_LEN);
}

///
/// @brief
///     ��ȡ��̫������֡�е���̫������
///
/// @param[in]
///     msgBuf    - ָ����֡�����ָ��
/// @param[in]
///     bufSize   - ����֡����Ĵ�С
/// @param[in]
///     etherType - ������̫������
///
void MsgRapidDecoder::GetEtherType(const uint8_t *msgBuf, size_t bufSize, uint16_t &etherType)
{
	if ( msgBuf == NULL || bufSize < ETHERNET_TYPE1_OFFSET+2 )
	{
		etherType = 0;
		return;
	}
	uint16_t tmp= MAKE_WORD_BIGEND(msgBuf[ETHERNET_TYPE1_OFFSET], msgBuf[ETHERNET_TYPE1_OFFSET+1]);
	if ( tmp == 0x8100 )
	{
		if ( bufSize >= ETHERNET_TYPE1_OFFSET+2 )
		{
			tmp = *((uint16_t *)&msgBuf[ETHERNET_TYPE2_OFFSET]);
			etherType = BSWAP_16(tmp);
			return;
		}
	}
	etherType = tmp;
}

///
/// @brief
///     ��ȡISO/IEC8802-3��ʽ����Vlan Tag������̫������֡�е�AppID
///     ������������appID��0
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     appID   - ����AppID
///
void MsgRapidDecoder::GetAppID(const uint8_t *msgBuf, size_t bufSize, uint16_t &appID)
{
#if ZY_PRJ
	if ( msgBuf == NULL || bufSize < ISOIEC8802_3_APPID_OFFSET+2 )
	{
		appID = 0;
		return;
	}
	uint16_t tmp = *((uint16_t *)&msgBuf[ETHERNET_TYPE1_OFFSET]);
	tmp = BSWAP_16(tmp);
	if ( tmp != 0x8100 )
	{
		appID = 0;
		return;
	}
	appID = *((uint16_t *)&msgBuf[ISOIEC8802_3_APPID_OFFSET]);
	appID = BSWAP_16(appID);
#endif
	//codeMgr_c *cmr = GetDecodeMgr();
	//bool isHasVlan = cmr->HadVlanType(msgBuf,bufSize);

	if ( msgBuf == NULL || bufSize < ISOIEC8802_3_APPID_OFFSET+2 )
	{
		appID = 0;
		return;
	}
	uint16_t tmp= MAKE_WORD_BIGEND(msgBuf[ETHERNET_TYPE1_OFFSET], msgBuf[ETHERNET_TYPE1_OFFSET+1]);
	if ( tmp != 0x8100 )
	{
		//appID = 0;
		//return;
		appID = *((uint16_t *)&msgBuf[NET_noVLAN_APPID_POS]);
		appID = BSWAP_16(appID);
	}
	else
	{	 
		appID = *((uint16_t *)&msgBuf[ISOIEC8802_3_APPID_OFFSET]);
		appID = BSWAP_16(appID);
	}
	 
}

///
/// @brief
///     ��ȡЭ�����ݵ�Ԫ�ĳ��ȣ�ISO/IEC8802-3��ʽ����Vlan Tag������̫������֡�д�AppID��ʼ���ֽ���
///     ������������len��0
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     len     - ����Э�����ݵ�Ԫ�ĳ���
///
void MsgRapidDecoder::GetPDULen(const uint8_t *msgBuf, size_t bufSize, uint16_t &len)
{
#if ZY_PRJ
	if ( msgBuf == NULL || bufSize < ISOIEC8802_3_APPID_OFFSET+2+2 )
	{
		len = 0;
		return;
	}
	uint16_t tmp = *((uint16_t *)&msgBuf[ETHERNET_TYPE1_OFFSET]);
	tmp = BSWAP_16(tmp);
	if ( tmp != 0x8100 )
	{
		len = 0;
		return;
	}
	len = *((uint16_t *)&msgBuf[ISOIEC8802_3_APPID_OFFSET+2]);
	len = BSWAP_16(len);
#endif
	if ( msgBuf == NULL || bufSize < ISOIEC8802_3_APPID_OFFSET+2+2 )
	{
		len = 0;
		return;
	}
	uint16_t tmp= MAKE_WORD_BIGEND(msgBuf[ETHERNET_TYPE1_OFFSET], msgBuf[ETHERNET_TYPE1_OFFSET+1]);
	if ( tmp == 0x8100 )
	{
		len = *((uint16_t *)&msgBuf[ISOIEC8802_3_APPID_OFFSET+2]);
		len = BSWAP_16(len);
	}
	else if (tmp == ETHERTYPE_IEC61850_SV || tmp == ETHERTYPE_IEC61850_GOOSE || tmp == ETHERTYPE_IEEE1588)
	{
		len = *((uint16_t *)&msgBuf[NET_noVLAN_APPID_POS+2]);
		len = BSWAP_16(len);
	}
	else
	{
		len = 0;
		return;
	}
}

///
/// @brief
///     ��ȡ����֡�е�����ͨ������Ŀ����Ҫ����GOOSE��SAV91/2��FT3
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     type       - ������ı��ĵ�����
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNum(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, size_t &channelNum)
{
	if ( msgBuf == NULL )
		return;

	switch (type)
	{
	case FRAME_TYPE_FT3_NANRUI:
		GetChannelNumInNanRuiFT3(msgBuf,bufSize,channelNum);
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		GetChannelNumInGuoWangFT3(msgBuf,bufSize,channelNum);
		break;
	case FRAME_TYPE_FT3_STD52:
		channelNum = 12;
		break;
	case FRAME_TYPE_FT3_ONEPHASE_CTORVT:
		channelNum = 5;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_CT:
		channelNum = 9;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_VT_2010:
		channelNum = 6;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_VT_2011:
		channelNum = 6;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_CTORVT:
		channelNum = 15;
		break;
	case FRAME_TYPE_NET_9_1:
		GetChannelNumInSav91(msgBuf,bufSize,channelNum);
		break;
	case FRAME_TYPE_NET_9_1_EX:
		GetChannelNumInSav91Ex(msgBuf,bufSize,channelNum);
		break;
	case FRAME_TYPE_NET_9_2:
		GetChannelNumInSav92(msgBuf,bufSize,channelNum);
		break;
	case FRAME_TYPE_NET_GOOSE:
		GetChannelNumInGoose(msgBuf,bufSize,channelNum);
		break;
	default:
		channelNum = 0;
	}
}

///
/// @brief
///     ��ȡ����֡�е�LNName����Ҫ����SAV91��FT3
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     type    - ������ı��ĵ�����
/// @param[in]
///     lnName  - �߼��ڵ���
///
void MsgRapidDecoder::GetLNName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &lnName)
{
	if ( msgBuf == NULL )
		return;

	switch (type)
	{
	case FRAME_TYPE_FT3_NANRUI:
		GetLNNameInNanRuiFT3(msgBuf,bufSize,lnName);
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		GetLNNameInGuoWangFT3(msgBuf,bufSize,lnName);
		break;
	case FRAME_TYPE_FT3_STD52:
		GetLNNameInStd52FT3(msgBuf,bufSize,lnName);
		break;
	case FRAME_TYPE_NET_9_1:
		GetLNNameInSav91(msgBuf,bufSize,lnName);
		break;
	case FRAME_TYPE_NET_9_1_EX:
		GetLNNameInSav91Ex(msgBuf,bufSize,lnName);
		break;
	default:
		lnName = 0;
	}
}

///
/// @brief
///     ��ȡ����֡�е�LDName����Ҫ����SAV91��FT3
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     type    - ������ı��ĵ�����
/// @param[in]
///     ldName  - �߼��豸��
///
void MsgRapidDecoder::GetLDName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &ldName)
{
	if ( msgBuf == NULL )
		return;

	switch (type)
	{
	case FRAME_TYPE_FT3_NANRUI:
		GetLDNameInNanRuiFT3(msgBuf,bufSize,ldName);
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		GetLDNameInGuoWangFT3(msgBuf,bufSize,ldName);
		break;
	case FRAME_TYPE_FT3_STD52:
		GetLDNameInStd52FT3(msgBuf,bufSize,ldName);
		break;
	case FRAME_TYPE_NET_9_1:
		GetLDNameInSav91(msgBuf,bufSize,ldName);
		break;
	case FRAME_TYPE_NET_9_1_EX:
		GetLDNameInSav91Ex(msgBuf,bufSize,ldName);
		break;
	default:
		ldName = 0;
	}
}

///
/// @brief
///     ��ȡ����֡�еĲ����ʣ���Ҫ����SAV91/2��FT3
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     type    - ������ı��ĵ�����
/// @param[in]
///     smpRate - ������
///
void MsgRapidDecoder::GetSmpRate(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &smpRate)
{
	if ( msgBuf == NULL )
		return;

	switch (type)
	{
	case FRAME_TYPE_FT3_NANRUI:
		GetSmpRateInNanRuiFT3(msgBuf,bufSize,smpRate);
		break;
	case FRAME_TYPE_NET_9_1:
		GetSmpRateInSav91(msgBuf,bufSize,smpRate);
		break;
	case FRAME_TYPE_NET_9_2:
		GetSmpRateInSav92(msgBuf,bufSize,smpRate);
		break;
	default:
		smpRate = 0;
	}
}

///
/// @brief
///     ��ȡ����֡�е�ASDU��Ŀ����Ҫ����SAV91/2��FT3
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     type    - ������ı��ĵ�����
/// @param[in]
///     noASDU  - ASDU��Ŀ
///
void MsgRapidDecoder::GetNoASDU(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &noASDU)
{
	if ( msgBuf == NULL )
		return;

	switch (type)
	{
	case FRAME_TYPE_FT3_NANRUI:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_GUOWANG:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_STD52:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_ONEPHASE_CTORVT:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_CT:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_VT_2010:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_VT_2011:
		noASDU = 1;
		break;
	case FRAME_TYPE_FT3_TRIPHASE_CTORVT:
		noASDU = 1;
		break;
	case FRAME_TYPE_NET_9_1:
		GetNoASDUInSav91(msgBuf,bufSize,noASDU);
		break;
	case FRAME_TYPE_NET_9_1_EX:
		GetNoASDUInSav91(msgBuf,bufSize,noASDU);
		break;
	case FRAME_TYPE_NET_9_2:
		GetNoASDUInSav92(msgBuf,bufSize,noASDU);
		break;
	default:
		noASDU = 0;
	}
}

///
/// @brief
///     ��ȡSav92����֡�еĿ�ѡ����Ϣ
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     opts    - ��ѡ��
///
void MsgRapidDecoder::GetSav92Opts(const uint8_t *msgBuf, size_t bufSize, SAV92_SAVPDU_OPTS &opts)
{
	if ( msgBuf == NULL )
		return;

#if ZY_PRJ
	if ( bufSize <= ISOIEC8802_3_APDU_OFFSET || msgBuf[ISOIEC8802_3_APDU_OFFSET] != SAV92_TAG_SAV_PDU )
		return;

	opts.bSecurityOpt = false;
	opts.bDataSetOpt  = false;
	opts.bRefrTmOpt   = false;
	opts.bSmpRateOpt  = false;

	// SavPDU��������
	size_t valIdx = 0;
	NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}
	if ( bufSize <= ISOIEC8802_3_APDU_OFFSET_index || msgBuf[ISOIEC8802_3_APDU_OFFSET_index] != SAV92_TAG_SAV_PDU )
		return;

	opts.bSecurityOpt = false;
	opts.bDataSetOpt  = false;
	opts.bRefrTmOpt   = false;
	opts.bSmpRateOpt  = false;

	// SavPDU��������
	size_t valIdx = 0;
	NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
#endif
	size_t nextIdx = valIdx;
	for ( size_t i = 0; i < 3; i++ )
	{
		if ( nextIdx == 0 || nextIdx >= bufSize )
			return;
		if ( msgBuf[nextIdx] == SAV92_TAG_SECURITY )
		{
			opts.bSecurityOpt = true;
		}
		if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_ASDU )
		{
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
			break;
		}
		nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
	}

	// ASDU����
	if ( valIdx != 0 && valIdx < bufSize && msgBuf[valIdx] != SAV92_TAG_ASDU )
	{
		return;
	}
	NextTLVData(valIdx,msgBuf,bufSize,valIdx);
	nextIdx = valIdx;
	for ( size_t i = 0; i < 8; i++ )
	{
		if ( nextIdx == 0 || nextIdx >= bufSize )
			return;
		if ( msgBuf[nextIdx] == SAV92_TAG_DATA_SET )
		{
			opts.bDataSetOpt = true;
		}
		if ( msgBuf[nextIdx] == SAV92_TAG_REFR_TM )
		{
			opts.bRefrTmOpt  = true;
		}
		if ( msgBuf[nextIdx] == SAV92_TAG_SMP_RATE )
		{
			opts.bSmpRateOpt = true;
		}
		if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_OF_DATA )
		{
			break;
		}
		nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
	}
}

///
/// @brief
///     �ж�Goose����֡���Ƿ���Security��ѡ��
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
///
/// @return
///     true - �У� false - ��
///
bool MsgRapidDecoder::IsHaveSecurityInGoose(const uint8_t *msgBuf, size_t bufSize)
{
#if ZY_PRJ
	if ( msgBuf == NULL )
		return false;
	if ( bufSize <= ISOIEC8802_3_APDU_OFFSET || msgBuf[ISOIEC8802_3_APDU_OFFSET] != GOOSE_TAG_GOOSE_PDU )
		return false;

	size_t valIdx = 0;
	NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
	size_t nextIdx = valIdx;
	for ( size_t i = 0; i < 12; i++ )
	{
		if ( nextIdx == 0 )
			return false;
		nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
	}
	if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_SECURITY )
	{
		return true;
	}
	return false;
#endif
	if ( msgBuf == NULL )
		return false;
	if (GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		if ( bufSize <= ISOIEC8802_3_APDU_OFFSET || msgBuf[ISOIEC8802_3_APDU_OFFSET] != GOOSE_TAG_GOOSE_PDU )
			return false;

		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		size_t nextIdx = valIdx;
		for ( size_t i = 0; i < 12; i++ )
		{
			if ( nextIdx == 0 )
				return false;
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
		}
		if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_SECURITY )
		{
			return true;
		}
		return false;
	}
	else
	{	
		if ( bufSize <= NET_noVLAN_PDU_POS || msgBuf[NET_noVLAN_PDU_POS] != GOOSE_TAG_GOOSE_PDU )
			return false;

		size_t valIdx = 0;
		NextTLVData(NET_noVLAN_PDU_POS,msgBuf,bufSize,valIdx);
		size_t nextIdx = valIdx;
		for ( size_t i = 0; i < 12; i++ )
		{
			if ( nextIdx == 0 )
				return false;
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
		}
		if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_SECURITY )
		{
			return true;
		}
		return false;
	}
}

///
/// @brief
///     ��ȡIEEE1588���ĵİ汾��Ϣ
///
/// @param[in]
///     msgBuf  - ָ��IEEE1588����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     version - ����IEEE1588���ĵİ汾��Ϣ
///
/// @return
///     
///
void MsgRapidDecoder::GetIeee1588Version(const uint8_t *msgBuf, size_t bufSize, uint8_t &version)
{
	if ( msgBuf == NULL )
	{
		version = 0;
		return;
	}

	const uint16_t k_Ieee1588VersionOff = 15;
	if ( bufSize <= k_Ieee1588VersionOff )
	{
		version = 0;
		return;
	}
	version = msgBuf[k_Ieee1588VersionOff] & 0x0F;
}

///
/// @brief
///     ��ȡIEEE1588���ĵı��ķ����˿ڣ�Դ�˿ڣ��ı�ʶ��Ϣ
///
/// @param[in]
///     msgBuf    - ָ��IEEE1588����֡�����ָ��
/// @param[in]
///     bufSize   - ����֡����Ĵ�С
/// @param[in]
///     srcPortID - ���汨�ķ����˿ڣ�Դ�˿ڣ��ı�ʶ��Ϣ�Ľṹ��
///
/// @return
///     
///
void MsgRapidDecoder::GetIeee1588SrcPortID(const uint8_t *msgBuf, size_t bufSize, IEEE1588_SOURCE_PORT_ID &srcPortID)
{
	if ( msgBuf == NULL )
	{
		srcPortID.clockIdentity = 0;
		srcPortID.sourcePortID  = 0;
		return;
	}

	const uint16_t k_Ieee1588SrcPortIDOff = 34;
	if ( bufSize < k_Ieee1588SrcPortIDOff+8+2 )
	{
		srcPortID.clockIdentity = 0;
		srcPortID.sourcePortID  = 0;
		return;
	}
	uint64_t uint64Tmp = 0;
	uint16_t uint16Tmp = 0;
	memcpy_s((uint8_t *)&uint64Tmp,8,&msgBuf[k_Ieee1588SrcPortIDOff],8);
	memcpy_s((uint8_t *)&uint16Tmp,2,&msgBuf[k_Ieee1588SrcPortIDOff+8],2);
	srcPortID.clockIdentity = BSWAP_64(uint64Tmp);
	srcPortID.sourcePortID  = BSWAP_16(uint16Tmp);
}

///
/// @brief
///     ��ȡSync�����뿪��ʱ��ʱ��ʱ�̣���Sync���Ļ���Follow_Up���ĵ�OriginTimestamp�ֶ�
///
/// @param[in]
///     msgBuf   - ָ��IEEE1588����֡�����ָ��
/// @param[in]
///     bufSize  - ����֡����Ĵ�С
/// @param[in]
///     originTs - OriginTimestamp
///
/// @return
///     true - �����ɹ��� false - ����ʧ��
///
bool MsgRapidDecoder::GetIeee1588OriginTimeStamp(const uint8_t *msgBuf, size_t bufSize, IEEE1588_TIMESTAMP &originTs)
{
	const size_t k_MsgTypeOff = 14;
	const size_t k_OriginTimestampOff = 48;
	if ( msgBuf != NULL && bufSize >= k_OriginTimestampOff+10 )
	{
		IEEE1588_TYPE_FIELD   type = {0,0};
		type.messageType = msgBuf[k_MsgTypeOff]  & 0x0F;
		IEEE1588_MESSAGE_TYPE msgType = type.GetMessageType();
		if ( msgType == IEEE1588_TYPE_SYNC || msgType == IEEE1588_TYPE_FOLLOW_UP )
		{
			// ��ȡʱ�������������
			memcpy_s((uint8_t *)&originTs.t_s,sizeof(uint64_t),&msgBuf[k_OriginTimestampOff],6);
			originTs.t_s = BSWAP_64(originTs.t_s);
			originTs.t_s = originTs.t_s>>16;

			// ��ȡʱ�����С������
			memcpy_s((uint8_t *)&originTs.t_ns,sizeof(uint32_t),&msgBuf[k_OriginTimestampOff+6],4);
			originTs.t_ns = BSWAP_32(originTs.t_ns);
			return true;
		}
	}

	originTs.t_s  = 0;
	originTs.t_ns = 0;
	return false;
}

///
/// @brief
///     ��ȡ����FT3����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 12;
}

///
/// @brief
///     ��ȡ����FT3����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 22;
}

///
/// @brief
///     ��ȡSav91����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInSav91(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 12;
}

///
/// @brief
///     ��ȡSav91��չ����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInSav91Ex(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 0;
	if ( msgBuf == NULL )
		return;

	
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx) != 0 )
		{
			if ( valIdx+2+2 <= bufSize )
			{
				uint16_t asduLen;
				memcpy((uint8_t *)&asduLen,&msgBuf[valIdx+2],2);
				asduLen = BSWAP_16(asduLen);
				channelNum = (asduLen-12)/2;
			}
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx) != 0 )
		{
			if ( valIdx+2+2 <= bufSize )
			{
				uint16_t asduLen;
				memcpy((uint8_t *)&asduLen,&msgBuf[valIdx+2],2);
				asduLen = BSWAP_16(asduLen);
				channelNum = (asduLen-12)/2;
			}
		}
	}
#endif
}

///
/// @brief
///     ��ȡSav92����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInSav92(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU�Ĺ�������
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		size_t nextIdx = valIdx;
		for ( size_t i = 0; i < 3; i++ )
		{
			if ( nextIdx == 0 || nextIdx >= bufSize )
				return;
			if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_ASDU )
			{
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);

				// ASDU����
				if ( valIdx != 0 && valIdx < bufSize && msgBuf[valIdx] == SAV92_TAG_ASDU )
				{
					nextIdx = NextTLVData(valIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx == 0 )
						return;
					nextIdx = valIdx;
					for ( size_t i = 0; i < 8; i++ )
					{
						if ( nextIdx < bufSize && msgBuf[nextIdx] == SAV92_TAG_SEQ_OF_DATA )
						{
							nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
							if ( nextIdx != 0 )
								channelNum = (nextIdx-valIdx)/SAV92_CHANNEL_DATA_SIZE;
							return;
						}
						nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
						if ( nextIdx == 0 )
							return;
					}
				}
			}
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU�Ĺ�������
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		size_t nextIdx = valIdx;
		for ( size_t i = 0; i < 3; i++ )
		{
			if ( nextIdx == 0 || nextIdx >= bufSize )
				return;
			if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_ASDU )
			{
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);

				// ASDU����
				if ( valIdx != 0 && valIdx < bufSize && msgBuf[valIdx] == SAV92_TAG_ASDU )
				{
					nextIdx = NextTLVData(valIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx == 0 )
						return;
					nextIdx = valIdx;
					for ( size_t i = 0; i < 8; i++ )
					{
						if ( nextIdx < bufSize && msgBuf[nextIdx] == SAV92_TAG_SEQ_OF_DATA )
						{
							nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
							if ( nextIdx != 0 )
								channelNum = (nextIdx-valIdx)/SAV92_CHANNEL_DATA_SIZE;
							return;
						}
						nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
						if ( nextIdx == 0 )
							return;
					}
				}
			}
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
		}
	}
#endif
}

///
/// @brief
///     ��ȡGoose����֡�е�����ͨ������Ŀ
///
/// @param[in]
///     msgBuf     - ָ����֡�����ָ��
/// @param[in]
///     bufSize    - ����֡����Ĵ�С
/// @param[in]
///     channelNum - ����ͨ������Ŀ
///
void MsgRapidDecoder::GetChannelNumInGoose(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum)
{
	channelNum = 0;
	if ( msgBuf == NULL )
		return;

#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == GOOSE_TAG_GOOSE_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		size_t nextIdx = valIdx;
		for ( size_t i = 0; i < 11; i++ )
		{
			if ( nextIdx == 0 || nextIdx >= bufSize )
				return;
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
		}
		if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_ALL_DATA )
		{
			// GOOSE DATA ����
			nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
			for ( size_t i = valIdx; i < nextIdx; )
			{
				if ( i == 0 || i >= bufSize )
				{
					channelNum = 0;
					return;
				}
				i = NextTLVData(i,msgBuf,bufSize,valIdx);
				channelNum++;
			}
		}
	}
#endif
	if (GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{	
		if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == GOOSE_TAG_GOOSE_PDU )
		{
			size_t valIdx = 0;
			NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 11; i++ )
			{
				if ( nextIdx == 0 || nextIdx >= bufSize )
					return;
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
			}
			if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_ALL_DATA )
			{
				// GOOSE DATA ����
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				for ( size_t i = valIdx; i < nextIdx; )
				{
					if ( i == 0 || i >= bufSize )
					{
						channelNum = 0;
						return;
					}
					i = NextTLVData(i,msgBuf,bufSize,valIdx);
					channelNum++;
				}
			}
		}
	}
	else
	{
		if ( bufSize > NET_noVLAN_PDU_POS && msgBuf[NET_noVLAN_PDU_POS] == GOOSE_TAG_GOOSE_PDU )
		{
			size_t valIdx = 0;
			NextTLVData(NET_noVLAN_PDU_POS,msgBuf,bufSize,valIdx);
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 11; i++ )
			{
				if ( nextIdx == 0 || nextIdx >= bufSize )
					return;
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
			}
			if ( nextIdx != 0 && nextIdx < bufSize && msgBuf[nextIdx] ==  GOOSE_TAG_ALL_DATA )
			{
				// GOOSE DATA ����
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				for ( size_t i = valIdx; i < nextIdx; )
				{
					if ( i == 0 || i >= bufSize )
					{
						channelNum = 0;
						return;
					}
					i = NextTLVData(i,msgBuf,bufSize,valIdx);
					channelNum++;
				}
			}
		}
	}
 

 
}

///
/// @brief
///     ��ȡ����FT3����֡�е�LNName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     lnName  - �߼��ڵ���
///
void MsgRapidDecoder::GetLNNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName)
{
	if ( msgBuf != NULL && bufSize > LNNAME_OFFSET_NANRUI_FT3 )
		lnName = msgBuf[LNNAME_OFFSET_NANRUI_FT3];
	else
		lnName = 0;
}

///
/// @brief
///     ��ȡ����FT3����֡�е�LNName
///
/// @param[in]
///     msgBuf   - ָ����֡�����ָ��
/// @param[in]
///     bufSize  - ����֡����Ĵ�С
/// @param[in]
///     lnName   - �߼��ڵ���
///
void MsgRapidDecoder::GetLNNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName)
{
	if ( msgBuf != NULL && bufSize > LNNAME_OFFSET_GUOWANG_FT3 )
		lnName = msgBuf[LNNAME_OFFSET_GUOWANG_FT3];
	else
		lnName = 0;
}

///
/// @brief
///     ��ȡ��׼FT3����֡��52�ֽڰ汾���е�LNName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     lnName  - �߼��ڵ���
///
void MsgRapidDecoder::GetLNNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName)
{
	if ( msgBuf != NULL && bufSize > LNNAME_OFFSET_STD52_FT3 )
		lnName = msgBuf[LNNAME_OFFSET_STD52_FT3];
	else
		lnName = 0;
}

///
/// @brief
///     ��ȡSav91����֡�е�LNName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     lnName  - �߼��ڵ���
///
void MsgRapidDecoder::GetLNNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName)
{
	lnName = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2 < bufSize )
			lnName = msgBuf[valIdx+2+2];
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2 < bufSize )
			lnName = msgBuf[valIdx+2+2];
	}
#endif
}

///
/// @brief
///     ��ȡSav91��չ����֡�е�LNName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     lnName  - �߼��ڵ���
///
void MsgRapidDecoder::GetLNNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName)
{
	lnName = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2 < bufSize )
			lnName = msgBuf[valIdx+2+2];
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2 < bufSize )
			lnName = msgBuf[valIdx+2+2];
	}
#endif
}

///
/// @brief
///     ��ȡ����FT3����֡�е�LDName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     ldName  - �߼��豸��
///
void MsgRapidDecoder::GetLDNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName)
{
	if ( msgBuf != NULL && LNNAME_OFFSET_NANRUI_FT3+1+2 <= bufSize )
		memcpy_s((uint8_t *)&ldName,2,&msgBuf[LNNAME_OFFSET_NANRUI_FT3+1],2);
	else
		ldName = 0;
}

///
/// @brief
///     ��ȡ����FT3����֡�е�LDName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     ldName  - �߼��豸��
///
void MsgRapidDecoder::GetLDNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName)
{
	if ( msgBuf != NULL && LNNAME_OFFSET_GUOWANG_FT3+1+2 <= bufSize )
		memcpy_s((uint8_t *)&ldName,2,&msgBuf[LNNAME_OFFSET_GUOWANG_FT3+1],2);
	else
		ldName = 0;
}

///
/// @brief
///     ��ȡ��׼FT3����֡��52�ֽڰ汾���е�LDName
///
/// @param[in]
///     msgBuf - ָ����֡�����ָ��
/// @param[in]
///     ldName - �߼��豸��
///
void MsgRapidDecoder::GetLDNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName)
{
	if ( msgBuf != NULL && LNNAME_OFFSET_STD52_FT3+1+2 <= bufSize )
		memcpy_s((uint8_t *)&ldName,2,&msgBuf[LNNAME_OFFSET_STD52_FT3+1],2);
	else
		ldName = 0;
}

///
/// @brief
///     ��ȡSav91����֡�е�LDName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     ldName  - �߼��豸��
///
void MsgRapidDecoder::GetLDNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName)
{
	ldName = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2+2+2 <= bufSize )
		{
			memcpy_s((uint8_t *)&ldName,2,&msgBuf[valIdx+2+2+2],2);
			ldName = BSWAP_16(ldName);
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2+2+2 <= bufSize )
		{
			memcpy_s((uint8_t *)&ldName,2,&msgBuf[valIdx+2+2+2],2);
			ldName = BSWAP_16(ldName);
		}
	}
#endif
}

///
/// @brief
///     ��ȡSav91��չ����֡�е�LDName
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     ldName  - �߼��豸��
///
void MsgRapidDecoder::GetLDNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName)
{
	ldName = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2+2+2 <= bufSize )
		{
			memcpy_s((uint8_t *)&ldName,2,&msgBuf[valIdx+2+2+2],2);
			ldName = BSWAP_16(ldName);
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+2+2+2 <= bufSize )
		{
			memcpy_s((uint8_t *)&ldName,2,&msgBuf[valIdx+2+2+2],2);
			ldName = BSWAP_16(ldName);
		}
	}
#endif
}

///
/// @brief
///     ��ȡ����FT3����֡�еĲ�����
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     smpRate - ������
///
void MsgRapidDecoder::GetSmpRateInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate)
{
	if ( msgBuf != NULL && bufSize > 53 )
		smpRate = msgBuf[53];
	else
		smpRate = 0;
}

///
/// @brief
///     ��ȡSav91����֡�еĲ�����
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     smpRate - ������
///
void MsgRapidDecoder::GetSmpRateInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate)
{
	smpRate = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+44 < bufSize && msgBuf[valIdx+2+3] == 0x01 )
			smpRate = msgBuf[valIdx+2+44];
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx);
		if ( valIdx != 0 && valIdx+2+44 < bufSize && msgBuf[valIdx+2+3] == 0x01 )
			smpRate = msgBuf[valIdx+2+44];
	}
#endif
}

///
/// @brief
///     ��ȡSav92����֡�еĲ�����
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     smpRate - ������
///
void MsgRapidDecoder::GetSmpRateInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate)
{
	smpRate = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx) != 0 )
		{
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 3; i++ )
			{
				if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_ASDU )
				{
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx == 0 )
						return;
					break;
				}
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				if ( nextIdx == 0 )
					return;
			}
			// ASDU
			if ( valIdx < bufSize && msgBuf[valIdx] == SAV92_TAG_ASDU )
			{
				if ( NextTLVData(valIdx,msgBuf,bufSize,valIdx) == 0 )
					return;
				nextIdx = valIdx;
				for ( size_t i = 0; i < 8; i++ )
				{
					if ( nextIdx == 0 || nextIdx >= bufSize )
						return;

					if ( msgBuf[nextIdx] == SAV92_TAG_SMP_RATE )
					{
						nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
						if ( nextIdx != 0 && valIdx+2 <= bufSize)
						{
							memcpy((uint8_t *)&smpRate,&msgBuf[valIdx],2);
							smpRate = BSWAP_16(smpRate);
						}
						return;
					}
					if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_OF_DATA )
						break;
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				}
			}
		}
	}

#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx) != 0 )
		{
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 3; i++ )
			{
				if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_ASDU )
				{
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx == 0 )
						return;
					break;
				}
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				if ( nextIdx == 0 )
					return;
			}
			// ASDU
			if ( valIdx < bufSize && msgBuf[valIdx] == SAV92_TAG_ASDU )
			{
				if ( NextTLVData(valIdx,msgBuf,bufSize,valIdx) == 0 )
					return;
				nextIdx = valIdx;
				for ( size_t i = 0; i < 8; i++ )
				{
					if ( nextIdx == 0 || nextIdx >= bufSize )
						return;

					if ( msgBuf[nextIdx] == SAV92_TAG_SMP_RATE )
					{
						nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
						if ( nextIdx != 0 && valIdx+2 <= bufSize)
						{
							memcpy((uint8_t *)&smpRate,&msgBuf[valIdx],2);
							smpRate = BSWAP_16(smpRate);
						}
						return;
					}
					if ( msgBuf[nextIdx] == SAV92_TAG_SEQ_OF_DATA )
						break;
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				}
			}
		}
	}
#endif
}

///
/// @brief
///     ��ȡSav91����֡�е�ASDU��Ŀ��Ϣ
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     noASDU  - ASDU��Ŀ
///
void MsgRapidDecoder::GetNoASDUInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU)
{
	noASDU = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx) != 0 )
		{
			if ( valIdx+2 <= bufSize )
			{
				memcpy((uint8_t *)&noASDU,&msgBuf[valIdx],2);
				noASDU = BSWAP_16(noASDU);
			}
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV91_TAG_SAV_PDU )
	{
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx) != 0 )
		{
			if ( valIdx+2 <= bufSize )
			{
				memcpy((uint8_t *)&noASDU,&msgBuf[valIdx],2);
				noASDU = BSWAP_16(noASDU);
			}
		}
	}
#endif
}

///
/// @brief
///     ��ȡSav92����֡�е�ASDU��Ŀ��Ϣ
///
/// @param[in]
///     msgBuf  - ָ����֡�����ָ��
/// @param[in]
///     bufSize - ����֡����Ĵ�С
/// @param[in]
///     noASDU  - ASDU��Ŀ
///
void MsgRapidDecoder::GetNoASDUInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU)
{
	noASDU = 0;
	if ( msgBuf == NULL )
		return;
#if ZY_PRJ
	if ( bufSize > ISOIEC8802_3_APDU_OFFSET && msgBuf[ISOIEC8802_3_APDU_OFFSET] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET,msgBuf,bufSize,valIdx) != 0 )
		{
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 3; i++ )
			{
				if ( nextIdx < bufSize && msgBuf[nextIdx] == SAV92_TAG_NO_ASDU )
				{
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx - valIdx == 1 )
					{
						if ( nextIdx != 0 && valIdx < bufSize )
							noASDU = msgBuf[valIdx];
					}
					else
					{
						if ( nextIdx != 0 && valIdx+2 <= bufSize )
						{
							memcpy((uint8_t *)&noASDU,&msgBuf[valIdx],2);
							noASDU = BSWAP_16(noASDU);
						}
					}
					break;
				}
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				if ( nextIdx == 0 )
					return;
			}
		}
	}
#else
	/*size_t ISOIEC8802_3_APDU_OFFSET_index = ISOIEC8802_3_APDU_OFFSET;
	if(!GetDecodeMgr()->HadVlanType((PBYTE)msgBuf,bufSize))
	{
		ISOIEC8802_3_APDU_OFFSET_index = NET_noVLAN_PDU_POS;*/{
	}

	if ( bufSize > ISOIEC8802_3_APDU_OFFSET_index && msgBuf[ISOIEC8802_3_APDU_OFFSET_index] == SAV92_TAG_SAV_PDU )
	{
		// SavPDU
		size_t valIdx = 0;
		if ( NextTLVData(ISOIEC8802_3_APDU_OFFSET_index,msgBuf,bufSize,valIdx) != 0 )
		{
			size_t nextIdx = valIdx;
			for ( size_t i = 0; i < 3; i++ )
			{
				if ( nextIdx < bufSize && msgBuf[nextIdx] == SAV92_TAG_NO_ASDU )
				{
					nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
					if ( nextIdx - valIdx == 1 )
					{
						if ( nextIdx != 0 && valIdx < bufSize )
							noASDU = msgBuf[valIdx];
					}
					else
					{
						if ( nextIdx != 0 && valIdx+2 <= bufSize )
						{
							memcpy((uint8_t *)&noASDU,&msgBuf[valIdx],2);
							noASDU = BSWAP_16(noASDU);
						}
					}
					break;
				}
				nextIdx = NextTLVData(nextIdx,msgBuf,bufSize,valIdx);
				if ( nextIdx == 0 )
					return;
			}
		}
	}
#endif
}

///
/// @brief
///     ������ǰTLV���ݣ�����ȡ��һ��TLV���ݵ�ƫ�Ƶ�ַ
///
/// @param[in]
///     beginIdx - ��ǰTLV���ݵ�ƫ�Ƶ�ַ
/// @param[in]
///     dataBuf  - ָ��TLV���ݻ��棨��ǰTLV���ݷ�����λ����ָ��
/// @param[in]
///     bufSize  - TLV���ݻ���Ĵ�С
/// @param[in]
///     valIdx   - ���ص�ǰTLV���ݵ�value���ֵ�ƫ�Ƶ�ַ
///
/// @return
///     ��һ��TLV���ݵ�ƫ�Ƶ�ַ
///
size_t MsgRapidDecoder::NextTLVData(const size_t beginIdx, const uint8_t *dataBuf, size_t bufSize, size_t &valIdx)
{
	if ( dataBuf == NULL || beginIdx+1 >= bufSize )
	{
		valIdx = 0;
		return 0;
	}
	uint32_t lenTmp = 0;
	size_t   nxtIdx = 1;

	// ����TLV�����е�Value���ֵĳ��ȼ������ƫ�Ƶ�ַ
	if ( ( dataBuf[beginIdx+1] & 0x80 ) == 0 )
	{
		lenTmp = dataBuf[beginIdx+1];
		nxtIdx = 2;
	}
	else
	{
		int lByte = dataBuf[beginIdx+1] & 0x7F;
		if ( lByte > 4 || beginIdx+lByte+1 >= bufSize )
		{
			valIdx = 0;
			return 0;
		}
		for ( int i = 0; i < lByte; i++ )
		{
			lenTmp |= ( ((uint32_t)dataBuf[beginIdx+1+lByte-i]) << (i*8) );
		}
		nxtIdx = 2+lByte;
	}

	valIdx = beginIdx + nxtIdx;

//	// �ж�Value�����Ƿ񳬹����������ݵĿռ䣬���򷵻�0
// 	if ( nxtIdx + lenTmp > bufSize )
// 		return 0;
	return (beginIdx + nxtIdx + lenTmp);
}
