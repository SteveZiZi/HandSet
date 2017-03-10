/// @file
///     sav92_pdu_seg.cpp
///
/// @brief
///     Sav92（IEC91850-9-2）采样值报文的SavPDU段
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
///    |2013.11.15     |1.0.1.1115 |将对ASDU可选项的设置提前至AddAnASDU()之前，避免在预算9-2报文长度时发生错误|
///
#include "stdafx.h"
#include "../include/etypes.h"
#include "Sav92_pdu_seg.h"
#include "Sav92_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     最大ASDU数目
///
#define SAV92_PDU_MAX_NO_ASDU 8

///
/// @brief
///     Sav92 PDU段落中去除ASDU后的剩余部分的最大Size（不含Security）
///
#define SAV92_PDU_NO_ASDU_MAX_SIZE1 11

///
/// @brief
///     Sav92 PDU段落中去除ASDU后的剩余部分的最大Size（含Security）
///
#define SAV92_PDU_NO_ASDU_MAX_SIZE2 48

Sav92PduSeg::Sav92PduSeg()
: m_pNoASDU(CreateMsgDataElemImpl(DT_TLV_DATA))
, m_pSecurity(CreateMsgDataElemImpl(DT_TLV_DATA))
, m_PduLen(0)
, m_PduOff(0)
, m_AsduSize(0)
, m_nNoASDU(0)
, m_MaxPduSize(ETHER_FRAME_MAX_LEN-26)
{
	m_pNoASDU->SetTag(SAV92_TAG_NO_ASDU);
	m_pSecurity->SetTag(SAV92_TAG_SECURITY);
	m_PduOpts.bSecurityOpt = false;
	m_PduOpts.bDataSetOpt  = false;
	m_PduOpts.bRefrTmOpt   = false;
	m_PduOpts.bSmpRateOpt  = false;
	m_AsduSubSeg.reserve(SAV92_PDU_MAX_NO_ASDU);
	for ( int i = 0; i < SAV92_PDU_MAX_NO_ASDU; i++ )
	{
		Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
}

Sav92PduSeg::~Sav92PduSeg()
{
	ReleaseMsgDataElemImpl(m_pNoASDU);
	ReleaseMsgDataElemImpl(m_pSecurity);
	std::vector<Sav92AsduSeg *>::iterator it;
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
///     对报文帧中的Sav92PDU部分进行解码
///
/// @param[in]
///     offset      - 待解码的Sav92PDU在报文帧中的偏移地址
/// @param[in]
///     pduDataBuf  - 保存待解码的Sav92PDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Sav92PduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	if ( pduDataBuf == NULL )
		return 0;

	// 解析savPDU的value信息
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV92_TAG_SAV_PDU);
	size_t dSize  = savPDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(savPDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;
	offTmp  += dSize - savPDU->GetValueLen();
	ReleaseMsgDataElemImpl(savPDU);

	// 解析noASDU
	dSize = m_pNoASDU->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
		return 0;
	offTmp += dSize;

	// 获取noASDU的value值，即ASDU的个数
	m_nNoASDU = 0;
	size_t noAsduLen = m_pNoASDU->GetValueLen();
	memcpy((uint8_t *)&m_nNoASDU,&pduDataBuf[offTmp-noAsduLen],noAsduLen);
	if ( noAsduLen > 1 )
		m_nNoASDU = BSWAP_16(m_nNoASDU);
	if ( m_nNoASDU == 0 )
		return 0;

	// 解析Security
	dSize = m_pSecurity->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
		m_PduOpts.bSecurityOpt = false;
	else
		m_PduOpts.bSecurityOpt = true;
	offTmp += dSize;
	
	// 解析Sequence of ASDU的value信息
	MsgDataElem *seqOfASDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	seqOfASDU->SetTag(SAV92_TAG_SEQ_ASDU);
	dSize = seqOfASDU->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(seqOfASDU);
		return 0;
	}
	offTmp += dSize - seqOfASDU->GetValueLen();
	ReleaseMsgDataElemImpl(seqOfASDU);

	// 解析Sequence of ASDU
	for (size_t i = 0; i < m_nNoASDU; i++ )
	{
		if ( offTmp >= bufSize )
		{
			m_nNoASDU = i;
			if ( m_nNoASDU == 0 )
				return 0;
			break;
		}
		// 解析单个ASDU的value
		if ( i >= m_AsduSubSeg.size() )
		{
			Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
			m_AsduSubSeg.push_back(asduSeg);
		}
		
		Sav92AsduSeg *asduSeg = m_AsduSubSeg.at(i);
		if ( asduSeg == NULL )
			return 0;

		dSize = asduSeg->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( dSize == 0 )
			return 0;
		
		offTmp += dSize;
	}

	// 更新可选项配置信息
	SAV92_ASDU_OPTS asduOpts;
	m_AsduSubSeg.at(0)->GetASDUOpts(asduOpts);
	m_PduOpts.bDataSetOpt = asduOpts.bDataSetOpt;
	m_PduOpts.bRefrTmOpt  = asduOpts.bRefrTmOpt;
	m_PduOpts.bSmpRateOpt = asduOpts.bSmpRateOpt;

	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，获得Sav92PDU数据流
///
/// @param[in]
///     pduDataArr - 保存Sav92PDU数据流的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存Sav92PDU数据流的数组的实际大小 - 成功； 0 - 失败
///
size_t Sav92PduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	if ( pduDataArr == NULL )
		return 0;

	// 获取ASDU的数目
	if ( m_nNoASDU == 0 )
		return 0;

	size_t cSize = 0;
	size_t arrOff = 0;

	// 编码noASDU
	if ( ( m_nNoASDU & 0xFF00 ) == 0 )
	{
		uint8_t uint8Tmp;
		uint8Tmp = (uint8_t)m_nNoASDU;
		m_pNoASDU->SetValue(&uint8Tmp,1);
	}
	else
	{
		uint16_t uint16Tmp;
		uint16Tmp = BSWAP_16(m_nNoASDU);
		m_pNoASDU->SetValue((uint8_t *)&uint16Tmp,2);
	}
	cSize = m_pNoASDU->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
		return 0;
	arrOff += cSize;

	// 编码Security
	if ( m_PduOpts.bSecurityOpt )
	{
		cSize = m_pSecurity->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// 编码Sequence of ASDU的value值
	size_t asduLen = 0;
	for (int i = 0; i < m_nNoASDU; i++ )
	{
		if ( arrOff + asduLen >= arrSize )
			break;
		// 编码单个ASDU
		cSize = m_AsduSubSeg.at(i)->Code(&pduDataArr[arrOff+asduLen],arrSize-arrOff-asduLen);
		if ( cSize == 0 )
			return 0;
		asduLen += cSize;
	}

	// 编码Sequence of ASDU
	MsgDataElem *seqOfASDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	seqOfASDU->SetTag(SAV92_TAG_SEQ_ASDU);
	seqOfASDU->SetValue(&pduDataArr[arrOff],asduLen);
	cSize = seqOfASDU->Code(&pduDataArr[arrOff],arrSize-arrOff);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(seqOfASDU);
		return 0;
	}
	arrOff += cSize;
	ReleaseMsgDataElemImpl(seqOfASDU);

	// 编码savPDU
	MsgDataElem *savPDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	savPDU->SetTag(SAV92_TAG_SAV_PDU);
	savPDU->SetValue(pduDataArr,arrOff);
	cSize = savPDU->Code(pduDataArr,arrSize);
	ReleaseMsgDataElemImpl(savPDU);
	m_PduLen = cSize;

	return cSize;
}

