/// @file
///     sav92_msg_impl.cpp
///
/// @brief
///     Sav92采样值报文帧（IEC61850-9-2LE）的编解码接口对象的实例的创建及释放函数
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
#include "stdafx.h"
#include "../include/sav92_msg.h"
#include "sav92_msg_impl.h"

///
/// @brief
///     创建IEC61850-9-2采样值报文的编解码接口对象的实例
///
/// @return
///     指向Sav92Msg实例的指针
///
Sav92Msg *CreateSav92MsgImpl()
{
	return new Sav92MsgImpl;
}

///
/// @brief
///     释放IEC61850-9-2采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Sav92Msg实例的指针
///
/// @return
///     
///
void ReleaseSav92MsgImpl(Sav92Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
