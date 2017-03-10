/// @file
///     nanrui_ft3_msg.cpp
///
/// @brief
///     ����FT3����ֵ����֡��IEEE60044-7/8��׼֡���ı����ӿڶ����ʵ���Ĵ������ͷź���
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.3.18
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
#include "../include/nanrui_ft3_msg.h"
#include "nanrui_ft3_msg_impl.h"

///
/// @brief
///     ��������FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��NanRuiFT3Msgʵ����ָ��
///
NanRuiFT3Msg *CreateNanRuiFT3MsgImpl()
{
	return new NanRuiFT3MsgImpl;
}

///
/// @brief
///     �ͷ�����FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��NanRuiFT3Msgʵ����ָ��
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