///
/// @brief
///     获取报文帧中Sav92PDU(TLV形式)的长度
///
/// @return
///     Sav92PDU的长度
///
size_t Sav92PduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     获取报文帧中ASDU的数目
///
/// @return
///     该报文帧包含的ASDU的数目
///
uint16_t Sav92PduSeg::GetNoASDU()
{
	return m_nNoASDU;
//	return (uint16_t)m_AsduSubSeg.size();
}

///
/// @brief
///     获取报文帧中某一ASDU（TLV形式）的长度
///
/// @return
///     ASDU（TLV形式）的长度
///
size_t Sav92PduSeg::GetASDULen(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDULen();
}

///
/// @brief
///     获取报文帧中某一ASDU的Sequence of Data（TLV形式）的长度
///
/// @return
///     Sequence of Data（TLV形式）的长度
///
size_t Sav92PduSeg::GetSeqOfDatLen(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetSeqOfDatLen();
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
bool Sav92PduSeg::GetASDUPublicInfo(const size_t idxSeq, const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
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
bool Sav92PduSeg::GetSmpCntInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpCntInASDU(msgDataBuf,smpCnt);
}


///
/// @brief
///     获取报文帧中的ASDU中的同步标识位信息
///
/// @param[in]
///     idxSeq     - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpSynch   - 保存同步标识位信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92PduSeg::GetSmpSynchInASDU(const size_t idxSeq, const uint8_t *msgDataBuf, bool &smpSynch)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;

	return m_AsduSubSeg.at(idxSeq)->GetSmpSynchInASDU(msgDataBuf,smpSynch);
}

