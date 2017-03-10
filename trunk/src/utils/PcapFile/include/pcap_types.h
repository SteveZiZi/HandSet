/// @file
///     pcap_types.h
///
/// @brief
///     PCAP文件操作接口所用到的数据类型
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.2.27
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.03.05     |1.0.1.305  |修改PCAP文件相关类型的注释                               |
///    |2013.03.05     |1.0.1.305  |将PCAP_FILE_TIME_VAL的成员改为有符号类型                 |
///
#ifndef PCAP_TYPES_H_

#define PCAP_TYPES_H_

#include "src\include\stdint.h"

#define MAX_ETHERNET_SIZE      1514      ///< 以太网帧的最大长度

///
/// @brief
///     自定义类型，表示PCAP文件操作的返回值
///
typedef int PCAP_FILE_RESULT;
#define PCAP_FILE_ERROR   PCAP_FILE_RESULT(-1)   ///< 操作发生错误
#define PCAP_FILE_SUCCESS PCAP_FILE_RESULT(0)    ///< 操作成功
#define PCAP_FILE_END     PCAP_FILE_RESULT(1)    ///< 到达文件结尾

///
/// @brief
///     PCAP文件头的默认值
///
#define PCAP_DEFAULT_MAGIC         0xA1B2C3D4
#define PCAP_DEFAULT_MAJOR_VERSION 0x0002
#define PCAP_DEFAULT_MINOR_VERSION 0x0004
#define PCAP_DEFAULT_THIS_ZONE     0x00000000
#define PCAP_DEFAULT_SIGFIGS       0x00000000
#define PCAP_DEFAULT_SANPLEN       0x0000FFFF

///
/// @brief
///     定义录波文件使用的链路层类型
///
#define PCAP_LINK_TYPE_ETHERNET 1      ///< 以太网类型，对应9-1/2、GOOSE
#define PCAP_LINK_TYPE_FT3      10     ///< FDDI类型，对应FT3

///
/// @brief
///     文件访问模式，分为读和写模式
///
enum PCAP_FILE_MODE{PFM_READ = 0, PFM_WRITE};

#pragma pack(push, 1)

///
/// @brief
///     PCAP文件头结构,共24个字节
///
typedef struct
{
	uint32_t magic;                    ///< 标识位，标识文件开始，可用于判断文件的大小端存储，默认值为0xA1B2C3D4，4个字节
	uint16_t majorVersion;             ///< 主版本号，默认值为0x02，2个字节
	uint16_t minorVersion;             ///< 副版本号，默认值为0x04，2个字节
	uint32_t thisZone;                 ///< 区域时间（相对于格林尼治时间的局部校正），实际中并不使用，默认为0，4个字节
	uint32_t sigfigs;                  ///< 时间戳的精度，实际中并不使用，默认为0，4个字节
	uint32_t snapLen;                  ///< PCAP文件中数据包的最大存储长度，其值为0xFFFF时表示保存整个数据包，4个字节
	uint32_t linkType;                 ///< 链路层类型，由数据包的链路层包头决定，4个字节
}PCAP_FILE_HEADER;

///
/// @brief
///     时间戳的结构体，共8个字节
///
typedef struct
{
	int32_t timeStamp_Second;          ///< 时间戳的秒数，4个字节
	int32_t timeStamp_USec;            ///< 时间戳的微秒数,即该时间戳的小数部分，4个字节
}PCAP_FILE_TIME_VAL;

///
/// @brief
///     数据包头结构，共16个字节
///
typedef struct
{
	PCAP_FILE_TIME_VAL timeStamp;      ///< 从格林尼治时间的1970-1-1 00:00:00到抓包时经过的时间，8个字节
	uint32_t  capLen;                  ///< 表示保存在PCAP文件中的数据包的实际长度，4个字节
	uint32_t  len;                     ///< 表示数据包的真实长度（原始报文长度），4个字节
	                                   ///< len比capLen大则表示当前保存的数据包不完整
}PCAP_PACKET_HEADER;

#pragma pack( pop)

#endif
