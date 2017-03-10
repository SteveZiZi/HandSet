/// @file
///     msg_rapid_decoder.h
///
/// @brief
///     报文帧的快速解码接口
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.5.13
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.15     |1.0.0.515  |添加Sav92的可选项和Goose的可选项的信息的接口             |
///    |2013.05.15     |1.0.0.515  |添加获取报文帧中ASDU数目的接口                           |
///    |2013.05.15     |1.0.0.515  |修改南瑞/国网FT3通道数的获取，不对DataSetName做判断，直接返回通道数|
///    |2013.07.15     |1.0.0.709  |添加获取协议数据单元长度的接口                           |
///    |2013.09.10     |1.0.1.910  |添加报文帧类型（互感器FT3、52字节标准FT3和9-1扩展），并更新相应接口的实现|
///    |2013.09.11     |1.0.1.911  |添加三相电压互感器FT3帧类型（2010/2011版本）的判断（通过CRC校验）|
///    |2013.09.30     |1.0.1.930  |添加IEEE1588报文，更新报文帧类型获取接口                 |
///    |2013.09.30     |1.0.1.930  |添加获取IEEE1588报文版本信息和源端口标识信息的接口       |
///
#ifndef MESSAGE_RAPID_DECODER_H_

#define MESSAGE_RAPID_DECODER_H_

#include "src\include\stdint.h"
#include "src\include\pub_datatypes.h"
#include "sav92_types.h"
#include "ieee1588_types.h"


class MsgRapidDecoder
{
public:
	///
	/// @brief
	///     获取报文帧的类型
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	///
	/// @return
	///     报文类型
	///
	enumFrameType GetMessageType(const uint8_t *msgBuf, size_t bufSize);

