/// @file
///     sav92_asdu_seg.cpp
///
/// @brief
///     Sav92（IEC91850-9-2）采样值报文的ASDU段
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
///    |2013.09.27     |1.0.1.927  |修改ASDU中通道数的最大值为64                              |
///
#include "stdafx.h"
#include "../include/etypes.h"
#include "SAV92_asdu_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     Sav92采样值报文的ASDU的数据通道的Size
///
#define SAV92_CHANNEL_SIZE       8

///
/// @brief
///     Sav92采样值报文的ASDU的数据通道的最大通道数
///
#define SAV92_MAX_CHANNEL_NUM   64

///
/// @brief
///     Sav92采样值报文的ASDU的格式
///
typedef enum {
	SAV92_SV_ID = 0,
	SAV92_DATA_SET,
	SAV92_SMP_CNT,
	SAV92_CONF_REV,
	SAV92_REFR_TM,
	SAV92_SMP_SYNCH,
	SAV92_SMP_RATE,
	SAV92_SEQ_OF_DATA
}SAV92_ASDU_FORMAT;

///
/// @brief
///     对应上面的ASDU格式，Sav92采样值报文的ASDU各字段的符合ASN.1的Tag值列表
///
static uint8_t g_Sav92_Asdu_Tag_List[] = {
	SAV92_TAG_SV_ID,
	SAV92_TAG_DATA_SET,
	SAV92_TAG_SMP_CNT,
	SAV92_TAG_CONF_REV,
	SAV92_TAG_REFR_TM,
	SAV92_TAG_SMP_SYNCH,
	SAV92_TAG_SMP_RATE,
	SAV92_TAG_SEQ_OF_DATA
};

Sav92AsduSeg::Sav92AsduSeg()
: m_AsduData(new std::vector<MsgDataElem *>)
, m_AsduLen(0)
, m_AsduOff(0)
{
	Init();
}

Sav92AsduSeg::~Sav92AsduSeg()
{
	if ( m_AsduData != NULL )
	{
		std::vector<MsgDataElem *>::iterator it;
		for ( it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
		{
			ReleaseMsgDataElemImpl(*it);
		}
		delete m_AsduData;
		m_AsduData = NULL;
	}
}

///
/// @brief
///     对报文帧中的ASDU部分进行解码
///
/// @param[in]
///     offset      - 待解码的ASDU在报文帧中的偏移地址
/// @param[in]
///     asduDataBuf - 保存待解码的ASDU数据的缓存
/// @param[in]
///     bufSize     - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Sav92AsduSeg::Decode(const size_t offset, const uint8_t *asduDataBuf, const size_t bufSize)
{
	if ( asduDataBuf == NULL )
		return 0;

	// 提取asdu的Value信息
	size_t dSize  = 0;
	size_t offTmp = 0;
	MsgDataElem *asduInst = CreateMsgDataElemImpl(DT_TLV_DATA);
	asduInst->SetTag(SAV92_TAG_ASDU);
	dSize = asduInst->Decode(offset+offTmp,&asduDataBuf[offTmp],bufSize-offTmp);
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(asduInst);
		return 0;
	}
	m_AsduLen = dSize;
	m_AsduOff = offset;
	offTmp   += dSize - asduInst->GetValueLen();
	ReleaseMsgDataElemImpl(asduInst);

	// 按定义好的格式依次对asdu的Value部分进行解码
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;
		// 解析单个TLV数据字段
		dSize = (*it)->Decode(offset+offTmp,&asduDataBuf[offTmp],bufSize-offTmp);
		// 可选项判断
		switch ( (*it)->GetTag() )
		{
		case SAV92_TAG_DATA_SET:
			{
				if ( dSize == 0 )
					m_AsduOpts.bDataSetOpt = false;
				else
					m_AsduOpts.bDataSetOpt = true;
			}
			break;
		case SAV92_TAG_REFR_TM:
			{
				if ( dSize == 0 )
					m_AsduOpts.bRefrTmOpt = false;
				else
					m_AsduOpts.bRefrTmOpt = true;
			}
			break;
		case SAV92_TAG_SMP_RATE:
			{
				if ( dSize == 0 )
					m_AsduOpts.bSmpRateOpt = false;
				else
					m_AsduOpts.bSmpRateOpt = true;
			}
			break;
		default:
			if ( dSize == 0 )
				return 0;
		}
		offTmp += dSize;
	}
	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，获得ASDU数据流
