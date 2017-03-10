/// @file
///     sav91_pdu_seg.cpp
///
/// @brief
///     Sav91（IEC91850-9-1）采样值报文的PDU段
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
#include "../include/etypes.h"
#include "sav91_pdu_seg.h"
#include "sav91_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     最大ASDU数目
///
#define SAV91_PDU_MAX_NO_ASDU 8

///
/// @brief
///     Sav91 PDU段落中去除ASDU后的剩余部分的最大Size（不含Security）
///
#define SAV91_PDU_NO_ASDU_MAX_SIZE 6

Sav91PduSeg::Sav91PduSeg()
: m_NoASDU(0)
, m_NoASDUOff(0)
, m_PduLen(0)
, m_PduOff(0)
, m_AsduSize(0)
, m_MaxPduSize(ETHER_FRAME_MAX_LEN-26)
{
	m_AsduSubSeg.reserve(SAV91_PDU_MAX_NO_ASDU);
	for ( int i = 0; i < SAV91_PDU_MAX_NO_ASDU; i++ )
	{
		Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
}

Sav91PduSeg::~Sav91PduSeg()
{
	std::vector<Sav91AsduSeg *>::iterator it;
	for ( it = m_AsduSubSeg.begin(); it != m_AsduSubSeg.end(); it++ )
	{
		if ( *it != NULL )
		{
			delete *it;
			*it = NULL;
		}
	}
}

///
/// @brief
///     对报文帧中的SavPDU部分进行解码
///
/// @param[in]
///     offset      - 待解码的SavPDU部分在报文帧中的偏移地址
/// @param[in]
///     pduDataBuf  - 保存待解码的SavPDU部分的数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Sav91PduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	// 解析SavPDU的value部分
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV91_TAG_SAV_PDU);
	size_t dSize  = savPDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(savPDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;
	offTmp   = dSize - savPDU->GetValueLen();
	ReleaseMsgDataElemImpl(savPDU);

	// 解析noASDU
	memcpy((uint8_t *)&m_NoASDU,&pduDataBuf[offTmp],2);
	m_NoASDU    = BSWAP_16(m_NoASDU);
	m_NoASDUOff = offset + offTmp;
	offTmp += 2;

	// 解析ASDU
	for (size_t i = 0; i < m_NoASDU; i++ )
	{
		if ( offTmp >= bufSize )
		{
			m_NoASDU = i;
			break;
		}
		if ( i >= m_AsduSubSeg.size() )
		{
			Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
			m_AsduSubSeg.push_back(asduSeg);
		}

		Sav91AsduSeg *asduSeg = m_AsduSubSeg.at(i);
		if ( asduSeg == NULL )
			return 0;

		// 解析单个ASDU的value
		dSize = asduSeg->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( dSize == 0 )
			return 0;
		
		offTmp += dSize;
	}

	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，获得SavPDU数据流
///
/// @param[in]
///     pduDataArr - 保存SavPDU数据流的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存SavPDU数据流的数组的实际大小 - 成功； 0 - 失败
///
size_t Sav91PduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	size_t arrOff = 0;
	// 设置当前已添加的ASDU数
	if ( m_NoASDU == 0 )
		return 0;
	uint16_t noAsdu = BSWAP_16(m_NoASDU);
	memcpy(&pduDataArr[arrOff],(uint8_t *)&noAsdu,2);
	arrOff += 2;

	// 编码ASDU
	size_t cSize  = 0;
	for (int i = 0; i < m_NoASDU; i++ )
	{
		if ( arrOff >= arrSize )
			break;
		cSize = m_AsduSubSeg.at(i)->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// 编码SavPDU
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV91_TAG_SAV_PDU);
	savPDU->SetValue(pduDataArr,arrOff);
	cSize = savPDU->Code(pduDataArr,arrSize);
	ReleaseMsgDataElemImpl(savPDU);

	m_PduLen = cSize;
	return cSize;
}

///
/// @brief
///     获取报文帧中Sav91PDU的长度
///
/// @return
///     Sav91PDU的长度
///
size_t Sav91PduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     获取报文帧中ASDU的数目
///
/// @return
///     报文帧包含的ASDU的数目
///
uint16_t Sav91PduSeg::GetNoASDU()
{
	return m_NoASDU;
}

///
/// @brief
///     获取报文帧中ASDU的公共信息，即ASDU中的采样值数据以外的部分
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91PduSeg::GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV91_ASDU_PUBLIC &asduPublic)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfo(msgDataBuf,asduPublic);
}

