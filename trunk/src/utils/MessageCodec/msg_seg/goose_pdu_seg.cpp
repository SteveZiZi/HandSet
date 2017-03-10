/// @file
///     ethernetII_seg.cpp
///
/// @brief
///     GOOSE报文帧中的GoosePDU段
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
///    |2013.09.27     |1.0.1.927  |GOOSE报文组帧：固定stNum和sqNum的长度为4字节              |
///
#include "stdafx.h"
#include "goose_pdu_seg.h"
#include "goose_data_seg.h"
#include "../msg_data_elem/msg_data_elem.h"
#include "bswap.h"

///
/// @brief
///     GOOSE PDU段落中去除Data序列后的剩余部分的最大Size（不含Security）
///
#define GOOSE_PDU_NO_DATA_MAX_SIZE 247

///
/// @brief
///     GoosePdu中gocbRef项所允许的最大Size
///
#define GOOSE_PDU_GOCB_REF_MAX_SIZE 65

///
/// @brief
///     GoosePdu中datSet项所允许的最大Size
///
#define GOOSE_PDU_DATA_SET_MAC_SIZE 65

///
/// @brief
///     GoosePdu中goID项所允许的最大Size
///
#define GOOSE_PDU_GO_ID_MAX_SIZE    65

///
/// @brief
///     协议中GoosePdu中公共部分（数据序列AllData以外的部分）的格式
///
typedef enum{
	GOOSE_PDU_GOCB_REF = 0,
	GOOSE_PDU_TIME_ALLOWED,
	GOOSE_PDU_DATA_SET,
	GOOSE_PDU_GO_ID,
	GOOSE_PDU_EVENT_TIME,
	GOOSE_PDU_ST_NUM,
	GOOSE_PDU_SQ_NUM,
	GOOSE_PDU_TEST,
	GOOSE_PDU_CONF_REV,
	GOOSE_PDU_NDS_COM,
	GOOSE_PDU_NUM_DS_ENTRIES,
	GOOSE_PDU_SECURITY
}GOOSE_PDU_PUBLIC_TBL;

///
/// @brief
///     协议中GoosePdu中公共部分（数据序列AllData以外的部分）的Tag值列表
///
static const uint8_t g_Goose_Pdu_Public_Tag_List[] = {
	GOOSE_TAG_GOCB_REF,
	GOOSE_TAG_TIME_ALLOWED,
	GOOSE_TAG_DATA_SET,
	GOOSE_TAG_GO_ID,
	GOOSE_TAG_EVENT_TIME,
	GOOSE_TAG_ST_NUM,
	GOOSE_TAG_SQ_NUM,
	GOOSE_TAG_TEST,
	GOOSE_TAG_CONF_REV,
	GOOSE_TAG_NDS_COM,
	GOOSE_TAG_NUM_DS_ENTRIES,
	GOOSE_TAG_SECURITY
};

GoosePduSeg::GoosePduSeg()
: m_PduPblc(new std::vector<MsgDataElem *>)
, m_GooseData(new GooseDataSeg)
, m_bSecurityOpt(false)
, m_GooseDataSize(0)
, m_MaxPduSize(1514-26)
{
	m_PduPblc->reserve(sizeof(g_Goose_Pdu_Public_Tag_List)/sizeof(uint8_t));
	for ( size_t i = 0; i < sizeof(g_Goose_Pdu_Public_Tag_List)/sizeof(uint8_t); i++ )
	{
		MsgDataElem *newElem = CreateMsgDataElemImpl(DT_TLV_DATA);
		newElem->SetTag(g_Goose_Pdu_Public_Tag_List[i]);
		m_PduPblc->push_back(newElem);
	}
}

GoosePduSeg::~GoosePduSeg()
{
	if ( m_PduPblc != NULL )
	{
		std::vector<MsgDataElem *>::iterator it;
		for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
		{
			ReleaseMsgDataElemImpl(*it);
		}
		delete m_PduPblc;
		m_PduPblc = NULL;
	}
	if ( m_GooseData != NULL )
	{
		delete m_GooseData;
		m_GooseData = NULL;
	}
}