///
/// @param[in]
///     asduDataArr - 保存ASDU数据流的数组
/// @param[in]
///     arrSize     - 数组大小
///
/// @return
///     保存ASDU数据流的数组的实际大小 - 成功； 0 - 失败
///
size_t Sav92AsduSeg::Code(uint8_t *asduDataArr, const size_t arrSize)
{
	if ( asduDataArr == NULL )
		return 0;

	size_t cSize = 0;
	size_t arrOff = 0;
	// 编码asdu的Value部分
	for ( std::vector<MsgDataElem *>::iterator it = m_AsduData->begin(); it != m_AsduData->end(); it++ )
	{
		// 编码单个字段，可选项非true，则跳过
		if ( (*it)->GetTag() == SAV92_TAG_DATA_SET && !m_AsduOpts.bDataSetOpt )
			continue;
		if ( (*it)->GetTag() == SAV92_TAG_REFR_TM  && !m_AsduOpts.bRefrTmOpt  )
			continue;
		if ( (*it)->GetTag() == SAV92_TAG_SMP_RATE && !m_AsduOpts.bSmpRateOpt )
			continue;
		cSize = (*it)->Code(&asduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
	}

	// 编码ASDU
	MsgDataElem *asduInst = CreateMsgDataElemImpl(DT_TLV_DATA);
	asduInst->SetTag(SAV92_TAG_ASDU);
	asduInst->SetValue(asduDataArr,arrOff);
	cSize = asduInst->Code(asduDataArr,arrSize);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(asduInst);
		return 0;
	}
	ReleaseMsgDataElemImpl(asduInst);
	m_AsduLen = cSize;
	return cSize;
}

///
/// @brief
///     获取ASDU的长度
///
/// @return
///     ASDU的长度
///
size_t Sav92AsduSeg::GetASDULen()
{
	return m_AsduLen;
}

///
/// @brief
///     获取ASDU的公共信息，即ASDU中的采样值数据以外的部分
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92AsduSeg::GetASDUPublicInfo(const uint8_t *msgDataBuf, SAV92_ASDU_PUBLIC &asduPublic)
{
	if ( msgDataBuf == NULL )
		return false;

	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize;

	// 获取svID的value值
	strBuf = (char *)&msgDataBuf[m_AsduData->at(SAV92_SV_ID)->GetValueOffset()];
	datSize = m_AsduData->at(SAV92_SV_ID)->GetValueLen();
	if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
		datSize = SAV92_VISIBLE_STR_MAX_SIZE-1;
	memcpy_s(asduPublic.svIDStr,SAV92_VISIBLE_STR_MAX_SIZE,strBuf,datSize);
	asduPublic.svIDStr[datSize] = '\0';

	// 获取datSet的value值
	if ( m_AsduOpts.bDataSetOpt )
	{
		strBuf = (char *)&msgDataBuf[m_AsduData->at(SAV92_DATA_SET)->GetValueOffset()];
		datSize = m_AsduData->at(SAV92_DATA_SET)->GetValueLen();
		if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
			datSize = SAV92_VISIBLE_STR_MAX_SIZE-1;
		memcpy_s(asduPublic.datSetStr,SAV92_VISIBLE_STR_MAX_SIZE,strBuf,datSize);
		asduPublic.datSetStr[datSize] = '\0';
	}
	else
	{
		asduPublic.datSetStr[0] = '\0';
	}

	// 获取smpCnt的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_CNT)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_SMP_CNT)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	asduPublic.smpCnt = BSWAP_16(uint16Tmp);

	// 获取confRev的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_CONF_REV)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_CONF_REV)->GetValueLen();
	memcpy_s((uint8_t *)&uint32Tmp,4,uint8Buf,datSize);
	asduPublic.confRev = BSWAP_32(uint32Tmp);

	// 获取refrTime的value值
	if ( m_AsduOpts.bRefrTmOpt )
	{
		uint8_t refrTm[8];
		datSize = m_AsduData->at(SAV92_REFR_TM)->GetValueLen();
		memcpy_s(refrTm,8,&msgDataBuf[m_AsduData->at(SAV92_REFR_TM)->GetValueOffset()],datSize);
		asduPublic.refrTm.ts_s  = BSWAP_32(*(uint32_t *)refrTm);
		asduPublic.refrTm.ts_ns = BSWAP_32(*(uint32_t *)&refrTm[4])>>8;
		// IEC61850中的UTC时间的小数部分等于其4~6字节转换为10进制后除上2的24次方，而微秒值则需再乘上10的9次方
		asduPublic.refrTm.ts_ns = (uint32_t)(((uint64_t)asduPublic.refrTm.ts_ns)*(1000000000)/(1<<24));
		asduPublic.refrTm.ts_q  = refrTm[7];
	}
	else
	{
		asduPublic.refrTm.ts_s  = 0;
		asduPublic.refrTm.ts_ns = 0;
		asduPublic.refrTm.ts_q  = 0;
	}

	// 获取smpSynch的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_SYNCH)->GetValueOffset()];
	asduPublic.smpSynch = !( (*uint8Buf) == 0 );

	// 获取smpRate的value值
	if ( m_AsduOpts.bSmpRateOpt )
	{
		uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_RATE)->GetValueOffset()];
		datSize   = m_AsduData->at(SAV92_SMP_RATE)->GetValueLen();
		memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
		asduPublic.smpRate = BSWAP_16(uint16Tmp);
	}
	else
	{
		asduPublic.smpRate = 0;
	}

	return true;
}

