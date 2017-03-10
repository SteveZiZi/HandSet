/// @file
///     ctorvt_ft3_types.h
///
/// @brief
///     定义电流/电压互感器FT3采样值报文帧编解码过程中使用的数据类型
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
///    |               |           |                                                          |
///
#ifndef CTORVT_FT3_TYPES_H_

#define CTORVT_FT3_TYPES_H_

#include "src\include\stdint.h"

///
/// @brief
///     单相电流/电压互感器FT3采样值报文每帧报文长度为13words（含CRC）
///
#define ONEPHASE_CTORVT_FT3_MSG_WORD       13

///
/// @brief
///     三相电流互感器FT3采样值报文每帧报文长度为17words（含CRC）
///
#define TRIPHASE_CT_FT3_MSG_WORD           17

///
/// @brief
///     三相电压互感器FT3采样值报文(2010)每帧报文长度为14words（含CRC）
///
#define TRIPHASE_VT_FT3_2010_MSG_WORD      14

///
/// @brief
///     三相电压互感器FT3采样值报文(2011)每帧报文长度为14words（含CRC）
///
#define TRIPHASE_VT_FT3_2011_MSG_WORD      14

///
/// @brief
///     三相电流/电压互感器FT3采样值报文每帧报文长度为24words（含CRC）
///
#define TRIPHASE_CTORVT_FT3_MSG_WORD       24

///
/// @brief
///     电流/电压互感器FT3采样值报文每帧报文的最大长度（含CRC）
///
#define MAX_CTORVT_FT3_MSG_WORD            TRIPHASE_CTORVT_FT3_MSG_WORD

///
/// @brief
///     电流/电压互感器FT3采样值报文的类型
///
typedef enum{
	CFT_ONEPHASE_CTORVT = 0,
	CFT_TRIPHASE_CT,
	CFT_TRIPHASE_VT_2010,
	CFT_TRIPHASE_VT_2011,
	CFT_TRIPHASE_CTORVT
}CTORVT_FT3_TYPE;

///
/// @brief
///     电流/电压互感器FT3采样值报文的应用服务数据单元（ASDU）中除采样值（通道值）信息外的公共信息
///
typedef struct
{
	uint8_t  msgType;                      ///< 报文类型
	int8_t   temperature;                  ///< 温度（摄氏度）
	uint16_t delayTime;                    ///< 额定延时时间（us）
	uint16_t statusWord1;                  ///< 状态字1
	uint16_t statusWord2;                  ///< 状态字2
	uint16_t smpCnt;                       ///< 样本计数器，表示当前发送的采样点数目
}CTORVT_FT3_ASDU_PUBLIC;

///
/// @brief
///     电流/电压互感器FT3采样值报文的应用服务数据单元（ASDU）中除采样值（通道值）信息外的公共信息的偏移地址信息
///
typedef struct
{
	size_t msgTypeOff;                     ///< 报文类型的偏移地址
	size_t temperatureOff;                 ///< 温度的偏移地址
	size_t delayTimeOff;                   ///< 额定延时时间的偏移地址
	size_t statusWord1Off;                 ///< 状态字1的偏移地址
	size_t statusWord2Off;                 ///< 状态字2的偏移地址
	size_t smpCntOff;                      ///< 样本计数器的偏移地址
}CTORVT_FT3_ASDU_PUBLIC_OFFSET;

#endif
