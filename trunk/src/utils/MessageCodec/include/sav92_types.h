/// @file
///     sav92_types.h
///
/// @brief
///     定义IEC61850-9-2LE报文帧编解码过程中使用的数据类型
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
#ifndef SAV92_TYPES_H_

#define SAV92_TYPES_H_

#include <string>
#include "src\include\stdint.h"

///
/// @brief
///     9-2报文帧中Visible String类型数据的最大Size（包括'\0'）
///
#define SAV92_VISIBLE_STR_MAX_SIZE 35+1

///
/// @brief
///     符合9-2标准的SavPDU中应用服务数据单元（ASDU）中除采样值信息外的公共信息
///
typedef struct
{
	char         svIDStr[SAV92_VISIBLE_STR_MAX_SIZE];///< 表示采样值控制块标识
	char         datSetStr[SAV92_VISIBLE_STR_MAX_SIZE]; ///< 表示采样数据集的索引（数据集名），可选项
	uint16_t     smpCnt;                   ///< 表示采样计数器,即采样点数目
	uint32_t     confRev;                  ///< 表示配置版本号
	struct
	{
		uint32_t ts_s;                     ///< 时间戳的秒数部分
		uint32_t ts_ns;                    ///< 时间戳的纳秒数部分，即小数部分
		uint8_t  ts_q;                     ///< 时间戳的品质
	}refrTm;                               ///< 缓存区刷新时间（UTC时间），可选项，报文中共占8 Byte
	bool         smpSynch;                 ///< 表示同步标识位，用于反映合并单元的同步状态
	uint16_t     smpRate;                  ///< 采样率,可选项
}SAV92_ASDU_PUBLIC;

///
/// @brief
///     符合9-2标准的SavPDU中应用服务数据单元（ASDU）中除采样值信息外的公共信息在报文帧中的偏移地址信息
///
typedef struct
{
	size_t svIDOff;                        ///< 采样值控制块标识的偏移地址
	size_t datSetOff;                      ///< 采样数据集的索引（数据集名）的偏移地址
	size_t smpCntOff;                      ///< 采样计数器的偏移地址
	size_t confRevOff;                     ///< 配置版本号的偏移地址
	size_t refrTmOff;                      ///< 缓存区刷新时间的偏移地址
	size_t smpSynchOff;                    ///< 同步标识位的偏移地址
	size_t smpRateOff;                     ///< 采样率的偏移地址
}SAV92_ASDU_PUBLIC_OFFSET;

///
/// @brief
///     符合9-2采样值报文中的可选项配置信息，表示报文中是否包含该项
///
typedef struct 
{
	bool bSecurityOpt;                     ///< 安全信息
	bool bDataSetOpt;                      ///< 数据集名
	bool bRefrTmOpt;                       ///< 刷新时间
	bool bSmpRateOpt;                      ///< 采样率
}SAV92_SAVPDU_OPTS;

#endif
