/// @file
///     goose_data_seg.cpp
///
/// @brief
///     GOOSE����֡�е�Goose�������У�AllData����
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
///    |2013.07.09     |1.0.1.709  |����ͨ�������ѽ����־�����ýӿڣ����ڱ��Ŀ��ٽ���       |
///    |2013.11.05     |1.0.1.1105 |��������AllData�ֶ�û�н��н�β�жϵ��½���ʧ�ܵ�Bug     |
///
#include "stdafx.h"
#include "goose_data_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

#define GOOSE_ONE_DATA_MAX_SIZE 256

GooseDataSeg::GooseDataSeg()
: m_bChange2ActData(false)
, m_GseDataLen(0)
, m_GseDataOff(0)
{

}

GooseDataSeg::~GooseDataSeg()
{
	ReleaaeChannelMsgData(m_vChannelMsgData);
	ReleaaeChannelActData(m_vChannelActData);
}

///
/// @brief
///     ��GOOSEͨ�����ݽ��н���
///
/// @param[in]
///     offset     - �������GOOSE���������ڱ���֡�е�ƫ�Ƶ�ַ
/// @param[in]
///     gseDataBuf - ����������GOOSE�������еĻ���
/// @param[in]
///     bufSize    - �����С
///
/// @return
///     ʵ�ʵĽ������ݴ�С - �ɹ��� 0 - ʧ��
///
size_t GooseDataSeg::Decode(const size_t offset, const uint8_t *gseDataBuf, const size_t bufSize)
{
	// ����Goose�������У�AllData����Value����
	MsgDataElem *gooseData = CreateMsgDataElemImpl(DT_TLV_DATA);
	gooseData->SetTag(GOOSE_TAG_ALL_DATA);
	size_t dSize  = gooseData->Decode(offset,gseDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(gooseData);
		return 0;
	}
	m_GseDataLen = dSize;
	m_GseDataOff = offset;
	offTmp      += dSize - gooseData->GetValueLen();
	ReleaseMsgDataElemImpl(gooseData);

	// ��������ͨ��
	while (offTmp < m_GseDataLen)
	{
		MsgDataElem *channelData = CreateMsgDataElemImpl(DT_TLV_DATA);
		dSize = channelData->Decode(offset+offTmp,&gseDataBuf[offTmp],bufSize-offTmp);
		if ( dSize == 0 )
		{
			ReleaseMsgDataElemImpl(channelData);
			return 0;
		}
		GOOSE_CHANNEL_MSG_DATA *channel = DecodeGooseChannel(channelData,gseDataBuf,bufSize);
		if ( channel == NULL )
		{
			ReleaseMsgDataElemImpl(channelData);
			return 0;
		}
		m_vChannelMsgData.push_back(channel);
		offTmp += dSize;
	}
	m_bChange2ActData = false;
	return offTmp;
}