///
/// @brief
///     获取报文帧中的ASDU中的采样计数器信息
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpCnt     - 保存采样计数器信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91PduSeg::GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpCntInASDU(msgDataBuf,smpCnt);
}

///
/// @brief
///     获取报文帧中的ASDU中的状态字信息
///
/// @param[in]
///     idxSeq      - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     msgDataBuf  - 保存报文帧数据的缓存
/// @param[in]
///     statusWord1 - 保存状态字1信息
/// @param[in]
///     statusWord2 - 保存状态字2信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91PduSeg::GetStatusWordsInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &statusWord1, uint16_t &statusWord2)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetStatusWordsInASDU(msgDataBuf,statusWord1,statusWord2);
}

///
/// @brief
///     获取协议定义的ASDU的采样值数据部分的通道数
///
/// @return
///     协议定义的ASDU的采样值数据部分的通道数
///
size_t Sav91PduSeg::GetASDUSmpsNum()
{
	return SAV91_ASDU_CHANNEL_NUM;
}

///
/// @brief
///     获取报文帧中ASDU中的采样值数据
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpValArr  - 保存采样瞬时值的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     数组的实际大小
///
size_t Sav91PduSeg::GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmps(msgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     获取Sav91PDU在报文帧中的偏移地址
///
/// @return
///     Sav91PDU在报文帧中的偏移地址
///
size_t Sav91PduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     获取ASDU的数目信息在报文帧中的偏移地址
///
/// @return
///     ASDU的数目信息在报文帧中的偏移地址
///
size_t Sav91PduSeg::GetNoASDUOff()
{
	return m_NoASDUOff;
}

///
/// @brief
///     获取报文帧中ASDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91PduSeg::GetASDUPublicInfoOff(const size_t idxSeq, SAV91_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return false;
	m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfoOff(asduPublicOff);
	return true;
}

///
/// @brief
///     获取某一ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     arrSize   - 数组大小
///
/// @return
///     数组的实际大小
///
size_t Sav91PduSeg::GetASDUSmpsOff(const size_t idxSeq, size_t *smpOffArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_NoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsOff(smpOffArr,arrSize);
}

///
/// @brief
///     清空编解码结果
///
/// @return
///     
///
void Sav91PduSeg::Clear()
{
	m_NoASDU    = 0;
	m_NoASDUOff = 0;
	m_PduLen    = 0;
	m_PduOff    = 0;
	m_AsduSize  = 0;
}

///
/// @brief
///     设置Sav91PDU所属的上层协议类型，以重置Sav91Pdu的最大Size
///
/// @param[in]
///     type - Sav91PDU所属的上层协议类型
///
/// @return
///     
///
void Sav91PduSeg::SetULPType(const SAV91_ULP_TYPE type)
{
	switch (type)
	{
	case SAV91_IEEE802_1Q:
		// 以太网帧最大长度减去Sav91PDU之前的部分的长度
		m_MaxPduSize = 1514 - 26;
		break;
	default:
		m_MaxPduSize = 1514 - 26;
	}
}

///
/// @brief
///     添加一个ASDU到报文帧中
///
/// @param[in]
///     asduPublic - ASDU的公共信息
/// @param[in]
///     smpValBuf  - 保存采样瞬时值的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     true - 成功； false - 失败
///
bool Sav91PduSeg::AddAnASDU(const SAV91_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize)
{
	// 记录第1个ASDU的Size
	if ( m_NoASDU == 1 )
	{
		uint8_t asduTmp[ETHER_FRAME_MAX_LEN];
		m_AsduSize = m_AsduSubSeg.at(0)->Code(asduTmp,ETHER_FRAME_MAX_LEN);
	}

	// 判断添加下一个ASDU是否会造成报文帧溢出
	if ( m_AsduSize * ( m_NoASDU + 1 ) > ( m_MaxPduSize - SAV91_PDU_NO_ASDU_MAX_SIZE ) )
		return false;

	// 添加ASDU
	if ( m_NoASDU >= m_AsduSubSeg.size() )
	{
		Sav91AsduSeg *asduSeg = new Sav91AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
	Sav91AsduSeg *asduSeg = m_AsduSubSeg.at(m_NoASDU);
	if ( asduSeg == NULL )
		return false;

	if ( asduSeg->SetASDU(asduPublic,smpValBuf,bufSize) == false )
		return false;
	
	m_NoASDU ++;
	return true;
}