///
/// @brief
///     获取某一ASDU中的采样值数据部分的通道数
///
/// @param[in]
///     idxSeq  - 报文帧中ASDU的顺序号，从0开始
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t Sav92PduSeg::GetASDUSmpsNum(const size_t idxSeq)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU)
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsNum();
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
///     smpQltArr  - 保存采样瞬时值的品质值的数组
/// @param[in]
///     arrSize    - 两数组的大小
///
/// @return
///     数组的实际大小
///
size_t Sav92PduSeg::GetASDUSmps(const size_t idxSeq, const uint8_t *msgDataBuf, uint32_t *smpValArr,
								uint32_t *smpQltArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmps(msgDataBuf,smpValArr,smpQltArr,arrSize);
}

///
/// @brief
///     获取SavPDU中的可选项的配置信息
///
/// @param[in]
///     asduOpts - 保存可选项配置信息的结构体
///
/// @return
///     
///
void Sav92PduSeg::GetSavPduOpts(SAV92_SAVPDU_OPTS &pduOpts)
{
	pduOpts = m_PduOpts;
}

///
/// @brief
///     获取Sav92PDU在报文帧中的偏移地址
///
/// @return
///     Sav92PDU在报文帧中的偏移地址
///
size_t Sav92PduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     获取ASDU的数目信息在报文帧中的偏移地址
///
/// @return
///     报文帧中的ASDU的数目信息的偏移地址
///
size_t Sav92PduSeg::GetNoASDUOff()
{
	return m_pNoASDU->GetElemOffset();
}

