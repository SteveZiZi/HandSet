/// @file
///     sav91_msg.cpp
///
/// @brief
///     Sav91采样值报文帧（IEC61850-9-1）的编解码接口对象的实例的创建及释放函数
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
#include "../include/sav91_msg.h"
#include "sav91_msg_impl.h"

///
/// @brief
///     创建IEC61850-9-1采样值报文的编解码接口对象的实例
///
/// @return
///     指向Sav91Msg实例的指针
///
Sav91Msg *CreateSav91MsgImpl()
{
	return new Sav91MsgImpl;
}

///
/// @brief
///     释放IEC61850-9-1采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Sav91Msg实例的指针
///
/// @return
///     
///
void ReleaseSav91MsgImpl(Sav91Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
