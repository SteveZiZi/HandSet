/// @file
///     std52_ft3_msg.cpp
///
/// @brief
///     标准FT3采样值报文帧（52字节版本,含CRC）的编解码接口对象的实例的创建及释放函数
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
#include "../include/std52_ft3_msg.h"
#include "std52_ft3_msg_impl.h"

///
/// @brief
///     创建标准FT3采样值报文（52字节版本,含CRC）的编解码接口对象的实例
///
/// @return
///     指向Std52FT3Msg实例的指针
///
Std52FT3Msg *CreateStd52FT3MsgImpl()
{
	return new Std52FT3MsgImpl;
}

///
/// @brief
///     释放标准FT3采样值报文（52字节版本,含CRC）的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Std52FT3Msg实例的指针
///
/// @return
///     
///
void ReleaseStd52FT3MsgImpl(Std52FT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
