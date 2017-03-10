/// @file
///     ctorvt_ft3_msg.cpp
///
/// @brief
///     ����/��ѹ������FT3����ֵ����֡�ı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "../include/ctorvt_ft3_msg.h"
#include "ctorvt_ft3_msg_impl.h"

///
/// @brief
///     ��������/��ѹ������FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     eCTorVTFt3Type - ����/��ѹ������FT3����ֵ���ĵ�����
///
/// @return
///     ָ��CTorVTFT3Msgʵ����ָ��
///
CTorVTFT3Msg *CreateCTorVTFT3MsgImpl(CTORVT_FT3_TYPE eCTorVTFt3Type)
{
	return new CTorVTFT3MsgImpl(eCTorVTFt3Type);
}

///
/// @brief
///     �ͷŵ���/��ѹ������FT3����ֵ���ĵı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��CTorVTFT3Msgʵ����ָ��
///
/// @return
///     
///
void ReleaseCTorVTFT3MsgImpl(CTorVTFT3Msg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
