/// @file
///     nanrui_ft3_msg.cpp
///
/// @brief
///     南瑞FT3采样值报文帧（IEEE60044-7/8标准帧）的编解码接口对象的实例的创建及释放函数
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
#include "../include/nanrui_ft3_msg.h"
#include "nanrui_ft3_msg_impl.h"

///
/// @brief
///     创建南瑞FT3采样值报文的编解码接口对象的实例
///
/// @return
///     指向NanRuiFT3Msg实例的指针
///
NanRuiFT3Msg *CreateNanRuiFT3MsgImpl()
{
	return new NanRuiFT3MsgImpl;
}

///
/// @brief
///     释放南瑞FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向NanRuiFT3Msg实例的指针
///
/// @return
///     
///
void ReleaseNanRuiFT3MsgImpl(NanRuiFT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