///
/// @brief
///     对报文帧中的GoosePdu进行解码
///
/// @param[in]
///     offset     - 待解码的GoosePdu数据数据在报文帧中的偏移地址
/// @param[in]
///     pduDataBuf - 保存待解码的GoosePdu数据数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t GoosePduSeg::Decode(const size_t offset, const uint8_t *pduDataBuf, const size_t bufSize)
{
	// 提取goosePdu的Value信息
	MsgDataElem *goosePDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	goosePDU->SetTag(GOOSE_TAG_GOOSE_PDU);
	size_t dSize  = goosePDU->Decode(offset,pduDataBuf,bufSize);
	size_t offTmp = 0;
	if ( dSize == 0 )
	{
		ReleaseMsgDataElemImpl(goosePDU);
		return 0;
	}
	m_PduLen = dSize;
	m_PduOff = offset;	
	offTmp  += dSize - goosePDU->GetValueLen();
	ReleaseMsgDataElemImpl(goosePDU);

	// 按定义好的格式依次对goosePdu的Value部分进行解码
	std::vector<MsgDataElem *>::iterator it;
	for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
	{
		if ( offTmp >= bufSize )
			break;
		// 根据定义好的格式，依次解析单个TLV数据项
		dSize = (*it)->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
		if ( (*it)->GetTag() == GOOSE_TAG_SECURITY )
		{
			if ( dSize == 0 )
				m_bSecurityOpt = false;
			else
				m_bSecurityOpt = true;
		}
		else
		{
			if ( dSize == 0 )
				return 0;
		}	
		offTmp += dSize;

		// 当前项为numDatSetEntries，则解析紧跟其后的数据序列（AllData）
		if ( (*it)->GetTag() == GOOSE_TAG_NUM_DS_ENTRIES )
		{
			dSize = m_GooseData->Decode(offset+offTmp,&pduDataBuf[offTmp],bufSize-offTmp);
			if ( dSize == 0 )
				return 0;
			offTmp += dSize;
		}
	}
	return offTmp;
}

///
/// @brief
///     根据设置的参数进行编码，得到GoosePdu
///
/// @param[in]
///     pduDataArr - 保存GoosePdu数据的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存GoosePdu数据的数组的实际大小 - 成功； 0 - 失败
///
size_t GoosePduSeg::Code(uint8_t *pduDataArr, const size_t arrSize)
{
	size_t cSize  = 0;
	size_t arrOff = 0;
	std::vector<MsgDataElem *>::iterator it;

	// 编码goosePdu的Value部分
	for ( it = m_PduPblc->begin(); it != m_PduPblc->end(); it++ )
	{
		// 根据定义好的格式，依次编码单个TLV数据项
		if ( arrOff >= arrSize )
			break;
		if ( (*it)->GetTag() == GOOSE_TAG_SECURITY && !m_bSecurityOpt )
			continue;
		cSize = (*it)->Code(&pduDataArr[arrOff],arrSize-arrOff);
		if ( cSize == 0 )
			return 0;
		arrOff += cSize;
		// 当前项为numDatSetEntries，则继续编码数据序列
		if ( (*it)->GetTag() == GOOSE_TAG_NUM_DS_ENTRIES )
		{
			cSize = m_GooseData->Code(&pduDataArr[arrOff],arrSize-arrOff);
			if ( cSize == 0 )
				return 0;
			arrOff += cSize;
		}
	}

	// 编码完整的goosePdu（TLV）
	MsgDataElem *goosePDU = CreateMsgDataElemImpl(DT_TLV_DATA);
	goosePDU->SetTag(GOOSE_TAG_GOOSE_PDU);
	goosePDU->SetValue(pduDataArr,arrOff);
	cSize = goosePDU->Code(pduDataArr,arrSize);
	if ( cSize == 0 )
	{
		ReleaseMsgDataElemImpl(goosePDU);
		return 0;
	}
	ReleaseMsgDataElemImpl(goosePDU);
	m_PduLen = cSize;
	return cSize;
}

///
/// @brief
///     获取报文帧中goosePDU的长度，即整个TLV数据的长度
///
/// @return
///     goosePDU的长度
///
size_t GoosePduSeg::GetPduLen()
{
	return m_PduLen;
}

///
/// @brief
///     获取goosePDU中Goose数据序列（All Data）的长度（整个TLV数据）
///
/// @return
///     Goose数据序列（All Data）的长度（整个TLV数据）
///
size_t GoosePduSeg::GetGooseDataLen()
{
	return m_GooseData->GetGooseDataLen();
}

