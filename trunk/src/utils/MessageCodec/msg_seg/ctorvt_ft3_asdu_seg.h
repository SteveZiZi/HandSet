/// @file
///     ctorvt_ft3_asdu_seg.h
///
/// @brief
///     电流/电压互感器FT3的ASDU段的编解码接口及接口对象实例的创建和释放接口
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
#ifndef CTORVT_FT3_ASDU_SEG_H_

#define CTORVT_FT3_ASDU_SEG_H_

#include "../include/ctorvt_ft3_types.h"

class MsgDataElem;

class CTorVTFT3AsduSeg
{
public:
	virtual ~CTorVTFT3AsduSeg(){};

	virtual size_t Decode(const size_t offset, const uint16_t *asduDataBuf, const size_t bufSize) = 0;
	virtual size_t Code(uint16_t *asduDataArr, const size_t arrSize) = 0;

	virtual bool   GetASDUPublicInfo(const uint16_t *msgDataBuf, CTORVT_FT3_ASDU_PUBLIC &asduPublic) = 0;
	virtual size_t GetASDUSmpsNum() = 0;
	virtual size_t GetASDUSmps(const uint16_t *msgDataBuf, uint16_t *smpValArr, const size_t arrSize) = 0;
	virtual size_t GetASDULen() = 0;

	virtual size_t GetASDUOff() = 0;
	virtual void   GetASDUPublicInfoOff(CTORVT_FT3_ASDU_PUBLIC_OFFSET &asduPublicOff) = 0;
	virtual size_t GetASDUSmpsOff(size_t *smpOffArr, const size_t arrSize) = 0;

	virtual void   SetASDU(const CTORVT_FT3_ASDU_PUBLIC &asduPublic, const uint16_t *smpValBuf, const size_t &bufSize) = 0;
};

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
CTorVTFT3AsduSeg *CreateCTorVTFT3AsduSegImpl(CTORVT_FT3_TYPE eCTorVTFt3Type);

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
void ReleaseCTorVTFT3AsduSegImpl(CTorVTFT3AsduSeg *impl);

#endif