	///
	/// @brief
	///     获取报文帧中的目的Mac地址
	///
	/// @param[in]
	///     msgBuf    - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize   - 报文帧缓存的大小
	/// @param[in]
	///     dstMacArr - 保存目的Mac地址的数组，大小需>=6
	///
	void GetDstMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *dstMacArr);

	///
	/// @brief
	///     获取报文帧中的源Mac地址
	///
	/// @param[in]
	///     msgBuf    - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize   - 报文帧缓存的大小
	/// @param[in]
	///     srcMacArr - 保存源Mac地址的数组，大小需>=6
	///
	void GetSrcMac(const uint8_t *msgBuf, size_t bufSize, uint8_t *srcMacArr);

	///
	/// @brief
	///     获取以太网报文帧中的以太网类型
	///
	/// @param[in]
	///     msgBuf    - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize   - 报文帧缓存的大小
	/// @param[in]
	///     etherType - 返回以太网类型
	///
	void GetEtherType(const uint8_t *msgBuf, size_t bufSize, uint16_t &etherType);

	///
	/// @brief
	///     获取ISO/IEC8802-3格式（含Vlan Tag）的以太网报文帧中的AppID
	///     其他报文类型appID置0
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     appID   - 返回AppID
	///
	void GetAppID(const uint8_t *msgBuf, size_t bufSize, uint16_t &appID);

	///
	/// @brief
	///     获取协议数据单元的长度，ISO/IEC8802-3格式（含Vlan Tag）的以太网报文帧中从AppID开始的字节数
	///     其他报文类型len置0
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     len     - 返回协议数据单元的长度
	///
	void GetPDULen(const uint8_t *msgBuf, size_t bufSize, uint16_t &len);

	///
	/// @brief
	///     获取报文帧中的数据通道的数目，主要用于GOOSE、SAV91/2、FT3
	///
	/// @param[in]
	///     msgBuf     - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize    - 报文帧缓存的大小
	/// @param[in]
	///     type       - 待解码的报文的类型
	/// @param[in]
	///     channelNum - 数据通道的数目
	///
	void GetChannelNum(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, size_t &channelNum);

	///
	/// @brief
	///     获取报文帧中的LNName，主要用于SAV91、FT3
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     type    - 待解码的报文的类型
	/// @param[in]
	///     lnName  - 逻辑节点名
	///
	void GetLNName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &lnName);

	///
	/// @brief
	///     获取报文帧中的LDName，主要用于SAV91、FT3
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     type    - 待解码的报文的类型
	/// @param[in]
	///     ldName  - 逻辑设备名
	///
	void GetLDName(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &ldName);

	///
	/// @brief
	///     获取报文帧中的采样率，主要用于SAV91/2、FT3
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     type    - 待解码的报文的类型
	/// @param[in]
	///     smpRate - 采样率
	///
	void GetSmpRate(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &smpRate);

	///
	/// @brief
	///     获取报文帧中的ASDU数目，主要用于SAV91/2、FT3
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     type    - 待解码的报文的类型
	/// @param[in]
	///     noASDU  - ASDU数目
	///
	void GetNoASDU(const uint8_t *msgBuf, size_t bufSize, const enumFrameType type, uint16_t &noASDU);

	///
	/// @brief
	///     获取Sav92报文帧中的可选项信息
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     opts    - 可选项
	///
	void GetSav92Opts(const uint8_t *msgBuf, size_t bufSize, SAV92_SAVPDU_OPTS &opts);


	///
	/// @brief
	///     判断Goose报文帧中是否含有Security可选项
	///
	/// @param[in]
	///     msgBuf  - 指向报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	///
	/// @return
	///     true - 有； false - 无
	///
	bool IsHaveSecurityInGoose(const uint8_t *msgBuf, size_t bufSize);

	///
	/// @brief
	///     获取IEEE1588报文的版本信息
	///
	/// @param[in]
	///     msgBuf  - 指向IEEE1588报文帧缓存的指针
	/// @param[in]
	///     bufSize - 报文帧缓存的大小
	/// @param[in]
	///     version - 保存IEEE1588报文的版本信息
	///
	/// @return
	///     
	///
	void GetIeee1588Version(const uint8_t *msgBuf, size_t bufSize, uint8_t &version);

	///
	/// @brief
	///     获取IEEE1588报文的报文发出端口（源端口）的标识信息
	///
	/// @param[in]
	///     msgBuf    - 指向IEEE1588报文帧缓存的指针
	/// @param[in]
	///     bufSize   - 报文帧缓存的大小
	/// @param[in]
	///     srcPortID - 保存报文发出端口（源端口）的标识信息的结构体
	///
	/// @return
	///     
	///
	void GetIeee1588SrcPortID(const uint8_t *msgBuf, size_t bufSize, IEEE1588_SOURCE_PORT_ID &srcPortID);

	///
	/// @brief
	///     获取Sync报文离开主时钟时的时刻，即Sync报文或其Follow_Up报文的OriginTimestamp字段
	///
	/// @param[in]
	///     msgBuf   - 指向IEEE1588报文帧缓存的指针
	/// @param[in]
	///     bufSize  - 报文帧缓存的大小
	/// @param[in]
	///     originTs - OriginTimestamp
	///
	/// @return
	///     true - 操作成功； false - 操作失败
	///
	bool GetIeee1588OriginTimeStamp(const uint8_t *msgBuf, size_t bufSize, IEEE1588_TIMESTAMP &originTs);

protected:
	///
	/// @brief
	///     辅助函数
	///
	void GetChannelNumInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav91(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav91Ex(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInSav92(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);
	void GetChannelNumInGoose(const uint8_t *msgBuf, size_t bufSize, size_t &channelNum);

	void GetLNNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);
	void GetLNNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &lnName);

	void GetLDNameInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInGuoWangFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInStd52FT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);
	void GetLDNameInSav91Ex(const uint8_t *msgBuf, size_t bufSize, uint16_t &ldName);

	void GetSmpRateInNanRuiFT3(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);
	void GetSmpRateInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);
	void GetSmpRateInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &smpRate);

	void GetNoASDUInSav91(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU);
	void GetNoASDUInSav92(const uint8_t *msgBuf, size_t bufSize, uint16_t &noASDU);

	size_t NextTLVData(const size_t beginIdx, const uint8_t *dataBuf, size_t bufSize, size_t &valIdx);

};

#endif
