/// @file
///     msg_data_elem.h
///
/// @brief
///     数据元素对象实例的创建和释放函数实现
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
#include "msg_data_elem.h"
#include "msg_base_data_elem.h"
#include "msg_tlv_data_elem.h"

///
/// @brief
///     创建数据元素对象的实例
///
/// @return
///     指向MsgDataElem实例的指针
///
MsgDataElem *CreateMsgDataElemImpl(DATA_ELEM_TYPE dt)
{
	switch(dt)
	{
	case DT_BASE_DATA:
		return new MsgBaseDataElem;
		break;
	case DT_TLV_DATA:
		return new MsgTlvDataElem;
		break;
	default:
		return NULL;
	}
}

///
/// @brief
///     释放数据元素对象的实例
///
/// @param[in]
///     impl - 指向MsgDataElem实例的指针
///
/// @return
///     
///
void ReleaseMsgDataElemImpl(MsgDataElem *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}