///
/// @brief
///     �������õĲ������б��룬�õ�GOOSE��������
///
/// @param[in]
///     gseDataArr - ����GOOSEͨ�����ݵ�����
/// @param[in]
///     arrSize    - �����Ԫ����Ŀ,��>=GOOSEͨ�����ݵĳ���+6
///
/// @return
///     ���汨��֡���ݵ������ʵ�ʴ�С - �ɹ��� 0 - ʧ��
///
size_t GooseDataSeg::Code(uint8_t *gseDataArr, const size_t arrSize)
{
	size_t arrOff = 0;
	size_t cSize  = 0;

	// ����Goose����ͨ��
	std::vector<GOOSE_CHANNEL_MSG_DATA *>::iterator it;
	for ( it = m_vChannelMsgData.begin(); it != m_vChannelMsgData.end(); it++ )
	{
		cSize = CodeGooseChannel((*it),&gseDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// ����Goose�������У�All Data��
	MsgDataElem *gooseData = CreateMsgDataElemImpl(DT_TLV_DATA);
	gooseData->SetTag(GOOSE_TAG_ALL_DATA);
	gooseData->SetValue(gseDataArr,arrOff);
	cSize = gooseData->Code(gseDataArr,arrSize);
	ReleaseMsgDataElemImpl(gooseData);
	m_GseDataLen = cSize;
	m_bChange2ActData = false;
	return cSize;
}

///
/// @brief
///     ����ͨ�������ѽ��루����ֵ->��ʵֵ���ı�־
///
void GooseDataSeg::ResetChannelDecodedFlag()
{
	m_bChange2ActData = false;
}

///
/// @brief
///     ��ȡGoosePdu�е�����ͨ������Ŀ
///
/// @return
///     ����ͨ������Ŀ
///
size_t GooseDataSeg::GetChannelNum()
{
	return m_vChannelMsgData.size();
}

///
/// @brief
///     ��ȡGoosePdu�еĵ���ͨ��������
///
/// @param[in]
///     idxSeq     - GOOSE����ͨ����˳��ţ���0��ʼ
/// @param[in]
///     msgDataBuf - ���汨��֡���ݵĻ���
/// @param[in]
///     datArr     - ����GOOSEͨ�����ݵ�����
/// @param[in]
///     arrSize    - ����Ĵ�С����ֵ��>=GetAChannelSize()
///
/// @return
///     �����ʵ�ʴ�С
///
GOOSE_DATA* GooseDataSeg::GetAChannel(size_t idxSeq, const uint8_t *msgDataBuf, const size_t bufSize)
{
	if ( idxSeq >= m_vChannelMsgData.size() )
		return NULL;

	if ( !m_bChange2ActData )
	{
		ReleaaeChannelActData(m_vChannelActData);
		ChannelMsgToAct(msgDataBuf,bufSize);
		m_bChange2ActData = true;
	}
	// ��ȡͨ������
	return m_vChannelActData.at(idxSeq);
}

///
/// @brief
///     ��ȡGoose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
/// @return
///     Goose�������У�All Data���ĳ��ȣ�����TLV���ݣ�
///
size_t GooseDataSeg::GetGooseDataLen()
{
	return m_GseDataLen;
}

///
/// @brief
///     ��ȡGoose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
///
/// @return
///     Goose�������У�All Data�����ڱ���֡�е�ƫ�Ƶ�ַ
///
size_t GooseDataSeg::GetGooseDataOff()
{
	return m_GseDataOff;
}

///
/// @brief
///     ���һ������ͨ��
///
/// @param[in]
///     datArr  - ���浥��GOOSE����ͨ�����е����ݵ�����
/// @param[in]
///     arrSize - ����GOOSE����ͨ����������Ŀ��Ĭ��Ϊ1����ʾ��ͨ�������ݷǽṹ�����ͣ�
///
/// @return
///     true - �ɹ��� false - ʧ��
///
bool GooseDataSeg::AddAChannel(const GOOSE_DATA *gseData)
{
	// ������ݵ�ͨ�����ݱ�
	GOOSE_CHANNEL_MSG_DATA *channel = ActDataToMsgData(gseData);
	m_vChannelMsgData.push_back(channel);
	return true;
}

///
/// @brief
///     ��ձ������
///
/// @return
///     
///
void GooseDataSeg::Clear()
{
	ReleaaeChannelMsgData(m_vChannelMsgData);
	ReleaaeChannelActData(m_vChannelActData);
}

///
/// @brief
///     GOOSE����ͨ���Ľ���
///
/// @param[in]
///     channelData - �����뵥��ͨ���ı�������
/// @param[in]
///     gseDataBuf  - ����������GOOSE�������еĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ָ������ı������ݵ�ָ��
///
GOOSE_CHANNEL_MSG_DATA *GooseDataSeg::DecodeGooseChannel(MsgDataElem *channelData, const uint8_t *gseDataBuf, const size_t bufSize)
{
	size_t dSize = 0;
	size_t offset = channelData->GetValueOffset();
	size_t offTmp = 0;
	GOOSE_CHANNEL_MSG_DATA *channel = new GOOSE_CHANNEL_MSG_DATA;
	channel->channelData = channelData;
	// ����ͨ��Ϊ�ṹ�����ͣ���������Ա
	if ( channelData->GetTag() == GOOSE_DATA_TAG_STRUCT )
	{
		while ( offTmp < channelData->GetValueLen() )
		{
			MsgDataElem *subData = CreateMsgDataElemImpl(DT_TLV_DATA);
			dSize = subData->Decode(offset+offTmp,&gseDataBuf[offset-m_GseDataOff+offTmp],channelData->GetValueLen()-offTmp);
			if ( dSize == 0 )
			{
				ReleaseMsgDataElemImpl(subData);
				return NULL;
			}
			offTmp += dSize;
			channel->vSubChannelData.push_back(DecodeGooseChannel(subData,gseDataBuf,bufSize));
		}
	}
	return channel;
}

///
/// @brief
///     GOOSE����ͨ���ı���
///
/// @param[in]
///     channelData - ����ͨ���ı�������
/// @param[in]
///     gseDataBuf  - ���������GOOSEͨ�����ݵ�����
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ʵ�ʱ������ݵĴ�С
///
size_t GooseDataSeg::CodeGooseChannel(GOOSE_CHANNEL_MSG_DATA *channelData, uint8_t *channelDataArr, const size_t arrSize)
{
	// ���뵥��ͨ��������
	size_t arrOff = 0;
	size_t cSize  = 0;
	if ( channelData->vSubChannelData.size() > 0 )
	{
		// ����ͨ��Ϊ�ṹ�����ͣ���������Ա
		size_t subArrOff = 0;
		std::vector<GOOSE_CHANNEL_MSG_DATA *>::iterator it;
		for ( it = channelData->vSubChannelData.begin(); it != channelData->vSubChannelData.end(); it ++ )
		{
			cSize = CodeGooseChannel((*it),&channelDataArr[arrOff+subArrOff],arrSize-arrOff-subArrOff);
			if ( cSize == 0 )
				return 0;
			subArrOff += cSize;
		}
		MsgDataElem *channel = CreateMsgDataElemImpl(DT_TLV_DATA);
		channel->SetTag(GOOSE_DATA_TAG_STRUCT);
		channel->SetValue(&channelDataArr[arrOff],subArrOff);
		cSize = channel->Code(&channelDataArr[arrOff],arrSize-arrOff);
		ReleaseMsgDataElemImpl(channel);
		return cSize;
	}
	else
	{
		return channelData->channelData->Code(&channelDataArr[arrOff],arrSize-arrOff);
	}
}

///
/// @brief
///     ������õ���GOOSE����ͨ���ı���ֵ->��ʵֵ
///
/// @param[in]
///     msgDataBuf  - ����GOOSE����֡���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     
///
void GooseDataSeg::ChannelMsgToAct(const uint8_t *msgDataBuf, const size_t bufSize)
{
	std::vector<GOOSE_CHANNEL_MSG_DATA *>::iterator it;
	for ( it = m_vChannelMsgData.begin(); it != m_vChannelMsgData.end(); it++ )
	{
		GOOSE_DATA *channel = MsgDataToActData((*it),msgDataBuf,bufSize);
		m_vChannelActData.push_back(channel);
	}
}

///
/// @brief
///     ����GOOSE����ͨ��������ֵ->��ʵֵ
///
/// @param[in]
///     channelData - ����õ��ĵ���ͨ���ı�������
/// @param[in]
///     msgDataBuf  - ����GOOSE����֡���ݵĻ���
/// @param[in]
///     bufSize     - �����С
///
/// @return
///     ָ����ʵֵ��ָ��
///
GOOSE_DATA *GooseDataSeg::MsgDataToActData(GOOSE_CHANNEL_MSG_DATA *channel, const uint8_t *msgDataBuf, const size_t bufSize)
{
	if ( channel == NULL || msgDataBuf == NULL )
		return NULL;

	if ( msgDataBuf[channel->channelData->GetElemOffset()] != channel->channelData->GetTag() )
		return NULL;
	
	switch (channel->channelData->GetTag())
	{
	case GOOSE_DATA_TAG_BOOLEAN:
		{
			GOOSE_BOOLEAN *result = new GOOSE_BOOLEAN;
			result->id.type   = GSEDT_BOOLEAN;
			result->id.offset = channel->channelData->GetElemOffset();
			if ( msgDataBuf[channel->channelData->GetValueOffset()] == 0 )
				result->boolVal = false;
			else
				result->boolVal = true;
			return (GOOSE_DATA *)result;
		}
		break;
	case GOOSE_DATA_TAG_BIT_STR:
		{
			GOOSE_BIT_STRING *result = new GOOSE_BIT_STRING;
			result->id.type   = GSEDT_BIT_STR;
			result->id.offset = channel->channelData->GetElemOffset();
			result->padding   = msgDataBuf[channel->channelData->GetValueOffset()];
			result->strSize   = channel->channelData->GetValueLen()-1;
			result->bitStr    = new uint8_t[result->strSize];
			memcpy(result->bitStr,&msgDataBuf[channel->channelData->GetValueOffset()+1],result->strSize);
			return (GOOSE_DATA *)result;
		}
		break;
	case GOOSE_DATA_TAG_INTEGER:
		{
			switch (channel->channelData->GetValueLen())
			{
			case 1:
				{
					GOOSE_INT8 *result = new GOOSE_INT8;
					result->id.type   = GSEDT_INT8;
					result->id.offset = channel->channelData->GetElemOffset();
					result->datVal    = msgDataBuf[channel->channelData->GetValueOffset()];
					return (GOOSE_DATA *)result;
				}
				break;
			case 2:
				{
					GOOSE_INT16 *result = new GOOSE_INT16;
					result->id.type   = GSEDT_INT16;
					result->id.offset = channel->channelData->GetElemOffset();
					int16_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],2);
					result->datVal    = BSWAP_16(tmp);
					return (GOOSE_DATA *)result;
				}
				break;
			case 3:
				{
					GOOSE_INT24 *result = new GOOSE_INT24;
					result->id.type   = GSEDT_INT24;
					result->id.offset = channel->channelData->GetElemOffset();
					int32_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],3);
					tmp = BSWAP_32(tmp);
					result->datVal    = tmp>>8;
					return (GOOSE_DATA *)result;
				}
				break;
			case 4:
				{
					GOOSE_INT32 *result = new GOOSE_INT32;
					result->id.type   = GSEDT_INT32;
					result->id.offset = channel->channelData->GetElemOffset();
					int32_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],4);
					result->datVal    = BSWAP_32(tmp);
					return (GOOSE_DATA *)result;
				}
				break;
			}
		}
		break;
	case GOOSE_DATA_TAG_UNSIGNED:
		{
			switch (channel->channelData->GetValueLen())
			{
			case 1:
				{
					GOOSE_UINT8 *result = new GOOSE_UINT8;
					result->id.type   = GSEDT_UINT8;
					result->id.offset = channel->channelData->GetElemOffset();
					result->datVal    = msgDataBuf[channel->channelData->GetValueOffset()];
					return (GOOSE_DATA *)result;
				}
				break;
			case 2:
				{
					GOOSE_UINT16 *result = new GOOSE_UINT16;
					result->id.type   = GSEDT_UINT16;
					result->id.offset = channel->channelData->GetElemOffset();
					uint16_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],2);
					result->datVal    = BSWAP_16(tmp);
					return (GOOSE_DATA *)result;
				}
				break;
			case 3:
				{
					GOOSE_UINT24 *result = new GOOSE_UINT24;
					result->id.type   = GSEDT_UINT24;
					result->id.offset = channel->channelData->GetElemOffset();
					uint32_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],3);
					tmp = BSWAP_32(tmp);
					result->datVal    = tmp>>8;
					return (GOOSE_DATA *)result;
				}
				break;
			case 4:
				{
					GOOSE_UINT32 *result = new GOOSE_UINT32;
					result->id.type   = GSEDT_UINT32;
					result->id.offset = channel->channelData->GetElemOffset();
					uint32_t tmp;
					memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()],4);
					result->datVal    = BSWAP_32(tmp);
					return (GOOSE_DATA *)result;
				}
				break;
			}
		}
		break;
	case GOOSE_DATA_TAG_FLOAT_POINT:
		{
			if ( channel->channelData->GetValueLen() == 5 &&
				 msgDataBuf[channel->channelData->GetValueOffset()] == 0x08 )
			{
				GOOSE_FLOAT *result = new GOOSE_FLOAT;
				result->id.type   = GSEDT_FLOAT;
				result->id.offset = channel->channelData->GetElemOffset();
				uint32_t tmp;
				memcpy(&tmp,&msgDataBuf[channel->channelData->GetValueOffset()+1],4);
				tmp = BSWAP_32(tmp);
				result->datVal    = *((float *)&tmp);
				return (GOOSE_DATA *)result;
			}
		}
		break;
	case GOOSE_DATA_TAG_OCTET_STR:
		{
			GOOSE_OCTET_STRING *result = new GOOSE_OCTET_STRING;
			result->id.type   = GSEDT_OCTET_STR;
			result->id.offset = channel->channelData->GetElemOffset();
			result->strSize   = channel->channelData->GetValueLen();
			result->octetStr  = new uint8_t[result->strSize];
			memcpy(result->octetStr,&msgDataBuf[channel->channelData->GetValueOffset()],result->strSize);
			return (GOOSE_DATA *)result;
		}
		break;
	case GOOSE_DATA_TAG_VISIBLE_STR:
		{
			GOOSE_VISIBLE_STRING *result = new GOOSE_VISIBLE_STRING;
			result->id.type    = GSEDT_VISIBLE_STR;
			result->id.offset  = channel->channelData->GetElemOffset();
			result->strSize    = channel->channelData->GetValueLen();
			result->visibleStr = new int8_t[result->strSize];
			memcpy(result->visibleStr,&msgDataBuf[channel->channelData->GetValueOffset()],result->strSize);
			return (GOOSE_DATA *)result;
		}
		break;
	case GOOSE_DATA_TAG_UTC_TIME:
		{
			GOOSE_UTC_TIME *result = new GOOSE_UTC_TIME;
			result->id.type = GSEDT_UTC_TIME;
			result->id.offset = channel->channelData->GetElemOffset();
			memcpy((uint8_t *)&result->t_s,&msgDataBuf[channel->channelData->GetValueOffset()],4);
			result->t_s     = BSWAP_32(result->t_s);
			memcpy((uint8_t *)&result->t_ns,&msgDataBuf[channel->channelData->GetValueOffset()+4],4);
			result->t_ns    = (uint32_t)(((uint64_t)(BSWAP_32(result->t_ns)>>8))*(1000000000)/(1<<24));
			result->t_q     = msgDataBuf[channel->channelData->GetValueOffset()+7];
			return (GOOSE_DATA *)result;
		}
		break;
	case GOOSE_DATA_TAG_STRUCT:
		{
			GOOSE_STRUCT *result = new GOOSE_STRUCT;
			result->id.type   = GSEDT_STRUCT;
			result->id.offset = channel->channelData->GetElemOffset();
			result->member.reserve(channel->vSubChannelData.size());
			std::vector<GOOSE_CHANNEL_MSG_DATA *>::iterator it;
			for ( it = channel->vSubChannelData.begin(); it != channel->vSubChannelData.end(); it++ )
			{
				GOOSE_DATA *subChannel = MsgDataToActData((*it),msgDataBuf,bufSize);
				result->member.push_back(subChannel);
			}
			return (GOOSE_DATA *)result;
		}
		break;
	default:
		{
			GOOSE_UNKNOW *result = new GOOSE_UNKNOW;
			result->id.type   = GSEDT_UNKNOW;
			result->id.offset = channel->channelData->GetElemOffset();
			result->gooseTag  = channel->channelData->GetTag();
			result->datLen    = channel->channelData->GetValueLen();
			result->datVal    = new uint8_t[result->datLen];
			memcpy(result->datVal,&msgDataBuf[channel->channelData->GetValueOffset()],result->datLen);
			return (GOOSE_DATA *)result;
		}
		;
	}
	return NULL;
}

