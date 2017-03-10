/// @file
///     sav91_types.h
///
/// @brief
///     定义IEC61850-9-1报文帧编解码过程中使用的数据类型
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
#ifndef SAV91_TYPES_H_

#define SAV91_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     符合9-1标准的SavPDU中应用服务数据单元（ASDU）中除采样值信息外的公共信息
///
typedef struct
{
	uint16_t asduLen;                      ///< ASDU的长度
	uint8_t  lnName;                       ///< 逻辑节点名称
	uint8_t  dataSetName;                  ///< 数据集名称
	uint16_t ldName;                       ///< 逻辑设备名称
	uint16_t phsArtg;                      ///< 额定相电流
	uint16_t neutArtg;                     ///< 额定中性点电流
	uint16_t phsVrtg;                      ///< 额定相电压
	uint16_t delayTime;                    ///< 额定延时时间
	uint16_t statusWord1;                  ///< 状态字1
	uint16_t statusWord2;                  ///< 状态字2
	uint16_t smpCnt;                       ///< 样本计数器，表示当前发送的采样点数目
	uint8_t  smpRate;                      ///< 给出额定频率下每周波时间内输出的采样数据集数目
	uint8_t  confRev;                      ///< 配置版本号
}SAV91_ASDU_PUBLIC;

///
/// @brief
///     符合9-1标准的SavPDU中应用服务数据单元（ASDU）中除采样值信息外的公共信息的偏移地址信息
///
typedef struct
{
	size_t   asduLenOff;                   ///< ASDU的长度的偏移地址
	size_t   lnNameOff;                    ///< 逻辑节点名称的偏移地址
	size_t   dataSetNameOff;               ///< 数据集名称的偏移地址
	size_t   ldNameOff;                    ///< 逻辑设备名称的偏移地址
	size_t   phsArtgOff;                   ///< 额定相电流的偏移地址
	size_t   neutArtgOff;                  ///< 额定中性点电流的偏移地址
	size_t   phsVrtgOff;                   ///< 额定相电压的偏移地址
	size_t   delayTimeOff;                 ///< 额定延时时间的偏移地址
	size_t   statusWord1Off;               ///< 状态字1的偏移地址
	size_t   statusWord2Off;               ///< 状态字2的偏移地址
	size_t   smpCntOff;                    ///< 样本计数器，表示当前发送的采样点数目的偏移地址
	size_t   smpRateOff;                   ///< 给出额定频率下每周波时间内输出的采样数据集数目的偏移地址
	size_t   confRevOff;                   ///< 配置版本号的偏移地址
}SAV91_ASDU_PUBLIC_OFFSET;

#endif
