/// @file
///     sav91_ex_msg.cpp
///
/// @brief
///    IEC61850-9-1扩展报文帧（由南京新宁光电制定，不常见）的编解码接口对象的实例的创建及释放函数
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.06
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
#include "../include/sav91_ex_msg.h"
#include "sav91_ex_msg_impl.h"

///
/// @brief
///     创建IEC61850-9-1扩展报文的编解码接口对象的实例
///
/// @return
///     指向Sav91ExMsg实例的指针
///
Sav91ExMsg *CreateSav91ExMsgImpl()
{
	return new Sav91ExMsgImpl;
}

///
/// @brief
///     释放IEC61850-9-1扩展报文的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向Sav91ExMsg实例的指针
///
/// @return
///     
///
void ReleaseSav91ExMsgImpl(Sav91ExMsg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
