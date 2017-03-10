/// @file
///     ieee1588_msg.h
///
/// @brief
///     IEEE1588报文帧编解码的接口及接口对象实例的创建和释放接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.9.29
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.09.29     |1.0.0.929  |当前版本仅提供对以太网报文头和IEEE1588报文头的解码        |
///
#ifndef IEEE1588_MESSAGE_H_

#define IEEE1588_MESSAGE_H_

#include "etypes.h"
#include "ieee1588_types.h"

///
/// @brief
///     IEEE1588报文的编解码接口
///
class Ieee1588Msg
{
public:
	virtual ~Ieee1588Msg(){};

	///
	/// @brief
	///     对IEEE1588报文帧进行解码
	///
	/// @param[in]
	///     msgDataBuf - 保存待解码的IEEE1588报文帧数据的缓存
	/// @param[in]
	///     bufSize    - 缓存大小
	///
	/// @return
	///     实际的解码数据大小 - 成功； 0 - 失败
	///
	virtual size_t Decode(const uint8_t *msgDataBuf, const size_t bufSize) = 0;

	///
	/// @brief
	///     获取以太网报文帧头信息
	///
	/// @param[in]
	///     ethHdr - 保存以太网报文帧头信息的结构体
	///
	/// @return
	///
	///
	virtual void GetEthernetHeader(ETHER_II_HEADER &ethHdr) = 0;

	///
	/// @brief
	///     获取IEEE1588报文头信息
	///
	/// @param[in]
	///     ieee1588Hdr - 保存IEEE1588报文头信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIeee1588Header(IEEE1588_HEADER &ieee1588Hdr) = 0;

	///
	/// @brief
	///     获取Sync报文离开主时钟时的时刻，即Sync报文或其Follow_Up报文的OriginTimestamp字段
	///
	/// @param[in]
	///     originTs - OriginTimestamp
	///
	/// @return
	///     true - 操作成功； false - 操作失败
	///
	virtual bool GetOriginTimestamp(IEEE1588_TIMESTAMP &originTimestamp) = 0;

	///
	/// @brief
	///     获取以太网报文帧头信息在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     ethHdrOff - 保存以太网报文帧头信息的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void GetEthernetHeaderOff(ETHER_II_HEADER_OFFSET &ethHdrOff) = 0;

	///
	/// @brief
	///     获取IEEE1588报文头在报文帧中的偏移地址信息
	///
	/// @param[in]
	///     ieee1588HdrOff - 保存IEEE1588报文头在报文帧中的偏移地址信息的结构体
	///
	/// @return
	///
	///
	virtual void GetIeee1588HeaderOff(IEEE1588_HEADER_OFFSET &ieee1588HdrOff) = 0;

	///
	/// @brief
	///     获取Sync报文离开主时钟时的时刻，即Sync报文或其Follow_Up报文的OriginTimestamp字段的偏移地址信息
	///
	/// @param[in]
	///     originTimestampOff - OriginTimestamp的偏移地址
	///
	/// @return
	///     true - 操作成功； false - 操作失败
	///
	virtual bool GetOriginTimestampOff(IEEE1588_TIMESTAMP_OFFSET &originTimestampOff) = 0;
};

///
/// @brief
///     创建IEEE1588报文的编解码接口对象的实例
///
/// @return
///     指向Ieee1588Msg实例的指针
///
Ieee1588Msg *CreateIeee1588MsgImpl();

///
/// @brief
///     释放IEEE1588报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Ieee1588Msg实例的指针
///
/// @return
///     
///
void ReleaseIeee1588MsgImpl(Ieee1588Msg *impl);

#endif // IEEE1588_MESSAGE_H_
