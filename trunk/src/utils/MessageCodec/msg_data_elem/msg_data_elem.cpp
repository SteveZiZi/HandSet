/// @file
///     msg_data_elem.h
///
/// @brief
///     ����Ԫ�ض���ʵ���Ĵ������ͷź���ʵ��
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
#include "msg_data_elem.h"
#include "msg_base_data_elem.h"
#include "msg_tlv_data_elem.h"

///
/// @brief
///     ��������Ԫ�ض����ʵ��
///
/// @return
///     ָ��MsgDataElemʵ����ָ��
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
///     �ͷ�����Ԫ�ض����ʵ��
///
/// @param[in]
///     impl - ָ��MsgDataElemʵ����ָ��
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