///
/// @brief
///     ����GOOSE����ͨ������ʵֵ->����ֵ
///
/// @param[in]
///     gseData - ָ�򵥸�ͨ������ʵֵ���ݵ�ָ��
///
/// @return
///     ָ����ֵ��ָ��
///
GOOSE_CHANNEL_MSG_DATA * GooseDataSeg::ActDataToMsgData(const GOOSE_DATA *gseData)
{
	static uint8_t datArr[GOOSE_ONE_DATA_MAX_SIZE];
	size_t  arrSize = GOOSE_ONE_DATA_MAX_SIZE;
	uint8_t tag = TAG_NONE;
	int     len = 0;
	switch (gseData->type)
	{
	case GSEDT_BOOLEAN:
		{
			tag = GOOSE_DATA_TAG_BOOLEAN;
			GOOSE_BOOLEAN *tmp = (GOOSE_BOOLEAN *)gseData;
			datArr[0] = (uint8_t)tmp->boolVal;
			len = 1;
		}
		break;
	case GSEDT_BIT_STR:
		{
			tag = GOOSE_DATA_TAG_BIT_STR;
			GOOSE_BIT_STRING *tmp = (GOOSE_BIT_STRING *)gseData;
			datArr[0] = tmp->padding;
			len = tmp->strSize < (arrSize - 1) ? tmp->strSize:(arrSize - 1);
			memcpy(&datArr[1],tmp->bitStr,len);
			len += 1;
		}
		break;
	case GSEDT_INT8:
		{
			tag = GOOSE_DATA_TAG_INTEGER;
			GOOSE_INT8 *tmp = (GOOSE_INT8 *)gseData;
			len = 1;
			datArr[0] = tmp->datVal;
		}
		break;
	case GSEDT_INT16:
		{
			tag = GOOSE_DATA_TAG_INTEGER;
			GOOSE_INT16 *tmp = (GOOSE_INT16 *)gseData;
			len = 2;
			int16_t intTmp = BSWAP_16(tmp->datVal);
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_INT24:
		{
			tag = GOOSE_DATA_TAG_INTEGER;
			GOOSE_INT24 *tmp = (GOOSE_INT24 *)gseData;
			len = 3;
			int32_t intTmp = BSWAP_32(tmp->datVal);
			intTmp = intTmp >> 8;
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_INT32:
		{
			tag = GOOSE_DATA_TAG_INTEGER;
			GOOSE_INT32 *tmp = (GOOSE_INT32 *)gseData;
			len = 4;
			int32_t intTmp = BSWAP_32(tmp->datVal);
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_UINT8:
		{
			tag = GOOSE_DATA_TAG_UNSIGNED;
			GOOSE_UINT8 *tmp = (GOOSE_UINT8 *)gseData;
			len = 1;
			datArr[0] = tmp->datVal;
		}
		break;
	case GSEDT_UINT16:
		{
			tag = GOOSE_DATA_TAG_UNSIGNED;
			GOOSE_UINT16 *tmp = (GOOSE_UINT16 *)gseData;
			len = 2;
			uint16_t intTmp = BSWAP_16(tmp->datVal);
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_UINT24:
		{
			tag = GOOSE_DATA_TAG_UNSIGNED;
			GOOSE_UINT24 *tmp = (GOOSE_UINT24 *)gseData;
			len = 3;
			uint32_t intTmp = BSWAP_32(tmp->datVal);
			intTmp = intTmp >> 8;
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_UINT32:
		{
			tag = GOOSE_DATA_TAG_UNSIGNED;
			GOOSE_UINT32 *tmp = (GOOSE_UINT32 *)gseData;
			len = 4;
			uint32_t intTmp = BSWAP_32(tmp->datVal);
			memcpy(datArr,&intTmp,len);
		}
		break;
	case GSEDT_FLOAT:
		{
			tag = GOOSE_DATA_TAG_FLOAT_POINT;
			GOOSE_FLOAT *tmp = (GOOSE_FLOAT *)gseData;
			uint32_t intTmp = *((uint32_t *)&tmp->datVal);
			intTmp = BSWAP_32(intTmp);
			len    = 4;
			datArr[0] = 0x08;
			memcpy(&datArr[1],&intTmp,len);
			len++;
		}
		break;
	case GSEDT_OCTET_STR:
		{
			tag = GOOSE_DATA_TAG_OCTET_STR;
			GOOSE_OCTET_STRING *tmp = (GOOSE_OCTET_STRING *)gseData;
			len = tmp->strSize < arrSize ? tmp->strSize:arrSize;
			memcpy(datArr,tmp->octetStr,len);
		}
		break;
	case GSEDT_VISIBLE_STR:
		{
			tag = GOOSE_DATA_TAG_VISIBLE_STR;
			GOOSE_VISIBLE_STRING *tmp = (GOOSE_VISIBLE_STRING *)gseData;
			len = tmp->strSize < arrSize ? tmp->strSize:arrSize;
			memcpy(datArr,tmp->visibleStr,len);
		}
		break;
	case GSEDT_UTC_TIME:
		{
			tag = GOOSE_DATA_TAG_UTC_TIME;
			GOOSE_UTC_TIME *tmp = (GOOSE_UTC_TIME *)gseData;
			uint32_t uint32Tmp  = BSWAP_32(tmp->t_s);
			memcpy(datArr,(uint8_t *)&uint32Tmp,4);
			uint32Tmp = ((uint32_t)((uint64_t)tmp->t_ns*(1<<28)/(1000000000)))<<4;
			// �ж�ֵ�Ƿ��1
			if ( (uint32Tmp & 0xFF) >= 0x80 )
				uint32Tmp += 0x100;
			uint32Tmp = uint32Tmp & 0xFFFFFF00;
			uint32Tmp = BSWAP_32(uint32Tmp);
			memcpy(&datArr[4],(uint8_t *)&uint32Tmp,3);
			datArr[7] = tmp->t_q;
			len =  8;
		}
		break;
	case GSEDT_STRUCT:
		{
			tag = GOOSE_DATA_TAG_STRUCT;
			GOOSE_STRUCT *tmp = (GOOSE_STRUCT *)gseData;
			GOOSE_CHANNEL_MSG_DATA *channelData = new GOOSE_CHANNEL_MSG_DATA;
			channelData->channelData = NULL;
			channelData->vSubChannelData.reserve(tmp->member.size());
			std::vector<GOOSE_DATA *>::iterator it;
			for ( it = tmp->member.begin(); it != tmp->member.end(); it++ )
			{
				GOOSE_CHANNEL_MSG_DATA *subChannelData = ActDataToMsgData((*it));
				channelData->vSubChannelData.push_back(subChannelData);
			}
			return channelData;
		}
		break;
	default:
		{
			GOOSE_UNKNOW *tmp = (GOOSE_UNKNOW *)gseData;
			tag = tmp->gooseTag;
			len = tmp->datLen < arrSize ? tmp->datLen:arrSize;
			memcpy(datArr,tmp->datVal,len);
		}
	}
	GOOSE_CHANNEL_MSG_DATA *channelData = new GOOSE_CHANNEL_MSG_DATA;
	channelData->channelData = CreateMsgDataElemImpl(DT_TLV_DATA);
	channelData->channelData->SetTag(tag);
	channelData->channelData->SetValue(datArr,len);
	return channelData;
}

///
/// @brief
///     �ͷŽ���õ���GOOSEͨ����������
///
/// @param[in]
///     vt - GOOSEͨ���������ݵ�����
///
/// @return
///     
///
void GooseDataSeg::ReleaaeChannelMsgData(std::vector<GOOSE_CHANNEL_MSG_DATA *> &vt)
{
	std::vector<GOOSE_CHANNEL_MSG_DATA *>::iterator it;
	for ( it = vt.begin(); it != vt.end(); it++ )
	{
		ReleaseMsgDataElemImpl((*it)->channelData);
		if ( (*it)->vSubChannelData.size() > 0 )
		{
			ReleaaeChannelMsgData((*it)->vSubChannelData);
		}
		delete (*it);
	}
	vt.clear();
}

///
/// @brief
///     �ͷ�GOOSEͨ������ʵֵ����
///
/// @param[in]
///     vt - GOOSEͨ������ʵֵ���ݵ�����
///
/// @return
///     
///
void GooseDataSeg::ReleaaeChannelActData(std::vector<GOOSE_DATA *> &vt)
{
	std::vector<GOOSE_DATA *>::iterator it;
	for ( it = vt.begin(); it != vt.end(); it++ )
	{
		GooseDataFree((*it));
	}
	vt.clear();
}
