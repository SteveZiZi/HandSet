/// @file
///     sav92_msg_impl.cpp
///
/// @brief
///     Sav92����ֵ����֡��IEC61850-9-2LE���ı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "../include/sav92_msg.h"
#include "sav92_msg_impl.h"

///
/// @brief
///     ����IEC61850-9-2����ֵ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Sav92Msgʵ����ָ��
///
Sav92Msg *CreateSav92MsgImpl()
{
	return new Sav92MsgImpl;
}

///
/// @brief
///     �ͷ�IEC61850-9-2����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Sav92Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseSav92MsgImpl(Sav92Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
