/// @file
///     guowang_ft3_msg.cpp
///
/// @brief
///     ����FT3����ֵ����֡��IEEE60044-7/8��չ֡���ı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "../include/guowang_ft3_msg.h"
#include "guowang_ft3_msg_impl.h"

///
/// @brief
///     ��������FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��GuoWangFT3Msgʵ����ָ��
///
GuoWangFT3Msg *CreateGuoWangFT3MsgImpl()
{
	return new GuoWangFT3MsgImpl;
}

///
/// @brief
///     �ͷŹ���FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��GuoWangFT3Msgʵ����ָ��
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