///
/// @brief
///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     pduPublic  - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool GoosePduSeg::GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC &pduPublic)
{
	ASSERT(msgDataBuf);
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize   = 0;

	// 获取gocbRef的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueLen();
	pduPublic.gocbRef.assign(strBuf,datSize);
	pduPublic.gocbRef += '\0';

	// 获取timeAllowedtoLive的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.timeAllowedtoLive = 0;
	}
	
	// 获取datSet的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueLen();
	pduPublic.datSet.assign(strBuf,datSize);
	pduPublic.datSet += '\0';

	// 获取goID的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueLen();
	pduPublic.goID.assign(strBuf,datSize);
	pduPublic.goID += '\0';

	// 获取eventTime的value值
	uint8_t eventTime[8];
	datSize = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueLen();
	memcpy_s(eventTime,8,&msgDataBuf[m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueOffset()],datSize);
	pduPublic.eventTm.ts_s  = BSWAP_32(*(uint32_t *)&eventTime[0]);
	pduPublic.eventTm.ts_ns = BSWAP_32(*(uint32_t *)&eventTime[4])>>8;
	// IEC61850中的UTC时间的小数部分等于其4~6字节转换为10进制后除上2的24次方，而微秒值则需再乘上10的9次方。
	pduPublic.eventTm.ts_ns = (uint32_t)(((uint64_t)pduPublic.eventTm.ts_ns)*(1000000000)/(1<<24));
	pduPublic.eventTm.ts_q  = eventTime[7];

	// 获取stNum的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.stNum = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.stNum = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.stNum = 0;
	}

	// 获取sqNum的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.sqNum = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.sqNum = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.sqNum = 0;
	}

	// 获取test的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TEST)->GetValueOffset()];
	pduPublic.test = *uint8Buf;

	// 获取confRev的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.confRev = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.confRev = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.confRev = 0;
	}

	// 获取ndsCom的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetValueOffset()];
	pduPublic.ndsCom = *uint8Buf;

	// 获取numDatSetEntries的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueLen();
	if ( datSize <= sizeof(uint32_t) )
	{
		memcpy_s((uint8_t *)&uint32Tmp,sizeof(uint32_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = BSWAP_32(uint32Tmp) >> ((sizeof(uint32_t)-datSize)*8);
	}
	else if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = (uint32_t)(BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8));
	}
	else
	{
		pduPublic.numDatSetEntries = 0;
	}

	// 获取Security的value值
	if ( m_bSecurityOpt )
	{
		strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueOffset()];
		datSize = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueLen();
		pduPublic.security.assign(strBuf,datSize);
		pduPublic.security += '\0';
	}
	else
		pduPublic.security = "";
	
	return true;
}

