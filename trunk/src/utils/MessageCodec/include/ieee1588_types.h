/// @file
///     ieee1588_types.h
///
/// @brief
///     定义精密时钟同步协议 IEEE1588 报文帧编解码过程中使用的数据类型
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.29
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#ifndef IEEE1588_TYPES_H_

#define IEEE1588_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     IEEE1588协议的报文类型
///
typedef enum
{
	IEEE1588_TYPE_UNKNOWN = 0,                 ///< 未知
	IEEE1588_TYPE_SYNC,                        ///< 同步报文
	IEEE1588_TYPE_DELAY_REQUEST,               ///< 延迟请求响应机制的请求报文
	IEEE1588_TYPE_PDELAY_REQUEST,              ///< 对等延迟机制的请求报文
	IEEE1588_TYPE_PDELAY_RESPONSE,             ///< 对等延迟机制的响应报文
	IEEE1588_TYPE_FOLLOW_UP,                   ///< 跟随报文
	IEEE1588_TYPE_DELAY_RESPONSE,              ///< 延迟请求响应机制的响应报文
	IEEE1588_TYPE_PDELAY_RESPONSE_FOLLOW_UP,   ///< 对等延迟机制的跟随报文
	IEEE1588_TYPE_ANNOUNCE,                    ///< Announce报文中的信息主要用于系统中最佳主时钟的选择计算
	IEEE1588_TYPE_SIGNALING,                   ///< Signaling报文用于PTP时钟节点间其他目的的通讯
	IEEE1588_TYPE_MANAGEMENT                   ///< Management消息用于查询和更新时钟所维护的PTP数据集
}IEEE1588_MESSAGE_TYPE;

///
/// @brief
///     IEEE1588报文的类型信息字段
///
typedef struct
{
	uint8_t  transportSpecific : 4;            ///< 标识承载IEEE1588通信的底层协议，默认为0
	uint8_t  messageType : 4;                  ///< 当前报文所属的类型（Sync、Follow_Up...）

	///
	/// @brief
	///     获取当前报文所属的类型
	///
	/// @return
	///     当前报文所属的类型
	///
	IEEE1588_MESSAGE_TYPE GetMessageType();

	///
	/// @brief
	///     设置当前报文所属的类型
	///
	/// @param[in]
	///     eMessageType - 当前报文所属的类型
	///
	void SetMessageType(IEEE1588_MESSAGE_TYPE eMessageType);

}IEEE1588_TYPE_FIELD;

///
/// @brief
///     IEEE1588报文的版本信息字段
///
typedef struct
{
	uint8_t  reserved : 4;                     ///< 用于将来扩展
	uint8_t  versionPTP : 4;                   ///< IEEE1588协议的版本号，V1或V2
}IEEE1588_VERSION_FIELD;