///
/// @brief
///     获取报文帧中的ASDU中的采样计数器信息
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpCnt     - 保存采样计数器信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92AsduSeg::GetSmpCntInASDU(const uint8_t *msgDataBuf, uint16_t &smpCnt)
{
	if ( msgDataBuf == NULL )
		return false;

	uint16_t uint16Tmp = 0;
	size_t   datSize;

	// 获取smpCnt的value值
	uint8_t *uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_CNT)->GetValueOffset()];
	datSize   = m_AsduData->at(SAV92_SMP_CNT)->GetValueLen();
	memcpy_s((uint8_t *)&uint16Tmp,2,uint8Buf,datSize);
	smpCnt = BSWAP_16(uint16Tmp);

	return true;
}

///
/// @brief
///     获取报文帧中的ASDU中的同步标识位信息
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpSynch   - 保存同步标识位信息
///
/// @return
///     true - 成功； false - 失败
///
bool Sav92AsduSeg::GetSmpSynchInASDU(const uint8_t *msgDataBuf, bool &smpSynch)
{
	if ( msgDataBuf == NULL )
		return false;

	// 获取smpSynch的value值
	uint8_t *uint8Buf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SMP_SYNCH)->GetValueOffset()];
	smpSynch = !( (*uint8Buf) == 0 );

	return true;
}

///
/// @brief
///     获取ASDU中的采样值数据部分的通道数
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t Sav92AsduSeg::GetASDUSmpsNum()
{
	return m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemLen()/SAV92_CHANNEL_SIZE;
}

///
/// @brief
///     获取ASDU中的采样值数据
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     smpValArr  - 保存采样瞬时值的数组
/// @param[in]
///     smpQltArr  - 保存采样瞬时值的品质值的数组
/// @param[in]
///     arrSize    - 两数组大小
///
/// @return
///     数组的实际大小
///
size_t Sav92AsduSeg::GetASDUSmps(const uint8_t *msgDataBuf, uint32_t *smpValArr, uint32_t *smpQltArr, const size_t arrSize)
{
	if ( msgDataBuf == NULL )
		return 0;
	if ( smpValArr == NULL && smpQltArr == NULL )
		return 0;

	// 提取Sequence of Data 的value值
	size_t   datLen = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetValueLen();
	uint8_t *datBuf = (uint8_t *)&msgDataBuf[m_AsduData->at(SAV92_SEQ_OF_DATA)->GetValueOffset()];

	// 获取各通道数据
	size_t i = 0;
	size_t bufOff = 0;
	uint32_t uint32Tmp = 0;
	for ( i = 0; i < datLen/SAV92_CHANNEL_SIZE ; i++ )
	{
		if ( i >= arrSize )
			break;
		if ( smpValArr != NULL )
		{
			memcpy((uint8_t *)&uint32Tmp,&datBuf[bufOff],4);
			smpValArr[i] = BSWAP_32(uint32Tmp);
		}
		if ( smpQltArr != NULL )
		{
			memcpy((uint8_t *)&uint32Tmp,&datBuf[bufOff+SAV92_CHANNEL_SIZE/2],4);
			smpQltArr[i] = BSWAP_32(uint32Tmp);
		}
		bufOff      += SAV92_CHANNEL_SIZE;
	}
	return i;
}

///
/// @brief
///     获取ASDU的中Sequence of Data项（TLV形式）的长度
///
/// @return
///     Sequence of Data项（TLV形式）的长度
///
size_t Sav92AsduSeg::GetSeqOfDatLen()
{
	return m_AsduLen - ( m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemOffset() - m_AsduOff );
}