///
/// @brief
///     获取报文帧中的GoosePdu的公共信息，即GoosePdu中的通道数据以外的部分
///
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     pduPublic  - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool GoosePduSeg::GetPduPublicInfo(const uint8_t *msgDataBuf, GOOSE_PDU_PUBLIC_EX &pduPublic)
{
	ASSERT(msgDataBuf);
	uint32_t uint32Tmp = 0;
	uint64_t uint64Tmp = 0;
	uint8_t *uint8Buf  = NULL;
	char    *strBuf    = NULL;
	size_t   datSize   = 0;

	// 获取gocbRef的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetValueLen();
	pduPublic.gocbRef.assign(strBuf,datSize);
	pduPublic.gocbRef += '\0';

	// 获取timeAllowedtoLive的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.timeAllowedtoLive = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.timeAllowedtoLive = 0;
	}

	// 获取datSet的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetValueLen();
	pduPublic.datSet.assign(strBuf,datSize);
	pduPublic.datSet += '\0';

	// 获取goID的value值
	strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueOffset()];
	datSize = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetValueLen();
	pduPublic.goID.assign(strBuf,datSize);
	pduPublic.goID += '\0';

	// 获取eventTime的value值
	uint8_t eventTime[8];
	datSize = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueLen();
	memcpy_s(eventTime,8,&msgDataBuf[m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetValueOffset()],datSize);
	pduPublic.eventTm.ts_s  = BSWAP_32(*(uint32_t *)&eventTime[0]);
	pduPublic.eventTm.ts_ns = BSWAP_32(*(uint32_t *)&eventTime[4])>>8;
	// IEC61850中的UTC时间的小数部分等于其4~6字节转换为10进制后除上2的24次方，而微秒值则需再乘上10的9次方。
	pduPublic.eventTm.ts_ns = (uint32_t)(((uint64_t)pduPublic.eventTm.ts_ns)*(1000000000)/(1<<24));
	pduPublic.eventTm.ts_q  = eventTime[7];

	// 获取stNum的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.stNum = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.stNum = 0;
	}

	// 获取sqNum的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.sqNum = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.sqNum = 0;
	}

	// 获取test的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_TEST)->GetValueOffset()];
	pduPublic.test = *uint8Buf;

	// 获取confRev的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.confRev = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.confRev = 0;
	}

	// 获取ndsCom的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetValueOffset()];
	pduPublic.ndsCom = *uint8Buf;

	// 获取numDatSetEntries的value值
	uint8Buf = (uint8_t *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueOffset()];
	datSize   = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetValueLen();
	if ( datSize <= sizeof(uint64_t) )
	{
		memcpy_s((uint8_t *)&uint64Tmp,sizeof(uint64_t),uint8Buf,datSize);
		pduPublic.numDatSetEntries = BSWAP_64(uint64Tmp) >> ((sizeof(uint64_t)-datSize)*8);
	}
	else
	{
		pduPublic.numDatSetEntries = 0;
	}

	// 获取Security的value值
	if ( m_bSecurityOpt )
	{
		strBuf = (char *)&msgDataBuf[m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueOffset()];
		datSize = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetValueLen();
		pduPublic.security.assign(strBuf,datSize);
		pduPublic.security += '\0';
	}
	else
		pduPublic.security = "";

	return true;
}

///
/// @brief
///     重置通道数据已解码（报文值->真实值）的标志
///
void GoosePduSeg::ResetChannelDecodedFlag()
{
	return m_GooseData->ResetChannelDecodedFlag();
}

///
/// @brief
///     获取GoosePdu中的数据通道的数目
///
/// @return
///     数据通道的数目
///
size_t GoosePduSeg::GetChannelNum()
{
	return m_GooseData->GetChannelNum();
}

///
/// @brief
///     获取GoosePdu中的单个通道的数据
///
/// @param[in]
///     idxSeq     - GOOSE数据通道的顺序号，从0开始
/// @param[in]
///     msgDataBuf - 保存报文帧数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     指向Goose通道数据的指针
///
GOOSE_DATA *GoosePduSeg::GetAChannel(size_t idxSeq, const uint8_t *msgDataBuf, size_t bufSize)
{
	return m_GooseData->GetAChannel(idxSeq,msgDataBuf,bufSize);
}

///
/// @brief
///     获取Security可选项的配置信息
///
/// @return
///     true - 报文中含有Security； false - 不含有
///
bool GoosePduSeg::GetSecurityOpt()
{
	return m_bSecurityOpt;
}

///
/// @brief
///     获取goosePDU在报文帧中偏移地址
///
/// @return
///     goosePDU在报文帧中偏移地址
///
size_t GoosePduSeg::GetPduOff()
{
	return m_PduOff;
}

///
/// @brief
///     获取goosePDU中Goose数据序列（All Data）的在报文帧中的偏移地址
///
/// @return
///     Goose数据序列（All Data）的在报文帧中的偏移地址
///
size_t GoosePduSeg::GetGooseDataOff()
{
	return m_GooseData->GetGooseDataOff();
}

