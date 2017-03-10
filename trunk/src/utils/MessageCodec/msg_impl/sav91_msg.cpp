/// @file
///     sav91_msg.cpp
///
/// @brief
///     Sav91����ֵ����֡��IEC61850-9-1���ı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "../include/sav91_msg.h"
#include "sav91_msg_impl.h"

///
/// @brief
///     ����IEC61850-9-1����ֵ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Sav91Msgʵ����ָ��
///
Sav91Msg *CreateSav91MsgImpl()
{
	return new Sav91MsgImpl;
}

///
/// @brief
///     �ͷ�IEC61850-9-1����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Sav91Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseSav91MsgImpl(Sav91Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
