/// @file
///     sav91_ex_msg.cpp
///
/// @brief
///    IEC61850-9-1��չ����֡�����Ͼ���������ƶ������������ı����ӿڶ����ʵ���Ĵ������ͷź���
///
/// @note
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.09.06
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
#include "../include/sav91_ex_msg.h"
#include "sav91_ex_msg_impl.h"

///
/// @brief
///     ����IEC61850-9-1��չ���ĵı����ӿڶ����ʵ��
///
/// @return
///     ָ��Sav91ExMsgʵ����ָ��
///
Sav91ExMsg *CreateSav91ExMsgImpl()
{
	return new Sav91ExMsgImpl;
}

///
/// @brief
///     �ͷ�IEC61850-9-1��չ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��Sav91ExMsgʵ����ָ��
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
