/// @file
///     ctorvt_ft3_asdu_seg.cpp
///
/// @brief
///     ����/��ѹ������FT3��ASDU�εı����ӿڶ����ʵ���Ĵ������ͷź���
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
#include "ctorvt_ft3_asdu_seg.h"
#include "onephase_ctorvt_ft3_asdu_seg.h"
#include "triphase_ct_ft3_asdu_seg.h"
#include "triphase_vt_ft3_2010_asdu_seg.h"
#include "triphase_vt_ft3_2011_asdu_seg.h"
#include "triphase_ctorvt_ft3_asdu_seg.h"

///
/// @brief
///     ��������/��ѹ������FT3��ASDU�εı����ӿڶ����ʵ��
///
/// @param[in]
///     eCTorVTFt3Type - ����/��ѹ������FT3����ֵ���ĵ�����
///
/// @return
///     ָ��CTorVTFT3AsduSegʵ����ָ��
///
CTorVTFT3AsduSeg *CreateCTorVTFT3AsduSegImpl(CTORVT_FT3_TYPE eCTorVTFt3Type)
{
	switch ( eCTorVTFt3Type )
	{
	case CFT_ONEPHASE_CTORVT:
		return new OnePhaseCTorVTFT3AsduSeg;
		break;
	case CFT_TRIPHASE_CT:
		return new TriphaseCTFT3AsduSeg;
		break;
	case CFT_TRIPHASE_VT_2010:
		return new TriphaseVTFT3_2010AsduSeg;
		break;
	case CFT_TRIPHASE_VT_2011:
		return new TriphaseVTFT3_2011AsduSeg;
		break;
	case CFT_TRIPHASE_CTORVT:
		return new TriphaseCTorVTFT3AsduSeg;
		break;
	default:
		return NULL;
	}
}

///
/// @brief
///     �ͷŵ���/��ѹ������FT3��ASDU�εı����ӿڶ����ʵ��
///
/// @param[in]
///     impl - ָ��CTorVTFT3AsduSegʵ����ָ��
///
/// @return
///     
///
void ReleaseCTorVTFT3AsduSegImpl(CTorVTFT3AsduSeg *impl)
{
	if ( impl != NULL )
	{
		delete impl;
		impl = NULL;
	}
}