///
/// @brief
///     获取GoosePdu的公共信息部分在报文帧中的偏移地址信息
///
/// @param[in]
///     pduPblcOff - 保存GoosePdu的公共信息部分在报文帧中的偏移地址信息的结构体
///
/// @return
///     
///
void GoosePduSeg::GetPduPublicInfoOff(GOOSE_PDU_PUBLIC_OFFSET &pduPblcOff)
{
	pduPblcOff.gocbRefOff           = m_PduPblc->at(GOOSE_PDU_GOCB_REF)->GetElemOffset();
	pduPblcOff.timeAllowedtoLiveOff = m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->GetElemOffset();
	pduPblcOff.datSetOff            = m_PduPblc->at(GOOSE_PDU_DATA_SET)->GetElemOffset();
	pduPblcOff.goIDOff              = m_PduPblc->at(GOOSE_PDU_GO_ID)->GetElemOffset();
	pduPblcOff.eventTmOff           = m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->GetElemOffset();
	pduPblcOff.stNumOff             = m_PduPblc->at(GOOSE_PDU_ST_NUM)->GetElemOffset();
	pduPblcOff.sqNumOff             = m_PduPblc->at(GOOSE_PDU_SQ_NUM)->GetElemOffset();
	pduPblcOff.testOff              = m_PduPblc->at(GOOSE_PDU_TEST)->GetElemOffset();
	pduPblcOff.confRevOff           = m_PduPblc->at(GOOSE_PDU_CONF_REV)->GetElemOffset();
	pduPblcOff.ndsComOff            = m_PduPblc->at(GOOSE_PDU_NDS_COM)->GetElemOffset();
	pduPblcOff.numDatSetEntriesOff  = m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->GetElemOffset();
	pduPblcOff.securityOff          = m_PduPblc->at(GOOSE_PDU_SECURITY)->GetElemOffset();
}

