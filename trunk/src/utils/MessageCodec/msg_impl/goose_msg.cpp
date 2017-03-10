/// @file
///     goose_msg.cpp
///
/// @brief
///     GOOSE报文帧的编解码接口对象的实例的创建及释放函数
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
#include "../include/goose_msg.h"
#include "goose_msg_impl.h"

///
/// @brief
///     创建IEC61850 GOOSE报文的编解码接口对象的实例
///
/// @return
///     指向GooseMsg实例的指针
///
GooseMsg *CreateGooseMsgImpl()
{
	return new GooseMsgImpl;
}

///
/// @brief
///     释放IEC61850 GOOSE报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向GooseMsg实例的指针
///
/// @return
///     
///
void ReleaseGooseMsgImpl(GooseMsg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}