///
/// @brief
///     获取ASDU中的可选项的配置信息
///
/// @param[in]
///     asduOpts - 保存可选项配置信息的结构体
///
/// @return
///     
///
void Sav92AsduSeg::GetASDUOpts(SAV92_ASDU_OPTS &asduOpts)
{
	asduOpts = m_AsduOpts;
}

///
/// @brief
///     获取ASDU在报文帧中的偏移地址
///
/// @return
///     ASDU在报文帧中的偏移地址
///
size_t Sav92AsduSeg::GetASDUOff()
{
	return m_AsduOff;
}

///
/// @brief
///     获取ASDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
///
/// @return
///     
///
void Sav92AsduSeg::GetASDUPublicInfoOff(SAV92_ASDU_PUBLIC_OFFSET &asduPublicOff)
{
	asduPublicOff.svIDOff     = m_AsduData->at(SAV92_SV_ID)->GetElemOffset();
	asduPublicOff.datSetOff   = m_AsduData->at(SAV92_DATA_SET)->GetElemOffset();
	asduPublicOff.smpCntOff   = m_AsduData->at(SAV92_SMP_CNT)->GetElemOffset();
	asduPublicOff.confRevOff  = m_AsduData->at(SAV92_CONF_REV)->GetElemOffset();
	asduPublicOff.refrTmOff   = m_AsduData->at(SAV92_REFR_TM)->GetElemOffset();
	asduPublicOff.smpSynchOff = m_AsduData->at(SAV92_SMP_SYNCH)->GetElemOffset();
	asduPublicOff.smpRateOff  = m_AsduData->at(SAV92_SMP_RATE)->GetElemOffset();
}

///
/// @brief
///     获取ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     seqOfDataOff - 采样瞬时值序列(TLV)的偏移地址
/// @param[in]
///     smpValOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     smpQltOffArr - 保存采样瞬时值的品质值的偏移地址信息的数组
/// @param[in]
///     arrSize      - 数组的大小
///
/// @return
///     数组的实际大小
///
size_t Sav92AsduSeg::GetASDUSmpsOff(size_t &seqOfDataOff, size_t *smpValOffArr, size_t *smpQltOffArr, const size_t arrSize)
{
	if ( smpQltOffArr == NULL && smpValOffArr == NULL )
		return 0;

	seqOfDataOff  = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemOffset();
	size_t datLen = m_AsduData->at(SAV92_SEQ_OF_DATA)->GetElemLen();

	// 获取第一个通道的偏移地址
	if ( smpValOffArr != NULL )
		smpValOffArr[0] = m_AsduOff + m_AsduLen - datLen;
	if ( smpQltOffArr != NULL )
		smpQltOffArr[0] = m_AsduOff + m_AsduLen - datLen + SAV92_CHANNEL_SIZE/2;

	// 获取剩余通道的偏移地址
	size_t i;
	for ( i = 1; i < datLen/SAV92_CHANNEL_SIZE ; i++ )
	{
		if ( i == arrSize )
			break;
		if ( smpValOffArr != NULL )
			smpValOffArr[i] = smpValOffArr[i-1] + SAV92_CHANNEL_SIZE;
		if ( smpQltOffArr != NULL )
			smpQltOffArr[i] = smpQltOffArr[i-1] + SAV92_CHANNEL_SIZE;
	}
	return i;
}

