/// @file
///     etypes.h
///
/// @brief
///     定义以太网报文帧编解码过程中使用的数据类型
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
#ifndef ETHERNET_TYPES_H_

#define ETHERNET_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     以太网V2的Mac地址的长度
///
#define ETHER_MAC_LEN 6

///
/// @brief
///     以太网帧的最大长度
///
#define ETHER_FRAME_MAX_LEN 1514

///
/// @brief
///     以太网类型值：用来标识该数据帧为带IEEE 802.1Q标签的数据帧
///
#ifndef ETHERTYPE_VLAN
#define ETHERTYPE_VLAN		0x8100
#endif

///
/// @brief
///     以太网类型值：用来标识该数据帧为GOOSE报文
///
#ifndef ETHERTYPE_IEC61850_GOOSE
#define ETHERTYPE_IEC61850_GOOSE    0x88b8
#endif

///
/// @brief
///     以太网类型值：用来标识该数据帧为9-1/2 Sav报文
///
#ifndef ETHERTYPE_IEC61850_SV
#define ETHERTYPE_IEC61850_SV       0x88ba
#endif

///
/// @brief
///     以太网类型值：用来标识该数据帧为IEEE1588报文
///
//
#ifndef ETHERTYPE_IEEE1588
#define ETHERTYPE_IEEE1588          0x88F7
#endif

///
/// @brief
///     以太网V2的帧头信息
///
typedef struct
{
	uint8_t  desMac[ETHER_MAC_LEN];        ///< 目的地址
	uint8_t  srcMac[ETHER_MAC_LEN];        ///< 源地址
	uint16_t etherType;                    ///< 以太网帧类型标识，其值为0x8100时标识该报文带有IEEE802.1Q标签
}ETHER_II_HEADER;

///
/// @brief
///     IEEE802.1Q的VLAN标签中的TCI(标签控制信息)
///
typedef struct
{
	uint16_t vlanID : 12;              ///< 虚拟局域网标识，用于标记该报文应该发送到哪个VLAN
	uint16_t cfi : 1;                  ///< 规范格式标记，总被设置为0
	uint16_t userPriority : 3;         ///< 用户优先级	
}IEEE802_1Q_TAG_TCI;

///
/// @brief
///     IEEE802.1Q的VLAN标签信息
///
typedef struct
{
	IEEE802_1Q_TAG_TCI tci;                                  ///< TCI(标签控制信息)
	uint16_t etherType;                    ///< 以太网帧类型标识
}IEEE802_1Q_VLAN_INFO;

///
/// @brief
///     ISO/IEC8802-3以太网帧中协议数据单元(PDU)的公共部分的信息
///
typedef struct
{
	uint16_t appID;                        ///< 应用信息标识码，用于区别应用关联，即区分GOOSE和模拟量采样值（SAV）协议
	uint16_t len;                          ///< PDU的长度，即从APPID开始到APDU结束的全部字节数
	uint16_t reserved1;                    ///< 保留位1
	uint16_t reserved2;                    ///< 保留位2
}IEC8802_3_PDU_PUBLIC;

///
/// @brief
///     以太网V2的帧头信息的偏移地址信息
///
typedef struct
{
	size_t desMacOff;                      ///< 目的地址的偏移地址
	size_t srcMacOff;                      ///< 源地址的偏移地址
	size_t etherTypeOff;                   ///< 以太网帧类型标识的偏移地址
}ETHER_II_HEADER_OFFSET;

///
/// @brief
///     IEEE802.1Q的VLAN标签信息的偏移地址信息
///
typedef struct
{
	size_t tciOff;                         ///< TCI(标签控制信息)的偏移地址
	size_t etherTypeOff;                   ///< 以太网帧类型标识的偏移地址
}IEEE802_1Q_VLAN_INFO_OFFSET;

///
/// @brief
///     ISO/IEC8802-3以太网帧中协议数据单元(PDU)的公共部分的信息的偏移地址信息
///
typedef struct
{
	size_t appIDOff;                       ///< 应用信息标识码的偏移地址
	size_t lenOff;                         ///< PDU的长度的偏移地址
	size_t reserved1Off;                   ///< 保留位1的偏移地址
	size_t reserved2Off;                   ///< 保留位2的偏移地址
}IEC8802_3_PDU_PUBLIC_OFFSET;

#endif
