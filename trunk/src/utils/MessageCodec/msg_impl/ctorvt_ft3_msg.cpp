/// @file
///     ctorvt_ft3_msg.cpp
///
/// @brief
///     电流/电压互感器FT3采样值报文帧的编解码接口对象的实例的创建及释放函数
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
#include "stdafx.h"
#include "../include/ctorvt_ft3_msg.h"
#include "ctorvt_ft3_msg_impl.h"

///
/// @brief
///     创建电流/电压互感器FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     eCTorVTFt3Type - 电流/电压互感器FT3采样值报文的类型
///
/// @return
///     指向CTorVTFT3Msg实例的指针
///
CTorVTFT3Msg *CreateCTorVTFT3MsgImpl(CTORVT_FT3_TYPE eCTorVTFt3Type)
{
	return new CTorVTFT3MsgImpl(eCTorVTFt3Type);
}

///
/// @brief
///     释放电流/电压互感器FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向CTorVTFT3Msg实例的指针
///
/// @return
///     
///
void ReleaseCTorVTFT3MsgImpl(CTorVTFT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
