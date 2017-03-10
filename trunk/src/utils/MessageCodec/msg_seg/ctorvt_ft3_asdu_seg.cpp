/// @file
///     ctorvt_ft3_asdu_seg.cpp
///
/// @brief
///     电流/电压互感器FT3的ASDU段的编解码接口对象的实例的创建及释放函数
///
/// @note
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.09.05
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
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
///     创建电流/电压互感器FT3的ASDU段的编解码接口对象的实例
///
/// @param[in]
///     eCTorVTFt3Type - 电流/电压互感器FT3采样值报文的类型
///
/// @return
///     指向CTorVTFT3AsduSeg实例的指针
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
///     释放电流/电压互感器FT3的ASDU段的编解码接口对象的实例
///
/// @param[in]
///     impl - 指向CTorVTFT3AsduSeg实例的指针
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