///
/// @brief
///     IEEE1588报文的报文头中的标志位集合
///
typedef struct
{
	uint16_t iFlags;

	///
	/// @brief
	///     判断发出端口是否为非Master状态
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpAlternateMaster();

	///
	/// @brief
	///     设置报文发出端口是否为非Master状态
	///
	/// @param[in]
	///     bAlternateMaster - 为非Master状态则为true，否则相反
	///
	void SetPtpAlternateMaster(bool bAlternateMaster);

	///
	/// @brief
	///     判断同步时钟是否为两步法时钟
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpTwoStep();

	///
	/// @brief
	///     设置同步时钟是否为两步法时钟
	///
	/// @param[in]
	///     bTwoStep - 两步法时钟则为true，两步法时钟则为false
	///
	void SetPtpTwoStep(bool bTwoStep);

	///
	/// @brief
	///     判断该报文是否以单播方式发送
	///
	/// @return
	///     以单播方式发送 - true
	///     以多播方式发送 - false
	///
	bool IsPtpUnicast();

	///
	/// @brief
	///     设置该报文是否以单播方式发送
	///
	/// @param[in]
	///     bTwoStep - 以单播方式发送则为true，以多播方式发送则为false
	///
	void SetPtpUnicast(bool bUnicast);

	///
	/// @brief
	///     判断时钟是否被alternate PTP配置文件定义（标志1）
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpProfileSpecific1();

	///
	/// @brief
	///     设置时钟是否被alternate PTP配置文件定义（标志1）
	///
	/// @param[in]
	///     bSpecific - 是则为true，否则相反
	///
	void SetPtpProfileSpecific1(bool bSpecific);

	///
	/// @brief
	///     判断时钟是否被alternate PTP配置文件定义（标志2）
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpProfileSpecific2();

	///
	/// @brief
	///     设置时钟是否被alternate PTP配置文件定义（标志2）
	///
	/// @param[in]
	///     bSpecific - 是则为true，否则相反
	///
	void SetPtpProfileSpecific2(bool bSpecific);

	///
	/// @brief
	///     判断报文是否采用了信息安全机制
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpSecurity();

	///
	/// @brief
	///     设置报文是否采用了信息安全机制
	///
	/// @param[in]
	///     bSecurity - 是则为true，否则相反
	///
	void SetPtpSecurity(bool bSecurity);

	///
	/// @brief
	///     判断当天最后1min是否为61s
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpLI61();

	///
	/// @brief
	///     设置当天最后1min是否为61s
	///
	/// @param[in]
	///     bLI61 - 对于采用PTP时元的对时系统，其值为true则表示当天最后1min为61s；
	///             对于不采用PTP时元的对时系统，其值为false。
	///
	void SetPtpLI61(bool bLI61);

	///
	/// @brief
	///     判断当天最后1min是否为59s
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpLI59();

	///
	/// @brief
	///     设置当天最后1min是否为59s
	///
	/// @param[in]
	///     bLI59 - 对于采用PTP时元的对时系统，其值为true则表示当天最后1min为59s；
	///             对于不采用PTP时元的对时系统，其值为false。
	///
	void SetPtpLI59(bool bLI59);

	///
	/// @brief
	///     判断是否采用UTC显示时间信息
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpUtcReasonable();

	///
	/// @brief
	///     设置是否采用UTC显示时间信息
	///
	/// @param[in]
	///     bUtcReasonable - 是则为true，否则相反
	///
	void SetPtpUtcReasonable(bool bUtcReasonable);

	///
	/// @brief
	///     判断最高级超主时钟是否采用PTP时间标尺
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsPtpTimescale();

	///
	/// @brief
	///     设置最高级超主时钟是否采用PTP时间标尺
	///
	/// @param[in]
	///     bTimescale - 是则为true，否则相反
	///
	void SetPtpTimescale(bool bTimescale);

	///
	/// @brief
	///     判断时间标尺的时间偏移是否可追溯到最高级超主时钟
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsTimeTraceble();

	///
	/// @brief
	///     设置时间标尺的时间偏移是否可追溯到最高级超主时钟
	///
	/// @param[in]
	///     bTimeTraceble - 是则为true，否则相反
	///
	void SetTimeTraceble(bool bTimeTraceble);

	///
	/// @brief
	///     判断频率决定标尺是否可以追溯到最高级超主时钟
	///
	/// @return
	///     是 - true
	///     否 - false
	///
	bool IsFrequencyTraceable();

	///
	/// @brief
	///     设置频率决定标尺是否可以追溯到最高级超主时钟
	///
	/// @param[in]
	///     bTraceable - 是则为true，否则相反
	///
	void SetFrequencyTraceable(bool bTraceable);

}IEEE1588_HEADER_FLAGS;

///
/// @brief
///     IEEE1588报文发出端口（源端口）的信息
///
typedef struct
{
	uint64_t clockIdentity;                    ///< 发出当前报文的时钟的地址（每个时钟都拥有一个唯一值）
	uint16_t sourcePortID;                     ///< 该时钟上的端口号
}IEEE1588_SOURCE_PORT_ID;

