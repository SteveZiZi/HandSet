/// @file
///     std52_ft3_msg.cpp
///
/// @brief
///     ��׼FT3����ֵ����֡��52�ֽڰ汾,��CRC���ı����ӿڶ����ʵ���Ĵ������ͷź���
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.05
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
#include "../include/std52_ft3_msg.h"
#include "std52_ft3_msg_impl.h"

///
/// @brief
///     ������׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC���ı����ӿڶ����ʵ��
///
/// @return
///     ָ��Std52FT3Msgʵ����ָ��
///
Std52FT3Msg *CreateStd52FT3MsgImpl()
{
	return new Std52FT3MsgImpl;
}

///
/// @brief
///     �ͷű�׼FT3����ֵ���ģ�52�ֽڰ汾,��CRC���ı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Std52FT3Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseStd52FT3MsgImpl(Std52FT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
