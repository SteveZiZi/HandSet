/// @file
///     ieee1588_msg.cpp
///
/// @brief
///     IEEE1588����֡�ı����ӿڶ����ʵ���Ĵ������ͷź���
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.9.29
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |               |           |                                                          |
///
#include "stdafx.h"
#include "../include/ieee1588_msg.h"
#include "ieee1588_msg_impl.h"

///
/// @brief
///     ����IEEE1588���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Ieee1588Msgʵ����ָ��
///
Ieee1588Msg *CreateIeee1588MsgImpl()
{
	return new Ieee1588MsgImpl;
}

///
/// @brief
///     �ͷ�IEEE1588���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Ieee1588Msgʵ����ָ��
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