///
/// @brief
///     设置GoosePdu中公共部分的信息（即除通道数据以外的部分的信息）
///
/// @param[in]
///     pduPblc - GoosePdu中公共部分的信息
///
/// @return
///
///
void GoosePduSeg::SetPduPublicInfo(GOOSE_PDU_PUBLIC &pduPblc)
{
	uint32_t uint32Tmp = 0;
	uint8_t  uint8Tmp  = 0;
	size_t   datSize   = 0;
	char    *strBuf    = NULL;
	
	// 设置gocbRef的value值
	datSize = pduPblc.gocbRef.size();
	strBuf  = const_cast<char *>(pduPblc.gocbRef.c_str());
	if ( datSize > GOOSE_PDU_GOCB_REF_MAX_SIZE )
		m_PduPblc->at(GOOSE_PDU_GOCB_REF)->SetValue((uint8_t *)strBuf,GOOSE_PDU_GOCB_REF_MAX_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_GOCB_REF)->SetValue((uint8_t *)strBuf,datSize);

	// 设置timeAllowedtoLive的value值
	datSize   = GetUint32MinLen(pduPblc.timeAllowedtoLive);
	uint32Tmp = BSWAP_32( (pduPblc.timeAllowedtoLive<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_TIME_ALLOWED)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// 设置datSet的value值
	datSize = pduPblc.datSet.size();
	strBuf  = const_cast<char *>(pduPblc.datSet.c_str());
	if ( datSize > GOOSE_PDU_DATA_SET_MAC_SIZE )
		m_PduPblc->at(GOOSE_PDU_DATA_SET)->SetValue((uint8_t *)strBuf,GOOSE_PDU_DATA_SET_MAC_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_DATA_SET)->SetValue((uint8_t *)strBuf,datSize);

	// 设置goID的value值
	datSize = pduPblc.goID.size();
	strBuf  = const_cast<char *>(pduPblc.goID.c_str());
	if ( datSize > GOOSE_PDU_GO_ID_MAX_SIZE )
		m_PduPblc->at(GOOSE_PDU_GO_ID)->SetValue((uint8_t *)strBuf,GOOSE_PDU_GO_ID_MAX_SIZE);
	else
		m_PduPblc->at(GOOSE_PDU_GO_ID)->SetValue((uint8_t *)strBuf,datSize);

	// 设置eventTime的value值
	uint8_t eventTm[8];
	uint32Tmp = BSWAP_32(pduPblc.eventTm.ts_s);
	memcpy(eventTm,(uint8_t *)&uint32Tmp,4);
	uint32Tmp = ((uint32_t)((uint64_t)pduPblc.eventTm.ts_ns*(1<<28)/(1000000000)))<<4;
	// 判断值是否进1
	if ( (uint32Tmp & 0xFF) >= 0x80 )
		uint32Tmp += 0x100;
	uint32Tmp = uint32Tmp & 0xFFFFFF00;
	uint32Tmp = BSWAP_32(uint32Tmp);
	memcpy(&eventTm[4],(uint8_t *)&uint32Tmp,3);
	eventTm[7] = pduPblc.eventTm.ts_q;
	m_PduPblc->at(GOOSE_PDU_EVENT_TIME)->SetValue(eventTm,8);

	// 设置stNum的value值
	datSize   = /*GetUint32MinLen(pduPblc.stNum)*/sizeof(uint32_t);
	uint32Tmp = BSWAP_32( (pduPblc.stNum<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_ST_NUM)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// 设置sqNum的value值
	datSize   = /*GetUint32MinLen(pduPblc.sqNum)*/sizeof(uint32_t);
	uint32Tmp = BSWAP_32( (pduPblc.sqNum<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_SQ_NUM)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// 设置test的value值
	uint8Tmp = pduPblc.test;
	m_PduPblc->at(GOOSE_PDU_TEST)->SetValue(&uint8Tmp,1);

	// 设置confRef的value值
	datSize   = GetUint32MinLen(pduPblc.confRev);
	uint32Tmp = BSWAP_32( (pduPblc.confRev<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_CONF_REV)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// 设置ndsCom的value值
	uint8Tmp = pduPblc.ndsCom;
	m_PduPblc->at(GOOSE_PDU_NDS_COM)->SetValue(&uint8Tmp,1);

	// 设置numDatSetEntries的value值
	datSize   = GetUint32MinLen(pduPblc.numDatSetEntries);
	uint32Tmp = BSWAP_32( (pduPblc.numDatSetEntries<<(sizeof(uint32_t)-datSize)*8) );
	m_PduPblc->at(GOOSE_PDU_NUM_DS_ENTRIES)->SetValue((uint8_t *)&uint32Tmp,datSize);

	// 设置security的value值
	if ( m_bSecurityOpt )
	{
		datSize = pduPblc.security.size();
		strBuf  = const_cast<char *>(pduPblc.security.c_str());
		m_PduPblc->at(GOOSE_PDU_SECURITY)->SetValue((uint8_t *)strBuf,datSize);
	}
	else
		m_PduPblc->at(GOOSE_PDU_SECURITY)->SetValue(NULL,0);
}

///
/// @brief
///     添加一个数据通道
///
/// @param[in]
///     datBuf  - 保存单个GOOSE数据通道含有的数据的缓存
/// @param[in]
///     bufSize - 缓存大小，即单个GOOSE数据通道的数据数目（默认为1，表示该通道的数据非结构体类型）
///
/// @return
///     true - 成功； false - 失败
///
bool GoosePduSeg::AddAChannel(const GOOSE_DATA *channel)
{
	size_t len = m_GooseDataSize + GooseDataTLVBytes(channel);
	if ( len > ( m_MaxPduSize - GOOSE_PDU_NO_DATA_MAX_SIZE ) )
		return false;

	if ( m_GooseData->AddAChannel(channel) == false )
		return false;
	m_GooseDataSize = len;
	return true;
}

///
/// @brief
///     设置Security可选项的配置信息
///
/// @param[in]
///     opt - 报文中是否包含该项
///
/// @return
///     
///
void GoosePduSeg::SetSecurityOpt(bool opt)
{
	m_bSecurityOpt = opt;
}

///
/// @brief
///     清空编解码结果
///
/// @return
///     
///
void GoosePduSeg::Clear()
{
	m_GooseData->Clear();
	m_GooseDataSize = 0;
	m_PduLen        = 0;
	m_PduOff        = 0;
}

///
/// @brief
///     设置Sav92PDU所属的上层协议类型，以重置GoosePdu的最大Size
///
/// @param[in]
///     type - GoosePDU所属的上层协议类型
///
/// @return
///     
///
void GoosePduSeg::SetULPType(const GOOSE_ULP_TYPE type)
{
	switch (type)
	{
	case GOOSE_IEEE802_1Q:
		// 以太网帧最大长度减去GoosePDU之前的部分的长度
		m_MaxPduSize = 1514 - 26;
		break;
	default:
		m_MaxPduSize = 1514 - 26;
	}
}

///
/// @brief
///     获取32位无符号数的有效数据的字节数
///
/// @param[in]
///     inData - 32位无符号数
///
/// @return
///     有效数据的字节数
///
size_t GoosePduSeg::GetUint32MinLen(uint32_t &inData)
{
	if ( ( inData & 0xFF000000 ) != 0 )
		return 4;
	if ( ( inData & 0x00FF0000 ) != 0 )
		return 3;
	if ( ( inData & 0x0000FF00 ) != 0 )
		return 2;
	if ( ( inData & 0x000000FF ) != 0 )
		return 1;
	return 1;
}

