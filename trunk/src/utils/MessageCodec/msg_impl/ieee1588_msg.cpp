/// @file
///     ieee1588_msg.cpp
///
/// @brief
///     IEEE1588报文帧的编解码接口对象的实例的创建及释放函数
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
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "../include/ieee1588_msg.h"
#include "ieee1588_msg_impl.h"

///
/// @brief
///     创建IEEE1588报文的编解码接口对象的实例
///
/// @return
///     指向Ieee1588Msg实例的指针
///
Ieee1588Msg *CreateIeee1588MsgImpl()
{
	return new Ieee1588MsgImpl;
}

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
void ReleaseIeee1588MsgImpl(Ieee1588Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
