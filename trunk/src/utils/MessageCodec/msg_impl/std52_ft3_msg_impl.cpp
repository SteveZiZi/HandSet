/// @file
///     std52_ft3_msg_impl.cpp
///
/// @brief
///     标准FT3采样值报文帧（52字节版本,含CRC）的编解码接口的实现
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.05
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.09.06     |1.0.1.906  |缓存报文信息时添加对报文长度的判断，防止长度过长引起的异常 |
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
///     对捕获的标准FT3采样值报文（52字节版本,含CRC）帧进行解码
///
/// @param[in]
///     msgDataBuf - 保存捕获的报文帧数据的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     实际的解码数据大小 - 成功； 0 - 失败
///
size_t Std52FT3MsgImpl::Decode(const uint16_t *msgDataBuf, const size_t bufSize)
{
	if ( msgDataBuf[0] != 0x0564 )
		return 0;

	// 对报文帧数据进行大小端转换并缓存
	size_t msgLen = bufSize;
	if ( msgLen > STD52_FT3_MSG_WORD )
		msgLen = STD52_FT3_MSG_WORD;
	for ( size_t i = 0; i < msgLen; i++ )
		m_MsgDataBuf[i] = BSWAP_16(msgDataBuf[i]);

	// 由于南瑞FT3报文格式固定，可沿用上一次的解析结果
	if ( m_bHaveDecode )
		return msgLen;

	m_bHaveDecode = false;

	// 解码ASDU
	size_t dSize = m_AsduSeg->Decode(2,&m_MsgDataBuf[1],msgLen-1);
	if ( dSize == 0 )
		return 0;
	m_bHaveDecode = true;
	return (dSize+1);
}

///
/// @brief
///     根据设置的参数进行编码，获得标准FT3采样值报文（52字节版本,含CRC）帧的ASDU（即不含CRC及启动符）
///
/// @param[in]
///     msgDataArr - 保存报文帧数据的数组
/// @param[in]
///     arrSize    - 数组大小
///
/// @return
///     保存报文帧数据的数组的实际大小 - 成功； 0 - 失败
///
size_t Std52FT3MsgImpl::Code(uint16_t *msgDataArr, const size_t arrSize)
{
	m_bHaveDecode = false;
	return m_AsduSeg->Code(msgDataArr,arrSize);
}

///
/// @brief
///     获取ASDU的长度
///
/// @return
///     ASDU的长度
///
size_t Std52FT3MsgImpl::GetASDULen()
{
	return m_AsduSeg->GetASDULen();
}

///
/// @brief
///     获取报文帧中的ASDU的公共信息，即ASDU中的采样值数据以外的部分
///
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的结构体
///
/// @return
///     true - 成功； false - 失败
///
bool Std52FT3MsgImpl::GetASDUPublicInfo(STD52_FT3_ASDU_PUBLIC &asduPublic)
{
	return m_AsduSeg->GetASDUPublicInfo(m_MsgDataBuf,asduPublic);
}

///
/// @brief
///     获取报文帧中的ASDU中的采样值数据部分的通道数
///
/// @return
///     ASDU中的采样值数据部分的通道数
///
size_t Std52FT3MsgImpl::GetASDUSmpsNum()
{
	return m_AsduSeg->GetASDUSmpsNum();
}

///
/// @brief
///     获取报文帧中的ASDU中的采样值数据
///
/// @param[in]
///     smpValArr  - 保存采样瞬时值的数组
/// @param[in]
///     arrSize    - 数组大小,其值需>=通道数
///
/// @return
///     数组的实际大小
///
size_t Std52FT3MsgImpl::GetASDUSmps(uint16_t *smpValArr, const size_t arrSize)
{
	return m_AsduSeg->GetASDUSmps(m_MsgDataBuf,smpValArr,arrSize);
}

///
/// @brief
///     获取ASDU在报文帧中的偏移地址
///
/// @return
///     ASDU在报文帧中的偏移地址
///
size_t Std52FT3MsgImpl::GetASDUOff()
{
	return m_AsduSeg->GetASDUOff();
}

///
/// @brief
///     获取报文帧中的ASDU的公共信息在报文帧中的偏移地址信息
///
/// @param[in]
///     asduPublic - 保存ASDU的公共信息的偏移地址信息的结构体
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
///     获取报文帧中的ASDU中的采样值数据在报文帧中的偏移地址信息
///
/// @param[in]
///     smpOffArr - 保存采样瞬时值的偏移地址信息的数组
/// @param[in]
///     arrSize   - 数组的大小,其值需>=通道数
///
/// @return
///     数组的实际大小
///
size_t Std52FT3MsgImpl::GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize)
{
	return m_AsduSeg->GetASDUSmpsOff(smpOffArr,arrSize);
}

///
/// @brief
///     设置报文帧中的ASDU信息
///
/// @param[in]
///     asduPublic - ASDU的公共信息
/// @param[in]
///     smpValBuf  - 保存采样瞬时值的缓存
/// @param[in]
///     bufSize    - 缓存大小
///
/// @return
///     
///
void Std52FT3MsgImpl::SetASDU(const STD52_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t bufSize)
{
	m_AsduSeg->SetASDU(asduPublic,smpValBuf,bufSize);
}