///
/// @brief
///     设置ASDU信息
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
bool Sav92AsduSeg::SetASDU(const SAV92_ASDU_PUBLIC &asduPublic, const uint32_t *smpValBuf, const uint32_t *smpQltBuf, const size_t bufSize)
{
	if ( bufSize > SAV92_MAX_CHANNEL_NUM || smpValBuf == NULL || smpQltBuf == NULL )
		return false;
	uint16_t uint16Tmp = 0;
	uint32_t uint32Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	char    *strBuf    = NULL;

	// 设置svID的value值
	size_t datSize = strlen(asduPublic.svIDStr);
	if ( datSize >= SAV92_VISIBLE_STR_MAX_SIZE )
		m_AsduData->at(SAV92_SV_ID)->SetValue((uint8_t *)asduPublic.svIDStr,SAV92_VISIBLE_STR_MAX_SIZE-1);
	else
		m_AsduData->at(SAV92_SV_ID)->SetValue((uint8_t *)asduPublic.svIDStr,datSize);

	// 设置datSet的value值
	if ( m_AsduOpts.bDataSetOpt )
	{
		datSize  = strlen(asduPublic.datSetStr);
		if ( datSize > SAV92_VISIBLE_STR_MAX_SIZE )
			m_AsduData->at(SAV92_DATA_SET)->SetValue((uint8_t *)asduPublic.datSetStr,SAV92_VISIBLE_STR_MAX_SIZE);
		else
			m_AsduData->at(SAV92_DATA_SET)->SetValue((uint8_t *)asduPublic.datSetStr,datSize);
	}
	else
	{
		m_AsduData->at(SAV92_DATA_SET)->SetValue(NULL,0);
	}

	// 设置smpCnt的value值
	uint16Tmp = BSWAP_16(asduPublic.smpCnt);
	m_AsduData->at(SAV92_SMP_CNT)->SetValue((uint8_t *)&uint16Tmp,2);

	// 设置confRev的value值
	uint32Tmp = BSWAP_32(asduPublic.confRev);
	m_AsduData->at(SAV92_CONF_REV)->SetValue((uint8_t *)&uint32Tmp,4);

	// 设置refrTm的value值
	if ( m_AsduOpts.bRefrTmOpt )
	{
		uint8_t refrTm[8];
		uint32Tmp = BSWAP_32(asduPublic.refrTm.ts_s);
		memcpy(refrTm,(uint8_t *)&uint32Tmp,4);
		uint32Tmp = ((uint32_t)((uint64_t)asduPublic.refrTm.ts_ns*(1<<28)/(1000000000)))<<4;
		// 判断值是否进1
		if ( (uint32Tmp & 0xFF) >= 0x80 )
			uint32Tmp += 0x100;
		uint32Tmp = uint32Tmp & 0xFFFFFF00;
		uint32Tmp = BSWAP_32(uint32Tmp);
		memcpy(&refrTm[4],(uint8_t *)&uint32Tmp,3);
		refrTm[7] = asduPublic.refrTm.ts_q;
		m_AsduData->at(SAV92_REFR_TM)->SetValue(refrTm,8);
	}
	else
	{
		m_AsduData->at(SAV92_REFR_TM)->SetValue(NULL,0);
	}

	// 设置smpSynch的value值
	uint8Tmp = asduPublic.smpSynch;
	m_AsduData->at(SAV92_SMP_SYNCH)->SetValue(&uint8Tmp,1);

	// 设置smpRate的value值
	if ( m_AsduOpts.bSmpRateOpt )
	{
		uint16Tmp = BSWAP_16(asduPublic.smpRate);
		m_AsduData->at(SAV92_SMP_RATE)->SetValue((uint8_t *)&uint16Tmp,2);
	}
	else
	{
		m_AsduData->at(SAV92_SMP_RATE)->SetValue(NULL,0);
	}

	// 设置Sequence of Data的value值
	static uint8_t datBuf[ETHER_FRAME_MAX_LEN];
	int bufLen = 0;
	for ( size_t i = 0; i < bufSize ; i++ )
	{
		uint32Tmp = BSWAP_32(smpValBuf[i]);
		memcpy(&datBuf[bufLen],(uint8_t *)&uint32Tmp,4);
		uint32Tmp = BSWAP_32(smpQltBuf[i]);
		memcpy(&datBuf[bufLen+4],(uint8_t *)&uint32Tmp,4);
		bufLen += SAV92_CHANNEL_SIZE;
	}
	m_AsduData->at(SAV92_SEQ_OF_DATA)->SetValue(datBuf,bufLen);

	return true;
}

///
/// @brief
///     设置ASDU中的可选项信息
///
/// @param[in]
///     asduOpts - 可选项信息
///
/// @return
///     
///
void Sav92AsduSeg::SetASDUOpts(SAV92_ASDU_OPTS &asduOpts)
{
	m_AsduOpts = asduOpts;
}

void Sav92AsduSeg::Init()
{
	// 初始化可选项的配置
	m_AsduOpts.bDataSetOpt = false;
	m_AsduOpts.bRefrTmOpt  = false;
	m_AsduOpts.bSmpRateOpt = false;

	// 初始化ASDU Vector，并设置相应字段的Tag值
	m_AsduData->reserve(sizeof(g_Sav92_Asdu_Tag_List)/sizeof(uint8_t));
	for ( int i=0; i < sizeof(g_Sav92_Asdu_Tag_List)/sizeof(uint8_t); i++ )
	{
		m_AsduData->push_back(CreateMsgDataElemImpl(DT_TLV_DATA));
		m_AsduData->at(i)->SetTag(g_Sav92_Asdu_Tag_List[i]);
	}
}