///
/// @brief
///     IEEE1588报文的报文头
///
typedef struct
{
	IEEE1588_TYPE_FIELD type;                  ///< 当前报文的类型信息
	IEEE1588_VERSION_FIELD version;            ///< 当前报文的版本信息
	uint16_t messageLength;                    ///< 当前报文的长度，从报文头第一个字节开始计数，包括后缀所占的字节数
	uint8_t  domainNumber;                     ///< 表示PTP域号，取值范围为0~255，默认值为0
	uint8_t  reserved1;                        ///< 保留字段1
	IEEE1588_HEADER_FLAGS flags;               ///< IEEE1588标志位集合，只用到该字段的其中12bit，其余4bit为保留位，这些保留位的默认值为FALSE
	uint64_t correctionField;                  ///< 表示修正域，其值需要乘以2的-16次方才能换算成以纳秒为单位的真实的修正时间值
	uint32_t reserved2;                        ///< 保留字段2
	IEEE1588_SOURCE_PORT_ID sourcePortIdentity;///< 当前报文发出端口的唯一标识
	uint16_t sequenceId;                       ///< 表示PTP报文的递增序号
	uint8_t  controlField;                     ///< 设置该字段的目的是与IEEE1588标准第1版保持兼容
	uint8_t  logMessageInterval;               ///< 代表连续两帧同类型报文的报文之间的平均发送间隔，真实值为log2(t)
}IEEE1588_HEADER;

///
/// @brief
///     IEEE1588报文的时间戳
///
typedef struct
{
	uint64_t t_s;                              ///< 时间戳的整数部分，单位为秒，范围为0~2^48-1
	uint32_t t_ns;                             ///< 时间戳的小数部分，单位为纳秒，范围为0~10^9-1
}IEEE1588_TIMESTAMP;

///
/// @brief
///     IEEE1588报文发出端口（源端口）的信息在报文帧中的偏移地址信息
///
typedef struct
{
	size_t clockIdentityOff;                   ///< 发出当前报文的时钟的地址的偏移地址
	size_t sourcePortIDOff;                    ///< 该时钟上的端口号的偏移地址
}IEEE1588_SOURCE_PORT_ID_OFFSET;

///
/// @brief
///     IEEE1588报文的报文头在报文帧中的偏移地址信息
///
typedef struct
{
	size_t typeOff;                            ///< 当前报文的类型信息的偏移地址
	size_t versionOff;                         ///< 当前报文的版本信息的偏移地址
	size_t messageLengthOff;                   ///< 当前报文的长度的偏移地址
	size_t domainNumberOff;                    ///< PTP域号的偏移地址
	size_t reserved1Off;                       ///< 保留字段1的偏移地址
	size_t flagsOff;                           ///< IEEE1588标志位集合的偏移地址
	size_t correctionFieldOff;                 ///< 修正域的偏移地址
	size_t reserved2Off;                       ///< 保留字段2的偏移地址
	IEEE1588_SOURCE_PORT_ID_OFFSET sourcePortIdentityOff;///< 当前报文发出端口的唯一标识的偏移地址
	size_t sequenceIdOff;                      ///< PTP报文的递增序号的偏移地址
	size_t controlFieldOff;                    ///< 与IEEE1588标准第1版兼容性设置的的偏移地址
	size_t logMessageIntervalOff;              ///< 代表连续两帧同类型报文的报文之间的平均发送间隔的偏移地址
}IEEE1588_HEADER_OFFSET;

///
/// @brief
///     IEEE1588报文的时间戳的偏移地址信息
///
typedef struct
{
	size_t t_s;                                ///< 时间戳的整数部分的偏移地址
	size_t t_ns;                               ///< 时间戳的小数部分的偏移地址
}IEEE1588_TIMESTAMP_OFFSET;

#endif// IEEE1588_TYPES_H_
