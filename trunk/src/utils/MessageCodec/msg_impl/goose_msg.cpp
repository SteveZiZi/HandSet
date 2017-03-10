/// @file
///     goose_msg.cpp
///
/// @brief
///     GOOSE����֡�ı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "../include/goose_msg.h"
#include "goose_msg_impl.h"

///
/// @brief
///     ����IEC61850 GOOSE���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��GooseMsgʵ����ָ��
///
GooseMsg *CreateGooseMsgImpl()
{
	return new GooseMsgImpl;
}

///
/// @brief
///     �ͷ�IEC61850 GOOSE���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��GooseMsgʵ����ָ��
///
/// @return
///     
///
void ReleaseGooseMsgImpl(GooseMsg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}

