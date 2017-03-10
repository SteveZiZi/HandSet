/// @file
///     guowang_ft3_msg.cpp
///
/// @brief
///     国网FT3采样值报文帧（IEEE60044-7/8扩展帧）的编解码接口对象的实例的创建及释放函数
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
#include "../include/guowang_ft3_msg.h"
#include "guowang_ft3_msg_impl.h"

///
/// @brief
///     创建国网FT3采样值报文的编解码接口对象的实例
///
/// @return
///     指向GuoWangFT3Msg实例的指针
///
GuoWangFT3Msg *CreateGuoWangFT3MsgImpl()
{
	return new GuoWangFT3MsgImpl;
}

///
/// @brief
///     释放国网FT3采样值报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向GuoWangFT3Msg实例的指针
///
/// @return
///     
///
void ReleaseGuoWangFT3MsgImpl(GuoWangFT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