///
/// @brief
///     获取某个ASDU在报文帧中的偏移地址
///
/// @param[in]
///     idxSeq    - 报文帧中ASDU的顺序号，从0开始
///
/// @return
///     ASDU在报文帧中的偏移地址
///
size_t Sav92PduSeg::GetASDUOff(const size_t idxSeq)
{
	return m_AsduSubSeg.at(idxSeq)->GetASDUOff();
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
bool Sav92PduSeg::GetASDUPublicInfoOff(const size_t idxSeq, SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return false;
	m_AsduSubSeg.at(idxSeq)->GetASDUPublicInfoOff(asduPublicOff);
	return true;
}

///
/// @brief
///     获取ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     idxSeq       - 报文帧中ASDU的顺序号，从0开始
/// @param[in]
///     seqOfDataOff - 采样瞬时值序列的偏移地址
/// @param[in]
///     smpValOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     smpQltOffArr - 保存采样瞬时值的品质值的偏移地址信息的数组
/// @param[in]
///     arrSize      - 两数组的大小
///
/// @return
///     数组的实际大小
///
size_t Sav92PduSeg::GetASDUSmpsOff(const size_t idxSeq, size_t &seqofDataOff, size_t *smpValOffArr, size_t *smpQltOffArr, const size_t arrSize)
{
	if ( idxSeq >= m_AsduSubSeg.size() || idxSeq >= m_nNoASDU )
		return 0;
	return m_AsduSubSeg.at(idxSeq)->GetASDUSmpsOff(seqofDataOff,smpValOffArr,smpQltOffArr,arrSize);
}

///
/// @brief
///     清空编解码结果
///
/// @return
///     
///
void Sav92PduSeg::Clear()
{
	m_nNoASDU = 0;
	m_pNoASDU->SetValue(NULL,0);
	m_pSecurity->SetValue(NULL,0);
	m_PduLen    = 0;
	m_PduOff    = 0;
}

///
/// @brief
///     设置Sav92PDU所属的上层协议类型，以重置Sav92Pdu的最大Size
///
/// @param[in]
///     type - Sav92PDU所属的上层协议类型
///
/// @return
///     
///
void Sav92PduSeg::SetULPType(const SAV92_ULP_TYPE type)
{
	switch (type)
	{
	case SAV92_IEEE802_1Q:
		// 以太网帧最大长度减去Sav92PDU之前的部分的长度
		m_MaxPduSize = ETHER_FRAME_MAX_LEN - 26;
		break;
	default:
		m_MaxPduSize = ETHER_FRAME_MAX_LEN - 26;
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
///     smpQltBuf  - 保存采样瞬时值的品质值的缓存
/// @param[in]
///     bufSize    - 缓存的大小,即通道数
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92PduSeg::AddAnASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf, const size_t &bufSize)
{
	// 记录第1个ASDU的Size,同一帧中ASDU的格式一致
	if ( m_nNoASDU == 1 )
	{
		uint8_t asduTmp[ETHER_FRAME_MAX_LEN];
		m_AsduSize = m_AsduSubSeg.at(0)->Code(asduTmp,ETHER_FRAME_MAX_LEN);
	}

	// 判断添加下一个ASDU是否会造成报文帧溢出
	if ( m_PduOpts.bSecurityOpt )
	{
		if ( m_AsduSize * ( m_nNoASDU + 1 ) > ( m_MaxPduSize - SAV92_PDU_NO_ASDU_MAX_SIZE2 ) )
			return false;
	}
	else
	{
		if ( m_AsduSize * ( m_nNoASDU + 1 ) > ( m_MaxPduSize - SAV92_PDU_NO_ASDU_MAX_SIZE1 ) )
			return false;
	}

	// 添加ASDU
	if ( m_nNoASDU >= m_AsduSubSeg.size() )
	{
		Sav92AsduSeg *asduSeg = new Sav92AsduSeg;
		m_AsduSubSeg.push_back(asduSeg);
	}
	Sav92AsduSeg *asduSeg = m_AsduSubSeg.at(m_nNoASDU);
	if ( asduSeg == NULL )
		return false;

	// 设置可选项
	SAV92_ASDU_OPTS asduOpts = {
		m_PduOpts.bDataSetOpt,
		m_PduOpts.bRefrTmOpt,
		m_PduOpts.bSmpRateOpt
	};
	asduSeg->SetASDUOpts(asduOpts);
	// 设置ASDU信息
	if ( asduSeg->SetASDU(asduPublic,smpValBuf,smpQltBuf,bufSize) == false )
		return false;

	m_nNoASDU++;
	return true;
}

///
/// @brief
///     设置SavPDU中的可选项配置信息，必须在AddAnASDU()之前设置
///
/// @param[in]
///     asduOpts - 可选项配置信息
///
/// @return
///     
///
void Sav92PduSeg::SetSavPduOpts(const SAV92_SAVPDU_OPTS &pduOpts)
{
	m_PduOpts = pduOpts;
}
